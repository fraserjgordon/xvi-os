#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_V86_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_V86_H


#include <System/Kernel/Runpatch/Arch/x86/Runpatch.hh>

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


void unprepareV86Mode();


void v86CallBIOS(std::uint8_t vector, bios_call_params& params);


bool v86HandleInterrupt(interrupt_context* context);


// Utility methods for enabling access to user-mode memory.
//! @todo wrap these in an RAII object.
inline void enableUserMemoryAccess()
{
    asm volatile
    (
        RUNPATCH_IF_ENABLED("x86.mmu.smap", "stac")
        ::: "memory"
    );
}

inline void disableUserMemoryAccess()
{
    asm volatile
    (
        RUNPATCH_IF_ENABLED("x86.mmu.smap", "clac")
        ::: "memory"
    );
}


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_V86_H */
