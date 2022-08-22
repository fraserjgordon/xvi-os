#include <System/Allocator/Classic/Arena.hh>

#include <algorithm>
#include <bit>
#include <cstring>
#include <limits>
#include <new>

#include <System/Allocator/Classic/Classic.hh>


#if __SYSTEM_ALLOCATOR_CLASSIC_DEBUG_PRINT
#  include <array>
#  include <format>
#endif


namespace System::Allocator::Classic
{


namespace Impl
{


std::pair<void*, std::size_t> Arena::allocate(std::size_t size)
{
    return allocate(size, std::align_val_t{Alignment});
}

std::pair<void*, std::size_t> Arena::allocate(std::size_t user_size, std::align_val_t alignment_val)
{
    // If a zero-size allocation is requested, ignore it.
    if (user_size == 0)
        return {nullptr, 0UL};

    std::scoped_lock lock(m_mutex);

    // If the alignment is not a power of two, we won't be able to satisfy it.
    auto alignment = static_cast<std::size_t>(alignment_val);
    if (!std::has_single_bit(alignment))
    {
        m_accounting.allocation_failures += 1;
        return {nullptr, 0UL};
    }

    //! @TODO: support non-default alignments.
    if (alignment > Alignment)
    {
        m_accounting.allocation_failures += 1;
        return {nullptr, 0UL};
    }

    // Convert the requested size to an allocation size and check that we can support it.
    auto min_chunk_size = userSizeToChunkSize(user_size);
    if (min_chunk_size >= LargestBin)
    {
        m_accounting.allocation_failures += 1;
        return {nullptr, 0UL};
    }

    // Calculate the smallest bin we should search in.
    auto start_index = sizeToBinIndex(min_chunk_size);

    // Scan each bin until we find a chunk that is large enough.
    free_chunk* chunk = nullptr;
    for (auto index = start_index; index < BinCount; ++index)
    {
        // Perform the search accounting for this bin.
        auto& acct = m_binAccounting[index];
        acct.total_search_count += 1;
        if (index == start_index)
            acct.first_search_count += 1;

        // Find the best-fitting chunk within this bin.
        auto& bin = m_bins[index];
        if (bin)
        {
            chunk = free_chunk::bestFitInList(bin, min_chunk_size);
            if (chunk)
            {
                // Found a suitable chunk.
                acct.allocation_count += 1;
                break;
            }
        }
    }

    // Did we find a chunk?
    if (!chunk)
    {
        m_accounting.allocation_failures += 1;
        return {nullptr, 0UL};
    }

    // Remove the chunk from the bin it is in.
    removeChunkFromBin(chunk, &bin_accounting_info::allocation_count);

    // If the chunk is larger than requested, we'll need to split it.
    if (canSplit(chunk->size(), min_chunk_size))
    {
        // We're going to split the chunk.
        auto index = sizeToBinIndex(chunk->size());
        m_binAccounting[index].split_from_count += 1;

        // Create the chunk containing the un-needed space and add it to the appropriate bin.
        auto extra_chunk = chunk->split(min_chunk_size);
        addFreeChunk(extra_chunk, &bin_accounting_info::split_to_count);

        // Also record that the chunk that we are going to use is (nominally) in a bin.
        index = sizeToBinIndex(chunk->size());
        m_binAccounting[index].split_to_count += 1;

        m_accounting.chunk_splits += 1;
        m_accounting.overhead_size += ChunkOverheadSize;
    }

    // The chunk is no longer free. Mark it as such, update the accounting and return the pointer.
    chunk->markNotFree();
    m_accounting.allocation_count += 1;
    m_accounting.chunks_allocated += 1;
    m_accounting.chunks_allocated_high_mark = std::max(m_accounting.chunks_allocated_high_mark, m_accounting.chunks_allocated);
    m_accounting.allocated_size += chunk->size();
    m_accounting.allocated_size_high_mark = std::max(m_accounting.allocated_size_high_mark, m_accounting.allocated_size);

    return {chunk->data(), chunk->dataSize()};
}

void Arena::free(void* memory)
{
    // Ignore attempts to free null.
    if (!memory)
        return;

    std::scoped_lock lock(m_mutex);

    // Convert the user data pointer to a chunk pointer.
    auto chunk = chunk_header::fromDataPointer(memory);

    if (chunk->isFree()) [[unlikely]]
    {
        // Uh oh...
        return;
    }

    // Record the free attempt.
    m_accounting.free_count += 1;
    m_accounting.chunks_allocated -= 1;
    m_accounting.allocated_size -= chunk->size();

    // Mark the chunk as now being free.
    chunk->markFree();
    auto fchunk = static_cast<free_chunk*>(chunk);

    // If the successor chunk is free, remove it from its current bin and merge.
    bool merged = false;
    if (auto successor = fchunk->successor(); fchunk->isSuccessorFree())
    {
        merged = true;
        removeChunkFromBin(static_cast<free_chunk*>(successor), &bin_accounting_info::merge_from_count);
        fchunk = fchunk->mergeWithSuccessor();
        m_accounting.chunk_merges += 1;
        m_accounting.overhead_size -= sizeof(chunk_header);
    }

    // If the predecessor chunk is free, remove it from its current bin.
    if (auto predecessor = fchunk->predecessor(); fchunk->isPredecessorFree())
    {
        merged = true;
        removeChunkFromBin(predecessor, &bin_accounting_info::merge_from_count);
        fchunk = fchunk->mergeWithPredecessor();
        m_accounting.chunk_merges += 1;
        m_accounting.overhead_size -= sizeof(chunk_header);
    }

    // Create the free chunk footer.
    fchunk->doFree();

    // Add the (possibly now merged) chunk to the appropriate bin.
    addFreeChunk(fchunk, &bin_accounting_info::free_count);

    // If there was merging, record that information too.
    if (merged)
    {
        auto index = sizeToBinIndex(fchunk->size());
        m_binAccounting[index].merge_to_count += 1;
    }
}

std::size_t Arena::allocationSize(void* memory) const
{
    auto chunk = chunk_header::fromDataPointer(memory);
    return chunk->size();
}

std::size_t Arena::resize(void* memory, std::size_t requested_size)
{
    // Short-circuit resize-to-zero to a free operation.
    if (requested_size == 0)
    {
        free(memory);
        return 0;
    }

    std::scoped_lock lock(m_mutex);

    // Update the resize attempts stat.
    m_accounting.resize_attempts += 1;

    // Resize attempts for null pointers always fail.
    if (!memory)
        return 0;

    // First, find the existing size.
    auto chunk = chunk_header::fromDataPointer(memory);
    auto old_size = chunk->size();

    // Are we shrinking or growing the chunk?
    auto new_size = userSizeToChunkSize(requested_size);
    if (new_size > old_size)
    {
        // The chunk needs to grow. This is only possible if the subsequent block is free and is big enough.
        auto next = static_cast<free_chunk*>(chunk->successor());
        if (!next || !next->isFree() || old_size + next->size() < new_size)
            return chunkSizeToUserSize(old_size);

        // Remove the chunk from its bucket and merge with it.
        removeChunkFromBin(next, &bin_accounting_info::merge_from_count);
        chunk = chunk->mergeWithSuccessor();

        // Account for the merging that has just been done.
        m_accounting.chunk_merges += 1;
        m_accounting.overhead_size -= sizeof(chunk_header);

        // Proceed with the check for a shrinking resize as the chunk is now likely too big.
        old_size = chunk->size();
    }

    // The only possible failure cases are on the growing path, which is now past. The resize will always succeed now.
    m_accounting.resize_successes += 1;

    if (new_size < old_size)
    {
        // The block is shrinking (or staying the same). Is it by enough to create a new chunk?
        auto diff = old_size - new_size;
        if (diff < SmallestBin)
            return chunkSizeToUserSize(old_size);

        // Split the chunk to give a new free chunk.
        auto new_chunk = chunk->split(new_size);
        addFreeChunk(new_chunk, &bin_accounting_info::split_to_count);

        // Account for the splitting that has just been done.
        m_accounting.chunk_splits += 1;
        m_accounting.overhead_size += sizeof(chunk_header);
    }

    // Resize has completed successfully.
    return chunkSizeToUserSize(new_size);
}

std::pair<void*, std::size_t> Arena::reallocate(void* memory, std::size_t new_size)
{
    // Is this a free request?
    if (new_size == 0)
    {
        free(memory);
        return {nullptr, 0};
    }

    // Can the resize be done in-place?
    if (memory)
    {
        auto resized = resize(memory, new_size);
        if (resized >= new_size)
            return {memory, resized};
    }

    // Allocate new memory for this request.
    auto [p, s] = allocate(new_size);
    if (!p)
        return {nullptr, 0};

    // Copy the existing contents.
    if (memory)
        std::memcpy(p, memory, std::min(s, new_size));

    // Free the old memory
    free(memory);

    return {p, s};
}

void Arena::addMemoryBlock(void* where, std::size_t size)
{
    std::scoped_lock lock(m_mutex);

    // Add a header to the block and add it to the block list.
    auto block = reinterpret_cast<block_header*>(where);
    block->block_size = size;
    block->next = nullptr;
    block->prev = m_blocks;
    m_blocks = block;

    // Update the block-related stats.
    m_accounting.blocks_added += 1;
    m_accounting.blocks_high_mark = std::max(m_accounting.blocks_high_mark, m_accounting.blocks_added);

    // Update the size-related stats.
    m_accounting.total_size += size;
    m_accounting.overhead_size += sizeof(block_header);
    m_accounting.total_size_high_mark = std::max(m_accounting.total_size_high_mark, m_accounting.total_size);

    // Add a chunk header.
    auto chunk_size = size - sizeof(block_header);
    auto chunk = reinterpret_cast<free_chunk*>(block + 1);
    chunk->setSize(chunk_size);
    chunk->markLast();
    chunk->doFree();
    
    // Put the chunk onto the appropriate list.
    addFreeChunk(chunk, nullptr);
}

void Arena::prune()
{
    // Currently does nothing.
}

const accounting_info& Arena::accountingInfo() const
{
    return m_accounting;
}

void Arena::corruptionDetected(void* where, const char* message)
{
    // Assumes: m_mutex is held by the caller.    

    m_accounting.corruption_events += 1;

    // Currently does nothing.
    (void)where;
    (void)message;
}

void Arena::addFreeChunk(free_chunk* chunk, std::uintptr_t bin_accounting_info::* accounting_bucket)
{
    // Assumes: m_mutex is held by the caller.

    // Add the chunk to the appropriate bin.
    auto index = sizeToBinIndex(chunk->size());
    chunk->linkIntoSortedList(m_bins[index]);

    // Adjust the accounting information.
    auto& acct = m_binAccounting[index];
    acct.chunk_count += 1;
    acct.chunk_count_high_mark = std::max(acct.chunk_count_high_mark, acct.chunk_count);
    acct.*accounting_bucket += 1;
}

void Arena::removeChunkFromBin(free_chunk* chunk, std::uintptr_t bin_accounting_info::* accounting_bucket)
{
    // Assumes: m_mutex is held by the caller.

    // Remove the chunk from the bin.
    auto index = sizeToBinIndex(chunk->size());
    chunk->unlinkFromList(m_bins[index]);

    // Adjust the accounting information.
    auto& acct = m_binAccounting[index];
    acct.chunk_count -= 1;
    acct.*accounting_bucket += 1;
    m_accounting.chunks_free -= 1;
}

std::size_t Arena::userSizeToChunkSize(std::size_t user_size)
{
    // Add the overhead for the chunk header.
    auto size = user_size + ChunkOverheadSize;    

    // Round the size up to the next aligned size.
    size = (size + Alignment - 1) & ~(Alignment - 1);

    // Ensure that the size is at least the size of the smallest bin.
    size = std::max(size, SmallestBin);

    // Final sanity check: if the size overflowed, return a huge size.
    if (size < user_size) [[unlikely]]
        return std::numeric_limits<std::size_t>::max();

    return size;
}

std::size_t Arena::chunkSizeToUserSize(std::size_t chunk_size)
{
    // Much simpler than going the other way: just subtract the overheads.
    return chunk_size - ChunkOverheadSize;
}


Arena::Arena()
{
    //! @TODO: record how to free the arena memory.
}

Arena::~Arena()
{
    //! @TODO: sanity checks (like making sure no allocations are outstanding).
}

Arena* Arena::createFromMemory(void* memory, std::size_t size)
{
    // Ensure that we have some suitably-aligned space within the block.
    if (!std::align(alignof(Arena), sizeof(Arena), memory, size))
        return nullptr;

    // Create the arena inside the block of memory.
    auto* arena = new (memory) Arena();

    // Add the remainder of the block as usable heap memory.
    size -= sizeof(Arena);
    memory = arena + 1;
    if (std::align(Alignment, sizeof(block_header) + SmallestBin, memory, size))
        arena->addMemoryBlock(memory, size);

    return arena;
}

void Arena::destroy(Arena* arena)
{
    //! @TODO: free the memory that the arena object occupies.
    arena->~Arena();
}


} // namespace Impl


Arena* Arena::arenaCreateFromMemory(void* memory, std::size_t size)
{
    return reinterpret_cast<Arena*>(Impl::Arena::createFromMemory(memory, size));
}

void Arena::arenaDestroy(Arena* arena)
{
    Impl::Arena::destroy(reinterpret_cast<Impl::Arena*>(arena));
}

std::pair<void*, std::size_t> Arena::arenaAllocate(Arena* arena, std::size_t size)
{
    return reinterpret_cast<Impl::Arena*>(arena)->allocate(size);
}

std::pair<void*, std::size_t> Arena::arenaAllocateAligned(Arena* arena, std::size_t size, std::size_t alignment)
{
    return reinterpret_cast<Impl::Arena*>(arena)->allocate(size, std::align_val_t{alignment});
}

void Arena::arenaFree(Arena* arena, void* memory)
{
    reinterpret_cast<Impl::Arena*>(arena)->free(memory);
}

std::size_t Arena::arenaGetAllocationSize(Arena* arena, void* memory)
{
    return reinterpret_cast<Impl::Arena*>(arena)->allocationSize(memory);
}

std::size_t Arena::arenaResizeAllocation(Arena* arena, void* memory, std::size_t new_size)
{
    return reinterpret_cast<Impl::Arena*>(arena)->resize(memory, new_size);
}

std::pair<void*, std::size_t> Arena::arenaReallocate(Arena* arena, void* memory, std::size_t new_size)
{
    return reinterpret_cast<Impl::Arena*>(arena)->reallocate(memory, new_size);
}

void Arena::arenaAddMemoryBlock(Arena* arena, void* memory, std::size_t size)
{
    reinterpret_cast<Impl::Arena*>(arena)->addMemoryBlock(memory, size);
}


} // namespace System::Allocator::Classic
