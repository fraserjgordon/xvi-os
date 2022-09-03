#include <System/HW/CPU/Arch/x86/VM86/Monitor.hh>


namespace System::HW::CPU::X86
{


bool VM86Monitor::handleGeneralProtectionFault(vm86_interrupt_frame_t* frame, general_regs32_t* regs)
{
    return m_emulator.generalProtectionException(*frame, *regs);
}


} // namespace System::HW::CPU::X86
