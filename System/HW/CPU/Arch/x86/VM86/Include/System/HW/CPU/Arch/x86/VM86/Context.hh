#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_CONTEXT_H
#define __SYSTEM_HW_CPU_ARCH_X86_VM86_CONTEXT_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/HW/CPU/Arch/x86/ControlRegs/EFLAGS.hh>


namespace System::HW::CPU::X86
{


// This is the interrupt frame pushed on to the stack by the processor on an interrupt from VM86 mode.
struct vm86_interrupt_frame_t
{
    union { std::uint32_t eip; std::uint16_t ip; };
    std::uint16_t   cs;
    std::uint16_t   :16;
    eflags_t        eflags;
    union { std::uint32_t esp; std::uint16_t sp; };
    std::uint16_t   ss;
    std::uint16_t   :16;
    std::uint16_t   es;
    std::uint16_t   :16;
    std::uint16_t   ds;
    std::uint16_t   :16;
    std::uint16_t   fs;
    std::uint16_t   :16;
    std::uint16_t   gs;
    std::uint16_t   :16;
};

static_assert(sizeof(vm86_interrupt_frame_t) == 36);

// This is the interrupt frame pushed on to the stack by the processor in real mode.
struct realmode_interrupt_frame_t
{
    std::uint16_t   ip;
    std::uint16_t   cs;
    std::uint16_t   flags;      // Lower 16 bits of eflags register.
};

static_assert(sizeof(realmode_interrupt_frame_t) == 6);

// TODO: move this elsewhere
struct general_regs32_t
{
    union { std::uint32_t edi; std::uint16_t di; struct { std::uint8_t sil, sih; }; };
    union { std::uint32_t esi; std::uint16_t si; struct { std::uint8_t dil, dih; }; };
    union { std::uint32_t ebp; std::uint16_t bp; struct { std::uint8_t bpl, bph; }; };
    union { std::uint32_t esp; std::uint16_t sp; struct { std::uint8_t spl, sph; }; };
    union { std::uint32_t ebx; std::uint16_t bx; struct { std::uint8_t bl, bh; }; };
    union { std::uint32_t edx; std::uint16_t dx; struct { std::uint8_t dl, dh; }; };
    union { std::uint32_t ecx; std::uint16_t cx; struct { std::uint8_t cl, ch; }; };
    union { std::uint32_t eax; std::uint16_t ax; struct { std::uint8_t al, ah; }; };
};

static_assert(sizeof(general_regs32_t) == 32);


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_CONTEXT_H */
