#pragma once
#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_GENERALREGS_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_GENERALREGS_H


#include <cstdint>

#include <System/ABI/ExecContext/Arch/x86/Eflags.hh>


namespace System::ABI::ExecContext
{


#define __MAKE_X86_REG_HL32(e, x, h, l) \
    union { std::uint32_t e; std::uint16_t x; struct { std::uint8_t l, h; };}
#define __MAKE_X86_REG_L32(e, x, l) \
    union { std::uint32_t e; std::uint16_t x; std::uint8_t l; }
#define __MAKE_X86_REG_64(r, e, x, l) \
    union { std::uint64_t r; std::uint32_t e; std::uint16_t x; std::uint8_t l; }


// 32-bit registers as pushed onto the stack via the "pushal" opcode (there is no corresponding opcode in 64-bit mode).
struct pushl_t
{
    __MAKE_X86_REG_L32(edi, di, dil);
    __MAKE_X86_REG_L32(esi, si, sil);
    __MAKE_X86_REG_L32(ebp, bp, bpl);
    __MAKE_X86_REG_L32(esp, sp, spl);
    __MAKE_X86_REG_HL32(ebx, bx, bh, bl);
    __MAKE_X86_REG_HL32(edx, dx, dh, dl);
    __MAKE_X86_REG_HL32(ecx, cx, ch, cl);
    __MAKE_X86_REG_HL32(eax, ax, ah, al);
};

static_assert(sizeof(pushl_t) == 32);


// Helper type for interrupt stack frames with and without error codes.
template <bool ErrorCode>
struct interrupt_error_code_t
{
    std::uint32_t error_code;
};

template <>
struct interrupt_error_code_t<false>
{
};

// Automatically determines the presence or absence of an error code based on the interrupt vector number (which
// interrupts have error codes is determined by the hardware).
template <std::uint8_t Vector>
inline constexpr bool has_hw_error_code = (Vector == 8 || (10 <= Vector && Vector <= 14) || Vector == 17);


// 32-bit registers as pushed onto the stack because of an interrupt without a privilege level change.
template <bool ErrorCode>
struct interrupt32_near_t :
    interrupt_error_code_t<ErrorCode>
{
    __MAKE_X86_REG_L32(eip, ip, ipl);
    std::uint32_t   cs;
    eflags_t        eflags;
};

template <std::uint8_t Vector>
using interrupt32_near_vector_t = interrupt32_near_t<has_hw_error_code<Vector>>;

static_assert(sizeof(interrupt32_near_vector_t<8>) == 16);
static_assert(sizeof(interrupt32_near_vector_t<255>) == 12);

// 32-bit registers as pushed onto the stack because of an interrupt from a different privilege level.
template <bool ErrorCode>
struct interrupt32_far_t :
    interrupt_error_code_t<ErrorCode>
{
    __MAKE_X86_REG_L32(eip, ip, ipl);
    std::uint32_t   cs;
    eflags_t        eflags;
    __MAKE_X86_REG_L32(esp, sp, spl);
    std::uint32_t   ss;
};

template <std::uint8_t Vector>
using interrupt32_far_vector_t = interrupt32_far_t<has_hw_error_code<Vector>>;

static_assert(sizeof(interrupt32_far_vector_t<8>) == 24);
static_assert(sizeof(interrupt32_far_vector_t<255>) == 20);

// 32-bit registers pushed onto the stack because of an interrupt from Virtual-8086 mode. To tell if an interrupt came
// from this mode or not, the value of eflags.bits.V86 needs to be checked.
template <bool ErrorCode>
struct interrupt_v86_t :
    interrupt32_far_t<ErrorCode>
{
    std::uint32_t   es;
    std::uint32_t   ds;
    std::uint32_t   fs;
    std::uint32_t   gs;
};

template <std::uint8_t Vector>
using interrupt_v86_vector_t = interrupt_v86_t<has_hw_error_code<Vector>>;

static_assert(sizeof(interrupt_v86_vector_t<8>) == 40);
static_assert(sizeof(interrupt_v86_vector_t<255>) == 36);

// 64-bit registers as pushed on the stack because of an interrupt. Unlike interrupts in 32-bit mode, there are not
// separate frame layouts depending on privilege level.
template <bool ErrorCode>
struct interrupt64_t :
    interrupt_error_code_t<ErrorCode>
{
    __MAKE_X86_REG_64(rip, eip, ip, ipl);
    std::uint64_t   cs;
    rflags_t        rflags;
    __MAKE_X86_REG_64(rsp, esp, sp, spl);
    std::uint64_t   ss;
};

template <std::uint8_t Vector>
using interrupt64_vector_t = interrupt64_t<has_hw_error_code<Vector>>;

//static_assert(sizeof(interrupt64_vector_t<8>) == 48);
static_assert(sizeof(interrupt64_vector_t<255>) == 40);


struct tss32_t
{
    std::uint32_t   previous_task;
    __MAKE_X86_REG_L32(esp0, sp0, spl0);
    std::uint32_t   ss0;
    __MAKE_X86_REG_L32(esp1, sp1, spl1);
    std::uint32_t   ss1;
    __MAKE_X86_REG_L32(esp2, sp2, spl2);
    std::uint32_t   ss2;
    std::uint32_t   cr3;
    __MAKE_X86_REG_L32(eip, ip, ipl);
    eflags_t        eflags;
    __MAKE_X86_REG_HL32(eax, ax, ah, al);
    __MAKE_X86_REG_HL32(ecx, cx, ch, cl);
    __MAKE_X86_REG_HL32(edx, dx, dh, dl);
    __MAKE_X86_REG_HL32(ebx, bx, bh, bl);
    __MAKE_X86_REG_L32(esp, sp, spl);
    __MAKE_X86_REG_L32(ebp, bp, bpl);
    __MAKE_X86_REG_L32(esi, si, sil);
    __MAKE_X86_REG_L32(edi, di, dil);
    std::uint32_t   es;
    std::uint32_t   cs;
    std::uint32_t   ss;
    std::uint32_t   ds;
    std::uint32_t   fs;
    std::uint32_t   gs;
    std::uint32_t   ldt;
    std::uint16_t   debug_trap;
    std::uint16_t   iomap_base;
};

static_assert(sizeof(tss32_t) == 104);


// Segment registers.
//
// While there isn't a single instruction that saves the segment registers like this, they do get saved as a group under
// certain circumstances (interrupts from Virtual-8086 mode; hardware task switching) so this structure uses the same
// order as those places (where possible).
struct segment_regs_t
{
    std::uint32_t   es;     // Data segment register.
    std::uint32_t   cs;     // Code segment register.
    std::uint32_t   ss;     // Stack segment register.
    std::uint32_t   ds;     // Data segment register.
    std::uint32_t   fs;     // Extra segment register.
    std::uint32_t   gs;     // Extra segment register.
};


} // namespace System::ABI::ExecContext


#undef __MAKE_X86_REG_HL32
#undef __MAKE_X86_REG_L32
#undef __MAKE_X86_REG_64


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_GENERALREGS_H */
