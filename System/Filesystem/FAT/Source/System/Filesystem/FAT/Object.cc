#include <System/Filesystem/FAT/Object.hh>
#include <System/Filesystem/FAT/ObjectImpl.hh>

#include <System/Filesystem/FAT/Filesystem.hh>


namespace System::Filesystem::FAT
{


void ObjectImpl::readBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t callback)
{
    auto& fs = info().filesystem();

    // Clamp the length (and therefore the ending offset) to the size of the object.
    auto size = info().size();
    auto end = std::min(offset + length, size);

    // Check that the offset and end are within bounds.
    if (offset >= end)
        return;

    // Round the offset to a block boundary and re-adjust the length to compensate.
    auto sector_size = fs.sectorSize();
    auto adjustment = -(offset & (sector_size - 1));
    offset = offset - adjustment;
    length = end - offset;

    // Now that we've aligned everything to sectors, check for and handle root directory reads specially for FAT12 and FAT16.
    if (info().type() == ObjectType::RootDirectory && (fs.fatType() == fat_type::FAT12 || fs.fatType() == fat_type::FAT16))
        return readRootDirBlocks(offset, length, callback);

    // How many sectors are we going to read?
    auto first_sector = offset / sector_size;
    auto sector_count = length / sector_size;
    auto last_sector = first_sector + sector_count;

    // What is the index of the first and last clusters that we are interested in?
    auto sectors_per_cluster = fs.sectorsPerCluster();
    auto first_cluster_index = offset / sectors_per_cluster;
    auto cluster_count = (sector_count + sectors_per_cluster - 1) / sectors_per_cluster;
    auto last_cluster_index = first_cluster_index + cluster_count;

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
            auto result = callback((sector_index + i) * sector_size, std::span{data.get(), sector_size});
            if (!result)
                return false;
        }

        // Continue following the chain.
        return true;
    });
}


void ObjectImpl::readRootDirBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t callback)
{
    // With FAT12 and FAT16, the root directory is a pre-allocated contiguous area before the first data cluster. We can
    // just read it as requested.
    auto& fs = info().filesystem();
    auto sector_size = fs.sectorSize();
    auto sector_count = length / sector_size;
    auto start_sector = fs.rootDirStartSector() + (offset / sector_size);
    auto end_sector = start_sector + sector_count;

    for (std::uint32_t i = 0; i < sector_count; ++i)
    {
        auto data =fs.readBlock(start_sector + i);
        auto result = callback(offset + (i * sector_size), std::span{data.get(), sector_size});
        if (!result)
            return;
    }
}


} // namespace System::Filesystem::FAT
