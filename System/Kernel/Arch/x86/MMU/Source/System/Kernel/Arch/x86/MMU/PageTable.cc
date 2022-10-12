#include <System/Kernel/Arch/x86/MMU/PageTable.hh>
#include <System/Kernel/Arch/x86/MMU/PageTableImpl.hh>

#include <System/Kernel/Arch/x86/MMU/TLB.hh>


namespace System::Kernel::X86::MMU
{


template <class V, class P>
static bool isAligned(V address, V size, P physical, std::type_identity_t<P> page_size)
{
    auto mask = (page_size - 1);
    return (address & mask) == 0
        && (physical & mask) == 0
        && (size >= page_size);
}


// Returns the best page size to use for the next page of the range.
template <class V, class P>
static P bestPageSize(const MMU& mmu, V address, V size, P physical)
{
    bool use2M = mmu.supports2MPages();
    bool use4M = mmu.supports4MPages();
    bool use1G = mmu.supports1GPages();

    if (use1G && isAligned(address, size, physical, 1U<<30))
        return 1U<<30;
    
    if (use4M && isAligned(address, size, physical, 4U<<20))
        return 4U<<20;

    if (use2M && isAligned(address, size, physical, 2U<<20))
        return 2U<<20;

    return 4U<<10;
}

template <class PageTableType, class V = PageTableType::vaddr_t, class P = PageTableType::paddr_t>
static bool addOneMappingImpl(PageTableType& root_table, V address, P page_size, P physical, page_flags flags, unsigned int pat, TablePageAllocator& page_allocator, RecursiveMapper<PageTableType>& mapper)
{
    // Perform the mapping, using an identity mapping for the physical -> virtual addresses.
    return root_table.mapOnePage(address, physical, page_size, flags, pat, page_allocator, mapper);
}

template <class PageTableType, class V = PageTableType::vaddr_t, class P = PageTableType::paddr_t>
static bool mapRangeImpl(MMU& mmu, P root, V address, V size, P physical, page_flags flags, unsigned int pat, V mapped_at, std::size_t recurse_idx)
{
    // Wrap the root table into an object.
    auto root_table = PageTableType::fromRoot(root);

    // Create the mapper object.
    RecursiveMapper<PageTableType> mapper{mapped_at, recurse_idx};

    // Loop over the range until we've mapped all of it.
    while (size > 0)
    {
        // What's the best size of page to use for the next mapping?
        auto page_size = bestPageSize(mmu, address, size, physical);

        // Map this page.
        if (!addOneMappingImpl(root_table, address, page_size, physical, flags, pat, *mmu.allocator(), mapper))
            return false;

        // Move on to the next page.
        address += static_cast<V>(page_size);
        size -= static_cast<V>(page_size);
        physical += page_size;
    }

    // All pages mapped successfully.
    return true;
}

template <class PageTableType, class V = PageTableType::vaddr_t, class P = PageTableType::paddr_t>
static bool unmapRangeImpl(MMU& mmu, P root, V address, V size, V mapped_at, std::size_t recurse_idx)
{
    // Wrap the root table into an object.
    auto root_table = PageTableType::fromRoot(root);

    // Create the mapper object.
    RecursiveMapper<PageTableType> mapper{mapped_at, recurse_idx};

    return root_table.unmapPages(address, size, mapper);
}

static bool mapRange(MMU& mmu, paddr_t root, vaddr_t address, vsize_t size, paddr_t physical, page_flags flags, unsigned int pat, vaddr_t mapped_at, std::size_t recurse_idx)
{
    asm goto
    (
        RUNPATCH_START_JUMP(PATCH_PAGING_MODE)
    #if defined(__x86_64__)
        RUNPATCH_DEFAULT_JUMP(lm)
        RUNPATCH_ALTERNATIVE_JUMP(lm57)
    #else
        RUNPATCH_DEFAULT_JUMP(legacy)
        RUNPATCH_ALTERNATIVE_JUMP(pae)
        RUNPATCH_ALTERNATIVE_JUMP(lm)
        RUNPATCH_ALTERNATIVE_JUMP(lm57)
    #endif
        RUNPATCH_FINISH_JUMP()
        :
        :
        :
        :
    #if !defined(__x86_64__)
            legacy, pae,
    #endif
            lm, lm57
    );

    __builtin_unreachable();

#if !defined(__x86_64__)

    {
        legacy:

        return mapRangeImpl<PageTableLegacy>(mmu, static_cast<std::uint32_t>(root), static_cast<std::uint32_t>(address), static_cast<std::uint32_t>(size), static_cast<std::uint32_t>(physical), flags, pat, mapped_at, recurse_idx);
    }

    {
        pae:

        return mapRangeImpl<PageTablePAE>(mmu, root, static_cast<std::uint32_t>(address), static_cast<std::uint32_t>(size), physical, flags, pat, mapped_at, recurse_idx);
    }

#endif

    {
        lm:

        return mapRangeImpl<PageTableLongMode>(mmu, root, address, size, physical, flags, pat, mapped_at, recurse_idx);
    }

    {
        lm57:

        asm volatile ("xchgw %%bx, %%bx" : : "a"(3) : "memory");
        return mapRangeImpl<PageTableLongMode57>(mmu, root, address, size, physical, flags, pat, mapped_at, recurse_idx);
    }
}


static bool unmapRange(MMU& mmu, paddr_t root, vaddr_t address, vsize_t size, vaddr_t mapped_at, std::size_t recurse_idx)
{
    asm goto
    (
        RUNPATCH_START_JUMP(PATCH_PAGING_MODE)
    #if defined(__x86_64__)
        RUNPATCH_DEFAULT_JUMP(lm)
        RUNPATCH_ALTERNATIVE_JUMP(lm57)
    #else
        RUNPATCH_DEFAULT_JUMP(legacy)
        RUNPATCH_ALTERNATIVE_JUMP(pae)
        RUNPATCH_ALTERNATIVE_JUMP(lm)
        RUNPATCH_ALTERNATIVE_JUMP(lm57)
    #endif
        RUNPATCH_FINISH_JUMP()
        :
        :
        :
        :
    #if !defined(__x86_64__)
            legacy, pae,
    #endif
            lm, lm57
    );

    __builtin_unreachable();

#if !defined(__x86_64__)

    {
        legacy:

        return unmapRangeImpl<PageTableLegacy>(mmu, static_cast<std::uint32_t>(root), static_cast<std::uint32_t>(address), static_cast<std::uint32_t>(size), mapped_at, recurse_idx);
    }

    {
        pae:

        return unmapRangeImpl<PageTablePAE>(mmu, root, static_cast<std::uint32_t>(address), static_cast<std::uint32_t>(size), mapped_at, recurse_idx);
    }

#endif

    {
        lm:

        return unmapRangeImpl<PageTableLongMode>(mmu, root, address, size, mapped_at, recurse_idx);
    }

    {
        lm57:

        return unmapRangeImpl<PageTableLongMode57>(mmu, root, address, size, mapped_at, recurse_idx);
    }
}


static unsigned int cacheTypeValue(MMU& mmu, cache_type type)
{
    return mmu.mapCacheType(type);
}


PageTable::~PageTable()
{
}


MappedPageTable::~MappedPageTable()
{
}


bool MappedPageTable::add(const mapping_info& info) noexcept
{
    // Sanity-check the parameters.
    if ((info.address & (PageSize - 1)) || (info.physical & (PageSize - 1)) || (info.size & (PageSize - 1)))
        return false;
    
    return mapRange(*m_mmu, m_root, info.address, info.size, info.physical, info.flags, cacheTypeValue(*m_mmu, info.caching), m_address, m_recurseIndex);
}


bool MappedPageTable::remove(vaddr_t address, vsize_t size) noexcept
{
    // Sanity-check the parameters.
    if ((address & (PageSize - 1)) || (size & (PageSize - 1)))
        return false;

    bool result = unmapRange(*m_mmu, m_root, address, size, m_address, m_recurseIndex);

    //! @todo implement TLB flushes.
    return false;
}


} // namespace System::Kernel::x86::MMU
