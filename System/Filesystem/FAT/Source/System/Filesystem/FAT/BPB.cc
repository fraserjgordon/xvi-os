#include <System/Filesystem/FAT/BPB.hh>


namespace System::Filesystem::FAT
{


bool IsFloppyMediaType(media_descriptor type)
{
    return IsValidMediaType(type) && type != media_descriptor::FixedDisk;
}


bool IsValidMediaType(media_descriptor type)
{
    switch (static_cast<std::underlying_type_t<media_descriptor>>(type))
    {
        // Only particular values are valid.
        case 0xF0:
        case 0xF8:
        case 0xF9:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF:
            return true;

        default:
            return false;
    }
}


media_descriptor_geometry GetExpectedGeometry(media_descriptor type)
{
    //! @todo implement more of these.
    switch (type)
    {
        case media_descriptor::Floppy35_HighDensity:
            return {2, 80, 18};

        default:
            return {0, 0, 0};
    }
}


fat_type bpb_common::fat_type() const
{
    // Get the total number of sectors that the filesystem has. This could be stored in a couple of different locations
    // so check both.
    std::uint32_t sectors = sector_count;
    if (sectors == 0)
        sectors = static_cast<const bpb_v3_31*>(this)->sector_count_32;

    // Calculate how many of those sectors are used for data (as only that matters for FAT type).
    std::uint32_t root_dir_sectors = ((max_root_dir_entries * 32) + (sector_size - 1)) / sector_size;
    std::uint32_t data_sectors = sectors - (reserved_sectors + (fat_count * sectors_per_fat) + root_dir_sectors);

    // Convert the sector count to a cluster count as that determines the FAT type.
    std::uint32_t clusters = data_sectors / sectors_per_cluster;

    // Based on this, we can calculate the expected FAT type.
    FAT::fat_type expected_type = fat_type::Unknown;
    if (clusters <= MaxClustersFAT12)
        expected_type = fat_type::FAT12;
    else if (clusters <= MaxClustersFAT16)
        expected_type = fat_type::FAT16;
    else
        expected_type = fat_type::FAT32;

    // If the number of sectors per FAT is zero, this is a FAT32 volume even if we expect otherwise.
    FAT::fat_type actual_type = expected_type;
    if (sectors_per_fat == 0 && expected_type != fat_type::FAT32)
        actual_type = fat_type::FAT32;

    //! @TODO: any other clues that the FAT type might be other than expected? In particular, is there a way to reliably
    //!        distinguish a normal FAT12 filesystem from an undersized FAT16 filesystem?

    //! @TODO: diagnostic if actual != expected.
    if (expected_type != actual_type)
    {
        // ...
    }

    return actual_type;
}


} // namespace System::Filesystem::FAT
