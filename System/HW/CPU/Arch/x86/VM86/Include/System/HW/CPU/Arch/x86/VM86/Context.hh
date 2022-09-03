#ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_CONTEXT_H
#define __SYSTEM_HW_CPU_ARCH_X86_VM86_CONTEXT_H


#include <System/ABI/ExecContext/Arch/x86/GeneralRegs.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/HW/CPU/Arch/x86/ControlRegs/EFLAGS.hh>


namespace System::HW::CPU::X86
{


// This is the interrupt frame pushed on to the stack by the processor on an interrupt from VM86 mode.
using vm86_interrupt_frame_t = System::ABI::ExecContext::interrupt_v86_t<true>;

// This is the interrupt frame pushed on to the stack by the processor in real mode.
struct realmode_interrupt_frame_t
{
    std::uint16_t   ip;
    std::uint16_t   cs;
    std::uint16_t   flags;      // Lower 16 bits of eflags register.
};

static_assert(sizeof(realmode_interrupt_frame_t) == 6);

// TODO: move this elsewhere
using general_regs32_t = System::ABI::ExecContext::pushl_t;

static_assert(sizeof(general_regs32_t) == 32);


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_CONTEXT_H */
