#ifndef __SYSTEM_BOOT_IGNITER_TOOL_DISK_H
#define __SYSTEM_BOOT_IGNITER_TOOL_DISK_H


#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <span>

#include <System/IO/FileIO/FileHandle.hh>
#include <System/Storage/VirtualDisk/RawVirtualDisk.hh>

#include <System/Boot/Igniter/CHS.hh>


namespace System::Boot::Igniter
{


class Disk
{
public:

    struct options
    {
    };


    Disk(const std::filesystem::path& path, const options& = {});

    std::size_t sectorSize() const;

    void readSector(std::uint64_t index, std::span<std::byte> buffer) const;

    void writeSector(std::uint64_t index, std::span<const std::byte> buffer);

    const std::optional<chs_geometry>& geometry() const;

    System::Storage::BlockDevice::BlockDev& blockDevice() noexcept
    {
        return m_virt;
    }

private:

    const std::filesystem::path             m_path;
    Storage::VirtualDisk::RawVirtualDisk    m_virt;

    std::size_t                 m_sectorSize;

    // Disk geometry, for disks where that concept makes sense (mostly floppy disks).
    std::optional<chs_geometry> m_geometry;


    void inferDiskGeometry(const options&);
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_TOOL_DISK_H */
