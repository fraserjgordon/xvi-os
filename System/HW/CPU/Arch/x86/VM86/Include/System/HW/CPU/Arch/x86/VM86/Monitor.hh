#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_MONITOR_H
#define __SYSTEM_HW_CPU_ARCH_X86_VM86_MONITOR_H


#include <System/C++/Utility/UniquePtr.hh>


namespace System::HW::CPU::X86
{


// Forward declarations.
class VM86Emulator;


class VM86Monitor
{
private:

    // The emulator instance used to emulate instructions that can't be directly executed in VM86 mode.
    std::unique_ptr<VM86Emulator> m_emulator;

    // 
};


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_MONITOR_H */
