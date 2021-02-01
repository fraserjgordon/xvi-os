#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_POOLGROUP_H
#define __SYSTEM_ALLOCATOR_GALLOC_POOLGROUP_H


#include <System/Allocator/GAlloc/Config.hh>
#include <System/Allocator/GAlloc/Locks.hh>
#include <System/Allocator/GAlloc/Pool.hh>


namespace __GALLOC_NS_DECL
{


class PoolGroup
{
public:

    friend class PoolHeader;


    static constexpr std::size_t MinimumBlockSizeOrder      = std::bit_width(sizeof(std::uintptr_t)) - 1;
    static constexpr std::size_t SmallBlockSizeOrder        = std::bit_width(MaxSmallObjectSize) - 1;
    static constexpr std::size_t MaximumBlockSizeOrder      = (PageOrder + MaxPooledBlockSizeOrder);
    static constexpr std::size_t MinimumBlockSize           = std::size_t(1) << MinimumBlockSizeOrder;
    static constexpr std::size_t SmallBlockSize             = MaxSmallObjectSize;
    static constexpr std::size_t MaximumBlockSize           = (3 * std::size_t(1) << MaximumBlockSizeOrder) / 4;

    static constexpr std::size_t MinimumPoolSize            = PageSize;
    static constexpr std::size_t MaximumPoolSize            = PageSize << MaxPoolOrder;

    static constexpr std::size_t SmallPoolCount             = SmallBlockSize / sizeof(std::uintptr_t);
    static constexpr std::size_t MediumPoolCount            = 2 * (MaximumBlockSizeOrder - SmallBlockSizeOrder);
    static constexpr std::size_t PoolCount                  = SmallPoolCount + MediumPoolCount;

private:

    struct PoolList
    {
        Lock        m_lock;         // Lock protecting the lists.
        PoolHeader* m_partialList;  // List of partially-occupied pools.
        PoolHeader* m_fullList;     // List of fully-occupied pools.
        PoolHeader* m_emptyList;    // List of unoccupied pools.
        std::size_t m_freeBlocks;   // Number of unused blocks.
        std::size_t m_totalBlocks;  // Total number of blocks.
        std::size_t m_emptyPools;   // Number of pools in the empty list.

        // Note: the number of pools in the full list is never relevant for policy decisions. The number of partial
        //       pools could conceivably be useful but the number of blocks seems like a more useful statistic to track.

        static void removeFrom(PoolHeader** list, PoolHeader*);
        static void insertInto(PoolHeader** list, PoolHeader*);
    };


    // The list of object pools.
    PoolList    m_lists[PoolCount];


    // Policy method - a pool in the given list has just been emptied; should it be retained or destroyed?
    bool policyShouldKeepEmptyPool(PoolList& list);

    // Policy method - there are no non-full pools with the given size; should a new one be allocated or an existing one
    // taken from another pool group?
    //
    // The pool list is used to supply information about the pool block size as well as providing statistics that feed
    // into the policy decision.
    //
    // If an existing pool is to be taken, this method is responsible for removing it from the other pool group and will
    // return a pointer to the removed pool. If a new pool should be allocated, the method will allocate it and return a
    // pointer to the new pool (i.e the caller will always receive a pool - if memory is available - but it could be
    // partially-full rather than empty).
    PoolHeader* policyTakeOrAllocatePool(PoolList& list);

    // Assumption: list.m_lock is locked.
    // Assumption: the list contains at least one free block.
    void* allocateFromPoolList(PoolList& list);

    // The guard for the pool lock needs to be passed to this method as it needs be released as part of the freeing
    // process in order to avoid lock inversion.
    //
    // Assumption: block was previously returned by a call to allocate() on pool and hasn't been freed since.
    // Assumption: pool belongs to this pool group.
    // Assumption: pool_guard holds a valid lock on pool->m_lock.
    void freeBlockInPool(void* block, PoolHeader* pool, Lock::Guard pool_guard);

    // Allocates a new, empty pool for the given list.
    //
    // Assumption: list.m_lock is locked.
    PoolHeader* allocatePoolForList(PoolList& list);

    // Destroys the given pool and returns the associated memory to the page allocator.
    //
    // Assumption: pool has been removed from all pool lists.
    // Assumption: pool_guard holds a valid lock on pool->m_lock.
    void destroyPool(PoolHeader* pool, Lock::Guard pool_guard);


    // Returns the size of the n'th pool.
    static constexpr std::size_t poolSize(std::size_t index)
    {
        if (index <= SmallBlockSizeOrder)
            return index * sizeof(std::uintptr_t);
    
        auto relative_index = (index - SmallBlockSizeOrder);
        auto base_size = (SmallBlockSize << (relative_index / 2));

        if (relative_index % 2 != 0)
            base_size += (base_size / 2);
    }

    // Returns the index of the appropriate pool for an allocation of the given size.
    static constexpr std::size_t poolIndex(std::size_t size)
    {
        if (size <= MinimumBlockSize)
            return 0;

        if (size <= SmallBlockSize)
            return (size + sizeof(std::uintptr_t) - 1) / sizeof(std::uintptr_t);
        
        auto rounded = std::bit_ceil(size);
        auto log2 = std::bit_width(rounded) - 1;
        auto order = log2 - SmallBlockSizeOrder;
        auto index = order * 2;

        if (size > (rounded - (rounded / 4)))
            return index;
        else
            return index - 1;
    }
};


} // namespace __GALLOC_NS_DECL


#endif /* ifndef __SYSTEM_ALLOCATOR_GALLOC_POOLGROUP_H */
