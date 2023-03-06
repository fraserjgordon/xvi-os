#include <System/Storage/BlockDevice/BlockCache.hh>

#include <cstring>

#include <System/Storage/BlockDevice/BlockDev.hh>
#include <System/Storage/BlockDevice/FallbackBlockCache.hh>


namespace System::Storage::BlockDevice
{


static FallbackBlockCache s_fallbackBlockCache = {};


BlockCache::result_t<void> BlockCache::readCopy(BlockDev* device, std::uint64_t lba, std::uint64_t offset, std::span<std::byte> buffer) noexcept
{
    // Check that the read request lies entirely inside a single block.
    auto sector_size = device->sectorSize();
    if (offset > sector_size || buffer.size() > sector_size || (offset + buffer.size()) > sector_size)
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));

    // Perform a read of the sector in the normal way.
    auto result = read(device, lba);
    if (!result)
        return std::unexpected(result.error());

    // Copy the block data into the buffer.
    auto span = result->readSpan();
    std::memcpy(buffer.data(), span.data() + offset, buffer.size());

    return {};
}


BlockCache::result_t<void> BlockCache::write(BlockDev* device, std::uint64_t lba, std::uint64_t offset, std::span<const std::byte> buffer) noexcept
{
    // Check that the write request lies entirely inside a single block.
    auto sector_size = device->sectorSize();
    if (offset > sector_size || buffer.size() > sector_size || (offset + buffer.size()) > sector_size)
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));

    // If we're overwriting the entire block, don't bother fetching the existing contents.
    result_t<BlockHandle> result;
    if (offset == 0 && buffer.size() == sector_size)
        result = allocateEmpty(device, lba);
    else
        result = readForUpdate(device, lba);

    if (!result)
        return std::unexpected(result.error());

    // Copy the data into the block.
    auto span = result->writeSpan();
    std::memcpy(span.data() + offset, buffer.data(), buffer.size());

    return {};
}


BlockCache* BlockCache::fallbackCache() noexcept
{
    return &s_fallbackBlockCache;
}


} // namespace System::Storage::BlockDevice
