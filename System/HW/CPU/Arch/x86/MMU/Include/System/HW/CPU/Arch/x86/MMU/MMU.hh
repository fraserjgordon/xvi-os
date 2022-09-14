#ifndef __SYSTEM_HW_CPU_ARCH_X86_MMU_MMU_H
#define __SYSTEM_HW_CPU_ARCH_X86_MMU_MMU_H


#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

#include <System/HW/CPU/Arch/x86/ControlRegs/CR.hh>
#include <System/HW/CPU/Arch/x86/ControlRegs/MSR.hh>

#include <System/HW/CPU/Arch/x86/MMU/InitPageTable.hh>
#include <System/HW/CPU/Arch/x86/MMU/Types.hh>


namespace System::HW::CPU::X86::MMU
{


// Interface that a table page allocator should implement.
class TablePageAllocator
{
public:

    virtual ~TablePageAllocator() {};
    virtual std::uint64_t allocate() = 0;
    virtual void free(std::uint64_t) = 0;
};


class MMU
{
public:

    ~MMU();

    // Sets the table page allocator. If an existing allocator is being replaced, the new allocator must be able to
    // correctly free any pages allocated by previous allocators.
    void setAllocator(std::unique_ptr<TablePageAllocator>);

    // Returns the current allocator.
    TablePageAllocator* allocator() const noexcept;

    // Returns the values that should be written to MMU control registers of newly-booted CPUs to configure them
    // properly.
    X86::cr0_t cr0SetBits() const noexcept;
    X86::cr0_t cr0ClearBits() const noexcept;
    X86::cr4_t cr4SetBits() const noexcept;
    X86::efer_t eferSetBits() const noexcept;
    std::uint64_t patValue() const noexcept;
    std::span<const mtrr_record> mtrrValues() const noexcept;

    // Returns the maximum-supported paging mode.
    paging_mode highestSupportedPagingMode() const noexcept;

    // Returns the current paging mode.
    paging_mode currentPagingMode() const noexcept;

    // Returns the paging mode that is currently configured to be used when paging gets enabled.
    paging_mode targetPagingMode() const noexcept;

    // Returns the maximum number of new pages that might need to be allocated to add a mapping of the given size.
    unsigned int maxTablePagesNeeded(std::size_t size) const;

    // Creates an initial page table for the given paging mode.
    InitPageTable createInitPageTable(paging_mode);

    // Indicates whether the MMU supports the given page size in the current/target paging mode.
    bool supports2MPages() const noexcept;
    bool supports4MPages() const noexcept;
    bool supports1GPages() const noexcept;

    // Maps a cache type to the best combination of PAT bits.
    unsigned int mapCacheType(cache_type) const;


    static std::unique_ptr<MMU> create();

    static std::string_view pagingModeShortName(paging_mode);

private:

    // There are 11 fixed-range MTRR registers and an architectural limit of 256 variable-range MTRR registers.
    static constexpr std::size_t FixedMtrrCount     = 11;
    static constexpr std::size_t VariableMtrrCount  = 256;
    using fixed_mtrr_array = std::array<mtrr_fixed, FixedMtrrCount>;
    using variable_mtrr_array = std::array<mtrr_variable, VariableMtrrCount>;

    // Mapping between fixed MTRR array indices and MSR numbers.
    static constexpr std::array<std::uint64_t, FixedMtrrCount> FixedMtrrMSRs =
    {
        System::HW::CPU::X86::MSR::MTRR_FIX_64K_00000,
        System::HW::CPU::X86::MSR::MTRR_FIX_16K_80000,
        System::HW::CPU::X86::MSR::MTRR_FIX_16K_A0000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_C0000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_C8000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_D0000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_D8000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_E0000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_E8000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_F0000,
        System::HW::CPU::X86::MSR::MTRR_FIX_4K_F8000,
    };


    // Cached MMU control register settings.
    X86::cr0_t      m_cr0Set = X86::CR0::None;
    X86::cr0_t      m_cr0Clear = X86::CR0::None;
    X86::cr4_t      m_cr4Set = X86::CR4::None;
    X86::efer_t     m_eferSet = X86::EFER::None;
    std::uint64_t   m_patValue = 0;
    std::uint64_t   m_mtrrCaps = 0;
    std::uint64_t   m_mtrrDefaults = 0;
    fixed_mtrr_array    m_fixedMtrrs = {};
    variable_mtrr_array m_variableMtrrs = {};

    // Cache of some initial control values in case they need to be reset.
    std::uint64_t   m_originalPatValue = 0;

    // Paging mode information.
    paging_mode m_maxPagingMode = PagingMode::Disabled;
    paging_mode m_currentPagingMode = PagingMode::Disabled;
    paging_mode m_targetPagingMode = PagingMode::Disabled;

    // Implemented address bits.
    std::uint8_t    m_physicalAddressBits = 0;
    std::uint8_t    m_virtualAddressBits = 0;

    // Supported page sizes.
    std::uint64_t   m_pageSizes = 0;

    // Other MMU features.
    bool            m_supervisorWriteProtect = false;
    bool            m_alignmentChecking = false;
    bool            m_globalPages = false;
    bool            m_noExecute = false;
    bool            m_mtrrs = false;
    bool            m_pat = false;
    bool            m_writeCombine = false;
    bool            m_processContextID = false;
    bool            m_supervisorExecProtection = false;
    bool            m_supervisorAccessProtection = false;
    bool            m_protectionKeys = false;

    // Allocator object.
    std::unique_ptr<TablePageAllocator> m_allocator = {};


    MMU();
};


} // namespace System::HW::CPU::X86::MMU


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_MMU_MMU_H */
