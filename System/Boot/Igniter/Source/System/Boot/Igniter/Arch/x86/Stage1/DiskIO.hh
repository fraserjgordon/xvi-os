#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_DISKIO_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_DISKIO_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Span.hh>
#include <System/Storage/Partition/MBR.hh>


#include <System/Boot/Igniter/CHS.hh>


namespace System::Boot::Igniter
{


class Disk
{
public:

    constexpr Disk() = default;
    constexpr Disk(const Disk&) = default;

    explicit Disk(std::uint8_t drive);

    std::uint32_t read(std::uint64_t lba, std::uint32_t count, std::span<std::byte> buffer);

    // Call this if the medium in the drive has a different geometry to the drive itself (e.g. a 720kB floppy disk in a
    // 1.44MB drive).
    void setMediumGeometry(chs_geometry geometry);

private:

    static constexpr auto ReadRetryCount = 3;


    std::uint8_t        m_drive = 0;
    std::uint8_t        m_extensions = 0;
    std::uint32_t       m_sectorSize = 512;

    std::optional<chs_geometry>     m_geometry = {};


    static std::uint8_t getDiskExtensionsVersion(std::uint8_t drive);

    static chs_geometry getGeometry(std::uint8_t drive);

    static bool resetDisk(std::uint8_t drive);

    static bool readDiskCHS(std::uint8_t drive, const chs_address& sector, std::uint32_t count, std::span<std::byte> buffer);

    static bool readDiskLBA(std::uint8_t drive, std::uint64_t lba, std::uint32_t count, std::span<std::byte> buffer);
};


class Partition
{
public:

    constexpr Partition() = default;

    constexpr Partition(const Disk& disk, std::uint64_t first_sector, std::uint64_t length) :
        m_disk(disk),
        m_firstSector(first_sector),
        m_length(length)
    {
    }

    const Disk& disk() const
    {
        return m_disk;
    }

    std::uint64_t firstSector() const
    {
        return m_firstSector;
    }

    std::uint64_t length() const
    {
        return m_length;
    }

    std::uint32_t read(std::uint64_t lba, std::uint32_t count, std::span<std::byte> buffer);

private:

    Disk            m_disk = {};
    std::uint64_t   m_firstSector = 0;
    std::uint64_t   m_length = 0;
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_DISKIO_H */
