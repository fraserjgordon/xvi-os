#include <System/Boot/Igniter/Arch/x86/Stage2/V86.hh>

#include <cstring>

#include <System/ABI/ExecContext/ExecContext.h>
#include <System/HW/CPU/Arch/x86/Segmentation/RealMode.hh>
#include <System/HW/CPU/Arch/x86/VM86/Monitor.hh>
#include <System/Kernel/Arch/x86/MMU/UserCopy.hh>

#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>


namespace System::Boot::Igniter
{


namespace X86 = System::HW::CPU::X86;


// Symbols pointing to the real-mode helper functions.
extern const std::byte _REALMODE_TEXT_START asm("_REALMODE_TEXT_START");
extern const std::byte _REALMODE_TEXT_END asm("_REALMODE_TEXT_END");
extern const std::byte V86Exit asm("V86Exit");


// Page containing the relocated real-mode helper functions.
static std::uint32_t s_realModeHelperPage = 0;

// Page to use for the stack when executing real-mode code.
static std::uint32_t s_realModeStackPage = 0;

// Monitor for handling emulation etc.
static std::unique_ptr<X86::VM86Monitor> s_monitor = nullptr;

// Captured context to use when returning from a BIOS call.
static System::ABI::ExecContext::frame_t s_frame = {};

// Fake interrupt frame that we'll use to "return" to VM86 mode. We define this first as it'll also store the results
// when CaptureContext returns for the second time.
static interrupt_context s_v86Frame = {};


static bool v86HandleUndefinedOpcode(interrupt_context* context)
{
    // Get the instruction pointer so we can look at the faulting instruction.
    X86::realmode_ptr<std::uint8_t> rm_ip {context->v86.cs, context->v86.ip};
    auto ip = rm_ip.linear_ptr();
    if (ip[0] != 0xC4 || ip[1] != 0xC4)
        return false;

    // Which function is being requested?
    switch (ip[2])
    {
        case 0:
            // Requesting to terminate the V86 call. We're going to do what is effectively a longjmp back to the call
            // state just before we called into the BIOS.
            s_v86Frame = *context;
            System::ABI::ExecContext::ResumeContext(&s_frame, 1);
            
            // Shouldn't get here...
            return false;

        default:
            // Unknown.
            return false;
    }
}


void prepareV86Mode()
{
    log(priority::debug, "Preparing V86 supervisor");

    // Create the monitor.
    s_monitor = std::make_unique<X86::VM86Monitor>();

    // Allocate some real-mode memory and copy our helper functions there.
    s_realModeHelperPage = allocateEarlyRealModePage();
    log(priority::debug, "V86: copying real-mode helpers to {:#07x}", s_realModeHelperPage);
    Kernel::X86::MMU::copyToUserUnchecked(&_REALMODE_TEXT_START, &_REALMODE_TEXT_END-&_REALMODE_TEXT_START, s_realModeHelperPage);

    // Allocate a real-mode stack too.
    s_realModeStackPage = allocateEarlyRealModePage();
    log(priority::debug, "V86: real-mode stack at {:#07x}+{:x}", s_realModeStackPage, 4096);
}


void v86CallBIOS(std::uint8_t vector, bios_call_params& params)
{
    // Capture the current context. This call will return twice. The first time, we should set up and make the BIOS call.
    // The second time, the BIOS call has completed.
    auto [context, done] = System::ABI::ExecContext::CaptureContext(&s_frame, 0);
    if (done)
    {
        // BIOS call is complete. Extract the results.
        static_cast<System::ABI::ExecContext::pushl_t&>(params) = s_v86Frame.general_regs;
        params.eflags = s_v86Frame.v86.eflags;
        params.ds = s_v86Frame.v86.ds;
        params.es = s_v86Frame.v86.es;
        params.fs = s_v86Frame.v86.fs;
        params.gs = s_v86Frame.v86.gs;
    }
    else
    {
        // If using SMAP, enable access to user-mode while we're filling out the stack.
        enableUserMemoryAccess();

        // We'll push a fake real-mode interrupt frame into the real-mode stack so that when the BIOS does an iret at the
        // end of the interrupt, it'll return to our VM86Exit stub.
        X86::realmode_ptr<X86::realmode_interrupt_frame_t> stack {s_realModeStackPage >> 4, 4096};
        *(--stack) =
        {
            .ip = static_cast<std::uint16_t>(reinterpret_cast<std::uintptr_t>(&V86Exit)),
            .cs = static_cast<std::uint16_t>(s_realModeHelperPage >> 4),
            .flags = 0,
        };

        // Destination of the interrupt. It is actually at address zero.
        const X86::InterruptVectorTable* ivt = reinterpret_cast<const X86::InterruptVectorTable*>(sizeof(X86::InterruptVectorTable)) - 1;
        auto dest = ivt->getDestination(vector);

        // Fill out the interrupt frame.
        s_v86Frame.general_regs = params;
        s_v86Frame.v86.ds = params.ds;
        s_v86Frame.v86.es = params.es;
        s_v86Frame.v86.fs = params.fs;
        s_v86Frame.v86.gs = params.gs;
        s_v86Frame.v86.eflags = params.eflags;
        s_v86Frame.v86.eflags.bits.VM = true;
        s_v86Frame.v86.ss = stack.segment();
        s_v86Frame.v86.sp = stack.offset();
        s_v86Frame.v86.cs = dest.segment();
        s_v86Frame.v86.ip = dest.offset();

        // User memory access no longer needed.
        disableUserMemoryAccess();

        // "Return" to VM86 mode and execute the call we've just set up.
        asm volatile
        (
            "cli    \n\t"               // We're messing with the stack; ensure interrupts are disabled.
            "movl   %%esp, (%1)\n\t"    // Store the current stack pointer into the TSS as the ring0 stack pointer.
            "leal   16(%0), %%esp\n\t"  // Skip the protected-mode segment selectors.
            "popal  \n\t"               // Set general registers.
            "addl   $8, %%esp\n\t"      // Skip the interrupt vector number and error code.
            "iretl  \n\t"               // Jump to VM86 mode.
            :
            : "r" (&s_v86Frame), "r" (&tss().esp0)
            : "memory"
        );

        __builtin_unreachable();
    }
}


bool v86HandleInterrupt(interrupt_context* context)
{
    // Only applies to interrupts that happened while in VM86 mode.
    if (!context->near.eflags.bits.VM)
        return false;

    // Do nothing without a monitor.
    if (!s_monitor)
        return false;

    //log(priority::trace, "V86: exception {} at {:04x}:{:04x}",
    //    context->vector, context->v86.cs, context->v86.ip
    //);

    // Enable user memory access while handling the interrupt.
    enableUserMemoryAccess();

    // Pass the interrupt off to the monitor.
    bool result = false;
    if (context->vector == 6)
        result = v86HandleUndefinedOpcode(context);
    if (context->vector == 13)
        result = s_monitor->handleGeneralProtectionFault(&context->v86, &context->general_regs);
    
    disableUserMemoryAccess();

    return result;
}


}
