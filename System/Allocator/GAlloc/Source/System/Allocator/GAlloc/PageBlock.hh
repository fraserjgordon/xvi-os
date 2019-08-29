#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_PAGEBLOCK_H
#define __SYSTEM_ALLOCATOR_GALLOC_PAGEBLOCK_H


#include <System/C++/Atomic/Atomic.hh>

#include <System/Allocator/GAlloc/Config.hh>


namespace __GALLOC_NS_DECL
{


class PageBlock
{
public:

    PageBlock(void* beginning, std::size_t size)
        : m_cursor{beginning},
          m_limit{reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(beginning) + size)}
    {
    }

    PageBlock(const PageBlock&) = delete;
    PageBlock(PageBlock&&) = delete;

    PageBlock& operator=(const PageBlock&) = delete;
    PageBlock& operator=(PageBlock&&) = delete;

    ~PageBlock() = default;

    // Attempts to allocate from this page block. Note that the given size must be a multiple of the page size.
    //
    // Returns nullptr if the request could not be satisfied.
    void* allocate(std::size_t size)
    {
        // Pre-load the cursor value and check for sufficient remaining space.
        void* cursor = m_cursor.load(std::memory_order::relaxed);
        void* limit = m_limit.load(std::memory_order::relaxed);
        while (reinterpret_cast<std::uintptr_t>(limit) - reinterpret_cast<std::uintptr_t>(cursor) > size)
        {
            // Attempt to atomically advance the cursor.
            void* new_cursor = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(cursor) + size);
            if (m_cursor.compare_exchange_weak(cursor, new_cursor, std::memory_order::relaxed))
                return cursor;
        }

        return nullptr;
    }

private:

    std::atomic<void*> m_cursor;    //!< Next page(s) to be allocated.
    std::atomic<void*> m_limit;     //!< Can't allocate beyond this point.
};


} // namespace __GALLOC_NS


#endif /* ifndef __SYSTEM_ALLOCATOR_GALLOC_PAGEBLOCK_H */
