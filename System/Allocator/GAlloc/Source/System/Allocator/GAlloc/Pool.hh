#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_POOL_H
#define __SYSTEM_ALLOCATOR_GALLOC_POOL_H


#include <System/Allocator/GAlloc/Config.hh>
#include <System/Allocator/GAlloc/Locks.hh>


namespace __GALLOC_NS_DECL
{


// Forward declarations.
class PoolGroup;


class PoolHeader
{
public:

    friend class PoolGroup;


    struct Statistics {};


    PoolHeader() = delete;
    PoolHeader(const PoolHeader&) = delete;
    PoolHeader(PoolHeader&&) = delete;

    PoolHeader& operator=(const PoolHeader&) = delete;
    PoolHeader& operator=(PoolHeader&&) = delete;

    ~PoolHeader();

    // Note: There is no public allocate method as all allocations must go through a pool group. There is no public path
    //       to getting a pool that would be suitable for allocating memory.
    //
    //       In contrast, the deallocate method is public as the pool can be reached via a page table lookup on an
    //       existing allocation. The same applies for querying the size of an existing allocation.  


    // Returns the size of blocks allocated from this pool.
    //
    // This method is lock-free; no locks are taken nor need to be held.
    std::size_t getBlockSize() const
    {
        return m_blockSize;
    }

    // Deallocates the given block belonging to this pool.
    //
    // Warning: the pool may be deleted as a result of this call so it is not safe to call any other methods on the pool
    //          once this method returns without having some other way of guaranteeing the pool lifetime.
    //
    // Assumption: the given block was previously allocated from this pool and has not been freed since.
    // Assumption: no locks are held on this pool nor the pool group to which it belongs.
    void deallocate(void* block);

    Statistics getStats() const;

private:

    // Lock for this pool.
    mutable Lock m_lock;

    // Size parameters for this pool.
    const std::size_t m_poolSize;           // Total size of this pool.
    const std::size_t m_blockSize;          // Size of the individual memory blocks.
    const std::size_t m_blockCount;         // Number of blocks in the pool.

    // Fixed pool memory locations.
    void* const m_pool;                     // Start of pool memory.
    std::uintptr_t* const m_bitmap;         // Pool used/free flag bitmap.

    // Pool list pointers.
    //
    // Unlike the other fields in this class, the list pointers are protected by the pool list lock in the owning pool
    // group. The group pointer may only be updated if both the pool and pool list locks are held simultaneousy; it may
    // be read if either is held.
    PoolGroup*  m_group;
    PoolHeader* m_nextHeader;
    PoolHeader* m_prevHeader;

    // Number of free blocks remaining in this pool.
    std::size_t m_freeCount;


    // Allocates a block from this pool. This will never fail if the pool has any free blocks.
    //
    // Assumption: m_lock is held.
    void* allocate();

    // Frees a block belonging to this pool.
    //
    // Assumption: m_lock is held.
    // Assumption: block was previously returned by a call to allocate() on this pool and hasn't been freed since.
    void free(void* block);

    // Returns the number of words in the pool bitmap.
    std::size_t bitmapLength() const;

    // Converts a block index to a pointer.
    void* indexToBlock(std::size_t i) const;

    // Converts a pointer to a block index. If the pointer does not point into memory managed by this pool, the return
    // value is undefined.
    std::size_t blockToIndex(void* block) const;

    // Called when there is some sort of pool corruption.
    void error(const char* message) const;
};


} // namespace __GALLOC_NS_DECL


#endif // ifndef __SYSTEM_ALLOCATOR_GALLOC_POOL_H
