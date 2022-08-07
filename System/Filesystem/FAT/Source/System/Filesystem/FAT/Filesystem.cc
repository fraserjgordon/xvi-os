#include <System/Filesystem/FAT/Filesystem.hh>

#if !defined(__XVI_NO_STDLIB)
#  include <bit>
#  if __has_include(<format>)
#    include <format>
#  else
#    include <fmt/format.h>
#  endif
#else
#  include <System/C++/Format/Format.hh>
#  include <System/C++/Utility/Bit.hh>
#endif

#include <System/Filesystem/FAT/Logging.hh>


#if defined(__XVI_NO_STDLIB) || __has_include(<format>)
using std::format;
#else
using fmt::format;
#endif


namespace System::Filesystem::FAT
{


Filesystem::Filesystem(const params_t& params) :
    m_params(params)
{
}


Filesystem::~Filesystem()
{
}


bool Filesystem::mount()
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
    }
    else
    {
        // FAT12 or FAT16.
        m_fatSize = bpb.common.sectors_per_fat;
        m_rootDirStart = m_fatStart + (m_fatCount * m_fatSize);
        m_rootDirLength = bpb.common.max_root_dir_entries / m_sectorSize;

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
    }

    log(priority::verbose, "mounted {} as FAT{} volume", id(), static_cast<int>(m_type));
    return true;
}


bool Filesystem::isFATFilesystem(const bootsector& bootsector)
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


std::string Filesystem::id() const
{
    auto name = std::string_view{m_name.data(), m_name.size()};
    if (m_extendedBPB == extended_signature::EBPB && name != "NO NAME    ")
        return std::string{name};

    if (m_extendedBPB == extended_signature::EBPB || m_extendedBPB == extended_signature::VolumeSerial)
        return format("{:X}-{:X}", (m_serial >> 16), (m_serial & 0xFFFF));

    return "<no name>";
}


} // namespace System::Filesystem::FAT
