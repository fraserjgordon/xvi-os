#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_PAGETABLE_H
#define __SYSTEM_ALLOCATOR_GALLOC_PAGETABLE_H


#include <System/C++/Atomic/Atomic.hh>
#include <System/C++/Utility/Algorithm.hh>

#include <System/Allocator/GAlloc/Config.hh>


namespace __GALLOC_NS_DECL
{

// Page table entries are always the size of a pointer.
constexpr std::size_t PageTableEntriesPerPage = PointersPerPage;
constexpr std::size_t PageTableEntrySize      = sizeof(std::uintptr_t);

// Number of levels in the table, assuming all levels are the same size.
constexpr std::size_t MaxPageTableLevels = ((PointerBits - PageOrder) + AddressBitsPerPage - 1) / AddressBitsPerPage;

// The top level of the page table may not be full. Calculate how many bits it decodes.
constexpr std::size_t TopPageTableDefaultBits = (PointerBits - PageOrder - AddressBitsPerPage * (MaxPageTableLevels - 1));

// If the top level is not full, an expanded top level is used instead. This (slightly) increases memory consumption but
// does save a level of indirection.
constexpr bool UnequalTopPageTable = (TopPageTableDefaultBits != AddressBitsPerPage);
constexpr std::size_t TopPageTableLevels = 1 + (UnequalTopPageTable ? 1 : 0) + PageTableCollapseTopLevels;
constexpr std::size_t TopPageTableBits = ((TopPageTableLevels - 1) * AddressBitsPerPage) + TopPageTableDefaultBits;
constexpr std::size_t TopPageTableEntries = std::size_t(1) << TopPageTableBits;
constexpr std::size_t TopPageTableSize = TopPageTableEntries * PageTableEntrySize;

// The actual number of levels of page tables, given level collapsing in the top level table.
constexpr std::size_t PageTableLevels = MaxPageTableLevels - TopPageTableLevels + 1;


enum class PageType : std::uint8_t
{
    Pool        = 0,        //!< "Small" allocations (from a pool).
    Medium      = 1,        //!< Non-pooled but managed allocations.
    Direct      = 2,        //!< Direct virtual allocation.
    SubAlloc    = 3,        //!< Managed by a registered sub-allocator.
};


class PageTableBase
{
protected:

    static std::uintptr_t allocateTable();
    static void freeTable(std::uintptr_t);

    [[noreturn]] static void error(const char* message);
    [[noreturn]] static void error(std::size_t level, std::uintptr_t address);
};

template <std::size_t Level, std::size_t EntryCount = (Level == 0 ? TopPageTableEntries : PageTableEntriesPerPage)>
class PageTable
    : private PageTableBase
{
public:

    // Set if all entries at this table level are leaves.
    static constexpr bool LastLevel = (Level == PageTableLevels - 1);

    // Next page table level (if appropriate).
    using NextPageTable = std::conditional_t<!LastLevel, PageTable<Level+1>, void>;

    // Page table entry format.
    //
    // Note that leaf and internal pages have different numbers of address bits.
    static constexpr std::uintptr_t FlagsMask       = (std::uintptr_t(1) << PageOrder) - 1;
    static constexpr std::uintptr_t AddressMask     = ~FlagsMask;
    static constexpr std::uintptr_t FlagsMaskLeaf   = 0x00F;
    static constexpr std::uintptr_t AddressMaskLeaf = ~FlagsMaskLeaf;

    // These flags are present in both leaf and internal pages.
    static constexpr std::uintptr_t FlagValid   = 0x001;    //!< This entry is valid.
    static constexpr std::uintptr_t FlagLeaf    = 0x002;    //!< This entry is a leaf even if not at the last level.
    static constexpr std::uintptr_t FlagType    = 0x00C;    //!< Allocation type information.
    static constexpr std::uintptr_t FlagTypeShift = 2;

    // These flags are only present in internal pages.
    // --- none yet --- //

    // Shift value and mask for extracting the index bits at this table level.
    static constexpr std::size_t LevelFromBottom = PageTableLevels - Level - 1;
    static constexpr std::size_t IndexShift = (PageOrder + LevelFromBottom * AddressBitsPerPage);
    static constexpr std::size_t IndexBits = (Level == 0 ? TopPageTableBits : AddressBitsPerPage);
    static constexpr std::size_t IndexMask = (std::size_t(1) << IndexBits) - 1;

    // Size of the virtual address space span controlled by each entry at this level.
    static constexpr std::size_t EntrySpanSize = (std::size_t(1) << IndexBits);


    //! Looks up the entry for the given address and returns the stored pointer, if it exists in the table. If not,
    //! nullptr is returned.
    std::pair<void*, PageType> lookup(std::uintptr_t address) const
    {
        // Extract the index bits for this level of the table and look up the entry.
        auto index = getIndex(address);
        auto entry = m_entries[index].load(std::memory_order::relaxed);

        // If the entry is not valid, ignore its contents.
        if (!(entry & FlagValid))
            return {nullptr, PageType::Pool};

        // If this is the lowest level, ignore the leaf flag as it is set implicitly.
        if constexpr (LastLevel)
        {
            return getAddressAndType(entry);
        }
        else
        {
            // If this is a leaf, return without recursing any further.
            if (entry & FlagLeaf)
            {
                return getAddressAndType(entry);
            }
            
            // Otherwise, check the next level.
            auto next = reinterpret_cast<NextPageTable*>(entry & AddressMask);
            return next->lookup(address);
        }
    }

    //! Inserts one or more entries into the table, all with the same associated pointer.
    //!
    //! Important: any address must be aligned to a 16-byte boundary as the low bits are used for flags!
    void insert(std::uintptr_t address, std::size_t size, void* ptr, PageType type)
    {
        // Validate that the address meets the pre-conditions.
        if constexpr (ErrorChecking)
        {
            if (reinterpret_cast<std::uintptr_t>(ptr) & FlagsMaskLeaf) [[unlikely]]
                error("insert: address is not aligned; cannot be inserted");
        }
        
        // The pointer is more easily manipulated as an integer.
        auto value = reinterpret_cast<std::uintptr_t>(ptr);
        value |= static_cast<std::uintptr_t>(type) << FlagTypeShift;

        // There may be multiple entries needed at this level so loop over them all.
        while (size > 0)
        {
            // Is the address suitably aligned and the size large enough to warrant a "large page" entry here?
            if constexpr (!LastLevel)
            {
                if (isLargeCandidate(address, size))
                {
                    // Instead of recursing, create a "large page" entry in this table.
                    std::uintptr_t entry = value | FlagValid | FlagLeaf;
                    m_entries[getIndex(address)].store(entry, std::memory_order::relaxed);
                    address += EntrySpanSize;
                    size -= EntrySpanSize;
                    continue;
                }

                // Otherwise, we'll need to recurse into the lower tables. Does one exist yet?
                auto index = getIndex(address);
                auto entry = m_entries[index].load(std::memory_order::relaxed);
                while (!(entry & FlagValid))
                {
                    // Allocate a new table and generate the entry for it.
                    auto table = allocateTable();
                    auto new_entry = table | FlagValid;

                    // Attempt to add the new entry but be prepared for a concurrent update from another writer. If that
                    // happens, free the table we just allocated.
                    //
                    // Because this is a strong compare-exchange and (we assume) that concurrent updates which do not
                    // set the Valid flag are unlikely, no attempt has been made to optimise for this case (which will
                    // cause the allocated table to be free'd and then a fresh one allocated again).
                    if (!m_entries[index].compare_exchange_strong(entry, new_entry, std::memory_order::acquire)) [[unlikely]]
                        freeTable(table);
                }

                // Table consistency sanity check.
                if constexpr (ErrorChecking)
                {
                    if (entry & FlagLeaf) [[unlikely]]
                        error(address);
                }

                // Recurse into the sub-table.
                auto sub_table = reinterpret_cast<NextPageTable*>(entry & AddressMask);
                sub_table->insert(address, std::min(size, EntrySpanSize), ptr, type);
                address += EntrySpanSize;
                size -= std::min(size, EntrySpanSize);
            }
            else
            {
                // This is the bottom-level table; update the leaf entry.
                auto entry = value | FlagValid;
                m_entries[getIndex(address)].store(entry, std::memory_order::relaxed);
                address += EntrySpanSize;
                size -= EntrySpanSize;
            }
        }

        // Final memory barrier to ensure consistent ordering against other operations.
        if constexpr (Level == 0)
            std::atomic_thread_fence(std::memory_order::acquire);
    }

private:

    using PageTableBase::error;

    std::atomic<std::uintptr_t> m_entries[EntryCount];


    //! Extracts the page type from a page table entry.
    static constexpr PageType getType(std::uintptr_t entry)
    {
        return static_cast<PageType>((entry & FlagType) >> FlagTypeShift);
    }

    //! Extracts the address from a page table entry.
    static void* getAddress(std::uintptr_t entry)
    {
        if constexpr (LastLevel)
        {
            return reinterpret_cast<void*>(entry & AddressMaskLeaf);
        }
        else
        {
            if (entry & FlagLeaf)
                return reinterpret_cast<void*>(entry & AddressMaskLeaf);
            else
                return reinterpret_cast<void*>(entry & AddressMask);
        }
    }

    //! Extracts the address and page type from a page table entry.
    static std::pair<void*, PageType> getAddressAndType(std::uintptr_t entry)
    {
        return {getAddress(entry), getType(entry)};
    }

    //! Given an address, returns the appropriate index at this level of the page table.
    static constexpr std::size_t getIndex(std::uintptr_t address)
    {
        return (address >> IndexShift) & IndexMask;
    }

    //! Returns true if the given (address, size) pair is a candidate for a "large" entry.
    static constexpr bool isLargeCandidate(std::uintptr_t address, std::size_t size)
    {
        // As well as the size being right, the address needs to be properly aligned.
        return (size >= EntrySpanSize && (address & (EntrySpanSize - 1)) == 0);
    }

    // Wrapper for the PageTableBase::error method.
    [[noreturn]] static void error(std::uintptr_t address)
    {
        PageTableBase::error(Level, address);
    }
};


// Returns the top-level page table object.
using TopPageTable = PageTable<0>;
TopPageTable* topPageTable();


} // namespace __GALLOC_NS


#endif // ifndef __SYSTEM_ALLOCATOR_GALLOC_PAGETABLE_H
