#ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLEIMPL_H
#define __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLEIMPL_H


#include <atomic>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <System/Kernel/Arch/x86/MMU/MMU.hh>
#include <System/Kernel/Arch/x86/MMU/Ops.hh>
#include <System/Kernel/Arch/x86/MMU/Types.hh>


namespace System::Kernel::X86::MMU
{


struct PTE
{
    static constexpr std::uint32_t Present      = 0x001;
    static constexpr std::uint32_t Write        = 0x002;
    static constexpr std::uint32_t User         = 0x004;
    static constexpr std::uint32_t CacheBit0    = 0x008;
    static constexpr std::uint32_t CacheBit1    = 0x010;
    static constexpr std::uint32_t Accessed     = 0x020;
    static constexpr std::uint32_t Dirty        = 0x040;
    static constexpr std::uint32_t CacheBit2    = 0x080;
    static constexpr std::uint32_t Global       = 0x100;

    // The following flags are not used by the hardware and are available for software use.
    static constexpr std::uint32_t UserFlag0    = 0x200;
    static constexpr std::uint32_t UserFlag1    = 0x400;
    static constexpr std::uint32_t UserFlag2    = 0x800;

    // Despite being allocated as a user-defined flag, MMUs that are using HLAT paging define a hardware purpose: stop
    // HLAT and restart this lookup using ordinary paging.
    static constexpr std::uint32_t HLATRestart  = 0x800;

    // This is defined so all entry types have it but defining it as zero here means that any tests against this bit
    // will always be false.
    static constexpr std::uint32_t LargePage    = 0x000;

    // The no-execute bit is only available in PAE or Long Mode MMUs.
    static constexpr std::uint64_t NoExecute    = std::uint64_t(1) << 63;

    // The memory protection key is only available in Long Mode MMUs.
    static constexpr std::uint64_t PKeyMask     = 0x78000000'00000000;
    static constexpr std::size_t   PKeyShift    = 59;

    template <class T>
    static constexpr T AddressMask = static_cast<T>(0x01ffffff'fffff000);

    template <class T>
    static constexpr T LargeAddressMask = AddressMask<T>;
};

// PDEs have almost the same bit layout as PTEs.
struct PDE : PTE
{
    // The LargePage bit replaces the PTE CacheBit2 so CacheBit2 is moved elsewhere.
    static constexpr std::uint32_t LargePage    = 0x080;
    static constexpr std::uint32_t CacheBit2    = 0x1000;

    // The 32-bit mask for large pages is complex due to swizzling used as part of the PSE36 extension.
    template <class T>
    static constexpr T LargeAddressMask = (sizeof(T) == sizeof(std::uint32_t)) ? 0xffdfe000 : 0x000ffff'ffe00000;

    static constexpr std::size_t   PseSwizzleShift  = 19;
    static constexpr std::uint32_t PseSwizzleMask   = 0x001fe000;
};

// PDPEs use the same bits as PDEs.
struct PDPE : PDE
{
    template <class T>
    static constexpr T LargeAddressMask = static_cast<T>(0x01ffffff'c0000000);
};

// PML4Es use the same bits as PDPEs.
struct PML4E : PDPE
{
    // There is no hardware support for 512GB pages but they're not architecturally impossible.
    template <class T>
    static constexpr T LargeAddressMask = static_cast<T>(0x01ffff80'00000000);
};

// PML5Es use the same bits as PML4Es.
struct PML5E : PML4E
{
    // There is no hardware support for 256TB pages but they're not architecturally impossible.
    template <class T>
    static constexpr T LargeAddressMask = static_cast<T>(0x01ff0000'00000000);
};


template <class T, class ModelType>
struct entry_t
{
    using Model = ModelType;

    static constexpr T AddressMask = Model::template AddressMask<T>;
    
    // Import bit definitions from the model.
    static constexpr auto Present   = Model::Present;
    static constexpr auto Write     = Model::Write;
    static constexpr auto User      = Model::User;
    static constexpr auto CacheBit0 = Model::CacheBit0;
    static constexpr auto CacheBit1 = Model::CacheBit1;
    static constexpr auto CacheBit2 = Model::CacheBit2;
    static constexpr auto Accessed  = Model::Accessed;
    static constexpr auto Dirty     = Model::Dirty;
    static constexpr auto Global    = Model::Global;
    static constexpr auto LargePage = Model::LargePage;
    static constexpr auto NoExecute = Model::NoExecute;
    static constexpr auto PKeyMask  = Model::PKeyMask;
    static constexpr auto PKeyShift = Model::PKeyShift;

    // One of the user-defined flags is used as a spinlock bit. When set, the entry is being manipulated and should not
    // be modified until the bit is cleared.
    static constexpr std::uint32_t Lock         = Model::UserFlag0;

    // One of the user-defined flags is used as a pinning bit. When set in an entry pointing to a page table (i.e this
    // bit is not used in leaf pages), it indicates that the page table should not be freed nor moved. This is used, for
    // example, for the entries in a PAE page directory that hold the four PDPT entries - the CPU loads those pointers
    // when %cr3 is loaded so freeing them is not safe.
    static constexpr std::uint32_t PinnedTable  = Model::UserFlag1;

    // Do we need to worry about bit-swizzling because of the PSE36 feature? It only applies for legacy mode and only
    // for PDEs that are large pages.
    static constexpr bool Pse36Swizzle = (sizeof(T) == sizeof(std::uint32_t)) && std::is_same_v<Model, PDE>;
    
    using   paddr_t = T;
    
    T       raw;


    constexpr bool isPresent()
    {
        return (raw & Model::Present);
    }

    constexpr bool isLargePage()
    {
        return (raw & Model::LargePage);
    }

    constexpr paddr_t address()
    {
        // The PSE36 extension for legacy-mode MMUs has a non-trivial layout for the address.
        if constexpr (Pse36Swizzle)
        {
            if (isLargePage())
            {
                auto lower = (raw & AddressMask) & ~Model::PseSwizzleMask;
                auto upper = (raw & AddressMask) &  Model::PseSwizzleMask;

                return (static_cast<paddr_t>(upper) << Model::PseSwizzleShift) | lower;
            }
        }

        return (raw & AddressMask);
    }

    void setAddress(paddr_t address)
    {
        // The PSE36 extension for legacy-mode MMUs has a non-trivial layout for the address.
        if constexpr (Pse36Swizzle)
        {
            if (isLargePage())
            {
                auto lower = static_cast<paddr_t>(address);
                auto upper = (address >> Model::PseSwizzleShift) & Model::PseSwizzleMask;

                raw = (raw & ~AddressMask) | lower | upper;
                return;
            }
        }

        raw = (raw & ~AddressMask) | address;
    }
};


//! @todo RAII wrappers for locking.
template <class T>
T lockEntry(std::atomic<T>* entry)
{
    //! @todo implement
    return entry->load(std::memory_order_acquire);
}

template <class T>
void unlockEntry(T* /*entry*/)
{
    //! @todo implement
}

template <class T>
void writeAndUnlockEntry(std::atomic<T>* entry, T value)
{
    entry->store(value, std::memory_order_release);
}

template <class T>
struct pte_t : entry_t<T, PTE>
{
    using next_t = void;
};

using pte32_t = pte_t<std::uint32_t>;
using pte64_t = pte_t<std::uint64_t>;

template <class T>
struct pde_t : entry_t<T, PDE>
{
    using next_t = pte_t<T>;
};

using pde32_t = pde_t<std::uint32_t>;
using pde64_t = pde_t<std::uint64_t>;

template <class T>
struct pdpe_t : entry_t<T, PDPE>
{
    using next_t = pde_t<T>;
};

using pdpe32_t = pdpe_t<std::uint32_t>;
using pdpe64_t = pdpe_t<std::uint64_t>;

template <class T>
struct pml4e_t : entry_t<T, PML4E>
{
    using next_t = pdpe_t<T>;
};

using pml4e64_t = pml4e_t<std::uint64_t>;

template <class T>
struct pml5e_t : entry_t<T, PML5E>
{
    using next_t = pml4e_t<T>;
};

using pml5e64_t = pml5e_t<std::uint64_t>;


struct table_walk_info
{
    vaddr_t     address = 0;
    paddr_t     physical = 0;
    int         level = 0;
    vsize_t     size = 0;
    bool        present = false;
    bool        is_table = false;
};


template <class T, class PtrT, int Level>
struct table_t
{
    // Types used to represent physical and virtual addr`sses.
    using paddr_t   = typename T::paddr_t;
    using vaddr_t   = PtrT;
    using psize_t   = paddr_t;
    using vsize_t   = vaddr_t;


    static constexpr int TableLevel = Level;

    // All paging tables are 4kB in size except for the PDPT in PAE mode (which has 4 entries of 8 bytes each).
    static constexpr bool        LegacyPDPT         = (sizeof(PtrT) == sizeof(std::uint32_t) && Level == 2);
    static constexpr std::size_t LeafPageBits       = 12;
    static constexpr std::size_t LeafPageSize       = (1U << LeafPageBits);
    static constexpr std::size_t TableSize          = LegacyPDPT ? 32 : LeafPageSize;
    static constexpr std::size_t EntryCount         = TableSize / sizeof(T);
    static constexpr std::size_t FullEntryCount     = LeafPageSize / sizeof(T);
    static constexpr std::size_t IndexMask          = (EntryCount - 1);

    static constexpr std::size_t PtrBits            = std::numeric_limits<PtrT>::digits;
    static constexpr std::size_t DecodedBits        = std::bit_width(EntryCount) - 1;
    static constexpr std::size_t FullDecodedBits    = std::bit_width(LeafPageSize / sizeof(T)) - 1;
    static constexpr std::size_t OffsetBits         = 12 + (Level * FullDecodedBits);

    // The size of pages at this level of the paging hierarchy.
    static constexpr std::size_t PageShift          = OffsetBits;
    static constexpr psize_t     PageSize           = (psize_t(1) << OffsetBits);

    // The table at level 0 never points to another layer of tables.
    static constexpr bool        Leaf               = (Level == 0);


    // Type of entries in this table.
    using entry_t   = T;

    // Type of the next-lower table.
    using next_t    = std::conditional_t<Leaf, void, table_t<typename T::next_t, PtrT, Level - 1>>;


    //! @brief  Adds a sub-table to the current table.
    //!
    //! @remark The mapping given in the \p mapping parameter does not need to be valid until after the sub-table has
    //!         been added to the table. This is to permit mapping by means of an entry in the root table pointing back
    //!         at the root table.
    //!
    //! @pre    The page being supplied for the sub-table must be zeroed.
    //!
    //! @note   Only non-leaf tables can have sub-tables.
    //!
    //! @param  address     the virtual address that the sub-table will map
    //! @param  page        the page of physical memory to hold the sub-table
    //! @param  mapping     the virtual address at which \p page may be accessed
    //!
    bool addSubTable(PtrT address, paddr_t page, paddr_t extra_flags = 0)
        requires (!Leaf)
    {
        // Sub-tables are always configured with somewhat permissive flags (in particular: writeable) so that the
        // permissions are controlled entirely by the permission bits at the final level.
        constexpr auto SubTableFlags = entry_t::Present | entry_t::Write;

        // PAE page directory pointer table (PDPT) entries require a different set of flags.
        auto flags = SubTableFlags;
        if constexpr (LegacyPDPT)
        {
            flags &= ~entry_t::Write;
            extra_flags &= ~(entry_t::User | entry_t::Write | entry_t::NoExecute);
        }

        // Update the applicable entry to point to the new subtable.
        auto index = indexForAddress(address);
        const entry_t entry {page | flags | extra_flags};
        writeAndUnlockEntry(&entries[index], entry);

        // No TLB flushing is needed as there was no previous entry.
        return true;
    }

    template <class Mapper>
    bool addMapping(vaddr_t address, paddr_t page, psize_t page_size, page_flags flags, unsigned int pat, TablePageAllocator& page_allocator, const Mapper& page_mapper, paddr_t* parent_entry)
    {
        // To add this mapping, are we adding the page at this level or at a lower level?
        if (page_size == PageSize)
        {
            // We're adding a mapping to this table. Convert the flags into the right form.
            paddr_t entry_flags = entry_t::LargePage;   // Will be zero for 4kB pages.
            entry_flags |= entry_t::Present;
            if (flags & PageFlag::U)
                entry_flags |= entry_t::User;
            if (flags & PageFlag::W)
                entry_flags |= entry_t::Write;
            if ((flags & PageFlag::X) == 0)
                entry_flags |= entry_t::NoExecute;
            if (flags & PageFlag::G)
                entry_flags |= entry_t::Global;
            if (pat & 0b001)
                entry_flags |= entry_t::CacheBit0;
            if (pat & 0b010)
                entry_flags |= entry_t::CacheBit1;
            if (pat & 0b100)
                entry_flags |= entry_t::CacheBit2;

            // Write the entry, assuming the parent entry has been locked.
            auto index = indexForAddress(address);
            entry_t entry {entry_flags};
            entry.setAddress(page);
            writeAndUnlockEntry(&entries[index], entry);

            return true;
        }
        else if constexpr (!Leaf)
        {
            // Validate the page size.
            if (page_size > next_t::PageSize)
                return false;

            // We need to recurse into a lower-level table. Does it exist yet?
            auto index = indexForAddress(address);
            auto entry = lockEntry(&entries[index]);
           
            // Attempt to allocate a sub-table if needed.
            paddr_t subtable_addr = entry.isPresent() ? entry.address() : 0;
            next_t* subtable_ptr = next_t::mapFrom(address, subtable_addr, page_mapper);
            if (!entry.isPresent())
            {
                // Allocate a new sub-table.
                if (subtable_addr == 0)
                {
                    subtable_addr = page_allocator.allocate();

                    if (subtable_addr == 0)
                        return false;

                    if constexpr (Mapper::DependsOnPhysicalAddress)
                        subtable_ptr = next_t::mapFrom(address, subtable_addr, page_mapper);
                }

                // Calculate flags to add to the subtable.
                paddr_t subtable_flags = 0;
                if (flags & PageFlag::U)
                    subtable_flags |= next_t::entry_t::User;

                // Insert it.
                if (!addSubTable(address, subtable_addr, subtable_flags))
                {
                    page_allocator.free(subtable_addr);
                    return false;
                }
            }
            else if (false)
            {
                //! @todo handle subtables that need their flags altered (in particular, adding the User bit).
            }

            // Lock the entry and recurse into the next level of page tables.
            //
            // After locking, we release the lock on the parent entry so that independent parts of the page tables can
            // be modified in parallel.
            lockEntry(&entries[index]);
            if (parent_entry)
                unlockEntry(parent_entry);

            return subtable_ptr->addMapping(address, page, page_size, flags, pat, page_allocator, page_mapper, &raw_entries[index]);
        }

        // None of the expected conditions applied...
        return false;
    }

    template <class Mapper, class Callback>
        requires std::invocable<const Callback&, const table_walk_info&>
    bool walk(vaddr_t address, vsize_t size, const Mapper& page_mapper, const Callback& callback)
    {
        // This check is needed to prevent overflow when calculating the end.
        if (size == 0)
            return true;

        // Minus one here to allow walking the page table right up to the maximum address.
        auto end = address + size - 1;
        while (address < end)
        {
            // Find and lock the entry corresponding to this address.
            auto index = indexForAddress(address);
            auto entry = lockEntry(&entries[index]);

            // Extract the entry information.
            table_walk_info info {};
            info.address = address;
            info.physical = entry.address();
            info.size = PageSize;
            info.level = TableLevel;
            info.present = entry.isPresent();
            info.is_table = info.present && !Leaf && !entry.isLargePage();

            // Pass it to the callback.
            bool more = callback(info);
            if (more)
            {
                // If this entry points into a sub-table, recurse into it.
                if constexpr (!Leaf)
                {
                    if (info.is_table)
                    {
                        auto subtable = next_t::mapFrom(address, info.physical, page_mapper);
                        more = subtable->walk(address, std::min(PageSize, size), callback);
                    }
                }
            }

            unlockEntry(&entries[index]);

            if (!more)
                return false;

            address += PageSize;
            size -= PageSize;
        }

        return true;
    }


    template <class Mapper>
    bool removeMapping(vaddr_t address, vsize_t size, const Mapper& page_mapper)
    {
        // Get and lock the entry for this address.
        auto index = indexForAddress(address);
        auto entry = lockEntry(&entries[index]);

        // Do we have a valid entry?
        if (entry.isPresent())
        {
            // Does this entry describe a page or a sub-table?
            if constexpr (!Leaf)
            {
                if (entry.isLargePage())
                {
                    // Check that we're removing the entire large page.
                    if (size != PageSize)
                    {
                        // We were expecting to remove a page from a sub-table but we have a large page instead. This
                        // method doesn't support implicit splitting of large pages (as that requires allocating a new
                        // page to act as the sub-table) so treat this as a failure.
                        unlockEntry(&entries[index]);
                        return false;
                    }

                    // Clear the entry. This will implicitly unlock it.
                    writeAndUnlockEntry(&entries[index], entry_t{0});
                    return true;
                }

                // We found a sub-table. Pass the removal request on to it.
                auto subtable = next_t::mapFrom(address, entry.address(), page_mapper);
                auto result = subtable->removeMappings(address, size, page_mapper);
                unlockEntry(&entries[index]);
                return result;
            }

            // Clear the entry. This will implicitly unlock it.
            writeAndUnlockEntry(&entries[index], entry_t{0});
        }

        return true;
    }

    template <class Mapper>
    bool removeMappings(vaddr_t address, vsize_t size, const Mapper& mapper)
    {
        // Loop over the requested mappings until all have been unmapped.
        vsize_t done = 0;
        while (done < size)
        {
            if (!removeMapping(address, std::min<vsize_t>(PageSize, size - done), mapper))
                return false;

            done += PageSize;
            address += PageSize;
        }

        return true;
    }


    template <class Mapper>
    static table_t* mapFrom(vaddr_t address, paddr_t page, const Mapper& page_mapper)
    {
        address &= ~(EntryCount*PageSize - 1);
        return reinterpret_cast<table_t*>(page_mapper.map(address, TableLevel, page));
    }

private:

    /*[[nodiscard]] bool lockEntry(std::size_t index)
    {
        auto entry = entries[index].load(std::memory_order::acquire);
        while (true)
        {
            // Non-present entries cannot be locked.
            if (!entry.isPresent())
                return false;

            // Is the entry already locked?
            if (entry.raw & entry_t::Lock) [[unlikely]]
            {
                // Spin until it becomes free.
                asm volatile ("pause" ::: "memory");
                continue;
            }

            const entry_t locked_entry {.raw = entry.raw | entry_t::Lock};
            if (entries[index].compare_exchange_strong(locked_entry, entry, std::memory_order_acquire)) [[likely]]
                return true;
        }
    }

    void writeAndUnlockEntry(std::size_t index, entry_t entry)
    {
        // Make sure that the lock flag is clear and write the entry,
        entry.raw &= ~entry_t::Lock;
        entries[index].store(entry, std::memory_order::release);
    }*/

    static constexpr std::size_t indexForAddress(PtrT address)
    {
        return (address >> OffsetBits) & ((PtrT(1) << DecodedBits) - 1);
    }


    // The table itself.
    union
    {
        std::atomic<entry_t>    entries[EntryCount];
        paddr_t                 raw_entries[EntryCount];
    };
};

// Legacy page tables: 32-bit physical and virtual addresses.
using pt32_t        = table_t<pte32_t, std::uint32_t, 0>;
using pd32_t        = table_t<pde32_t, std::uint32_t, 1>;

// PAE page tables: 64-bit physical addresses, 32-bit virtual addresses.
using pt32pae_t     = table_t<pte64_t, std::uint32_t, 0>;
using pd32pae_t     = table_t<pde64_t, std::uint32_t, 1>;
using pdpt32pae_t   = table_t<pdpe64_t, std::uint32_t, 2>;

// Long mode page tables: 64-bit physical and virtual addresses.
using pt64_t    = table_t<pte64_t, std::uint64_t, 0>;
using pd64_t    = table_t<pde64_t, std::uint64_t, 1>;
using pdpt64_t  = table_t<pdpe64_t, std::uint64_t, 2>;
using pml4_t    = table_t<pml4e64_t, std::uint64_t, 3>;
using pml5_t    = table_t<pml5e64_t, std::uint64_t, 4>;


template <class Root>
class PageTableImpl
{
public:

    // The root of the paging hierarchy.
    using root_t    = Root;

    // Types representing physical and virtual addresses. These may be different (e.g. in PAE mode).
    using paddr_t   = typename Root::paddr_t;
    using vaddr_t   = typename Root::vaddr_t;
    using psize_t   = paddr_t;
    using vsize_t   = vaddr_t;
    using pdiff_t   = std::make_signed<psize_t>;
    using vdiff_t   = std::make_signed<vsize_t>;

    static constexpr auto Levels = root_t::TableLevel + 1;

    //! @todo this class needs to make sure no mapping requests cross the canonical addressing boundary.
    static constexpr bool CanonicalAddressing = Root::OffsetBits < Root::PtrBits;
    static constexpr auto CanonicalBoundary = CanonicalAddressing ? (vaddr_t(1) << (Root::OffsetBits - 1)) : 0;


    paddr_t root() const noexcept
    {
        return m_root;
    }

    template <class Mapper>
    bool mapOnePage(vaddr_t address, paddr_t to, psize_t page_size, page_flags flags, unsigned int pat, TablePageAllocator& page_allocator, const Mapper& page_mapper)
    {
        auto table = rootTable(page_mapper);
        return table->addMapping(address, to, page_size, flags, pat, page_allocator, page_mapper, nullptr);
    }

    template <class Mapper>
    bool unmapPages(vaddr_t address, vsize_t size, const Mapper& page_mapper)
    {
        auto table = rootTable(page_mapper);
        return table->removeMappings(address, size, page_mapper);
    }


    static PageTableImpl fromRoot(paddr_t root)
    {
        return PageTableImpl{root};
    }

    /*static constexpr bool isCanonical(vaddr_t address)
    {
        if constexpr (!CanonicalAddressing)
            return true;

        
    }*/

private:

    // Physical address of the root page table. Although this is a physical address, the root address may be restricted
    // to a subset of the physical address space: this happens when the CPU is in 32-bit mode but larger physical
    // addresses are being used because the %cr3 register is still only 32 bits wide.
    //
    // Zero is, in theory, a valid address for a page table root but declaring it as unusable isn't likely to be a
    // problem.
    paddr_t m_root  = 0;

    // Index of the entry in the top-level table used to recursively map the page table itself.
    unsigned int m_selfMapIndex = 0;


    explicit constexpr PageTableImpl(paddr_t root) :
        m_root{root}
    {
    }

    template <class Mapper>
    root_t* rootTable(const Mapper& page_mapper)
    {
        return reinterpret_cast<root_t*>(page_mapper.rootTableAddress(m_root));
    }
};


using PageTableLegacy   = PageTableImpl<pd32_t>;
using PageTablePAE      = PageTableImpl<pdpt32pae_t>;
using PageTableLongMode = PageTableImpl<pml4_t>;
using PageTableLongMode57 = PageTableImpl<pml5_t>;


template <class V, class P>
class IdentityMapper
{
public:

    static constexpr bool DependsOnPhysicalAddress = true;
    static constexpr bool TlbFlushMap = false;

    V map(V, unsigned int, P table_physical_addr) const noexcept
    {
        return static_cast<V>(table_physical_addr - offset);
    }

    V rootTableAddress(P root) const noexcept
    {
        return static_cast<V>(root - offset);
    }

    std::uint64_t offset = 0;
};


template <class Table>
class RecursiveMapper
{
public:

    using V = typename Table::vaddr_t;
    using P = typename Table::paddr_t;

    using root_t = typename Table::root_t;


    static constexpr bool DependsOnPhysicalAddress = false;
    static constexpr bool TlbFlushMap = true;


    V map(V address, unsigned int level, P) const noexcept
    {
        constexpr auto LeafBits = root_t::LeafPageBits;
        constexpr auto IndexBits = root_t::FullDecodedBits;

        // Which bits from the address are we going to use to index into the recursive map? The higher the level of the
        // page table, the fewer bits we use because we will recurse more.
        auto offset = address >> ((level + 1) * IndexBits);

        // What kind of page table are we dealing with?
        if constexpr (std::is_same_v<Table, PageTableLegacy>)
        {
            // Classic paging; level will be either 0 or 1.
            if (level > 0)
                offset |= (recurse_index << LeafBits);
        }
        else if constexpr (std::is_same_v<Table, PageTablePAE>)
        {
            // We treat PAE page tables in a bit of an odd way here.
            //
            // See the note at the end of the long comment for long mode paging for the explanation of why we treat PAE
            // paging this way.

            // The root table for PAE needs special handling. What we return as the "root" is actually the set of four
            // self-map entries in one of the four page directories, which are not necessarily the same physical memory
            // as the PDPT itself, due to restrictions on the flags that are settable in PAE PDPTEs (setting the dirty
            // or accessed flags is invalid and the CPU can raise an exception). However, another CPU quirk comes into
            // play: older CPUs load all 4 PDPTEs into an internal cache when %cr3 is written so updating them requires
            // writing to %cr3 again (and therefore flushing the TLBs).
            //
            // To avoid this, this MMU library pre-allocates all four page directories and doesn't modify the PDPTEs
            // afterwards. As long as the four self-map entries are set up to match, we can pretend they are the real
            // PDPTEs without any ill effects (so long as we don't modify them).
            if (level == pdpt32pae_t::TableLevel)
            {
                // For a 13-bit recurse index (incorporating both the PDPT and PD bits) of 0b'abcdefghijk, the offset of
                // our pretend PDPT is:
                //  0b00'0000000ab'cdefghi(j+a)(k+b)'cdefghi00000
                offset = (recurse_index << LeafBits) + ((recurse_index << 3) & 0x3FE0);
            }
            else if (level == pd32pae_t::TableLevel)
            {
                // Pretend the 11-bit recurse index is simply an index into a 16kB / 2048 entry page directory.
                //
                // We need to add (rather than or) the offset here as the recursive index might not be on an 8MB address
                // space boundary.
                offset = (address >> 18) & 0x0000'3FFF; 
                offset += (recurse_index << LeafBits) & 0x007F'F000;
            }
            else
            {
                // No recurse offset for page tables.
            }
        }
        else if constexpr (std::is_same_v<Table, PageTableLongMode> || std::is_same_v<Table, PageTableLongMode57>)
        {
            // Magic constant with a bit set every 9 bits. If we multiply the recurse index by this, it'll supply the
            // repeated recurse index bits to reach the desired table, as explained below.
            //
            // In particular, remembering that the base address already represents a single level of recursion, we need
            // to apply this many additional recursions for each type of table:
            //  - page table: 0 recursions
            //  - page directory: 1 recursion
            //  - PDPT: 2 recursions
            //  - PML4: 3 recursions
            //  - PML5: 4 recursions
            //
            // We'll start off by thinking about 5-level page tables only, then generalise that to fewer levels.
            //
            // These recursions start at the high bits and work downwards, giving us the following bitpatterns (given
            // the recursion index is abcdefghi):
            //  - page table: 0b?????????'?????????'?????????'?????????'?????????'000000000000
            //  - page dir  : 0b?????????'abcdefghi'?????????'?????????'?????????'000000000000
            //  - PDPT      : 0b?????????'abcdefghi'abcdefghi'?????????'?????????'000000000000
            //  - PML4      : 0b?????????'abcdefghi'abcdefghi'abcdefghi'?????????'000000000000
            //  - PML5      : 0b?????????'abcdefghi'abcdefghi'abcdefghi'abcdefghi'000000000000
            //  - self map  : 0b?????????'abcdefghi'abcdefghi'abcdefghi'abcdefghi'abcdefghi000
            //
            // To generate these bit patterns, we can see that they would be formed by multiplying abcdefghi by:
            //  - page table: all zeroes
            //  - page dir  : 0b000000000'000000001'000000000'000000000'000000000'000000000000
            //  - PDPT      : 0b000000000'000000001'000000001'000000000'000000000'000000000000
            //  - PML4      : 0b000000000'000000001'000000001'000000001'000000000'000000000000
            //  - PML5      : 0b000000000'000000001'000000001'000000001'000000001'000000000000
            //
            // Because the multipliers are similar in form, we can store a single one (for the PML5) and generate the
            // rest from that (expressed here in octal to save space -- one of the few times octal is useful!):
            //  - PML5      : 0o000'001'001'001'001'0000 (or 0x0080'4020'1000 in hex)
            //
            // The bottom 12 bits (the page offset) are always zero so we can remove those as a common factor:
            //  - PML5      : 0o001'001'001'001 (0r 0x0'0804'0201 in hex)
            //
            // From this constant K, we can generate the constants for the other levels at runtime fairly cheaply:
            //  - page table: (K >> 36) << 36 (which will equal zero)
            //  - page dir  : (K >> 27) << 27
            //  - PDPT      : (K >> 18) << 18
            //  - PML4      : (K >>  9) <<  9
            //
            // Or, more generally, ((K >> x) << x) where x is (9 * (5 - TableLevel)).
            //
            // To generalise this further, let's consider the patterns for page directories in each type of paging:
            // - LM57  : 0b?????????'abcdefghi'?????????'?????????'?????????'000000000000
            // - LM48  : 0b          ?????????'abcdefghi'?????????'?????????'000000000000
            // - PAE   : 0b                           ??'abcdefghi'?????????'000000000000 (*see note below)
            //
            // If we strip off the 12 trailing zeroes, we can see that the pattern is rightshifted by 9 bits for every
            // one fewer level of page tables. Rewriting our shift, we have:
            //
            //      k(n; L) = (K >> (9 * (5 - n))) << (9 * (L - n))
            //
            //      where:
            //          n   is the page table level (PML5 = 5, PML4 = 4, PDPT = 3, PD = 2, PT = 1)
            //          L   is the number of paging levels (5 for LM57, 4 for LM48, 3 for PAE).
            //
            // Note on PAE:
            //
            //      We don't actually use this form of recursive mapping for PAE because it would consume a quarter of
            //      the 32-bit address space (30 bits) despite only having 12+9+2=23 bits of page tables (the difference
            //      is because the PDPT only decodes 2 bits, not 9 bits, giving us the 7 bit difference). Instead, we
            //      pretend (for the purposes of recursive mapping only!) that 4 page directories referenced by the PDPT
            //      are actually one big page directory that decodes 11 bits (with a 16kB table). So our pattern is:
            //          - page table: 0b?????????'???????????'000000000000
            //          - page dir  : 0b?????????'abcdefghijk'000000000000
            //          - self map 0: 0b?????????'abcdefghijk'cdefghi00000
            //          - self map 1: 0b?????????'abcdefghijk'cdefghi01000
            //          - self map 2: 0b?????????'abcdefghijk'cdefghi10000
            //          - self map 3: 0b?????????'abcdefghijk'cdefghi11000
            //
            //      As such, we handle PAE paging as being a bit more like 2-level legacy paging than the deeply-
            //      recusive long mode paging.
            constexpr auto K = P(0x0804'0201);

            constexpr auto L = root_t::TableLevel;

            auto rshift = IndexBits * (5 - (level + 1));
            auto lshift = IndexBits * (L - (level + 1));
            auto k = (K >> rshift) << lshift;
            offset |= (k * recurse_index) << LeafBits;
        }
        else
        {
            // Oops - something has gone wrong.
            static_assert(!std::is_same_v<Table, Table>, "unimplemented page table type");
        }

        return base + offset;
    }

    V rootTableAddress(P) const noexcept
    {
        return map(base, root_t::TableLevel, 0);
    }


    V               base;
    std::size_t     recurse_index;
};


} // namespace System::Kernel::X86::MMU


#endif /* ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_PAGETABLEIMPL_H */
