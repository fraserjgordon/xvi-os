#include <System/Allocator/GAlloc/PoolGroup.hh>

#include <System/C++/LanguageSupport/Utility.hh>


namespace __GALLOC_NS_DECL
{


void PoolGroup::PoolList::removeFrom(PoolHeader** list, PoolHeader* pool)
{
    // Is this the first pool in the list?
    if (pool->m_prevHeader == nullptr)
    {
        // Yes - update the list header pointer.
        *list = pool->m_nextHeader;
    }
    else
    {
        // No - fix up the preceding pool
        pool->m_prevHeader->m_nextHeader = pool->m_nextHeader;
    }

    // Is this the last pool in the list?
    if (pool->m_nextHeader == nullptr)
    {
        // Yes - nothing to do.
    }
    else
    {
        // No - fix up the succeeding pool.
        pool->m_nextHeader->m_prevHeader = pool->m_prevHeader;
    }

    pool->m_nextHeader = nullptr;
    pool->m_prevHeader = nullptr;
}

void PoolGroup::PoolList::insertInto(PoolHeader** list, PoolHeader* pool)
{
    // The pool is always inserted at the head of the list. This implements a most-recently-used (MRU) strategy for
    // memory allocation, making it more likely that allocations are made out of "hot" (cached) memory.
    pool->m_nextHeader = *list;
    pool->m_prevHeader = nullptr;
    *list = pool;

    if (pool->m_nextHeader != nullptr)
    {
        pool->m_nextHeader->m_prevHeader = pool;
    }
}


void* PoolGroup::allocateFromPoolList(PoolList& list)
{
    // Assumption: the PoolList object has been locked.

    // Lock the first entry in the partial pool, if it exists.
    auto* pool = list.m_partialList;
    if (pool == nullptr)
    {
        // There are no partially-occupied pools. Try to get an empty one.
        pool = list.m_emptyList;
        if (pool == nullptr)
        {
            // There are no empty pools either. It is not possible to allocate from this pool list without allocating a
            // new pool (which will need to be done by the caller as it requires a policy decision).
            return nullptr;
        }

        // Move this pool onto the partial list as we're about to use it.
        //
        // Even though the pool is empty, we don't need to lock it yet as we hold the pool list lock so nobody else can
        // see the empty pool on the partial list yet.
        PoolList::removeFrom(&list.m_emptyList, pool);
        PoolList::insertInto(&list.m_partialList, pool);
        list.m_emptyPools -= 1;
    }

    // We have now chosen a pool. Lock it.
    auto pool_lock = pool->m_lock.lock();

    // Allocate from the pool. This cannot fail (in the absence of bugs...).
    void* block = pool->allocate();

    if (block == nullptr) [[unlikely]]
    {
        // The pool, pool list or pool group has become corrupt in some way.
        //! @TODO: report
        return nullptr;
    }

    // Update the list statistics.
    list.m_freeBlocks -= 1;

    // Check whether this pool has now become full.
    if (pool->m_freeCount == 0)
    {
        // The pool needs to be moved to the full list.
        PoolList::removeFrom(&list.m_partialList, pool);
        PoolList::insertInto(&list.m_fullList, pool);
    }

    return block;
}

void PoolGroup::freeBlockInPool(void* block, PoolHeader* pool, Lock::Guard pool_guard)
{
    // Assumption: block was previously returned by a call to allocate() on pool and hasn't been freed since.
    // Assumption: pool belongs to this pool group.
    // Assumption: pool_guard holds a valid lock on pool->m_lock.

    // While we have the pool lock, get the information we need to find the list containing it.
    auto pool_block_size = pool->m_blockSize;
    auto list_index = poolIndex(pool_block_size);
    auto& list = m_lists[list_index];

    // Drop the pool lock, take the list lock and reacquire the pool lock. Note that other theads may be able to
    // allocate or free blocks in the pool during this interval.
    pool_guard.release();
    auto list_guard = list.m_lock.lock();
    pool_guard = pool->m_lock.lock();

    // Now that both locks are held, the block can be freed safely and the pool moved to a different list if warranted.
    pool->free(block);
    list.m_freeBlocks += 1;

    if (pool->m_freeCount == 1)
    {
        // The pool needs to be moved from the full list to the partial list.
        PoolList::removeFrom(&list.m_fullList, pool);
        PoolList::insertInto(&list.m_partialList, pool);
    }
    else if (pool->m_freeCount == pool->m_blockCount)
    {
        // Remove the pool from the partial list.
        PoolList::removeFrom(&list.m_partialList, pool);

        // Are we going to keep this pool around or not?
        if (policyShouldKeepEmptyPool(list))
        {
            // The pool is being kept - move it to the empty list.
            PoolList::insertInto(&list.m_emptyList, pool);
            list.m_emptyPools += 1;
        }
        else
        {
            // The pool is being discarded so destroy it and update the list statistics.
            auto pool_blocks = pool->m_blockCount;
            destroyPool(pool, std::move(pool_guard));
            list.m_freeBlocks -= pool_blocks;
            list.m_totalBlocks -= pool_blocks;
        }
    }
}


} // namespace __GALLOC_NS_DECL
