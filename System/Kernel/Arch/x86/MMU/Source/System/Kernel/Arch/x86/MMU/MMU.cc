#include <System/Kernel/Arch/x86/MMU/MMU.hh>

#include <array>

#include <System/HW/CPU/CPUID/Arch/x86/CPUID.hh>

#include <System/Kernel/Arch/x86/MMU/InitPageTable.hh>
#include <System/Kernel/Arch/x86/MMU/PageTableImpl.hh>
#include <System/Kernel/Arch/x86/MMU/Runpatch.hh>
#include <System/Kernel/Arch/x86/MMU/TLB.hh>


namespace System::Kernel::X86::MMU
{


namespace CPUID = System::HW::CPU::CPUID;


MMU::MMU()
{
    // We need to detect which MMU features are available. The information we need is available from CPUID. If CPUID
    // isn't available, we just have the base x86 MMU.
    m_maxPagingMode = PagingMode::Legacy;
    m_physicalAddressBits = 32;
    m_virtualAddressBits = 32;
    m_pageSizes = (4U<<10);
    if (CPUID::SupportsCPUID())
    {
        // Some features are specific to certain manufacturers.
        bool isAMD = CPUID::GetVendor() == CPUID::Vendor::AMD;

        // All CPUs that support CPUID implement supervisor-mode write protection.
        m_supervisorWriteProtect = true;

        // All CPUs that support CPUID implement alignment checking (user-mode only).
        m_alignmentChecking = true;

        // All CPUs that support CPUID implement the invlpg instruction.
        m_invlpg = true;

        // Which extended paging modes are available?
        if (CPUID::HasFeature(CPUID::Feature::LA57))
            m_maxPagingMode = PagingMode::LongMode5Level;
        else if (CPUID::HasFeature(CPUID::Feature::LongMode))
            m_maxPagingMode = PagingMode::LongMode;
        else if (CPUID::HasFeature(CPUID::Feature::PhysicalAddressExtension))
            m_maxPagingMode = PagingMode::PAE;

        // How many address bits are implemented?
        auto bits = CPUID::GetFeature(CPUID::Feature::PhysicalAddrBits);
        if (bits > 0)
            m_physicalAddressBits = bits;
        else if (m_maxPagingMode >= PagingMode::PAE)
            m_physicalAddressBits = 36;

        if (m_maxPagingMode >= PagingMode::LongMode5Level)
            m_virtualAddressBits = 57;
        else if (m_maxPagingMode >= PagingMode::LongMode)
            m_virtualAddressBits = 48;

        // Which large page sizes are supported?
        if (CPUID::HasFeature(CPUID::Feature::PageSizeExtension))
        {
            // 4MB pages are always supported with this feature.
            m_pageSizes |= (4U<<20);

            // If both this and PAE are available, 2MB pages are supported too.
            if (m_maxPagingMode >= PagingMode::PAE)
                m_pageSizes |= (2U<<20);
        }

        if (CPUID::HasFeature(CPUID::Feature::Page1G))
            m_pageSizes |= (1U<<30);

        // Are global pages supported?
        if (CPUID::HasFeature(CPUID::Feature::GlobalPages))
            m_globalPages = true;

        // Is the no-execute flag supported?
        if (CPUID::HasFeature(CPUID::Feature::NoExecute))
            m_noExecute = true;

        // Are MTRRs and/or page attribute tables supported?
        if (CPUID::HasFeature(CPUID::Feature::Mtrr))
            m_mtrrs = true;
        if (CPUID::HasFeature(CPUID::Feature::PageAttributeTable))
            m_pat = true;

        // Are process context IDs supported?
        if (CPUID::HasFeature(CPUID::Feature::ProcessContextId))
            m_processContextID = true;

        // Are supervisor mode execution/access prevention features available?
        if (CPUID::HasFeature(CPUID::Feature::SvExecProtection))
            m_supervisorExecProtection = true;
        if (CPUID::HasFeature(CPUID::Feature::SvAccessProtection))
            m_supervisorAccessProtection = true;

        // Are protection keys supported?
        if (CPUID::HasFeature(CPUID::Feature::UserMemoryKeys))
            m_protectionKeys = true;
        if (CPUID::HasFeature(CPUID::Feature::SvMemoryKeys))
            m_supervisorProtectionKeys = true;

        // Are shadow stacks / CET supported?
        if (CPUID::HasFeature(CPUID::Feature::CetSS))
            m_shadowStacks = true;

        // Can we configure segment limits to be honoured in long lode?
        // Note: this feature is deprecated. Removal of support is indicated by a CPUID bit but presence of support
        //       isn't definitively identified.
        if (isAMD && m_maxPagingMode >= PagingMode::LongMode && !CPUID::HasFeature(CPUID::Feature::NoLmsle))
            m_longModeSegmentLimit = true;

        // Are the translation cache extensions supported? (more accurate TLB invalidation)
        //! @bug causes Bochs to panic...
        //if (CPUID::HasFeature(CPUID::Feature::XlateCacheExt))
        //    m_translationCacheExtensions = true;

        // Are any additional memory management opcodes supported?
        if (CPUID::HasFeature(CPUID::Feature::Cldemote))
            m_cldemote = true;
        if (CPUID::HasFeature(CPUID::Feature::Clflush))
            m_clflush = true;
        if (CPUID::HasFeature(CPUID::Feature::Clflushopt))
            m_clflushopt = true;
        if (CPUID::HasFeature(CPUID::Feature::Clzero))
            m_clzero = true;
        if (CPUID::HasFeature(CPUID::Feature::Invlpgb))
            m_invlpgb = true;
        if (CPUID::HasFeature(CPUID::Feature::Mcommit))
            m_mcommit = true;
        if (CPUID::HasFeature(CPUID::Feature::Wbnoinvd))
            m_wbnoinvd = true;
    }

    // Based on the features we've detected, configure the MMU control registers.
    m_cr0Set |= Creg::CR0::PE;
    m_cr0Clear |= Creg::CR0::CD | Creg::CR0::NW;

    if (m_supervisorWriteProtect)
        m_cr0Set |= Creg::CR0::WP;

    if (m_alignmentChecking)
        m_cr0Set |= Creg::CR0::AM;

    if (m_pageSizes > (4U<<10))
        m_cr4Set |= Creg::CR4::PSE;

    if (m_globalPages)
        m_cr4Set |= Creg::CR4::PGE;

    // Only settable once long mode has been activated.
    //if (m_processContextID)
    //    m_cr4Set |= X86::CR4::PCIDE;

    if (m_supervisorExecProtection)
        m_cr4Set |= Creg::CR4::SMEP;

    if (m_supervisorAccessProtection)
        m_cr4Set |= Creg::CR4::SMAP;

    if (m_protectionKeys)
        m_cr4Set |= Creg::CR4::PKE;

    if (m_noExecute)
        m_eferSet |= Creg::EFER::NXE;

    if (m_translationCacheExtensions)
        m_eferSet |= Creg::EFER::TCE;

    // The code below accesses CPU MSRs (model-specific registers) that are associated with the MMU. Because the x86
    // architecture requires that the MTRRs and PAT are configured identically on all CPUs within the system, it is safe
    // to use whichever CPU we're executing on as representative.

    // If the MTRR feature is available, probe it.
    if (m_mtrrs)
    {
        // Read the MTRR capabilities register.
        m_mtrrCaps = Creg::rdmsr(Creg::MSR::MTRR_CAP);
        auto count = (m_mtrrCaps & 0xFF);         // Number of variable-range registers.
        bool fixed = (m_mtrrCaps & 0x100);        // Fixed-range registers are supported.
        m_writeCombine = (m_mtrrCaps & 0x400);    // Write-combine memory type supported.

        // Read the default memory type register.
        m_mtrrDefaults = Creg::rdmsr(Creg::MSR::MTRR_DEFAULT_TYPE);

        // Read the fixed-range MTRRs.
        if (fixed)
        {
            for (unsigned int i = 0; i < FixedMtrrCount; ++i)
            {
                m_fixedMtrrs[i].raw = Creg::rdmsr(FixedMtrrMSRs[i]);
            }
        }

        // Read the variable-range MTRRs.
        for (unsigned int i = 0; i < count; ++i)
        {
            m_variableMtrrs[i] =
            {
                .base = Creg::rdmsr(Creg::MSR::MTRR_BASE(i)),
                .mask = Creg::rdmsr(Creg::MSR::MTRR_MASK(i)),
            };
        }
    }

    // If the PAT feature is available, program it to give access to all of the cache types.
    if (m_pat)
    {
        // Read the existing PAT.
        m_originalPatValue = Creg::rdmsr(Creg::MSR::PAT);

        // We program the PAT so that the two pre-PAT caching bits map to the same values with and without the PAT (this
        // allows the page table code to configure non-leaf entries as cached without knowing about PAT. The cache types
        // that are not covered by that are then added to the additional slots.
        //
        // The value here is carefully constructed to maintain the following invariants:
        //  1. the lower four entries match the interpretation of the caching control bits if the PAT feature wasn't
        //     available (this keeps the page table traversal code independent of PAT support; in particular, it can
        //     set the caching entries to 0/write-back for sub-tables in all cases).
        //  2. the first of the entries added by PAT (0b100) is interpreted in the same way as 0b000. The reason for
        //     this is that the PAT bit in PTEs is in the same position as the page size bit in PDEs/PDPTEs/PML4Es etc.
        //     As such, the page size flag will be interpreted as a PAT bit when using recursive page tables and will be
        //     set to 1 for large pages. Mapping pages multiple times with different cache types is generally not a good
        //     idea so keeping these two PAT entries the same ensures that page tables are consistently mapped with
        //     write-back caching.
        //
        // If this value is changed, the mapCacheType method needs to be adjusted to match.
        m_patValue = 0x00'01'05'06'00'07'04'06;
    }

    // With all the features detected, apply any runtime patching needed to handle the features properly.
    {
        using Kernel::Runpatch::X86::applyRunpatch;

        // TLB management instructions vary a lot with processor features.
        if (m_processContextID)
        {
            applyRunpatch(PATCH_TLBFLUSHALL_METHOD, TLBFLUSHALL_INVPCID);
            applyRunpatch(PATCH_TLBFLUSHNONGLOBAL_METHOD, TLBFLUSHNONGLOBAL_INVPCID);
            applyRunpatch(PATCH_TLBFLUSHPCID_METHOD, TLBFLUSHPCID_INVPCID);
        }
        else if (m_globalPages)
        {
            applyRunpatch(PATCH_TLBFLUSHALL_METHOD, TLBFLUSHALL_TOGGLE_PGE);
        }

        if (m_invlpg)
        {
            applyRunpatch(PATCH_TLBINVALIDATEPAGE_METHOD, TLBINVALIDATEPAGE_INVLPG);
        }

        // With SMAP, we need to use the new "stac" and "clac" instructions around accesses to user-mode memory.
        if (m_supervisorAccessProtection)
            applyRunpatch(PATCH_SMAP_SUPPORT, SMAP_ENABLED);
    }
}


MMU::~MMU()
{
}


void MMU::setAllocator(std::unique_ptr<TablePageAllocator> allocator)
{
    m_allocator = std::move(allocator);
}


TablePageAllocator* MMU::allocator() const noexcept
{
    return m_allocator.get();
}


Creg::cr0_t MMU::cr0SetBits() const noexcept
{
    return m_cr0Set;
}


Creg::cr0_t MMU::cr0ClearBits() const noexcept
{
    return m_cr0Clear;
}


Creg::cr4_t MMU::cr4SetBits() const noexcept
{
    return m_cr4Set;
}


Creg::efer_t MMU::eferSetBits() const noexcept
{
    return m_eferSet;
}


std::uint64_t MMU::patValue() const noexcept
{
    return m_patValue;
}


paging_mode MMU::highestSupportedPagingMode() const noexcept
{
    return m_maxPagingMode;
}


paging_mode MMU::currentPagingMode() const noexcept
{
    return m_currentPagingMode;
}


paging_mode MMU::targetPagingMode() const noexcept
{
    return m_targetPagingMode;
}


InitPageTable MMU::createInitPageTable(paging_mode target_mode, std::uint64_t self_map_address, std::uint64_t adjust)
{
    m_targetPagingMode = target_mode;

    // Get the MMU control bits based on the target mode.
    if (m_targetPagingMode >= PagingMode::Legacy)
        m_cr0Set |= Creg::CR0::PG;
    if (m_targetPagingMode >= PagingMode::PAE)
        m_cr4Set |= Creg::CR4::PAE;
    if (m_targetPagingMode >= PagingMode::LongMode)
        m_eferSet |= Creg::EFER::LME;
    if (m_targetPagingMode >= PagingMode::LongMode5Level)
        m_cr4Set |= Creg::CR4::LA57;

    // Make any runtime patches to support the selected paging mode.
    switch (m_targetPagingMode)
    {
    #if !defined(__x86_64__)
        case PagingMode::Legacy:
            Kernel::Runpatch::X86::applyRunpatch(PATCH_PAGING_MODE, PAGING_LEGACY);
            break;

        case PagingMode::PAE:
            Kernel::Runpatch::X86::applyRunpatch(PATCH_PAGING_MODE, PAGING_PAE);
            break;
    #endif

        case PagingMode::LongMode:
            Kernel::Runpatch::X86::applyRunpatch(PATCH_PAGING_MODE, PAGING_LM);
            break;

        case PagingMode::LongMode5Level:
            Kernel::Runpatch::X86::applyRunpatch(PATCH_PAGING_MODE, PAGING_LM57);
            break;

        default:
            return {};
    }

    // Root table address.
    std::uint64_t root;
    std::size_t self_map_index;

    // PAE mode needs special handling as the top-level page table is only 4 entries and (because we're making it
    // recursive) is embedded as four consecutive entries within one of the next-level tables.
    if (m_targetPagingMode == PagingMode::PAE)
    {
        //! @todo is embedding these entries workable? Will the CPU object when it loads them and sees that the dirty or
        //        accessed bits have been set? (the Intel docs seem to imply so...)

        // Allocate the PDPT and four page directory tables.
        root = m_allocator->allocate();
        auto pd0 = m_allocator->allocate();
        auto pd1 = m_allocator->allocate();
        auto pd2 = m_allocator->allocate();
        auto pd3 = m_allocator->allocate();
        std::uint64_t pds[4] = {pd0, pd1, pd2, pd3};

        // Which one are we embedding in?
        auto which_pd = (self_map_address >> 30);
        self_map_index = (self_map_address >> 21) & 0x7FF;

        // We set the pinning bit in PDPT entries due to the way in which CPUs load the entries. It isn't possible to
        // set the NoExecute flag on these entries, unfortunately, as the PDPTEs require that particular bit to be
        // unset.
        //
        // We don't set the User flag as that would give user-mode code access to the page tables!
        constexpr auto Flags = pdpt32pae_t::entry_t::PinnedTable;

        // Write the 4 page directories to the embedded PDPT.
        auto* pdpt = reinterpret_cast<pdpt32pae_t*>(root - adjust);
        pdpt->addSubTable(0x00000000, pd0, Flags);
        pdpt->addSubTable(0x40000000, pd1, Flags);
        pdpt->addSubTable(0x80000000, pd2, Flags);
        pdpt->addSubTable(0xC0000000, pd3, Flags);

        auto flags = Flags;
        if (m_noExecute)
            flags |= pde32_t::NoExecute;

        // And write the four self-map entries to the appropriate page directory.
        auto* pd = reinterpret_cast<pd32pae_t*>(pds[which_pd] - adjust);
        pd->addSubTable(self_map_address + (0<<21), pd0, flags);
        pd->addSubTable(self_map_address + (1<<21), pd1, flags);
        pd->addSubTable(self_map_address + (2<<21), pd2, flags);
        pd->addSubTable(self_map_address + (3<<21), pd3, flags);
    }
    else
    {
        // Allocate a single page as the root table.
        root = m_allocator->allocate();

        // If possible, add the recursive mappings with no-execute permissions. The pinning bit isn't required here but
        // we set it as it's an extra level of safety.
        //
        // We don't set the User flag as that would give user-mode code access to the page tables!
        std::uint64_t flags = pte32_t::PinnedTable;
        if (m_noExecute)
            flags |= pdpe64_t::NoExecute;

        // Create the recursive entry in the table.
        switch (m_targetPagingMode)
        {
            case PagingMode::Legacy:
                reinterpret_cast<pd32_t*>(root - adjust)->addSubTable(self_map_address, root, flags);
                self_map_index = (self_map_address >> pd32_t::OffsetBits) & pd32_t::IndexMask;
                break;

            case PagingMode::PAE:
                __builtin_unreachable();
                break;

            case PagingMode::LongMode:
                reinterpret_cast<pml4_t*>(root - adjust)->addSubTable(self_map_address, root, flags);
                self_map_index = (self_map_address >> pml4_t::OffsetBits) & pml4_t::IndexMask;
                break;

            case PagingMode::LongMode5Level:
                reinterpret_cast<pml5_t*>(root - adjust)->addSubTable(self_map_address, root, flags);
                self_map_index = (self_map_address >> pml5_t::OffsetBits) & pml5_t::IndexMask;
                break;
        }
    }

    // Create the root table object.
    InitPageTable init_pt{*this, root, self_map_address, self_map_index, adjust};

    return init_pt;
}


MappedPageTable MMU::convertInitPageTable(InitPageTable ipt)
{
    return MappedPageTable{PageTable{*this, ipt.root(), ipt.selfMapIndex()}, ipt.selfMapAddress()};
}


bool MMU::supports2MPages() const noexcept
{
    return (m_targetPagingMode >= PagingMode::PAE) && (m_pageSizes & (2U<<20));
}


bool MMU::supports4MPages() const noexcept
{
    return (m_targetPagingMode == PagingMode::Legacy) && (m_pageSizes & (4U<<20));
}


bool MMU::supports1GPages() const noexcept
{
    return (m_targetPagingMode >= PagingMode::LongMode) && (m_pageSizes & (1U<<30));
}


unsigned int MMU::mapCacheType(cache_type type) const
{
    if (m_pat)
    {
        // The PAT value in the constructor determines the mapping here; do not change this mapping without adjusting
        // the PAT value to match.
        switch (type)
        {
            case CacheType::Uncached:
                return 0b011;

            case CacheType::WriteCombine:
                // This cache type is not supported on all processors.
                if (m_writeCombine)
                    return 0b110;
                else
                    return 0b011;   // Uncached.

            case CacheType::WriteThrough:
                return 0b001;

            case CacheType::WriteProtect:
                return 0b101;

            case CacheType::WriteBack:
                return 0b000;

            case CacheType::UncachedMinus:
                return 0b010;
        }
    }
    else
    {
        switch (type)
        {
            case CacheType::Uncached:
            case CacheType::WriteProtect:
                return 0b011;

            case CacheType::UncachedMinus:
            case CacheType::WriteCombine:
                return 0b010;

            case CacheType::WriteThrough:
                return 0b001;

            case CacheType::WriteBack:
                return 0b000;
        }
    }

    // Unknown caching type (somehow). Select uncached.
    return 0b011;
}


std::unique_ptr<MMU> MMU::create()
{
    return std::unique_ptr<MMU>(new MMU());
}


std::string_view MMU::pagingModeShortName(paging_mode mode)
{
    switch (mode)
    {
        case PagingMode::Disabled:
            return "Disabled";

        case PagingMode::Legacy:
            return "Legacy";

        case PagingMode::PAE:
            return "PAE";

        case PagingMode::LongMode:
            return "LongMode";

        case PagingMode::LongMode5Level:
            return "LongMode5Level";
    }

    return "";
}



} // namespace System::Kernel::X86::MMU
