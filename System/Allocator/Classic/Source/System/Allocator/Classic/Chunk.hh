#ifndef __SYSTEM_ALLOCATOR_CLASSIC_CHUNK_H
#define __SYSTEM_ALLOCATOR_CLASSIC_CHUNK_H


#include <cstddef>
#include <cstdint>


namespace System::Allocator::Classic::Impl
{


// Forward declarations.
struct chunk_header;
struct free_chunk;
struct free_chunk_footer;


struct chunk_header
{
    static constexpr int            FlagBits    = 3;
    static constexpr std::uintptr_t FlagMask    = (1 << FlagBits) - 1;
    static constexpr std::uintptr_t SizeMask    = ~FlagMask;

    static constexpr unsigned int   FlagFree        = 0b001;    // Set if this chunk is free.
    static constexpr unsigned int   FlagPredFree    = 0b010;    // Set if the chunk before this one in memory is free.
    static constexpr unsigned int   FlagLast        = 0b100;    // Set if there is no chunk after this one in memory.

    // Field containing both the status bits and the chunk size.
    std::uintptr_t chunk_data;

    std::size_t size() const
    {
        return (chunk_data & SizeMask);
    }

    unsigned int flags() const
    {
        return (chunk_data & FlagMask);
    }

    std::size_t dataSize() const
    {
        return size() - sizeof(chunk_header);
    }

    void* data()
    {
        return (this + 1);
    }

    void setSizeAndFlags(std::size_t size, unsigned int flags)
    {
        chunk_data = (size | flags);
    }

    void setSize(std::size_t size)
    {
        setSizeAndFlags(size, flags());
    }

    void setFlags(unsigned int flags)
    {
        setSizeAndFlags(size(), flags);
    }

    bool isLast() const
    {
        return flags() & FlagLast;
    }

    bool isFree() const
    {
        return flags() & FlagFree;
    }

    bool isPredecessorFree() const
    {
        return flags() & FlagPredFree;
    }

    bool isSuccessorFree() const
    {
        return !isLast() && successor()->isFree();
    }

    void markLast()
    {
        setFlags(flags() | FlagLast);
    }

    void markNotLast()
    {
        setFlags(flags() & ~FlagLast);
    }

    void markFree()
    {
        setFlags(flags() | FlagFree);

        // Update the successor too.
        if (!isLast())
            successor()->markPredecessorFree();
    }

    void markNotFree()
    {
        setFlags(flags() & ~FlagFree);

        // Update the successor too.
        if (!isLast())
            successor()->markPredecessorNotFree();
    }

    void markPredecessorFree()
    {
        setFlags(flags() | FlagPredFree);
    }

    void markPredecessorNotFree()
    {
        setFlags(flags() & ~FlagPredFree);
    }

    // Needs to be defined after the free_chunk_footer struct.
    inline free_chunk_footer* predecessorFooter() const;

    // Needs to be defined after the free_chunk_footer struct.
    inline free_chunk* predecessor() const;

    chunk_header* successor() const
    {
        if (isLast())
            return nullptr;

        return reinterpret_cast<chunk_header*>(reinterpret_cast<std::uintptr_t>(this) + size());
    }

    inline free_chunk* split(std::size_t new_size);

    chunk_header* mergeWithSuccessor()
    {
        // Do nothing if there is no successor or it isn't free.
        if (isLast() || !successor()->isFree())
            return this;        

        // This chunk may become the last chunk as a result of merging.
        bool last = successor()->isLast();
        setSize(size() + successor()->size());
        if (last)
            markLast();

        return this;
    }


    // Given an object pointer, returns a pointer to the corresponding chunk header.
    static chunk_header* fromDataPointer(void* ptr)
    {
        return static_cast<chunk_header*>(ptr) - 1;
    }
};

struct free_chunk : chunk_header
{
    free_chunk*         next;       // Next item in the free chunk list.
    free_chunk*         prev;       // Previous item in the free chunk list.


    inline free_chunk_footer* footer() const;

    void unlinkFromList(free_chunk*& list_head)
    {
        if (next)
            next->prev = prev;

        if (prev)
            prev->next = next;
        else
            list_head = next;

        next = prev = nullptr;
    }

    void linkIntoSortedList(free_chunk*& list_head)
    {
        next = prev = nullptr;

        // Check for the degenerate case of an empty list.
        if (list_head == nullptr)
        {
            list_head = this;
            return;
        }

        // The other degnerate case: this chunk is smaller than all others in the list.
        if (list_head->size() >= size())
        {
            next = list_head;
            list_head->prev = this;
            list_head = this;
            return;
        }

        // Scan until we hit the end of the list or find a bigger chunk.
        auto* p = list_head;
        while (p->next && p->next->size() < size())
            p = p->next;

        // This chunk needs to be inserted after p.
        next = p->next;
        prev = p;
        p->next = this;
        if (next)
            next->prev = this;
    }

    free_chunk* mergeWithSuccessor()
    {
        return static_cast<free_chunk*>(chunk_header::mergeWithSuccessor());
    }

    free_chunk* mergeWithPredecessor()
    {
        // Do nothing if there is no predecessor or if it isn't free.
        if (!isPredecessorFree())
            return this;        

        return predecessor()->mergeWithSuccessor();
    }

    inline void doFree();

    static free_chunk* bestFitInList(free_chunk* list_head, std::size_t size)
    {
        // Scan the list until the best fit (smallest, large-enough) chunk is found.
        auto p = list_head;
        while (p && p->size() < size)
            p = p->next;

        return p;
    }
};

struct free_chunk_footer
{
    std::size_t         size;       // Size of this chunk.


    free_chunk* chunk() const
    {
        return reinterpret_cast<free_chunk*>(reinterpret_cast<std::uintptr_t>(this + 1) - size);
    }
};

free_chunk_footer* chunk_header::predecessorFooter() const
{
    if (!isPredecessorFree())
        return nullptr;

    return reinterpret_cast<free_chunk_footer*>(const_cast<chunk_header*>(this)) - 1;
}

free_chunk* chunk_header::predecessor() const
{
    if (!isPredecessorFree())
        return nullptr;

    return predecessorFooter()->chunk();
}

free_chunk* chunk_header::split(std::size_t new_size)
{
    auto remaining_size = size() - new_size;
    auto remaining_chunk = reinterpret_cast<free_chunk*>(reinterpret_cast<std::uintptr_t>(this) + new_size);

    // Initialise the header of the remaining chunk.
    remaining_chunk->next = remaining_chunk->prev = nullptr;
    remaining_chunk->setSizeAndFlags(remaining_size, 0);

    // If this chunk was the last chunk, the remainder chunk inherits that flag.
    if (isLast())
    {
        remaining_chunk->markLast();
        markNotLast();
    }

    // The remaining chunk is now ready to be marked as free.
    remaining_chunk->doFree();

    // Update the size of this chunk and re-create the footer.
    setSize(new_size);
    if (isFree())
        static_cast<free_chunk*>(this)->doFree();

    return remaining_chunk;
}

free_chunk_footer* free_chunk::footer() const
{
    return reinterpret_cast<free_chunk_footer*>(reinterpret_cast<std::uintptr_t>(this) + size()) - 1;
}

void free_chunk::doFree()
{
    // In addition to the header marking, also create the footer and update the successor (if it exists).
    markFree();
    footer()->size = size();
    if (!isLast())
        successor()->markPredecessorFree();
}


inline constexpr auto ChunkAlignment        = 2*alignof(void*);
inline constexpr auto ChunkOverheadSize     = sizeof(chunk_header);
inline constexpr auto ChunkMinSize          = sizeof(free_chunk) + sizeof(free_chunk_footer);
inline constexpr auto ChunkMinUsableSize    = ChunkMinSize - ChunkOverheadSize;


} // namespace System::Allocator::Classic


#endif /* ifndef __SYSTEM_ALLOCATOR_CLASSIC_CHUNK_H */
