#include <System/HW/CPU/Arch/x86/MMU/InitPageTable.hh>

#include <System/HW/CPU/Arch/x86/MMU/MMU.hh>
#include <System/HW/CPU/Arch/x86/MMU/PageTable.hh>


namespace System::HW::CPU::X86::MMU
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
static bool addOneMappingImpl(PageTableType& root_table, V address, P page_size, P physical, page_flags flags, unsigned int pat, TablePageAllocator& page_allocator)
{
    // Perform the mapping, using an identity mapping for the physical -> virtual addresses.
    return root_table.mapOnePage(address, physical, page_size, flags, pat, page_allocator, IdentityMapper<V, P>{});
}

template <class PageTableType, class V = PageTableType::vaddr_t, class P = PageTableType::paddr_t>
static bool mapRangeImpl(MMU& mmu, P root, V address, V size, P physical, page_flags flags, unsigned int pat)
{
    // Wrap the root table into an object.
    auto root_table = PageTableType::fromRoot(root);

    // Loop over the range until we've mapped all of it.
    while (size > 0)
    {
        // What's the best size of page to use for the next mapping?
        auto page_size = bestPageSize(mmu, address, size, physical);

        // Map this page.
        if (!addOneMappingImpl(root_table, address, page_size, physical, flags, pat, *mmu.allocator()))
            return false;

        // Move on to the next page.
        address += static_cast<V>(page_size);
        size -= static_cast<V>(page_size);
        physical += page_size;
    }

    // All pages mapped successfully.
    return true;
}

static bool mapRange(MMU& mmu, std::uint64_t root, std::uint64_t address, std::uint64_t size, std::uint64_t physical, page_flags flags, unsigned int pat)
{
    // What paging mode are we targeting?
    switch (mmu.targetPagingMode())
    {
        case PagingMode::Disabled:
            // Umm... shouldn't happen.
            return false;

        case PagingMode::Legacy:
            return mapRangeImpl<PageTableLegacy>(mmu, static_cast<std::uint32_t>(root), static_cast<std::uint32_t>(address), static_cast<std::uint32_t>(size), static_cast<std::uint32_t>(physical), flags, pat);

        case PagingMode::PAE:
            return mapRangeImpl<PageTablePAE>(mmu, root, static_cast<std::uint32_t>(address), static_cast<std::uint32_t>(size), physical, flags, pat);

        case PagingMode::LongMode:
            return mapRangeImpl<PageTableLongMode>(mmu, root, address, size, physical, flags, pat);

        case PagingMode::LongMode5Level:
            return mapRangeImpl<PageTableLongMode57>(mmu, root, address, size, physical, flags, pat);
    }

    // Shouldn't get here.
    return false;
}


static unsigned int cacheTypeValue(MMU& mmu, cache_type type)
{
    return mmu.mapCacheType(type);
}


bool InitPageTable::addMapping(std::uint64_t vaddr, std::uint64_t size, std::uint64_t physical, page_flags flags, cache_type cache)
{
    return mapRange(*m_mmu, m_root, vaddr, size, physical, flags, cacheTypeValue(*m_mmu, cache));
}


} // namespace System::HW::CPU::X86::MMU
