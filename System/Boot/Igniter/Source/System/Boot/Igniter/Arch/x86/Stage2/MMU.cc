#include <System/Boot/Igniter/Arch/x86/Stage2/MMU.hh>

#include <System/HW/CPU/Arch/x86/ControlRegs/CR.hh>
#include <System/Kernel/Arch/x86/MMU/InitPageTable.hh>
#include <System/Kernel/Arch/x86/MMU/MMU.hh>

#include <System/Boot/Igniter/Arch/x86/Stage2/Interrupts.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>


// Offset we need to apply to access the VGA memory at the right address.
extern std::uint32_t g_loadOffset;

// Symbols defining the limits of the in-memory image.
extern const std::byte _TEXT_START asm("_TEXT_START");
extern const std::byte _TEXT_END asm("_TEXT_END");
extern const std::byte _RODATA_START asm("_RODATA_START");
extern const std::byte _RODATA_END asm("_RODATA_END");
extern const std::byte _DATA_START asm("_DATA_START");
extern const std::byte _DATA_END asm("_DATA_END");
extern const std::byte _BSS_START asm("_BSS_START");
extern const std::byte _BSS_END asm("_BSS_END");



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


#if !defined(__x86_64__)
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
    auto init_pt = s_mmu->createInitPageTable(mode, SelfMapAddress, g_loadOffset);
    s_pagingRoot = init_pt.root();
    log(priority::debug, "MMU: creating initial page table at {:#010x}", s_pagingRoot);

    // Request that the probe code adds mappings for the memory it has detected so far.
    mapEarlyMemory();

    // Also create maps covering this loader and its memory.
    //! @bug bad things will happen if the link and load addresses are different but overlapping...
    namespace F = Kernel::X86::MMU::PageFlag;
    auto map = [&](const std::byte& start, const std::byte& end, Kernel::X86::MMU::page_flags flags)
    {
        constexpr auto WB = Kernel::X86::MMU::CacheType::WriteBack;
        auto v = reinterpret_cast<std::uintptr_t>(&start);
        auto s = (&end - &start + 0x0fff) & ~0xfff;
        auto p = v + g_loadOffset;

        log(priority::debug, "MMU: mapping loader {:#010x}+{:08x} -> {:#010x}", v, s, p);

        init_pt.addMapping(v, s, p, flags, WB);
    };

    map(_TEXT_START, _TEXT_END, F::X);
    map(_RODATA_START, _RODATA_END, 0);
    map(_DATA_START, _BSS_END, F::W);

    // Also map in the VGA memory.
    //! @todo find a better way to do this; this is hacky.
    init_pt.addMapping(0xA0000 - g_loadOffset, 0x20000, 0xA0000, F::W, Kernel::X86::MMU::CacheType::Uncached);

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
    asm volatile
    (   R"(
            # Switch to flat segments.
            leal    1f(%%ebx), %%edx
            pushl   %[codeseg]
            pushl   %%edx
            lretl
        1:  movl    %[dataseg], %%eax
            movl    %%eax, %%ss
            movl    %%eax, %%ds
            movl    %%eax, %%es

            # Enable paging.
            movl    %[pagebits], %%cr0

            # Long jump to clear prefetched instructions.
            ljmpl   %[codeseg], $2f

        2:
        )"
        :
        : [codeseg] "i" (Selector::CodeFlat.getValue()),
          [dataseg] "i" (Selector::DataFlat.getValue()),
          [offset] "b" (g_loadOffset),
          [pagebits] "c" (cr0)
        : "eax", "edx", "memory"
    );
    log(priority::debug, "MMU: paging enabled");

    reconfigureInterruptTableForPaging();
}
#endif /* if !defined(__x86_64__) */


void addEarlyMap(std::uint32_t address, std::uint32_t size, early_map_flag_t flags)
{
    // Re-create the page table object.
    Kernel::X86::MMU::InitPageTable init_pt {*s_mmu, s_pagingRoot, SelfMapAddress, g_loadOffset};

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
