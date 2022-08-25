#include <System/Filesystem/FAT/Filesystem.hh>
#include <System/Filesystem/FAT/FilesystemImpl.hh>

#if !defined(__XVI_NO_STDLIB)
#  include <bit>
#  include <cstring>
#  if __has_include(<format>)
#    include <format>
#  else
#    include <fmt/format.h>
#  endif
#else
#  include <System/C++/Format/Format.hh>
#  include <System/C++/Utility/Bit.hh>
#endif

#include <System/Filesystem/FAT/DirectoryImpl.hh>
#include <System/Filesystem/FAT/Logging.hh>
#include <System/Filesystem/FAT/ObjectImpl.hh>


#if defined(__XVI_NO_STDLIB) || __has_include(<format>)
using std::format;
#else
using fmt::format;
#endif


namespace System::Filesystem::FAT
{


bool Filesystem::mount()
{
    return FilesystemImpl::from(this).mount();
}


Directory* Filesystem::rootDirectory()
{
    return FilesystemImpl::from(this).rootDirectory()->asDirectory();
}


Object::handle_t Filesystem::openObject(const ObjectInfo* info)
{
    return Object::handle_t{FilesystemImpl::from(this).openObject(ObjectInfoImpl::from(info)).release()->asObject()};
}


Directory::handle_t Filesystem::openDirectory(const ObjectInfo* info)
{
    return Directory::handle_t{FilesystemImpl::from(this).openDirectory(ObjectInfoImpl::from(info)).release()->asDirectory()};
}


File::handle_t Filesystem::openFile(const ObjectInfo* info)
{
    return File::handle_t{FilesystemImpl::from(this).openFile(ObjectInfoImpl::from(info)).release()->asFile()};
}


std::uint32_t Filesystem::clusterToSector(std::uint32_t cluster) const
{
    return FilesystemImpl::from(this).clusterToSector(cluster);
}


std::uint32_t Filesystem::sectorsPerCluster() const
{
    return FilesystemImpl::from(this).sectorsPerCluster();
}


Filesystem* Filesystem::openImpl(const params_t* params)
{
    return reinterpret_cast<Filesystem*>(new FilesystemImpl{*params});
}


void Filesystem::deleteImpl(Filesystem* fs)
{
    delete reinterpret_cast<FilesystemImpl*>(fs);
}


FilesystemImpl::FilesystemImpl(const params_t& params) :
    m_params(params)
{
    // If we weren't given a read-for-write callback, synthesise one.
    if (!m_params.read_for_write)
        m_params.read_for_write = makeReadForWriteFunction();
}


FilesystemImpl::~FilesystemImpl()
{
}


bool FilesystemImpl::mount()
{
    // Read in the BPB of the filesystem.
    auto bs = std::reinterpret_pointer_cast<const bootsector>(m_params.read(0));
    if (!bs)
    {
        log(priority::error, "cannot mount - failed to read bootsector");
        return false;
    }

    // Ensure that this looks like a valid FAT filesystem before we try to mount it.
    if (!isFATFilesystem(*bs))
    {
        log(priority::error, "cannot mount - does not appear to be a valid FAT filesystem");
        return false;
    }

    // Extract various useful parameters from the BPB.
    const auto& bpb = bs->bpb;
    m_sectorSize = bpb.common.sector_size;
    m_sectorsPerCluster = bpb.common.sectors_per_cluster;
    m_clusterSize = m_sectorSize * m_sectorsPerCluster;
    m_fatStart = bpb.common.reserved_sectors;
    m_fatCount = bpb.common.fat_count;
    m_type = bpb.common.fat_type();
    
    // The sector count could be stored in a couple of different places.
    if (m_type == fat_type::FAT32 || bpb.common.sector_count == 0)
        m_sectorCount = bpb.v3_31.sector_count_32;
    else
        m_sectorCount = bpb.common.sector_count;

    // The other fields depend on the FAT type.
    if (m_type == fat_type::FAT32)
    {
        // FAT32.
        m_fatSize = bpb.fat32.sectors_per_fat_32;
        m_rootDirStart = bpb.fat32.root_dir_cluster;
        m_rootDirLength = 0;    // Determined later.

        m_extendedBPB = bpb.fat32.ext_signature;

        if (m_extendedBPB == extended_signature::VolumeSerial)
        {
            m_serial = bpb.fat32.volume_serial;
        }
        else if (m_extendedBPB == extended_signature::EBPB)
        {
            m_serial = bpb.fat32.volume_serial;
            m_name = bpb.fat32.volume_label;
            m_fsType = bpb.fat32.fs_type_label;
        }

        // The first data cluster comes after the FATs.
        m_firstCluster = m_fatStart + (m_fatSize * m_fatCount);
    }
    else
    {
        // FAT12 or FAT16.
        m_fatSize = bpb.common.sectors_per_fat;
        m_rootDirStart = m_fatStart + (m_fatCount * m_fatSize);
        m_rootDirLength = (bpb.common.max_root_dir_entries * sizeof(dirent_t)) / m_sectorSize;

        m_extendedBPB = bpb.ext.ext_signature;

        if (m_extendedBPB == extended_signature::VolumeSerial)
        {
            m_serial = bpb.ext.volume_serial;
        }
        else if (m_extendedBPB == extended_signature::EBPB)
        {
            m_serial = bpb.ext.volume_serial;
            m_name = bpb.ext.volume_label;
            m_fsType = bpb.ext.fs_type_label;
        }

        // The first data cluster comes after the root directory.
        m_firstCluster = m_rootDirStart + m_rootDirLength;
    }

    // Calculate the total number of data clusters.
    m_clusterCount = (m_sectorCount - m_firstCluster) / m_sectorsPerCluster;

    // Calculate the cluster mask to use when processing the FAT.
    switch (m_type)
    {
        case fat_type::FAT12:
            m_clusterMask = FAT12ClusterMask;
            break;

        case fat_type::FAT16:
            m_clusterMask = FAT16ClusterMask;
            break;

        case fat_type::FAT32:
            m_clusterMask = FAT32ClusterMask;
            break;
    }

    // Create the root directory object.
    m_rootDir = DirectoryImpl::open(ObjectInfoImpl{*this, ObjectInfoImpl::RootDirObjectInfo});

    log(priority::verbose, "mounted {} as FAT{} volume", id(), static_cast<int>(m_type));
    log(priority::debug, "FS info for {}:\n"
        "  sector size:             {}\n"
        "  sector count:            {}\n"
        "  FAT start sector:        {}\n"
        "  FAT size:                {}\n"
        "  FAT count:               {}\n"
        "  sectors per cluster:     {}\n"
        "  root dir start:          {}\n"
        "  root dir length:         {}\n"
        "  cluster size:            {}\n"
        "  first data sector:       {}\n"
        "  cluster count:           {}\n"
        "  cluster mask:            {:x}\n"
        "  FAT type:                {}",
        id(),
        m_sectorSize,
        m_sectorCount,
        m_fatStart,
        m_fatSize,
        m_fatCount,
        m_sectorsPerCluster,
        m_rootDirStart,
        m_rootDirLength,
        m_clusterSize,
        m_firstCluster,
        m_clusterCount,
        m_clusterMask,
        static_cast<int>(m_type)
    );
    return true;
}


DirectoryImpl* FilesystemImpl::rootDirectory()
{
    return m_rootDir.get();
}


std::uint32_t FilesystemImpl::sectorSize() const
{
    return m_sectorSize;
}


std::uint32_t FilesystemImpl::clusterSize() const
{
    return m_clusterSize;
}


std::uint32_t FilesystemImpl::sectorsPerCluster() const
{
    return m_sectorsPerCluster;
}


fat_type FilesystemImpl::fatType() const
{
    return m_type;
}


std::uint32_t FilesystemImpl::rootDirStartSector() const
{
    return m_rootDirStart;
}


std::uint32_t FilesystemImpl::rootDirSectorCount() const
{
    return m_rootDirLength;
}


std::uint32_t FilesystemImpl::rootDirFirstCluster() const
{
    return m_rootDirStart;
}


std::uint32_t FilesystemImpl::getFATEntry(std::uint32_t cluster) const
{
    // Only FAT32 needs extra masking here. As its mask is a superset of all the others, we can just apply the mask
    // unconditionally without affecting FAT12 and FAT16.
    return getFATEntryRaw(cluster) & FAT32ClusterMask;
}


std::uint32_t FilesystemImpl::getFATEntryRaw(std::uint32_t cluster) const
{
    //! @todo: validate that the cluster number is valid for the FAT size.

    // How we read the FAT depends on the type of FAT.
    if (m_type == fat_type::FAT12)
    {
        // The entries in a FAT12 are not whole bytes so special care is needed when reading them. In particular,
        // entries can span two sectors if they fall at the end of a FAT sector.

        // Calculate the byte offset within the FAT of the 16-bit word containing the 12-bit entry we want.
        auto byte_offset = cluster + (cluster / 2);     // Multiply by 1.5, rounding down.
        auto sector = byte_offset / m_sectorSize;
        auto offset = byte_offset % m_sectorSize;

        // Special handling for an entry that spans a sector.
        std::uint16_t entry;
        if (offset == m_sectorSize - 1) [[unlikely]]
        {
            // We need to read in two sectors of the FAT to get this entry.
            auto first = readBlock(m_fatStart + sector);
            auto second = readBlock(m_fatStart + sector + 1);

            // Read the two bytes of interest in little-endian order.
            auto low = static_cast<std::uint16_t>(first.get()[m_sectorSize - 1]);
            auto high = static_cast<std::uint16_t>(second.get()[0]);

            entry = low | (high << 8);
        }
        else
        {
            // Read the sector.
            auto data = readBlock(m_fatStart + sector);

            // Perform an unaligned 16-bit read to get the two bytes of interest.
            using Utility::Endian::uint16ule_t;
            entry = *reinterpret_cast<const uint16ule_t*>(&data.get()[offset]);
        }

        // Extract the 12 bits of interest (they're the low bits or the high bits, respectively, if the cluster number
        // is even or odd).
        if (cluster & 1)
            entry >>= 4;
        else
            entry &= 0x0FFF;

        return entry;
    }
    else if (m_type == fat_type::FAT16)
    {
        // Calculate which sector and the offset within that sector to read.
        auto entries_per_sector = m_sectorSize / sizeof(std::uint16_t);
        auto fat_sector = cluster / entries_per_sector;
        auto offset = cluster % entries_per_sector;

        // Read that sector.
        auto data = readBlock(m_fatStart + fat_sector);

        // And then read the entry within that sector.
        return (reinterpret_cast<const uint16le_t*>(data.get()))[offset];
    }
    else // if (m_type == fat_type::FAT32)
    {
        // Calculate which sector and the offset within that sector to read.
        auto entries_per_sector = m_sectorSize / sizeof(std::uint32_t);
        auto fat_sector = cluster / entries_per_sector;
        auto offset = cluster % entries_per_sector;

        // Read that sector.
        auto data = readBlock(m_fatStart + fat_sector);

        // And then read the entry within that sector.
        return (reinterpret_cast<const uint32le_t*>(data.get()))[offset];
    }
}


bool FilesystemImpl::isEndOfChain(std::uint32_t cluster) const
{
    return (cluster & m_clusterMask) >= (EndOfChain & m_clusterMask);
}


bool FilesystemImpl::isBadClusterMark(std::uint32_t cluster) const
{
    return (cluster & m_clusterMask) == (BadClusterMark & m_clusterMask);
}


bool FilesystemImpl::isFreeClusterMark(std::uint32_t cluster) const
{
    return (cluster & m_clusterMask) == (FreeClusterMark & m_clusterMask);
}


void FilesystemImpl::readClusterChain(std::uint32_t cluster, cluster_chain_callback_t callback) const
{
    // We want to invoke the callback with the first cluster value, too.
    std::uint32_t index = 0;
    while (true)
    {
        // Index into the FAT using the current cluster number. The value found there will give us the next cluster (or
        // the end of the chain or even a bad sector mark).
        //
        // Because of the possibility of finding a bad cluster mark, we want to do this lookup before invoking the
        // callback as its important to know if the cluster is bad before trying to read it.
        auto next = getFATEntry(cluster);
        if (isBadClusterMark(next))
        {
            // We won't be able to continue following the chain and there's not much point trying to read this cluster.
            log(priority::error, "found bad cluster in chain");
            return;
        }
        else if (isFreeClusterMark(next))
        {
            // Oh dear... something has gone badly wrong!
            log(priority::error, "found free cluster in chain");
            return;
        }

        // Invoke the callback.
        auto more = callback(index, cluster);
        if (!more)
            break;

        // Have we reached the end of the cluster chain?
        if (isEndOfChain(next))
        {
            // No more clusters.
            return;
        }

        // Move to the next cluster.
        cluster = next;
        ++index;
    }
}


std::uint32_t FilesystemImpl::nthChainEntry(std::uint32_t cluster, std::uint32_t n) const
{
    std::uint32_t entry = 0;
    readClusterChain(cluster, [n, &entry](std::uint32_t index, std::uint32_t value)
    {
        if (index == n)
        {
            entry = value;
            return false;
        }

        return true;
    });

    return entry;
}


std::uint32_t FilesystemImpl::clusterToSector(std::uint32_t cluster) const
{
    // Note that cluster indexing is 2-based while sector indexing is 0-based.
    return m_firstCluster + (cluster - 2) * m_sectorsPerCluster;
}


std::uint32_t FilesystemImpl::findFreeCluster(bool mark_as_allocated)
{
    // Linear scan of the FAT looking for free clusters.
    //! @todo - improvements:
    //!     - remember where the free clusters are to avoid scanning everything each time
    //!     - don't use getFATEntryRaw; re-implement the scan here.

    for (std::uint32_t cluster = 2; cluster < m_clusterCount; ++cluster)
    {
        if (isFreeClusterMark(getFATEntryRaw(cluster)))
        {
            if (mark_as_allocated)
            {
                log(priority::trace, "allocated cluster {}", cluster);
                writeClusterChainLink(cluster, EndOfChain & m_clusterMask);
            }

            return cluster;
        }
    }

    // No free clusters found.
    return 0;
}


void FilesystemImpl::writeClusterChainLink(std::uint32_t from, std::uint32_t to)
{
    //! @todo: validate that the cluster number is valid for the FAT size.

    log(priority::trace, "writing chain {} -> {}", from, to);

    // How we write the FAT depends on the type of FAT.
    if (m_type == fat_type::FAT12)
    {
        // The entries in a FAT12 are not whole bytes so special care is needed when reading them. In particular,
        // entries can span two sectors if they fall at the end of a FAT sector.

        // Calculate the byte offset within the FAT of the 16-bit word containing the 12-bit entry we want.
        auto byte_offset = from + (from / 2);       // Multiply by 1.5, rounding down.
        auto sector = byte_offset / m_sectorSize;
        auto offset = byte_offset % m_sectorSize;

        // Special handling for an entry that spans a sector.
        if (offset == m_sectorSize - 1) [[unlikely]]
        {
            // We need to read in two sectors of the FAT to get this entry.
            auto first = readBlockForUpdate(m_fatStart + sector);
            auto second = readBlockForUpdate(m_fatStart + sector + 1);

            // Read the two bytes of interest in little-endian order.
            auto low = static_cast<std::uint8_t>(first.get()[m_sectorSize - 1]);
            auto high = static_cast<std::uint8_t>(second.get()[0]);

            std::uint16_t value = low | (high << 8);

            // Update the appropriate 12 bits of the value.
            if (from & 1)
                value = static_cast<std::uint16_t>((value & 0x000F) | (to << 4));
            else
                value = static_cast<std::uint16_t>((value & 0xF000) | (to & 0x0FFF));

            // Write the value back.
            low = value & 0xFF;
            high = value >> 8;
            first.get()[m_sectorSize - 1] = static_cast<std::byte>(low);
            second.get()[0] = static_cast<std::byte>(high);   
        }
        else
        {
            // Read the sector.
            auto data = readBlockForUpdate(m_fatStart + sector);

            // Perform an unaligned 16-bit read to get the two bytes of interest.
            using Utility::Endian::uint16ule_t;
            auto& entry = *reinterpret_cast<uint16ule_t*>(&data.get()[offset]);
            std::uint16_t value = entry;

            // Update the appropriate 12 bits of the value.
            if (from & 1)
                value = static_cast<std::uint16_t>((value & 0x000F) | (to << 4));
            else
                value = static_cast<std::uint16_t>((value & 0xF000) | (to & 0x0FFF));

            // Write the value back.
            entry = value;
        }
    }
    else if (m_type == fat_type::FAT16)
    {
        // Calculate which sector and the offset within that sector to read.
        auto entries_per_sector = m_sectorSize / sizeof(std::uint16_t);
        auto fat_sector = from / entries_per_sector;
        auto offset = from % entries_per_sector;

        // Read that sector.
        auto data = readBlockForUpdate(m_fatStart + fat_sector);

        // And then update the entry within that sector.
        reinterpret_cast<uint16le_t*>(data.get())[offset] = static_cast<std::uint16_t>(to);
    }
    else // if (m_type == fat_type::FAT32)
    {
        // Calculate which sector and the offset within that sector to read.
        auto entries_per_sector = m_sectorSize / sizeof(std::uint32_t);
        auto fat_sector = from / entries_per_sector;
        auto offset = from % entries_per_sector;

        // Read that sector.
        auto data = readBlockForUpdate(m_fatStart + fat_sector);

        // And then update the entry within that sector. We need to preserve the top bits though.
        auto& entry = reinterpret_cast<uint32le_t*>(data.get())[offset];
        entry = (entry & ~m_clusterMask) | (to & m_clusterMask);
    }
}


void FilesystemImpl::writeClusterChain(std::uint32_t from, std::span<std::uint32_t> chain)
{
    // Write each link in the chain.
    for (auto to : chain)
    {
        writeClusterChainLink(from, to);
        from = to;
    }
}


void FilesystemImpl::freeClusterChain(std::uint32_t from)
{
    // Get each link in the chain and update it to mark the cluster as free.
    while (true)
    {
        auto next = getFATEntry(from);
        writeClusterChainLink(from, FreeClusterMark & m_clusterMask);
        log(priority::trace, "freed cluster {}", from);

        if (isEndOfChain(next))
            break;

        from = next;
    }
}


std::shared_ptr<const std::byte> FilesystemImpl::readBlock(std::uint32_t lba) const
{
    return m_params.read(lba);
}


std::shared_ptr<std::byte> FilesystemImpl::readBlockForUpdate(std::uint32_t lba)
{
    return m_params.read_for_write(lba);
}


bool FilesystemImpl::writeBlock(std::uint32_t lba, std::span<const std::byte> data)
{
    return m_params.write(lba, 1, data);
}


std::string FilesystemImpl::convertFromFilesystemCharset(std::string_view name) const
{
    //! @todo do this properly.
    return std::string{name};
}


std::string FilesystemImpl::convertToFilesystemCharset(std::string_view name) const
{
    //! @todo do this properly.
    return std::string{name};
}


std::string FilesystemImpl::convertFromUTF16(std::u16string_view name) const
{
    //! @todo do this properly.
    return std::string{name.begin(), name.end()};
}


std::u16string FilesystemImpl::convertToUTF16(std::string_view name) const
{
    //! @todo do this properly.
    return std::u16string{name.begin(), name.end()};
}


std::unique_ptr<ObjectImpl> FilesystemImpl::openObject(const ObjectInfoImpl& info)
{
    return ObjectImpl::open(info);
}


std::unique_ptr<DirectoryImpl> FilesystemImpl::openDirectory(const ObjectInfoImpl& info)
{
    return DirectoryImpl::open(info);
}


std::unique_ptr<FileImpl> FilesystemImpl::openFile(const ObjectInfoImpl& info)
{
    return FileImpl::open(info);
}


bool FilesystemImpl::isFATFilesystem(const bootsector& bootsector)
{
    // There are no magic numbers or checksums that can definitively identify a FAT filesystem. Instead, we have to use
    // a set of heuristics that check if it looks enough like a FAT filesystem.

    // First check: do the initial bytes look like a valid x86 jmp sequence?
    //
    // This check doesn't really have anything to do with FAT but Microsoft products seem to make use of it and it
    // serves as sort-of a magic number. At least well enough to rule out some non-FAT filesystems.
    bool valid_jmp = false;
    if (bootsector.boot_jump[0] == std::byte{0xEB} && bootsector.boot_jump[2] == std::byte{0x90})
    {
        // 8-bit jump then a nop.
        valid_jmp = true;
    }
    else if (bootsector.boot_jump[0] == std::byte{0xE9})
    {
        // 16-bit jump.
        valid_jmp = true;
    }

    if (!valid_jmp)
    {
        log(priority::debug, "not a valid FAT filesystem: invalid jmp sequence {} {} {}",
            std::to_integer<int>(bootsector.boot_jump[0]),
            std::to_integer<int>(bootsector.boot_jump[1]),
            std::to_integer<int>(bootsector.boot_jump[2])
        );

        return false;
    }

    // Second check: is this an ExFAT or NTFS filesystem?
    //
    // Normally, the OEM ID shouldn't be used for anything but for those two filesystems, it must explicitly be set to
    // a particular value.
    if (bootsector.oem_signature == BPBSignatureExFAT)
    {
        log(priority::debug, "not a valid FAT filesystem: has ExFAT OEM signature");
        return false;
    }
    else if (bootsector.oem_signature == BPBSignatureNTFS)
    {
        log(priority::debug, "not a valid FAT filesystem: has NTFS OEM signature");
        return false;
    }

    // Now we start checking the contents of the BPB.

    // Does the sector size match what we expect?
    const auto& bpb = bootsector.bpb;
    std::uint16_t sector_size = bpb.common.sector_size;
    if (!std::has_single_bit(sector_size)
        || sector_size < 512 
        || sector_size > 4096)
    {
        log(priority::debug, "not a valid FAT filesystem: sector size {} is invalid", sector_size);
        return false;
    }
    
    // Is the cluster size valid?
    std::uint8_t sectors_per_cluster = bpb.common.sectors_per_cluster;
    if (!std::has_single_bit(sectors_per_cluster)
        || sectors_per_cluster < 1
        || sectors_per_cluster > 128)
    {
        log(priority::debug, "not a valid FAT filesystem: sectors-per-cluster {} is invalid", sectors_per_cluster);
        return false;
    }
    if (sector_size * sectors_per_cluster > 65536)
    {
        log(priority::debug, "not a valid FAT filesystem: cluster size {} too large", sector_size * sectors_per_cluster);
        return false;
    }

    // Is this BPB properly covered by the reserved sector count?
    std::uint16_t reserved_sectors = bpb.common.reserved_sectors;
    if (reserved_sectors < 1)
    {
        log(priority::debug, "not a valid FAT filesystem: reserved sector count {} invalid", reserved_sectors);
        return false;
    }

    // Do we have a valid number of FATs?
    std::uint8_t fat_count = bpb.common.fat_count;
    if (fat_count < 1)
    {
        log(priority::debug, "not a valid FAT filesystem: FAT count {} invalid", fat_count);
        return false;
    }

    // Do we have a valid media type byte?
    media_descriptor medium_type = bpb.common.medium_type;
    if (!IsValidMediaType(medium_type))
    {
        log(priority::debug, "not a valid FAT filesystem: medium type {} invalid", static_cast<std::underlying_type_t<decltype(medium_type)>>(medium_type));
        return false;
    }

    // All checks passed. This is (probably...) a FAT filesystem.
    return true;
}


std::string FilesystemImpl::id() const
{
    auto name = std::string_view{m_name.data(), m_name.size()};
    if (m_extendedBPB == extended_signature::EBPB && name != "NO NAME    ")
        return std::string{name};

    if (m_extendedBPB == extended_signature::EBPB || m_extendedBPB == extended_signature::VolumeSerial)
        return format("{:X}-{:X}", (m_serial >> 16), (m_serial & 0xFFFF));

    return "<no name>";
}


Filesystem::read_for_write_fn FilesystemImpl::makeReadForWriteFunction()
{
    return [this](std::uint64_t lba) -> std::shared_ptr<std::byte>
    {
        // Read in the existing contents.
        auto existing = m_params.read(lba);
        if (!existing)
            return nullptr;

        // Custom deleter that writes out the sector on destruction.
        struct deleter_t
        {
            void operator()(std::byte* ptr)
            {
                fs->m_params.write(lba, 1, std::span(ptr, fs->sectorSize()));
            }

            FilesystemImpl*     fs = nullptr;
            std::uint64_t       lba = 0;
        };

        // Make a copy.
        std::shared_ptr<std::byte> copy(new std::byte[sectorSize()], deleter_t{.fs = this, .lba = lba});
        std::memcpy(copy.get(), existing.get(), sectorSize());

        return copy;
    };
}


} // namespace System::Filesystem::FAT
