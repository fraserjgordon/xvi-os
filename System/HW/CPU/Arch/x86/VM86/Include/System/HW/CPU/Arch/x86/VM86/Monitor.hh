#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_MONITOR_H
#define __SYSTEM_HW_CPU_ARCH_X86_VM86_MONITOR_H


#include <System/C++/Utility/UniquePtr.hh>

#include <System/HW/CPU/Arch/x86/VM86/Context.hh>
#include <System/HW/CPU/Arch/x86/VM86/Emulator.hh>


namespace System::HW::CPU::X86
{


class VM86Monitor
{
public:

    // Should be called from the #GP interrupt handler so VM86-related exceptions can be handled.
    bool handleGeneralProtectionFault(vm86_interrupt_frame_t* frame, general_regs32_t* regs);

private:

    // The emulator instance used to emulate instructions that can't be directly executed in VM86 mode.
    VM86Emulator m_emulator;
};


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_MONITOR_H */
