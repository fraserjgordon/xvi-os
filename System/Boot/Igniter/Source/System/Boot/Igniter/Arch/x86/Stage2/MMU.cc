#include <System/Boot/Igniter/Arch/x86/Stage2/MMU.hh>

#include <System/HW/CPU/Arch/x86/ControlRegs/CR.hh>
#include <System/Kernel/Arch/x86/MMU/InitPageTable.hh>
#include <System/Kernel/Arch/x86/MMU/MMU.hh>

#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>


namespace System::Boot::Igniter
{


namespace X86 = HW::CPU::X86;


static Kernel::X86::MMU::MMU* s_mmu = nullptr;
static std::uint64_t  s_pagingRoot = 0;


// The initial page table will be recursive. Use the highest possible address in the lower half of the 32-bit address
// space for this (so we don't infringe on addresses that the kernel might want to be mapped to when we load it).
constexpr std::uint32_t SelfMapAddress = 0x7F800000;


class InitTablePageAllocator final :
    public Kernel::X86::MMU::TablePageAllocator
{
public:

    std::uint64_t allocate() final
    {
        //! @todo zero the page before passing to the MMU.
        return allocateEarlyPage();
    }

    void free(std::uint64_t page) final
    {
        freeEarlyPage(static_cast<std::uint32_t>(page));
    }
};


static void initMMU()
{
    // Create the MMU object.
    s_mmu = Kernel::X86::MMU::MMU::create().release();

    // Configure the allocator.
    s_mmu->setAllocator(std::unique_ptr<Kernel::X86::MMU::TablePageAllocator>(new InitTablePageAllocator()));
}


void enablePaging()
{
    initMMU();

    // What paging mode are we targeting?
    auto mode = s_mmu->highestSupportedPagingMode();
    log(priority::debug, "MMU: max paging mode: {}", s_mmu->pagingModeShortName(mode));

    // Limit the paging mode to PAE - we need V86 mode to work.
    if (mode >= Kernel::X86::MMU::PagingMode::LongMode)
        mode = Kernel::X86::MMU::PagingMode::PAE;

    // Create the initial page table.
    auto init_pt = s_mmu->createInitPageTable(mode, SelfMapAddress);
    s_pagingRoot = init_pt.root();
    log(priority::debug, "MMU: creating initial page table at {:#010x}", s_pagingRoot);

    // Request that the probe code adds mappings for the memory it has detected so far.
    mapEarlyMemory();

    // Write the address of the root of the page tables to %cr3.
    X86::CR3::write(static_cast<std::uintptr_t>(init_pt.root()));

    // Configure the other MMU control registers as required.
    auto cr0 = X86::CR0::read();
    cr0 |= s_mmu->cr0SetBits();
    cr0 &= ~(s_mmu->cr0ClearBits());

    if (s_mmu->cr4SetBits() != 0)
    {
        auto cr4 = X86::CR4::read();
        cr4 |= s_mmu->cr4SetBits();
        X86::CR4::write(cr4);
    }

    if (s_mmu->eferSetBits() != 0)
    {
        auto efer = X86::EFER::read();
        efer |= s_mmu->eferSetBits();
        X86::EFER::write(efer);
    }

    if (s_mmu->patValue() != 0)
    {
        X86::wrmsr(X86::MSR::PAT, s_mmu->patValue());
    }

    // We write %cr0 last as this is the action that actually enables paging.
    log(priority::debug, "MMU: enabling paging");
    X86::CR0::write(cr0);
    log(priority::debug, "MMU: paging enabled");
}


void addEarlyMap(std::uint32_t address, std::uint32_t size, early_map_flag_t flags)
{
    // Re-create the page table object.
    Kernel::X86::MMU::InitPageTable init_pt {*s_mmu, s_pagingRoot, SelfMapAddress};

    // Calculate the flags for the mapping.
    Kernel::X86::MMU::page_flags page_flags = {};
    if (flags & EarlyMapFlag::W)
        page_flags |= Kernel::X86::MMU::PageFlag::W;
    if (flags & EarlyMapFlag::X)
        page_flags |= Kernel::X86::MMU::PageFlag::X;
    if (flags & EarlyMapFlag::U)
        page_flags |= Kernel::X86::MMU::PageFlag::U;

    // Calculate the caching type for the mapping.
    Kernel::X86::MMU::cache_type cache_type = Kernel::X86::MMU::CacheType::Uncached;
    if (flags & EarlyMapFlag::C)
        cache_type = Kernel::X86::MMU::CacheType::WriteBack;

    // Add the map as requested.
    init_pt.addMapping(address, size, address, page_flags, cache_type);
}


} // namespace System::Boot::Igniter
