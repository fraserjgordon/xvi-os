#include <System/Filesystem/FAT/BPB.hh>


namespace System::Filesystem::FAT
{


fat_type bpb_common::fat_type() const
{
    // Get the total number of sectors that the filesystem has. This could be stored in a couple of different locations
    // so check both.
    std::uint32_t sectors = sector_count;
    if (sectors == 0)
        sectors = static_cast<const bpb_v3_31*>(this)->sector_count_32;

    // Convert the sector count to a cluster count as that determines the FAT type.
    std::uint32_t clusters = sectors / sectors_per_cluster;

    // Based on this, we can calculate the expected FAT type.
    FAT::fat_type expected_type = fat_type::Unknown;
    if (clusters < MaxClustersFAT12)
        expected_type = fat_type::FAT12;
    else if (clusters < MaxClustersFAT16)
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
