#include <System/HW/CPU/Arch/x86/MMU/MMU.hh>

#include <array>

#include <System/HW/CPU/CPUID/Arch/x86/CPUID.hh>

#include <System/HW/CPU/Arch/x86/MMU/InitPageTable.hh>
#include <System/HW/CPU/Arch/x86/MMU/PageTable.hh>


namespace System::HW::CPU::X86::MMU
{


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
        // All CPUs that support CPUID implement supervisor-mode write protection.
        m_supervisorWriteProtect = true;

        // All CPUs that support CPUID implement alignment checking.
        m_alignmentChecking = true;

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
    }

    // Based on the features we've detected, configure the MMU control registers.
    m_cr0Set |= X86::CR0::PE;
    m_cr0Clear |= X86::CR0::CD | X86::CR0::NW;

    if (m_supervisorWriteProtect)
        m_cr0Set |= X86::CR0::WP;

    if (m_alignmentChecking)
        m_cr0Set |= X86::CR0::AM;

    if (m_pageSizes > (4U<<10))
        m_cr4Set |= X86::CR4::PSE;

    if (m_globalPages)
        m_cr4Set |= X86::CR4::PGE;

    // Only settable once long mode has been activated.
    //if (m_processContextID)
    //    m_cr4Set |= X86::CR4::PCIDE;

    if (m_supervisorExecProtection)
        m_cr4Set |= X86::CR4::SMEP;

    if (m_supervisorAccessProtection)
        m_cr4Set |= X86::CR4::SMAP;

    if (m_protectionKeys)
        m_cr4Set |= X86::CR4::PKE;

    if (m_noExecute)
        m_eferSet |= X86::EFER::NXE;

    // The code below accesses CPU MSRs (model-specific registers) that are associated with the MMU. Because the x86
    // architecture requires that the MTRRs and PAT are configured identically on all CPUs within the system, it is safe
    // to use whichever CPU we're executing on as representative.

    // If the MTRR feature is available, probe it.
    if (m_mtrrs)
    {
        // Read the MTRR capabilities register.
        m_mtrrCaps = X86::rdmsr(X86::MSR::MTRR_CAP);
        auto count = (m_mtrrCaps & 0xFF);         // Number of variable-range registers.
        bool fixed = (m_mtrrCaps & 0x100);        // Fixed-range registers are supported.
        m_writeCombine = (m_mtrrCaps & 0x400);    // Write-combine memory type supported.

        // Read the default memory type register.
        m_mtrrDefaults = X86::rdmsr(X86::MSR::MTRR_DEFAULT_TYPE);

        // Read the fixed-range MTRRs.
        if (fixed)
        {
            for (unsigned int i = 0; i < FixedMtrrCount; ++i)
            {
                m_fixedMtrrs[i].raw = X86::rdmsr(FixedMtrrMSRs[i]);
            }
        }

        // Read the variable-range MTRRs.
        for (unsigned int i = 0; i < count; ++i)
        {
            m_variableMtrrs[i] =
            {
                .base = X86::rdmsr(X86::MSR::MTRR_BASE(i)),
                .mask = X86::rdmsr(X86::MSR::MTRR_MASK(i)),
            };
        }
    }

    // If the PAT feature is available, program it to give access to all of the cache types.
    if (m_pat)
    {
        // Assume that the write-combining cache type is available as PAT is supported.
        //! @todo is this correct?
        m_writeCombine = true;

        // Read the existing PAT.
        m_originalPatValue = X86::rdmsr(X86::MSR::PAT);

        // We program the PAT so that the two pre-PAT caching bits map to the same values with and without the PAT (this
        // allows the page table code to configure non-leaf entries as cached without knowing about PAT. The cache types
        // that are not covered by that are then added to the additional slots.
        //
        // If this value is changed, the mapCacheType method needs to be adjusted to match.
        m_patValue = 0x0000010500070406;
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


cr0_t MMU::cr0SetBits() const noexcept
{
    return m_cr0Set;
}


cr0_t MMU::cr0ClearBits() const noexcept
{
    return m_cr0Clear;
}


cr4_t MMU::cr4SetBits() const noexcept
{
    return m_cr4Set;
}


efer_t MMU::eferSetBits() const noexcept
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


InitPageTable MMU::createInitPageTable(paging_mode target_mode)
{
    m_targetPagingMode = target_mode;

    // Get the MMU control bits based on the target mode.
    if (m_targetPagingMode >= PagingMode::Legacy)
        m_cr0Set |= X86::CR0::PG;
    if (m_targetPagingMode >= PagingMode::PAE)
        m_cr4Set |= X86::CR4::PAE;
    if (m_targetPagingMode >= PagingMode::LongMode)
        m_eferSet |= X86::EFER::LME;
    if (m_targetPagingMode >= PagingMode::LongMode5Level)
        m_cr4Set |= X86::CR4::LA57;

    // Create the root table.
    InitPageTable init_pt{*this, m_allocator->allocate()};

    // Special handling is needed for PAE mode - all four entries in the root PDPT must be filled.
    if (m_targetPagingMode == PagingMode::PAE)
    {
        auto* pdpt = reinterpret_cast<pdpt32pae_t*>(init_pt.root());
        pdpt->addSubTable(0x00000000, m_allocator->allocate());
        pdpt->addSubTable(0x40000000, m_allocator->allocate());
        pdpt->addSubTable(0x80000000, m_allocator->allocate());
        pdpt->addSubTable(0xC0000000, m_allocator->allocate());
    }

    return init_pt;
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
                    return 0b101;
                else
                    return 0b011;   // Uncached.

            case CacheType::WriteThrough:
                return 0b001;

            case CacheType::WriteProtect:
                return 0b100;

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



} // namespace System::HW::CPU::X86::MMU
