#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_EFLAGS_H
#define __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_EFLAGS_H


#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::HW::CPU::X86
{


struct eflags_bits_t
{
    std::uint32_t   cf:1;       // Carry flag.
    std::uint32_t   :1;
    std::uint32_t   pf:1;       // Parity flag.
    std::uint32_t   :1;
    std::uint32_t   af:1;       // Auxiliary flag.
    std::uint32_t   :1;
    std::uint32_t   zf:1;       // Zero flag.
    std::uint32_t   sf:1;       // Sign flag.
    std::uint32_t   tf:1;       // Trap flag.
    std::uint32_t   _if:1;      // Interrupt flag.
    std::uint32_t   df:1;       // Direction flag.
    std::uint32_t   of:1;       // Overflow flag.
    std::uint32_t   iopl:2;     // I/O privilege level.
    std::uint32_t   nt:1;       // Nested task.
    std::uint32_t   :1;
    std::uint32_t   rf:1;       // Resume flag.
    std::uint32_t   vm:1;       // Virtual-8086 mode.
    std::uint32_t   ac:1;       // Alignment check.
    std::uint32_t   vif:1;      // Virtual interrupt flag.
    std::uint32_t   vip:1;      // Virtual interrupt pending.
    std::uint32_t   id:1;       // CPUID detection flag.
    std::uint32_t   :0;
};

union eflags_t
{
    eflags_bits_t   bits;
    std::uint32_t   uint32;
};

union rflags_t
{
    eflags_bits_t   bits;
    std::uint32_t   uint32;
    std::uint64_t   uint64;
};


static_assert(sizeof(eflags_t) == sizeof(std::uint32_t));
static_assert(sizeof(rflags_t) == sizeof(std::uint64_t));


namespace EFLAGS
{

inline eflags_t read()
{
    std::uint32_t value;
    asm
    (
        "pushf          \n\t"
        "popl   %0      \n\t"
        : "=g" (value)
        :
        :
    );
    return {.uint32 = value};
}

inline void write(eflags_t value)
{
    asm volatile
    (
        "pushl  %0      \n\t"
        "popf           \n\t"
        :
        : "g" (value.uint32)
        : "cc", "memory"
    );
}

} // namespace EFLAGS

#if __x86_64__
namespace RFLAGS
{

inline rflags_t read()
{
    std::uint64_t value;
    asm
    (
        "pushfq         \n\t"
        "popq   %0      \n\t"
        : "=g" (value)
        :
        :
    );
    return {.uint64 = value};
}

inline void write(rflags_t value)
{
    asm
    (
        "pushq  %0      \n\t"
        "popfq          \n\t"
        :
        : "g" (value.uint64)
        : "cc", "memory"
    );
}

} // namespace RFLAGS
#endif


inline void clac()
{
    asm volatile ("clac" ::: "memory");
}

inline void stac()
{
    asm volatile ("stac" ::: "memory");
}

inline void cli()
{
    asm volatile ("cli" ::: "memory");
}

inline void sti()
{
    asm volatile ("sti" ::: "memory");
}


} // namespace System::HW::CPU::X86


#endif // ifndef __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_EFLAGS_H
