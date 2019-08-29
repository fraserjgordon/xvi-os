#pragma once
#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_EFLAGS_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_EFLAGS_H


#include <cstdint>


namespace System::ABI::ExecContext
{


struct eflags_bits_t
{
    std::uint32_t   CF:1;       // Carry flag.
    std::uint32_t     :1;
    std::uint32_t   PF:1;       // Parity flag.
    std::uint32_t     :1;
    std::uint32_t   AF:1;       // Auxiliary flag.
    std::uint32_t     :1;
    std::uint32_t   ZF:1;       // Zero flag.
    std::uint32_t   SF:1;       // Sign flag.
    std::uint32_t   TF:1;       // Trap flag.
    std::uint32_t   IF:1;       // Interrupt flag.
    std::uint32_t   DF:1;       // Direction flag.
    std::uint32_t   OF:1;       // Overflow flag.
    std::uint32_t   IOPL:2;     // I/O privilege level.
    std::uint32_t   NT:1;       // Nested task.
    std::uint32_t     :1;
    std::uint32_t   RF:1;       // Resume flag.
    std::uint32_t   VM:1;       // Virtual-8086 mode.
    std::uint32_t   AC:1;       // Alignment check.
    std::uint32_t   VIF:1;      // Virtual interrupt flag.
    std::uint32_t   VIP:1;      // Virtual interrupt pending.
    std::uint32_t   ID:1;       // CPUID test flag.
    std::uint32_t     :0;
};

static_assert(sizeof(eflags_bits_t) == 4);

struct eflags_t
{
    union
    {
        eflags_bits_t bits;
        std::uint32_t all;
    };
};

struct rflags_t
{
    union
    {
        eflags_bits_t bits;
        std::uint64_t all;
    };
};

static_assert(sizeof(eflags_t) == 4);
static_assert(sizeof(rflags_t) == 8);


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_EFLAGS_H */
