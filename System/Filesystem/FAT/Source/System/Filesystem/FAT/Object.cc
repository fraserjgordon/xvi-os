#include <System/Filesystem/FAT/Object.hh>
#include <System/Filesystem/FAT/ObjectImpl.hh>

#include <cstring>

#include <System/Filesystem/FAT/DirectoryImpl.hh>
#include <System/Filesystem/FAT/FileImpl.hh>
#include <System/Filesystem/FAT/FilesystemImpl.hh>
#include <System/Filesystem/FAT/Logging.hh>


namespace System::Filesystem::FAT
{


std::string_view ObjectInfo::name() const
{
    return ObjectInfoImpl::from(this).name();
}


std::uint32_t ObjectInfo::size() const
{
    return ObjectInfoImpl::from(this).size();
}


ObjectType ObjectInfo::type() const
{
    return ObjectInfoImpl::from(this).type();
}


std::uint32_t ObjectInfo::startCluster() const
{
    return ObjectInfoImpl::from(this).startCluster();
}


std::uint32_t ObjectInfo::nthCluster(std::uint32_t n) const
{
    return ObjectInfoImpl::from(this).nthCluster(n);
}


std::uint32_t ObjectInfo::clusterCount() const
{
    return ObjectInfoImpl::from(this).clusterCount();
}


std::uint32_t ObjectInfo::lastClusterLength() const
{
    return ObjectInfoImpl::from(this).lastClusterLength();
}


std::uint32_t ObjectInfo::startSector() const
{
    return ObjectInfoImpl::from(this).startSector();
}


std::uint32_t ObjectInfo::nthSector(std::uint32_t n) const
{
    return ObjectInfoImpl::from(this).nthSector(n);
}


std::uint32_t ObjectInfo::sectorCount() const
{
    return ObjectInfoImpl::from(this).sectorCount();
}


std::uint32_t ObjectInfo::lastSectorLength() const
{
    return ObjectInfoImpl::from(this).lastSectorLength();
}


Filesystem& ObjectInfo::filesystem() const
{
    return ObjectInfoImpl::from(this).filesystem().asFilesystem();
}


void ObjectInfo::close() const
{
    delete &ObjectInfoImpl::from(const_cast<ObjectInfo*>(this));
}


ObjectInfoImpl::ObjectInfoImpl(FilesystemImpl& fs, std::string name, const dirent_t& dirent) :
    m_filesystem{&fs},
    m_dirent{dirent},
    m_name{std::move(name)}
{
    // What type of object are we dealing with?
    if (m_dirent.hasAttribute(AttributeDirectory))
        m_type = ObjectType::Directory;
    else if (m_dirent.hasAttribute(AttributeVolumeLabel))
        m_type = ObjectType::VolumeLabel;
    else
        m_type = ObjectType::File;
}


ObjectInfoImpl::ObjectInfoImpl(FilesystemImpl& fs, root_dir_object_info_t) :
    m_filesystem{&fs},
    m_dirent{},
    m_name{},
    m_type{ObjectType::RootDirectory}
{
    // Special handling of the root directory to set the size.
    if (auto type = fs.fatType(); type == fat_type::FAT12 || type == fat_type::FAT16)
        m_dirent.size = fs.rootDirSectorCount() * fs.sectorSize();
    else
        m_dirent.size = 0;
}


std::string_view ObjectInfoImpl::name() const
{
    return m_name;
}


std::uint32_t ObjectInfoImpl::size() const
{
    if (m_type == ObjectType::File || m_dirent.size > 0)
        return m_dirent.size;

    if (m_dirSize == 0) [[unlikely]]
    {
        // Walk the cluster chain to calculate the size.
        std::uint32_t clusters = 0;
        m_filesystem->readClusterChain(startCluster(), [&clusters](std::uint32_t index, std::uint32_t)
        {
            clusters = index + 1;
            return true;
        });

        m_dirSize = clusters;
    }

    return m_dirSize;
}


ObjectType ObjectInfoImpl::type() const
{
    return m_type;
}


std::uint32_t ObjectInfoImpl::startCluster() const
{
    return m_dirent.getStartCluster();
}


std::uint32_t ObjectInfoImpl::nthCluster(std::uint32_t n) const
{
    return m_filesystem->nthChainEntry(m_dirent.getStartCluster(), n);
}


std::uint32_t ObjectInfoImpl::clusterCount() const
{
    auto cluster_size = m_filesystem->clusterSize();
    return (size() + cluster_size - 1) / cluster_size;
}


std::uint32_t ObjectInfoImpl::lastClusterLength() const
{
    auto cluster_size = m_filesystem->clusterSize();
    return size() & (cluster_size - 1);
}


std::uint32_t ObjectInfoImpl::startSector() const
{
    return m_filesystem->clusterToSector(startCluster());
}


std::uint32_t ObjectInfoImpl::nthSector(std::uint32_t n) const
{
    auto sectors_per_cluster = m_filesystem->sectorsPerCluster();
    auto cluster_index = n / sectors_per_cluster;
    auto cluster = nthCluster(cluster_index);

    return m_filesystem->clusterToSector(cluster) + n % sectors_per_cluster;
}


std::uint32_t ObjectInfoImpl::sectorCount() const
{
    auto sector_size = m_filesystem->sectorSize();
    return (size() + sector_size - 1) / sector_size;
}


std::uint32_t ObjectInfoImpl::lastSectorLength() const
{
    auto sector_size = m_filesystem->sectorSize();
    return size() & (sector_size - 1);
}


const dirent_t& ObjectInfoImpl::dirent() const
{
    return m_dirent;
}


FilesystemImpl& ObjectInfoImpl::filesystem() const
{
    return *m_filesystem;
}


void ObjectInfoImpl::setSize(std::uint32_t size)
{
    m_dirent.size = size;
}

void ObjectInfoImpl::setCluster(std::uint32_t cluster)
{
    m_dirent.cluster_low = cluster & 0xFFFF;
    m_dirent.cluster_high = cluster >> 16;
}


void ObjectInfoImpl::setMetadata(const ObjectInfoImpl& updated)
{
    m_dirent = updated.m_dirent;
}


const ObjectInfo& Object::info() const
{
    return *ObjectImpl::from(this).info().asObjectInfo();
}


std::uint32_t Object::readTo(std::uint32_t offset, std::uint32_t length, std::span<std::byte> buffer) const
{
    return ObjectImpl::from(this).readTo(offset, length, buffer);
}


void Object::close()
{
    delete &ObjectImpl::from(this);
}


ObjectImpl::ObjectImpl(const ObjectInfoImpl& info) :
    m_info{info}
{
}


const ObjectInfoImpl& ObjectImpl::info() const
{
    return m_info;
}


std::uint32_t ObjectImpl::readTo(std::uint32_t offset, std::uint32_t length, std::span<std::byte> out) const
{
    auto& fs = info().filesystem();

    // Sanity check the buffer.
    if (out.size_bytes() < length)
        return 0;

    log(priority::trace, "read {} bytes from {} offset {}", length, info().name(), offset);

    auto sector_size = fs.sectorSize();
    auto sector_offset = offset % sector_size;

    // Align the read to sector boundaries.
    auto end = offset + length;
    offset = offset & ~(sector_size - 1);
    end = (end + sector_size - 1) & ~(sector_size - 1);
    length = end - offset;

    std::uint32_t read = 0;
    readBlocks(offset, length, [&](std::uint32_t, std::span<const std::byte> data)
    {
        // If this is the first or last block, a partial copy might be needed.
        auto copy_offset = 0;
        auto copy_length = data.size_bytes();

        if (read == 0 || copy_length > out.size_bytes())
        {
            copy_offset = sector_offset;
            copy_length = std::min<std::size_t>(copy_length - copy_offset, out.size_bytes());
        }

        std::memcpy(out.data(), data.data() + copy_offset, copy_length);

        read += copy_length;
        out = out.subspan(copy_length);

        return !out.empty();
    });

    return read;
}


void ObjectImpl::readBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t callback) const
{
    auto& fs = info().filesystem();

    auto size = info().size();
    auto sector_size = fs.sectorSize();

    // Clamp the length (and therefore the ending offset) to the size of the object.
    auto end = std::min(offset + length, size);

    // Check that the offset and end are within bounds.
    if (offset >= end)
        return;

    // Round the offset to a block boundary and re-adjust the length to compensate.
    auto adjustment = (offset & (sector_size - 1));
    offset = offset - adjustment;
    length = end - offset;

    // Now that we've aligned everything to sectors, check for and handle root directory reads specially for FAT12 and FAT16.
    if (info().type() == ObjectType::RootDirectory && (fs.fatType() == fat_type::FAT12 || fs.fatType() == fat_type::FAT16))
        return readRootDirBlocks(offset, length, callback);

    // How many sectors are we going to read?
    auto first_sector = offset / sector_size;
    auto last_sector = (end + sector_size - 1) / sector_size;
    auto sector_count = last_sector - first_sector;

    // What is the index of the first and last clusters that we are interested in?
    auto sectors_per_cluster = fs.sectorsPerCluster();
    auto cluster_size = fs.clusterSize();
    auto first_cluster_index = offset / cluster_size;
    auto last_cluster_index = (end + cluster_size - 1) / cluster_size;

    // Read the cluster chain.
    fs.readClusterChain(info().startCluster(), [&](std::uint32_t n, std::uint32_t cluster)
    {
        // If we've gone beyond the last cluster we're interested in, stop the scan.
        if (n >= last_cluster_index)
            return false;

        // If we've not yet reached the first cluster we're interested in, continue.
        if (n < first_cluster_index)
            return true;

        // We have a cluster we care about.
        auto sector = fs.clusterToSector(cluster);
        auto sector_index = n * sectors_per_cluster;
        for (std::uint32_t i = 0; i < sectors_per_cluster; ++i)
        {
            // Skip any leading sectors we don't care about.
            if (sector_index + i < first_sector)
                continue;

            // End if we've processed all sectors that were asked for.
            if (sector_index + i >= last_sector)
                break;

            // Read the sector.
            auto data = fs.readBlock(sector + i);
            auto result = callback((sector_index + i) * sector_size, std::span{data->readSpan().data(), sector_size});
            if (!result)
                return false;
        }

        // Continue following the chain.
        return true;
    });
}


bool ObjectImpl::truncate(std::uint32_t length)
{
    auto& fs = m_info.filesystem();

    // Do nothing if we're not actually truncating.
    if (length >= m_info.size())
        return true;

    // FAT12 and FAT16 root directories cannot be truncated.
    if (m_info.type() == ObjectType::RootDirectory && fs.fatType() != fat_type::FAT32)
        return false;

    log(priority::trace, "truncating {} to {}", m_info.name(), length);

    // How far into the cluster chain are we truncating?
    auto cluster_size = fs.clusterSize();
    auto cluster_count = (length + cluster_size - 1) / cluster_size;

    // If this is a directory, we need to retain a minimum of one cluster.
    if (m_info.type() != ObjectType::File)
        cluster_count = std::max<std::uint32_t>(cluster_count, 1U);

    // Free any clusters beyond the point we're keeping.
    fs.freeClusterChain(m_info.nthCluster(cluster_count));
    if (cluster_count == 0)
        m_info.setCluster(0);

    // Update the size (files only - fixed at zero for directories).
    if (m_info.type() == ObjectType::File)
        m_info.setSize(length);

    return true;
}


bool ObjectImpl::append(std::span<const std::byte> data)
{
    return write(info().size(), data);
}


bool ObjectImpl::write(std::uint32_t offset, std::span<const std::byte> data)
{
    auto& fs = m_info.filesystem();

    log(priority::trace, "writing {} bytes {}+{}", m_info.name(), offset, data.size_bytes());

    // Find which cluster the write is going to start with.
    auto end = offset + data.size_bytes();
    auto cluster_size = fs.clusterSize();
    auto cluster_index = offset / cluster_size;
    auto cluster_count = m_info.clusterCount();

    // The write can be split into two parts: an overwrite and an append. Any writes that requirw writing to sectors
    // that don't currently contain data (including unused sectors within a cluster) are treated as an append.
    auto sector_size = fs.sectorSize();
    auto sector_count = m_info.sectorCount();
    auto valid_length = sector_size * sector_count;
    auto boundary = valid_length & ~(cluster_size - 1);

    // Shared state for the write operations.
    cluster_hint hint = {};
    buffer_t buffer = {};

    // Is any of the write below the overwrite boundary?
    if (offset < boundary)
    {
        // Perform an overwrite.
        auto overwrite_end = std::min<std::uint32_t>(end, boundary);
        auto overwrite_length = overwrite_end - offset;
        if (!overwriteClusters(offset, data.subspan(0, overwrite_length), hint, buffer, valid_length))
            return false;

        data = data.subspan(overwrite_length);
    }

    // Is any of the write above the append boundary?
    if (end > boundary)
    {
        // Perform an append.
        auto append_start = std::max<std::uint32_t>(offset, boundary);
        if (!appendClusters(append_start, data, hint, buffer, valid_length))
            return false;
    }

    // Update the size of the file, if necessary.
    auto file_size = std::max<std::uint32_t>(end, m_info.size());
    if (m_info.type() == ObjectType::File)
    {
        m_info.setSize(file_size);
    }

    return true;
}


std::unique_ptr<ObjectImpl> ObjectImpl::open(const ObjectInfoImpl& info)
{
    // What kind of object is being opened?
    switch (info.type())
    {
        case ObjectType::File:
            return std::unique_ptr<ObjectImpl>(new FileImpl{info});

        case ObjectType::Directory:
        case ObjectType::RootDirectory:
            return std::unique_ptr<ObjectImpl>(new DirectoryImpl{info});

        case ObjectType::VolumeLabel:
            // Not supported for opening.
            return nullptr;
    }

    // Shouldn't get here.
    return nullptr;
}


void ObjectImpl::readRootDirBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t callback) const
{
    // With FAT12 and FAT16, the root directory is a pre-allocated contiguous area before the first data cluster. We can
    // just read it as requested.
    auto& fs = info().filesystem();
    auto sector_size = fs.sectorSize();
    auto sector_count = std::min({length / sector_size, fs.rootDirSectorCount() - offset});
    auto start_sector = fs.rootDirStartSector() + (offset / sector_size);
    auto end_sector = start_sector + sector_count;

    for (std::uint32_t i = 0; i < sector_count; ++i)
    {
        auto data = fs.readBlock(start_sector + i);
        auto result = callback(offset + (i * sector_size), std::span{data->readSpan().data(), sector_size});
        if (!result)
            return;
    }
}


std::uint32_t ObjectImpl::clusterLookup(std::uint32_t index, cluster_hint& hint) const
{
    // If the hint isn't usable, reset it.
    if (index < hint.index || hint.cluster == 0)
        hint = {};

    // Use the hint to search the cluster chain.
    auto start = hint.cluster ? hint.cluster : info().startCluster();
    if (start == 0)
        return 0;
    auto cluster = info().filesystem().nthChainEntry(start, index - hint.index);

    // Update the hint.
    hint = {index, cluster};

    return cluster;
}


std::span<std::byte> ObjectImpl::ensureBuffer(buffer_t& buffer)
{
    if (!buffer)
        buffer = std::make_unique<std::byte[]>(info().filesystem().sectorSize());

    return std::span{buffer.get(), info().filesystem().sectorSize()};
}


bool ObjectImpl::overwriteClusters(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length)
{
    auto& fs = info().filesystem();

    // Calculate where we are going to be doing the write.
    auto sector_size = fs.sectorSize();
    auto cluster_size = fs.clusterSize();
    auto cluster_index = offset / cluster_size;
    auto cluster_offset = offset % cluster_size;
    auto end = offset + data.size_bytes();
    auto aligned_end = (end + sector_size - 1) & ~(sector_size - 1);

    // This method cannot extend a file beyond the last sector it currently occupies.
    if (end > valid_length)
        return false;

    // Iterate over each cluster in the request.
    while (data.size_bytes() > 0)
    {
        // Update the cluster.
        auto cluster = clusterLookup(cluster_index, hint);
        if (cluster == 0)
            return false;

        // Write to this cluster.
        auto length = std::min<std::uint32_t>(cluster_size - cluster_offset, data.size_bytes());
        overwriteCluster(offset, data.subspan(0, length), hint, buffer, valid_length);

        offset += length;
        cluster_offset = 0;
        ++cluster_index;

        data = data.subspan(length);
    }

    return true;
}


bool ObjectImpl::overwriteCluster(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length)
{
    auto& fs = info().filesystem();

    // Calculate where we are going to be doing the write.
    auto cluster_size = fs.clusterSize();
    auto sectors_per_cluster = fs.sectorsPerCluster();
    auto sector_size = fs.sectorSize();
    auto cluster_index = offset / cluster_size;
    auto cluster_offset = offset % cluster_size;
    auto end = cluster_offset + data.size_bytes();
    auto sector_index = cluster_offset / sector_size;
    auto sector_offset = cluster_offset % sector_size;

    // The write must not span the cluster boundary.
    if (end > cluster_size)
        return false;

    // Which cluster (and therefore sector(s)) are we going to be writing?
    auto cluster = clusterLookup(cluster_index, hint);
    if (cluster == 0)
        return false;
    auto sector = fs.clusterToSector(cluster);

    // Perform any initial partial overwrite.
    if (sector_offset != 0)
    {
        auto partial_size = std::min<std::uint32_t>(data.size_bytes(), sector_size - sector_offset);
        if (!overwritePartialSector(sector + sector_index, sector_offset, data.subspan(0, partial_size)))
            return false;

        sector_offset = 0;
        ++sector_index;

        data = data.subspan(partial_size);
    }

    // Perform any complete-sector overwrites.
    auto sector_count = data.size_bytes() / sector_size;
    for (std::uint32_t i = 0; i < sector_count; ++i)
    {
        if (!overwriteSector(sector + sector_index + i, data.subspan(0, sector_size)))
            return false;

        data = data.subspan(sector_size);
    }

    // Perfom any final partial overwrite.
    if (data.size_bytes() > 0)
    {
        if (!overwritePartialSector(sector + sector_index + sector_count, 0, data))
            return false;
    }

    // The file is valid up until at least the end of the last written sector.
    valid_length = std::max<std::uint32_t>(valid_length, (end + sector_size - 1) & ~(sector_size - 1));

    return true;
}


bool ObjectImpl::overwritePartialSector(std::uint32_t lba, std::uint32_t offset, std::span<const std::byte> data)
{
    auto& fs = info().filesystem();

    // Calculate the various parameters for the write.
    auto sector_size = fs.sectorSize();
    auto length = data.size_bytes();
    auto end = offset + length;

    // Check that the write is valid.
    if (end > sector_size)
        return false;

    // Read the sector.
    auto contents = fs.readBlockForUpdate(lba);
    if (!contents)
        return false;

    // Copy the new data over the old.
    std::memcpy(contents->writeSpan().data() + offset, data.data(), length);

    return true;
}


bool ObjectImpl::overwriteSector(std::uint32_t lba, std::span<const std::byte> data)
{
    auto& fs = info().filesystem();

    // Check that the buffer is the right size.
    if (data.size_bytes() != fs.sectorSize())
        return false;

    return fs.writeBlock(lba, data).has_value();
}


bool ObjectImpl::appendClusters(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length)
{
    auto& fs = info().filesystem();

    // Calculate where we are going to be doing the write.
    auto cluster_size = fs.clusterSize();
    auto cluster_index = offset / cluster_size;
    auto cluster_offset = offset % cluster_size;

    // Find the current last cluster in the file.
    auto valid_cluster_count = (valid_length / cluster_size);
    auto last_cluster_index = valid_cluster_count > 0 ? valid_cluster_count - 1 : 0;
    auto last_cluster = valid_cluster_count > 0 ? clusterLookup(last_cluster_index, hint) : 0;

    // Check that the offset corresponds to the last cluster.
    if (cluster_index < last_cluster_index)
        return false;

    // Check we're not skipping any uninitialised data.
    if (offset > valid_length)
        return false;

    // Iterate over each cluster in the request.
    while (data.size_bytes() > 0)
    {
        // Do we need to allocate a new cluster?
        if (last_cluster == 0 || last_cluster_index < cluster_index)
        {
            // Allocate a new cluster and add it to the chain.
            auto new_cluster = fs.findFreeCluster(true);
            if (new_cluster == 0)
                return false;

            if (last_cluster != 0)
                fs.writeClusterChainLink(last_cluster, new_cluster);

            // If this is the first cluster, update the metadata.
            if (last_cluster_index == 0 && last_cluster == 0)
                m_info.setCluster(new_cluster);

            ++last_cluster_index;
            last_cluster = new_cluster;
        }

        // Append to this cluster.
        auto length = std::min<std::uint32_t>(cluster_size - cluster_offset, data.size_bytes());
        appendCluster(offset, data.subspan(0, length), hint, buffer, valid_length);

        offset += length;
        cluster_offset = 0;
        ++cluster_index;

        data = data.subspan(length);
    }

    return true;
}


bool ObjectImpl::appendCluster(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length)
{
    auto& fs = info().filesystem();

    // Calculate where we are going to be doing the write.
    auto cluster_size = fs.clusterSize();
    auto sectors_per_cluster = fs.sectorsPerCluster();
    auto sector_size = fs.sectorSize();
    auto cluster_index = offset / cluster_size;
    auto cluster_offset = offset % cluster_size;
    auto end = cluster_offset + data.size_bytes();
    auto sector_index = cluster_offset / sector_size;
    auto sector_offset = cluster_offset % sector_size;

    // The file must be valid up until at least the point we're starting to append.
    if (valid_length < offset)
        return false;

    // The write must not span the cluster boundary.
    if (end > cluster_size)
        return false;

    // Which cluster (and therefore sector(s)) are we going to be writing?
    auto cluster = clusterLookup(cluster_index, hint);
    if (cluster == 0)
        return false;
    auto sector = fs.clusterToSector(cluster);

    // Perform any initial partial append.
    if (sector_offset != 0)
    {
        auto partial_size = std::min<std::uint32_t>(data.size_bytes(), sector_size - sector_offset);
        if (!appendPartialSector(sector + sector_index, sector_offset, data.subspan(0, partial_size), buffer))
            return false;

        sector_offset = 0;
        ++sector_index;

        data = data.subspan(partial_size);

        // appendPartialSector makes the contents valid until the end of the sector.
        valid_length += sector_size;
    }

    // Perform any complete-sector appends.
    auto sector_count = data.size_bytes() / sector_size;
    for (std::uint32_t i = 0; i < sector_count; ++i)
    {
        if (!appendSector(sector + sector_index + i, data.subspan(0, sector_size)))
            return false;

        data = data.subspan(sector_size);

        // Whole sectors are being appended.
        valid_length += sector_size;
    }

    // Perfom any final partial append.
    if (data.size_bytes() > 0)
    {
        if (!appendPartialSector(sector + sector_index + sector_count, 0, data, buffer))
            return false;

        // appendPartialSector makes the contents valid until the end of the sector.
        valid_length += sector_size;
    }

    return true;
}


bool ObjectImpl::appendPartialSector(std::uint32_t lba, std::uint32_t offset, std::span<const std::byte> data, buffer_t& buffer)
{
    auto& fs = info().filesystem();

    // Calculate the various parameters for the write.
    auto sector_size = fs.sectorSize();
    auto length = data.size_bytes();
    auto end = offset + length;

    // Check that the write is valid.
    if (end > sector_size)
        return false;

    // There are two scenarios we could be dealing with:
    //      1. offset==0: we do not care about the existing contents of the sector.
    //      2. offset!=0: we need to preserve parts of the sector.
    //
    // The first one can be written without reading anything but the second needs a read-modify-write and is equivalent
    // to a sector overwrite (if we assume that unused bytes in the sector are zeroes, which they should be).
    if (offset != 0)
        return overwritePartialSector(lba, offset, data);

    // Prepare a buffer to hold the new sector.
    auto contents = ensureBuffer(buffer);
    std::memcpy(contents.data(), data.data(), length);
    std::memset(contents.data() + length, 0, sector_size - length);

    // Write the new sector.
    fs.writeBlock(lba, contents);

    return true;
}


bool ObjectImpl::appendSector(std::uint32_t lba, std::span<const std::byte> data)
{
    // There's no difference between overwriting or appending when it comes to whole sectors.
    return overwriteSector(lba, data);
}


} // namespace System::Filesystem::FAT
