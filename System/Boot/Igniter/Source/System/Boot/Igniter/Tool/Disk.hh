#ifndef __SYSTEM_BOOT_IGNITER_TOOL_DISK_H
#define __SYSTEM_BOOT_IGNITER_TOOL_DISK_H


#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>


namespace System::Boot::Igniter
{


class Disk
{
public:

    Disk(const std::filesystem::path& path);

    std::size_t sectorSize() const;

    void readSector(std::uint64_t index, std::span<std::byte> buffer) const;

    void writeSector(std::uint64_t index, std::span<const std::byte> buffer);

private:

    mutable std::fstream    m_file;
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_TOOL_DISK_H */
