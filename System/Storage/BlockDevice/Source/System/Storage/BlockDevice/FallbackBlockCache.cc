#include <System/Storage/BlockDevice/FallbackBlockCache.hh>


#include <atomic>
#include <memory>
#include <new>
#include <stdexcept>

#include <System/Storage/BlockDevice/BlockDev.hh>


namespace System::Storage::BlockDevice
{


struct FallbackBlockCache::BlockInfo
{
    std::atomic<std::size_t>        m_refcount = 0;
    std::unique_ptr<std::byte[]>    m_buffer = nullptr;
    BlockDev*                       m_device = nullptr;
    std::uint64_t                   m_lba = 0;
    bool                            m_writeback = false;

    static BlockInfo* get(const BlockHandle& handle) noexcept
    {
        return static_cast<BlockInfo*>(handlePtr(handle));
    }
};


FallbackBlockCache::result_t<std::size_t> FallbackBlockCache::read(BlockDev* device, std::uint64_t lba, std::span<BlockHandle> out) noexcept
{
    auto sector_size = device->sectorSize();
    std::error_code error {};

    std::size_t i = 0;
    for (; i < out.size(); ++i)
    {
        // Allocate a buffer for the block.
        std::unique_ptr<std::byte[]> buffer(new (std::nothrow) std::byte[sector_size]);
        if (!buffer)
        {
            error = std::make_error_code(std::errc::not_enough_memory);
            break;
        }

        // Read from the device into the buffer.
        auto span = std::span{buffer.get(), sector_size};
        auto result = device->read(lba, span);
        if (!result)
        {
            error = result.error();
            break;
        }

        // Prepare a block info object.
        std::unique_ptr<BlockInfo> info(new (std::nothrow) BlockInfo{});
        if (!info)
        {
            error = std::make_error_code(std::errc::not_enough_memory);
            break;
        }

        info->m_refcount = 1;
        info->m_buffer = std::move(buffer);
        info->m_device = device;
        info->m_lba = lba;
        info->m_writeback = false;

        // Transfer ownership of the block info to the block handle.
        auto& handle = out[i];
        setHandle(handle, info.release(), span);
    }

    // Did we encounter an error while performing the read?
    if (error)
    {
        // If we managed to read one or more blocks before encountering the error, treat this as a successful but short
        // read. The user will (probably) get the error when they try to read the rest of the requested blocks.
        if (i > 0)
            return i;
        else
            return std::unexpected(error);
    }

    return out.size();
}


FallbackBlockCache::result_t<std::size_t> FallbackBlockCache::readForUpdate(BlockDev* device, std::uint64_t lba, std::span<BlockHandle> out) noexcept
{
    // Perform the read normally.
    auto result = read(device, lba, out);
    if (!result)
        return result;

    // Set the writeback flag on each block.
    for (std::size_t i = 0; i < *result; ++i)
    {
        auto& handle = out[i];
        auto* info = BlockInfo::get(handle);
        info->m_writeback = true;
    }

    return result;
}


FallbackBlockCache::result_t<std::size_t> FallbackBlockCache::allocateEmpty(BlockDev* device, std::uint64_t lba, std::span<BlockHandle> out) noexcept
{
    auto sector_size = device->sectorSize();
    std::error_code error {};

    std::size_t i = 0;
    for (; i < out.size(); ++i)
    {
        // Allocate a buffer for the block.
        std::unique_ptr<std::byte[]> buffer(new (std::nothrow) std::byte[sector_size]);
        if (!buffer)
        {
            error = std::make_error_code(std::errc::not_enough_memory);
            break;
        }

        // If we were reading, we would do that here. But we don't need to read anything for this method.
        auto span = std::span{buffer.get(), sector_size};

        // Prepare a block info object.
        std::unique_ptr<BlockInfo> info(new (std::nothrow) BlockInfo{});
        if (!info)
        {
            error = std::make_error_code(std::errc::not_enough_memory);
            break;
        }

        info->m_refcount = 1;
        info->m_buffer = std::move(buffer);
        info->m_device = device;
        info->m_lba = lba;
        info->m_writeback = false;

        // Transfer ownership of the block info to the block handle.
        auto& handle = out[i];
        setHandle(handle, info.release(), span);
    }

    // Did we encounter an error while performing the read?
    if (error)
    {
        // If we managed to allocate one or more blocks before encountering the error, treat this as a successful but
        // short allocation.
        if (i > 0)
            return i;
        else
            return std::unexpected(error);
    }

    return out.size();
}


FallbackBlockCache::result_t<void> FallbackBlockCache::readCopy(BlockDev* device, std::uint64_t lba, std::uint64_t offset, std::span<std::byte> buffer) noexcept
{
    auto sector_size = device->sectorSize();    

    // We're only able to optimise the read for block-aligned reads; for all others, fall back to the base class method
    // which will call the normal read (which will allocate) then perform a memcpy.
    if (offset != 0 || buffer.size() % sector_size != 0)
        return BlockCache::readCopy(device, lba, offset, buffer);

    // Read directly into the buffer.
    return device->read(lba, buffer);
}


FallbackBlockCache::result_t<void> FallbackBlockCache::write(BlockDev* device, std::uint64_t lba, std::uint64_t offset, std::span<const std::byte> buffer) noexcept
{
    auto sector_size = device->sectorSize();

    // We're only able to optimise the write for block-aligned wrie; for all others, fall back to the base class method
    // which will call the normal readForUpdate/allocateEmpty (which will allocate) then perform a memcpy.
    if (offset != 0 || buffer.size() % sector_size != 0)
        return BlockCache::write(device, lba, offset, buffer);

    // Write directly from the buffer.
    return device->write(lba, buffer);
}


void FallbackBlockCache::ref(const BlockHandle* handle) noexcept
{
    BlockInfo::get(*handle)->m_refcount.fetch_add(1, std::memory_order::acquire);
}


void FallbackBlockCache::unref(const BlockHandle* handle) noexcept
{
    // Don't free the block if there are references remaining.
    auto* info = BlockInfo::get(*handle);
    if (info->m_refcount.fetch_sub(1, std::memory_order::release) > 1)
        return;

    // If the block is marked for write-back, do that now. Note that we don't have a way of knowing if the block was
    // actually written to.
    //
    //! @todo error checking
    if (info->m_writeback)
        (info->m_device)->write(info->m_lba, std::span{info->m_buffer.get(), info->m_device->sectorSize()});

    // Free the buffer and the info object.
    std::unique_ptr<BlockInfo> {info}.reset();
}


} // namespace System::Storage::BlockDevice
