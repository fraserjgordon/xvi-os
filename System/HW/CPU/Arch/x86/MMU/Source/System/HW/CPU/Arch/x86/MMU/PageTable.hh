#ifndef __SYSTEM_HW_CPU_ARCH_X86_MMU_PAGETABLE_H
#define __SYSTEM_HW_CPU_ARCH_X86_MMU_PAGETABLE_H


#include <atomic>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <System/HW/CPU/Arch/x86/MMU/MMU.hh>
#include <System/HW/CPU/Arch/x86/MMU/Types.hh>


namespace System::HW::CPU::X86::MMU
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


template <class T>
void lockEntry(T* entry)
{
    //! @todo implement
}

template <class T>
void unlockEntry(T* entry)
{
    //! @todo implement
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


template <class T, class PtrT, int Level>
struct table_t
{
    // Types used to represent physical and virtual addr`sses.
    using paddr_t   = typename T::paddr_t;
    using vaddr_t   = PtrT;
    using psize_t   = paddr_t;
    using vsize_t   = vaddr_t;


    // All paging tables are 4kB in size except for the PDPT in PAE mode (which has 4 entries of 8 bytes each).
    static constexpr bool        LegacyPDPT         = (sizeof(PtrT) == sizeof(std::uint32_t) && Level == 2);
    static constexpr std::size_t LeafPageSize       = 4096;
    static constexpr std::size_t TableSize          = LegacyPDPT ? 32 : LeafPageSize;
    static constexpr std::size_t EntryCount         = TableSize / sizeof(T);
    static constexpr std::size_t FullEntryCount     = LeafPageSize / sizeof(T);

    static constexpr std::size_t PtrBits            = std::numeric_limits<PtrT>::digits();
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
    bool addSubTable(PtrT address, paddr_t page)
        requires (!Leaf)
    {
        // Sub-tables are always configured with the most permissive flags (user, write, not no-execute) so that the
        // permissions are controlled entirely by the permission bits at the final level.
        constexpr auto SubTableFlags = entry_t::Present | entry_t::Write | entry_t::User;

        // PAE page directory pointer table (PDPT) entries require a different set of flags.
        auto flags = SubTableFlags;
        if constexpr (LegacyPDPT)
            flags = entry_t::Present;

        // Update the applicable entry to point to the new subtable.
        const entry_t entry {page | flags};
        entry_t expected {0};
        if (!entries[indexForAddress(address)].compare_exchange_strong(expected, entry, std::memory_order::relaxed))
            return false;

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
            entry.setAddress(address);
            entries[index].store(entry, std::memory_order_relaxed);

            return true;
        }
        else if constexpr (!Leaf)
        {
            // Validate the page size.
            if (page_size > next_t::PageSize)
                return false;

            // We need to recurse into a lower-level table. Does it exist yet?
            auto index = indexForAddress(address);
            auto entry = entries[index].load(std::memory_order::relaxed);
           
            // Attempt to allocate a sub-table if needed.
            paddr_t subtable_addr = entry.isPresent() ? entry.address() : 0;
            next_t* subtable_ptr = next_t::mapFrom(address, subtable_addr, page_mapper);
            if (!entry.isPresent())
            {
                // Allocate a new sub-table.
                if (subtable_addr == 0)
                {
                    subtable_addr = page_allocator.allocate();
                    subtable_ptr = next_t::mapFrom(address, subtable_addr, page_mapper);
                }

                // Insert it.
                if (!addSubTable(address, subtable_addr))
                {
                    page_allocator.free(subtable_addr);
                    return false;
                }
            }

            // Lock the entry and recurse into the next level of page tables.
            //
            // After locking, we release the lock on the parent entry so that independent parts of the page tables can
            // be modified in parallel.
            lockEntry(&raw_entries[index]);
            if (parent_entry)
                unlockEntry(parent_entry);

            return subtable_ptr->addMapping(address, page, page_size, flags, pat, page_allocator, page_mapper, &raw_entries[index]);
        }

        // None of the expected conditions applied...
        return false;
    }


    template <class Mapper>
    static table_t* mapFrom(vaddr_t address, paddr_t page, const Mapper& page_mapper)
    {
        return reinterpret_cast<table_t*>(page_mapper.map(address, PageSize, page));
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
class PageTable
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


    static PageTable fromRoot(paddr_t root)
    {
        return PageTable{root};
    }

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


    explicit constexpr PageTable(paddr_t root) :
        m_root{root}
    {
    }

    template <class Mapper>
    root_t* rootTable(const Mapper& page_mapper)
    {
        return reinterpret_cast<root_t*>(page_mapper.rootTableAddress(m_root));
    }
};


using PageTableLegacy   = PageTable<pd32_t>;
using PageTablePAE      = PageTable<pdpt32pae_t>;
using PageTableLongMode = PageTable<pml4_t>;
using PageTableLongMode57 = PageTable<pml5_t>;


template <class V, class P>
class IdentityMapper
{
public:

    V map(V, P, P table_physical_addr) const noexcept
    {
        return static_cast<V>(table_physical_addr);
    }

    V rootTableAddress(P root) const noexcept
    {
        return static_cast<V>(root);
    }
};


} // namespace System::HW::CPU::X86::MMU


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_MMU_PAGETABLE_H */
