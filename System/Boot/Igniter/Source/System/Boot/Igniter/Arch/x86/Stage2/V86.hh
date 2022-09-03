#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_V86_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_V86_H


#include <System/Boot/Igniter/Arch/x86/Stage2/Interrupts.hh>


namespace System::Boot::Igniter
{


struct bios_call_params :
    public System::ABI::ExecContext::pushl_t
{
    std::uint16_t ds = 0;
    std::uint16_t es = 0;
    std::uint16_t fs = 0;
    std::uint16_t gs = 0;

    System::ABI::ExecContext::eflags_t eflags = {};
};


void prepareV86Mode();


void v86CallBIOS(std::uint8_t vector, bios_call_params& params);


bool v86HandleInterrupt(interrupt_context* context);


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_V86_H */
