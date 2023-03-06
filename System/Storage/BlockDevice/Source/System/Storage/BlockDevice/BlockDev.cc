#include <System/Storage/BlockDevice/BlockDev.hh>


namespace System::Storage::BlockDevice
{


BlockCache* BlockDev::cache() const noexcept
{
    return BlockCache::fallbackCache();
}


BlockDev::result_t<std::size_t> BlockDev::cachedRead(lba_t lba, std::span<BlockHandle> out) const noexcept
{
    return cache()->read(const_cast<BlockDev*>(this), lba, out);
}


BlockDev::result_t<BlockHandle> BlockDev::cachedRead(lba_t lba) const noexcept
{
    return cache()->read(const_cast<BlockDev*>(this), lba);
}


BlockDev::result_t<std::size_t> BlockDev::cachedReadForUpdate(lba_t lba, std::span<BlockHandle> out) noexcept
{
    return cache()->readForUpdate(this, lba, out);
}


BlockDev::result_t<BlockHandle> BlockDev::cachedReadForUpdate(lba_t lba) noexcept
{
    return cache()->readForUpdate(this, lba);
}


BlockDev::result_t<void> BlockDev::cachedRead(lba_t lba, std::uint64_t offset, std::span<std::byte> buffer) const noexcept
{
    return cache()->readCopy(const_cast<BlockDev*>(this), lba, offset, buffer);
}


BlockDev::result_t<void> BlockDev::cachedWrite(lba_t lba, std::uint64_t offset, std::span<const std::byte> buffer) noexcept
{
    return cache()->write(this, lba, offset, buffer);
}


} // namespace System::Storage::BlockDevice
