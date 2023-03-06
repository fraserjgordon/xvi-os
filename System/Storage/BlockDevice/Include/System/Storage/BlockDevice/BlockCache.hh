#ifndef __SYSTEM_STORAGE_BLOCKDEVICE_BLOCKCACHE_H
#define __SYSTEM_STORAGE_BLOCKDEVICE_BLOCKCACHE_H


#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <system_error>
#include <utility>


namespace System::Storage::BlockDevice
{


// Forward declarations.
class BlockDev;
class BlockCache;


// A handle represents a claim on a cached block. The block will not be freed while a handle referencing it exists.
//
// Block handles are intended to be held only for short periods.
//
//! @todo split out a separate ConstBlockHandle class for read-only blocks.
class BlockHandle
{
    friend class BlockCache;

public:

    constexpr BlockHandle() noexcept = default;

    BlockHandle(const BlockHandle& x) noexcept :
        m_cache(x.m_cache),
        m_metadata(x.m_metadata),
        m_span{x.m_span}
    {
        ref(this);
    }

    constexpr BlockHandle(BlockHandle&& x) noexcept :
        m_cache(std::exchange(x.m_cache, nullptr)),
        m_metadata(std::exchange(x.m_metadata, {})),
        m_span(std::exchange(x.m_span, {}))
    {
    }

    ~BlockHandle()
    {
        unref(this);
    }

    BlockHandle& operator=(const BlockHandle& x) noexcept
    {
        ref(&x);
        unref(this);

        m_cache = x.m_cache;
        m_metadata = x.m_metadata;
        m_span = x.m_span;

        return *this;
    }

    constexpr BlockHandle& operator=(BlockHandle&& x) noexcept
    {
        if (&x == this) [[unlikely]]
            return *this;

        unref(this);

        m_cache = std::exchange(x.m_cache, nullptr);
        m_metadata = std::exchange(x.m_metadata, {});
        m_span = std::exchange(x.m_span, {});

        return *this;
    }

    constexpr void swap(BlockHandle& x) noexcept
    {
        using std::swap;
        swap(m_cache, x.m_cache);
        swap(m_metadata, x.m_metadata);
        swap(m_span, x.m_span);
    }

    //! @todo should this be volatile?
    constexpr std::span<std::byte> writeSpan() noexcept
    {
        return m_span;
    }

    constexpr std::span<const std::byte> readSpan() const noexcept
    {
        return m_span;
    }

    //! @todo re-read, sync/flush methods etc.

private:

    // The cache that owns this block and some opaque metadata that the cache can use to identify the block in some way.
    BlockCache* m_cache = nullptr;
    union
    {
        void*           u_ptr;
        std::uint64_t   u_val = 0;
    } m_metadata = {};

    // The block's in-memory storage.
    std::span<std::byte>  m_span = {};


    static inline void ref(const BlockHandle*) noexcept;
    static inline void unref(const BlockHandle*) noexcept;
};


class BlockCache
{
    friend class BlockHandle;

public:

    template <class T> using result_t = std::expected<T, std::error_code>;


    //! @returns the number of blocks successfully read.
    //!
    //! @warning the returned blocks should not be written to.
    virtual result_t<std::size_t> read(BlockDev*, std::uint64_t lba, std::span<BlockHandle> out) noexcept = 0;

    // As above but reads a single block.
    result_t<BlockHandle> read(BlockDev* dev, std::uint64_t lba) noexcept
    {
        BlockHandle handle;
        auto res = read(dev, lba, std::span{&handle, 1});

        if (!res)
            return std::unexpected(res.error());

        if (*res < 1)
            return std::unexpected(std::make_error_code(std::errc::interrupted));

        return handle;
    }

    // Reads blocks that will be written into but whose current contents are wanted (e.g for partial update).
    virtual result_t<std::size_t> readForUpdate(BlockDev*, std::uint64_t lba, std::span<BlockHandle> out) noexcept = 0;

    // As above but reads a single block.
    result_t<BlockHandle> readForUpdate(BlockDev* dev, std::uint64_t lba) noexcept
    {
        BlockHandle handle;
        auto res = readForUpdate(dev, lba, std::span{&handle, 1});

        if (!res)
            return std::unexpected(res.error());

        if (*res < 1)
            return std::unexpected(std::make_error_code(std::errc::interrupted));

        return handle;
    }

    // Allocates empty (zeroed) blocks that will be written without regard to the current block contents.
    virtual result_t<std::size_t> allocateEmpty(BlockDev*, std::uint64_t lba, std::span<BlockHandle> out) noexcept = 0;

    // As above but allocates a single block.
    result_t<BlockHandle> allocateEmpty(BlockDev* dev, std::uint64_t lba) noexcept
    {
        BlockHandle handle;
        auto res = allocateEmpty(dev, lba, std::span{&handle, 1});

        if (!res)
            return std::unexpected(res.error());

        if (*res < 1)
            return std::unexpected(std::make_error_code(std::errc::interrupted));

        return handle;
    }

    // Reads a copy of the requested blocks to an existing buffer. Does not need to be block-aligned.
    //
    //! @todo scatter-gather form. Maybe split the buffer types out of the FileIO library?
    virtual result_t<void> readCopy(BlockDev*, std::uint64_t lba, std::uint64_t offset, std::span<std::byte>) noexcept;

    // Writes data to the cache. Does not need to be block-aligned.
    virtual result_t<void> write(BlockDev*, std::uint64_t lba, std::uint64_t offset, std::span<const std::byte>) noexcept;


    // Returns a slow-but-functional fallback cache - this 'cache' doesn't actually cache; it just allocates a buffer
    // per cached read and frees it when the block is no longer in use.
    static BlockCache* fallbackCache() noexcept;


protected:

    virtual void ref(const BlockHandle*) noexcept = 0;
    virtual void unref(const BlockHandle*) noexcept = 0;

    void setHandle(BlockHandle& handle, void* ptr, std::span<std::byte> span) noexcept
    {
        handle.m_cache = this;
        handle.m_metadata.u_ptr = ptr;
        handle.m_span = span;
    }

    void setHandle(BlockHandle& handle, std::uint64_t val, std::span<std::byte> span) noexcept
    {
        handle.m_cache = this;
        handle.m_metadata.u_val = val;
        handle.m_span = span;
    }

    static void* handlePtr(const BlockHandle& handle) noexcept
    {
        return handle.m_metadata.u_ptr;
    }

    static std::uint64_t handleVal(const BlockHandle& handle) noexcept
    {
        return handle.m_metadata.u_val;
    }
};


void BlockHandle::ref(const BlockHandle* h) noexcept
{
    if (!h->m_cache)
        return;

    h->m_cache->ref(h);
}


void BlockHandle::unref(const BlockHandle* h) noexcept
{
    if (!h->m_cache)
        return;

    h->m_cache->unref(h);
}


} // namespace System::Storage::BlockDevice


#endif /* ifndef __SYSTEM_STORAGE_BLOCKDEVICE_BLOCKCACHE_H */
