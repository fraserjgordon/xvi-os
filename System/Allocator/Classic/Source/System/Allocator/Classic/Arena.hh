#ifndef __SYSTEM_ALLOCATOR_CLASSIC_ARENA_H
#define __SYSTEM_ALLOCATOR_CLASSIC_ARENA_H


#include <bit>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <new>
#include <utility>

#include <System/Allocator/Classic/Chunk.hh>


namespace System::Allocator::Classic::Impl
{


// Header placed on blocks.
//
// Blocks are contiguous areas of memory that get subdivided into the chunks that get allocated.
struct block_header
{
    std::size_t     block_size;
    void*           next;
    void*           prev;
};

// It is required that a block header plus a chunk header cause the usable area of that chunk to be correctly aligned to
// the minimum alignment that this allocator guarantees. If this is not true, all chunks will be misaligned.
static_assert((sizeof(block_header) + sizeof(chunk_header)) % ChunkAlignment == 0, "Chunks will be misaligned");


// Accounting information.
struct accounting_info
{
    std::uintptr_t      allocation_count            = 0;
    std::uintptr_t      allocation_failures         = 0;
    std::uintptr_t      free_count                  = 0;

    std::uintptr_t      resize_attempts             = 0;
    std::uintptr_t      resize_successes            = 0;

    std::uintptr_t      blocks_added                = 0;
    std::uintptr_t      blocks_removed              = 0;
    std::uintptr_t      blocks_high_mark            = 0;

    std::uintptr_t      chunks_allocated            = 0;
    std::uintptr_t      chunks_free                 = 0;
    std::uintptr_t      chunks_allocated_high_mark  = 0;

    std::uintptr_t      chunk_splits                = 0;
    std::uintptr_t      chunk_merges                = 0;

    // Note: no free size as is misleading; doesn't account for overheads.
    std::uintptr_t      total_size                  = 0;
    std::uintptr_t      allocated_size              = 0;
    std::uintptr_t      overhead_size               = 0;
    std::uintptr_t      total_size_high_mark        = 0;
    std::uintptr_t      allocated_size_high_mark    = 0;

    std::uintptr_t      corruption_events           = 0;
};

// Bin-specific accounting info.
struct bin_accounting_info
{
    std::uintptr_t      first_search_count          = 0;
    std::uintptr_t      total_search_count          = 0;
    std::uintptr_t      allocation_count            = 0;
    std::uintptr_t      free_count                  = 0;

    std::uintptr_t      chunk_count                 = 0;
    std::uintptr_t      chunk_count_high_mark       = 0;

    std::uintptr_t      split_from_count            = 0;
    std::uintptr_t      split_to_count              = 0;
    std::uintptr_t      merge_from_count            = 0;
    std::uintptr_t      merge_to_count              = 0;
};


class Arena
{
public:

    // Number of bins per octave (i.e doubling of size).
    static constexpr unsigned       BinsPerOctave   = 8;

    static constexpr auto           Alignment       = ChunkAlignment;

    static constexpr std::size_t    SmallestBin     = ChunkMinSize;
    static constexpr std::size_t    FirstInexactBin = 512;
    static constexpr std::size_t    LargestBin      = 128*1024;     // 128kB.

    static constexpr auto           ExactBinCount   = (FirstInexactBin - SmallestBin) / Alignment;
    static constexpr auto           InexactBinCount = (std::bit_width(LargestBin) - std::bit_width(FirstInexactBin)) * BinsPerOctave;
    static constexpr auto           BinCount        = ExactBinCount + InexactBinCount;


    Arena(const Arena&) = delete;
    Arena(Arena&&) = delete;

    Arena& operator=(const Arena&) = delete;
    Arena& operator=(Arena&&) = delete;


    std::pair<void*, std::size_t> allocate(std::size_t size);

    std::pair<void*, std::size_t> allocate(std::size_t size, std::align_val_t alignment);

    void free(void* ptr);

    std::size_t allocationSize(void* ptr) const;

    std::size_t resize(void* ptr, std::size_t new_size);

    std::pair<void*, std::size_t> reallocate(void* ptr, std::size_t size);

    void addMemoryBlock(void*, std::size_t);

    void prune();


    static Arena* createFromMemory(void*, std::size_t size);

    static void destroy(Arena*);


    static constexpr bool canSplit(std::size_t chunk_size, std::size_t requested_size)
    {
        // Round the size up to the next alignment boundary.
        auto aligned_size = (requested_size + Alignment - 1) & ~(Alignment - 1);

        // How many bytes are left over?
        auto excess_size = (aligned_size < chunk_size) ? (chunk_size - aligned_size) : 0;

        // The chunk can be split if there is enough left for another whole chunk.
        return (excess_size >= SmallestBin);
    }

    static constexpr unsigned int sizeToBinIndex(std::size_t size)
    {
        if (size <= SmallestBin)
        {
            return 0;
        }
        else if (size >= LargestBin)
        {
            // The largest bin is the dumping ground for all chunks above its size.
            return BinCount - 1;
        }
        else if (size <= FirstInexactBin)
        {
            auto rounded_size = (size + Alignment - 1) & ~(Alignment - 1);
            return ((rounded_size - SmallestBin) / Alignment);
        }
        else
        {
            constexpr auto BitsPerOctave = std::bit_width(BinsPerOctave) - 1;
            auto shift = std::bit_width(std::bit_floor(size)) - 1;
            auto octave = shift - (std::bit_width(FirstInexactBin) - 1);
            
            auto octave_base_size = FirstInexactBin << octave;
            auto octave_subdivision = octave_base_size / BinsPerOctave;

            auto rounded_size = (size + octave_subdivision - 1) & ~(octave_subdivision - 1);

            auto offset = (rounded_size >> (shift - BitsPerOctave)) & (BinsPerOctave - 1);

            return ExactBinCount + (octave * BinsPerOctave) + offset;
        }
    }

    static constexpr std::size_t binIndexToSize(unsigned int index)
    {
        if (index < ExactBinCount)
            return SmallestBin + (index * Alignment);
        else
        {
            constexpr auto BitsPerOctave = std::bit_width(BinsPerOctave) - 1;
            auto octave = (index - ExactBinCount) >> BitsPerOctave;
            auto offset = (index - ExactBinCount) & (BinsPerOctave - 1);

            auto octave_base_size = FirstInexactBin << octave;
            auto octave_subdivision = octave_base_size / BinsPerOctave;

            return octave_base_size + (offset * octave_subdivision);
        }
    }

private:

    mutable std::mutex  m_mutex         = {};

    block_header*   m_blocks            = nullptr;
    free_chunk*     m_bins[BinCount]    = {};

    accounting_info     m_accounting                = {};
    bin_accounting_info m_binAccounting[BinCount]   = {};


    Arena();
    ~Arena();


    void corruptionDetected(void* where, const char* message);

    void addFreeChunk(free_chunk*, std::uintptr_t bin_accounting_info::* accounting_bucket);

    void removeChunkFromBin(free_chunk*, std::uintptr_t bin_accounting_info::* accounting_bucket);

    // Adjusts a user-requested size to the appropriate chunk size.
    std::size_t userSizeToChunkSize(std::size_t);

    // Converts a chunk size to the size of the user area.
    std::size_t chunkSizeToUserSize(std::size_t);
};


} // namespace System::Allocator::Classic


#endif /* ifndef __SYSTEM_ALLOCATOR_CLASSIC_ARENA_H */
