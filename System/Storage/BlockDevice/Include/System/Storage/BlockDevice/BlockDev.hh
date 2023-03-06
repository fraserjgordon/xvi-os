#ifndef __SYSTEM_STORAGE_BLOCKDEVICE_BLOCKDEV_H
#define __SYSTEM_STORAGE_BLOCKDEVICE_BLOCKDEV_H


#include <cstdint>
#include <expected>
#include <system_error>

#include <System/Storage/BlockDevice/BlockCache.hh>


namespace System::Storage::BlockDevice
{


class BlockDev
{
public:

    using lba_t = std::uint64_t;
    using buffer = std::span<std::byte>;
    using const_buffer = std::span<const std::byte>;

    using BlockHandle = Storage::BlockDevice::BlockHandle;

    template <class T> using result_t = std::expected<T, std::error_code>;


    virtual ~BlockDev() = default;

    virtual BlockCache* cache() const noexcept;

    result_t<std::size_t> cachedRead(lba_t, std::span<BlockHandle> out) const noexcept;
    result_t<BlockHandle> cachedRead(lba_t) const noexcept;
    result_t<std::size_t> cachedReadForUpdate(lba_t, std::span<BlockHandle> out) noexcept;
    result_t<BlockHandle> cachedReadForUpdate(lba_t) noexcept;

    result_t<void> cachedRead(lba_t, std::uint64_t offset, std::span<std::byte>) const noexcept;
    result_t<void> cachedWrite(lba_t, std::uint64_t offset, std::span<const std::byte>) noexcept;

    virtual std::size_t sectorSize() const noexcept = 0;
    virtual lba_t sectorCount() const noexcept = 0;

    virtual result_t<void> read(lba_t, buffer) const noexcept = 0;
    virtual result_t<void> write(lba_t, const_buffer) noexcept = 0;
};


} // namespace System::Storage::BlockDevice


#endif /* ifndef __SYSTEM_STORAGE_BLOCKDEVICE_BLOCKDEV_H */
