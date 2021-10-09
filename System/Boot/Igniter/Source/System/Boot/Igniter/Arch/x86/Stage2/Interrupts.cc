#include <System/Boot/Igniter/Arch/x86/Stage2/Interrupts.hh>

#include <array>
#include <string_view>
#include <type_traits>

#include <System/HW/CPU/Arch/x86/Segmentation/GDT.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/IDT.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/Selector.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/TSS.hh>

#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>


namespace System::Boot::Igniter
{


using namespace std::literals::string_view_literals;
using namespace System::HW::CPU::X86;


// Interrupt handler entry points. They're laid out regularly in memory so all of them can be derived easily.
extern "C"
{

extern std::byte interruptEntry0[];
extern std::byte interruptEntry1[];

}


// Storage for the interrupt descriptor table.
static std::array<SegmentDescriptor, 256> s_IDT;

// Returns a wrapper object for the IDT.
static inline InterruptDescriptorTable32 getIDT()
{
    std::span<SegmentDescriptor, 256> span = s_IDT;
    return InterruptDescriptorTable32::fromRaw(span);
}


// Storage for the global descriptor table.
//
// Exported as the entry code needs to be able to access it.
extern "C" std::array<SegmentDescriptor, 16> g_GDT;
std::array<SegmentDescriptor, 16> g_GDT;

// Returns a wrapper object for the GDT.
static inline GlobalDescriptorTable getGDT()
{
    std::span<SegmentDescriptor, g_GDT.size()> span = g_GDT;
    return GlobalDescriptorTable::fromRaw(span);
}


// A TSS is required in order to be able to handle interrupts from less privileged modes, including v86 mode.
using TSS = CustomTSS32<std::monostate, 0>;
static TSS s_TSS;


// Offset between virtual and physical addresses.
extern "C" std::uint32_t g_loadOffset;
std::uint32_t g_loadOffset = 0;


// Fallback handler for unexpected interrupts.
static void defaultHandler()
{
    log(priority::emergency, "FATAL: unexpected interrupt");
}


// Main interrupt processing routine.
extern "C" void handleInterrupt(interrupt_context*);
void handleInterrupt(interrupt_context* context)
{
    // Figure out a description of the interrupt.
    constexpr std::array<std::string_view, 32> exceptions = 
    {
        "#DE - divide-by-zero exception"sv,
        "#DB - debugging exception"sv,
        "#NMI - non-maskable interrupt"sv,
        "#BP - breakpoint trap"sv,
        "#OF - overflow exception"sv,
        "#BR - bounds exception"sv,
        "#UD - invalid opcode exception"sv,
        "#NM - FPU not available exception"sv,
        "#DF - double-fault exception"sv,
        "### - coprocessor segment overrun exception"sv,
        "#TS - invalid TSS exception"sv,
        "#NP - segment not present exception"sv,
        "#SS - stack fault exception"sv,
        "#GP - general protection fault exception"sv,
        "#PF - page fault exception"sv,
        "### - [reserved exception 15]"sv,
        "#MF - FPU exception pending"sv,
        "#AC - alignment check exception"sv,
        "#MC - machine check exception"sv,
        "#XF - SIMD floating-point exception"sv,
        "### - [reserved exception 20]"sv,
        "### - [reserved exception 21]"sv,
        "### - [reserved exception 22]"sv,
        "### - [reserved exception 23]"sv,
        "### - [reserved exception 24]"sv,
        "### - [reserved exception 25]"sv,
        "### - [reserved exception 26]"sv,
        "### - [reserved exception 27]"sv,
        "### - [reserved exception 28]"sv,
        "#VC - VMM communication exception"sv,
        "#SX - security exception"sv,
        "### - [reserved exception 31]"sv
    };
    auto desc = (context->vector < 32) ? exceptions[context->vector] : "unknown interrupt";

    log(priority::emergency, "FATAL: unhandled interrupt #{}", context->vector);
    log(priority::alert, 
        "Interrupt: {}, error code {:08x}\n"
        "           %cs:%eip={:04x}:{:08x} %ss:%esp=????:???????? %eflags={:08x}\n"
        "           %eax={:08x} %edx={:08x} %ecx={:08x} %ebx={:08x}\n"
        "           %esi={:08x} %edi={:08x} %ebp={:08x}\n"
        "           %ds={:04x} %es={:04x} %fs={:04x} %gs={:04x}",
        desc, context->near.error_code,
        context->near.cs, context->near.eip, context->near.eflags.all,
        context->general_regs.eax, context->general_regs.edx, context->general_regs.ecx, context->general_regs.ebx,
        context->general_regs.esi, context->general_regs.edi, context->general_regs.ebp,
        context->segment_regs.ds, context->segment_regs.es, context->segment_regs.fs, context->segment_regs.gs
    );

    while (true)
    {
        asm volatile ("cli\nhlt" ::: "memory");
    }
}


void configureInterruptTable()
{
    // Create the entries that are needed in the GDT.
    auto gdt = getGDT();
    gdt.writeEntry(Selector::CodeFlat.getIndex(), SegmentDescriptor::createUser(SegmentType::RX, 32, 0, 0));
    gdt.writeEntry(Selector::DataFlat.getIndex(), SegmentDescriptor::createUser(SegmentType::RW, 32, 0, 0));
    gdt.writeEntry(Selector::Code.getIndex(), SegmentDescriptor::createUser(SegmentType::RX, 32, 0, g_loadOffset));
    gdt.writeEntry(Selector::Data.getIndex(), SegmentDescriptor::createUser(SegmentType::RW, 32, 0, g_loadOffset));
    gdt.writeEntry(Selector::Tss.getIndex(), s_TSS.createDescriptor());

    // Activate the GDT and reload the segment descriptors.
    log(priority::debug, "GDT: activating {} entries at {} (code={:#04x} data={:#04x} flat={:#04x})", 
        g_GDT.size(), static_cast<void*>(g_GDT.data()), Selector::Code.getValue(), Selector::Data.getValue(), Selector::DataFlat.getValue());
    gdt.activate();
    SegmentSelector::setCS(Selector::Code);
    SegmentSelector::setDS(Selector::Data);
    SegmentSelector::setES(Selector::Data);
    SegmentSelector::setSS(Selector::Data);

    // Configure FS and GS with zero-length segments initially.
    gdt.writeEntry(Selector::CustomFs.getIndex(), SegmentDescriptor::createUser(SegmentType::R, 32, 0, 0, 0));
    gdt.writeEntry(Selector::CustomGs.getIndex(), SegmentDescriptor::createUser(SegmentType::R, 32, 0, 0, 0));
    SegmentSelector::setFS(Selector::CustomFs);
    SegmentSelector::setGS(Selector::CustomGs);

    // Ensure that there's no LDT active.
    log(priority::debug, "LDT: loading null selector");
    SegmentSelector::setLDTR(Selector::Null);

    // Activate the TSS.
    log(priority::debug, "TSS: activating {} bytes at {} (selector={:#4x})", s_TSS.size(), static_cast<void*>(&s_TSS.tss()), Selector::Tss.getValue());
    SegmentSelector::setTR(Selector::Tss);

    // Configure all of the IDT entries to point to the appropriate entry points.
    auto idt = getIDT();
    auto entry_base = reinterpret_cast<std::uint32_t>(&interruptEntry0);
    auto entry_size = static_cast<std::size_t>(&interruptEntry1[0] - &interruptEntry0[0]);
    for (int i = 0; i < 256; ++i)
    {
        auto offset = entry_base + static_cast<std::uint32_t>(i * entry_size);
        idt.writeEntry(i, SegmentDescriptor::createGate(SegmentType::INTERRUPT_GATE, 0, Selector::Code, offset));
    }

    log(priority::debug, "IDT: activating 256 entries at {}", static_cast<void*>(s_IDT.data()));

    idt.activate();
}


// Hooked system calls for setting up custom bases for %fs and %gs.
extern "C" void setFsBase(std::uintptr_t base) asm("Syscall.X86.SetFsBase");
extern "C" void setGsBase(std::uintptr_t base) asm("Syscall.X86.SetGsBase");

void setFsBase(std::uintptr_t base)
{
    // Get the GDT for the executing CPU.
    auto gdt = GlobalDescriptorTable::getCurrent();

    // Reconfigure the FS segment.
    gdt.writeEntry(Selector::CustomFs.getIndex(), SegmentDescriptor::createUser(SegmentType::RW, 32, 0, base));

    // Reload the selector register.
    SegmentSelector::setFS(Selector::CustomFs);
}

void setGsBase(std::uintptr_t base)
{
    // Get the GDT for the executing CPU.
    auto gdt = GlobalDescriptorTable::getCurrent();

    // Reconfigure the FS segment.
    gdt.writeEntry(Selector::CustomGs.getIndex(), SegmentDescriptor::createUser(SegmentType::RW, 32, 0, base));

    // Reload the selector register.
    SegmentSelector::setGS(Selector::CustomGs);
}


} // namespace System::Boot::Igniter
