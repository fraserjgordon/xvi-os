#ifndef __SYSTEM_STORAGE_BLOCKDEVICE_FALLBACKBLOCKCACHE_H
#define __SYSTEM_STORAGE_BLOCKDEVICE_FALLBACKBLOCKCACHE_H


#include <System/Storage/BlockDevice/BlockCache.hh>


namespace System::Storage::BlockDevice
{


class FallbackBlockCache final :
    public BlockCache
{
public:

    // Inherited from BlockCache.
    result_t<std::size_t> read(BlockDev*, std::uint64_t lba, std::span<BlockHandle> out) noexcept override;
    result_t<std::size_t> readForUpdate(BlockDev*, std::uint64_t lba, std::span<BlockHandle> out) noexcept override;
    result_t<std::size_t> allocateEmpty(BlockDev*, std::uint64_t lba, std::span<BlockHandle> out) noexcept override;
    result_t<void> readCopy(BlockDev*, std::uint64_t lba, std::uint64_t offset, std::span<std::byte> buffer) noexcept override;
    result_t<void> write(BlockDev*, std::uint64_t lba, std::uint64_t offset, std::span<const std::byte> buffer) noexcept override;

protected:

    // Inherited from BlockCache.
    void ref(const BlockHandle*) noexcept override;
    void unref(const BlockHandle*) noexcept override;

private:

    struct BlockInfo;
};


} // namespace System::Storage::BlockDevice


#endif /* ifndef __SYSTEM_STORAGE_BLOCKDEVICE_FALLBACKBLOCKCACHE_H */
