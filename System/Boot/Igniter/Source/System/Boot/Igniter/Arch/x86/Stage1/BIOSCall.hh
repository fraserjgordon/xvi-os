#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_BIOSCALL_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_BIOSCALL_H


#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/HW/CPU/Arch/x86/ControlRegs/EFLAGS.hh>


namespace System::Boot::Igniter
{


struct bios_call_params
{
    union { std::uint32_t edi = 0; std::uint16_t di; struct { std::uint8_t dil, dih; }; };
    union { std::uint32_t esi = 0; std::uint16_t si; struct { std::uint8_t sil, sih; }; };
    union { std::uint32_t ebp = 0; std::uint16_t bp; struct { std::uint8_t bpl, bph; }; };
    union { std::uint32_t esp = 0; std::uint16_t sp; struct { std::uint8_t spl, sph; }; };
    union { std::uint32_t ebx = 0; std::uint16_t bx; struct { std::uint8_t bl, bh; }; };
    union { std::uint32_t edx = 0; std::uint16_t dx; struct { std::uint8_t dl, dh; }; };
    union { std::uint32_t ecx = 0; std::uint16_t cx; struct { std::uint8_t cl, ch; }; };
    union { std::uint32_t eax = 0; std::uint16_t ax; struct { std::uint8_t al, ah; }; };

    union { std::uint32_t _ds = 0; std::uint16_t ds; };
    union { std::uint32_t _es = 0; std::uint16_t es; };

    System::HW::CPU::X86::eflags_t  eflags = {};
};

static_assert(sizeof(bios_call_params) == 44);


extern "C" void BIOSCall(std::uint32_t interrupt, bios_call_params* params);


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_BIOSCALL_H */
