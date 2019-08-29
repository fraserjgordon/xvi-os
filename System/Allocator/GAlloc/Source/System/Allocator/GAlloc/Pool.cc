#include <System/Allocator/GAlloc/Pool.hh>

#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/Utility.hh>

#include <System/Allocator/GAlloc/PoolGroup.hh>


namespace __GALLOC_NS_DECL
{


void PoolHeader::deallocate(void* block)
{
    // Take the lock on this pool so that the pool group pointer can be accessed.
    auto guard = m_lock.lock();

    // Pass the request on the group. The ownership of the lock is passed here and the pool will no longer be locked
    // when the call returns. In fact, the pool may have been destroyed at that point so nothing further can be done
    // after this call.
    m_group->freeBlockInPool(block, this, std::move(guard));
}

void* PoolHeader::allocate()
{
    // Assumption: m_lock is locked.
    // Assumption: m_freeCount is non-zero.

    // Scan the bitmap for the first free bit.
    auto bitmap_length = bitmapLength();
    for (std::size_t i = 0; i < bitmap_length; ++i)
    {
        // Check for any free bits.
        auto offset = std::countr_one(m_bitmap[i]);
        if (offset < std::numeric_limits<std::uintptr_t>::digits)
        {
            // Found a zero bit in the bitmap. Calculate the overall bit offset.
            auto block_index = offset + i * std::numeric_limits<std::uintptr_t>::digits;

            // Check that the offset is valid (the only case where it wouldn't be is the last word in the bitmap if the
            // number of blocks in the pool is not a multiple of the number of bits in a word).
            if (block_index >= m_blockCount)
                break;

            // A free block has been found. Mark it as in-use, update the statistics and return.
            m_bitmap[i] |= (std::size_t(1) << offset);
            m_freeCount -= 1;
            return indexToBlock(block_index);
        }
    }

    // No free blocks were found (this shouldn't happen...)
    return nullptr;
}

void PoolHeader::free(void* block)
{
    // Assumption: m_lock is locked.
    // Assumption: block was previously returned by a call to allocate() on this pool and hasn't been freed since.

    // Get the index of the block within this pool and set that bit in the bitmap to zero.
    auto index = blockToIndex(block);
    constexpr auto N = std::numeric_limits<std::uintptr_t>::digits;
    m_bitmap[index / N] |= (std::uintptr_t(1) << (index % N));

    // Update the statistics.
    m_freeCount += 1;
}

std::size_t PoolHeader::bitmapLength() const
{
    return (m_blockCount + 8 * sizeof(std::uintptr_t) - 1) / (8 * sizeof(std::uintptr_t));
}

void* PoolHeader::indexToBlock(std::size_t i) const
{
    auto offset = i * m_blockSize;
    auto address = reinterpret_cast<std::uintptr_t>(m_pool) + offset;
    auto block = reinterpret_cast<void*>(address);

    return block;
}

std::size_t PoolHeader::blockToIndex(void* block) const
{
    auto address = reinterpret_cast<std::uintptr_t>(block);
    auto pool_address = reinterpret_cast<std::uintptr_t>(m_pool);
    auto offset = address - pool_address;
    auto index = offset / m_blockSize;

    return index;
}


} // namespace __GALLOC_NS_DECL
