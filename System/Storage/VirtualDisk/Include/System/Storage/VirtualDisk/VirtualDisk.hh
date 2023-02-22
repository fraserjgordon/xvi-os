#ifndef __SYSTEM_STORAGE_VIRTUALDISK_VIRTUALDISK_H
#define __SYSTEM_STORAGE_VIRTUALDISK_VIRTUALDISK_H


#include <cstddef>
#include <cstdint>
#include <span>

#include <System/IO/FileIO/Types.hh>
#include <System/Storage/BlockDevice/BlockDev.hh>


namespace System::Storage::VirtualDisk
{


class VirtualDisk :
    public System::Storage::BlockDevice::BlockDev
{
public:

    using buffer = std::span<std::byte>;
    using const_buffer = std::span<const std::byte>;

    template <class T> using result = System::IO::FileIO::result<T>;


    virtual std::uint32_t sectorSize() const noexcept;

    virtual lba_t sectorCount() const noexcept = 0;

    virtual result<void> read(lba_t lba, buffer output) const noexcept = 0;

    virtual result<void> write(lba_t lba, const_buffer input) noexcept = 0;
};


} // namespace System::Storage::VirtualDisk


#endif /* ifndef __SYSTEM_STORAGE_VIRTUALDISK_VIRTUALDISK_H */
