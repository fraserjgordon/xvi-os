#pragma once
#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_FLOATINGPOINT_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_FLOATINGPOINT_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/ExecContext/Private/Config.h>


#if defined(__XVI_KERNEL)
#  define __ALL_FP_REGS
#else
#  define __ALL_FP_REGS "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)"
#endif


namespace System::ABI::ExecContext
{


struct fenv_t
{
    std::uint32_t control_word; // Control word.
    std::uint32_t status_word;  // Status word.
    std::uint32_t tag_word;     // Tag word.
    std::uint32_t fip;          // FPU instruction pointer.
    std::uint16_t fcs;          // FPU code segment selector.
    std::uint16_t opcode;       // FPU opcode bits 0-10.
    std::uint32_t fdp;          // FPU data pointer.
    std::uint32_t fds;          // FPU data segment selector.
};

static_assert(sizeof(fenv_t) == 28);


struct fsave_t :
    public fenv_t
{
    // Storage type for the x87 FPU registers. They are stored as packed 80-bit quantities.
    struct st_t { std::uint8_t bytes alignas(2) [10];  };

    // The x87 FPU register stack. Also stores the MMX state.
    st_t st[8];
};

static_assert(sizeof(fsave_t) == 108);


struct fxsave_t
{
    // Storage type for x87/MMX registers in this save area.
    struct st_t { std::uint8_t bytes alignas(4) [16]; };

    // Storage type for SSE registers in this save area.
    struct xmm_t { std::uint8_t bytes alignas(16) [16]; };

    // Control, status and other FPU information.
    std::uint16_t fcw;  // x87 FPU control word.
    std::uint16_t fsw;  // x87 FPU status word.
    std::uint16_t ftw;  // Abridged version of the x87 tag word.
    std::uint16_t fop;  // FPU opcode.
    union
    {
        // 64-bit instruction pointer or 16:32-bit cs:ip.
        std::uint64_t ip64;
        struct
        {
            std::uint32_t ip32;
            std::uint16_t cs;
        };
    };
    union
    {
        // 64-bit data pointer or 16:32-bit ds:offset.
        std::uint64_t dp64;
        struct
        {
            std::uint32_t dp32;
            std::uint16_t ds;
        };
    };
    std::uint32_t mxcsr;        // SSE control and status register.
    std::uint32_t mxcsr_mask;

    // The x87 FPU register stack (also the MMX registers).
    st_t st[8];

    // The lower 128 bits of the first 16 SSE/AVX registers.
    xmm_t xmm[16];

    // Padding - the processor requires this area to be 512 bytes in size.
    std::uint8_t padding[96];
};

static_assert(sizeof(fxsave_t) == 512);


// Saves the floatint-point control registers and then resets them.
static inline void fnstenv(fenv_t& save_area)
{
    asm volatile
    (
        "fnstenv %0"
        : "=m" (save_area)
        :
        : __ALL_FP_REGS
    );
}

// Saves the floating-point control registers and restores the saved state (the x87 instructions for saving these
// registers reset them as a side-effect so an explicit reload is necessary).
static inline void fnstenv_fldenv(fenv_t& save_area)
{
    asm volatile
    (
        "fnstenv    %0\n\t"
        "fldenv     %0\n\t"
        : "=m" (save_area)
        :
        :
    );
}

// Restores the floating-point control and status words.
static inline void fldenv(const fenv_t& save_area)
{
    asm volatile
    (
        "fldenv %0"
        :
        : "m" (save_area)
        : __ALL_FP_REGS
    );
}

// Saves the entire floating-point state and then resets the FPU.
static inline void fnsave(fsave_t& save_area)
{
    asm volatile
    (
        "fnsave %0"
        : "=m" (save_area)
        :
        : __ALL_FP_REGS
    );
}

// Saves the entire floating-point state and then reloads the floating-point control registers (the save operation
// resets the control registers -- but not the data registers -- as a side-effect so this reload is necessary if the 
// register contents need to be preserved).
static inline void fnsave_fldenv(fsave_t& save_area)
{
    asm volatile
    (
        "fnsave     %0\n\t"
        "fldenv     %0\n\t"
        : "=m" (save_area)
        :
        : __ALL_FP_REGS
    );
}

// Restores the floating-point state.
static inline void frstor(const fsave_t& save_area)
{
    asm volatile
    (
        "frstor %0\n\t"
        :
        : "m" (save_area)
        : __ALL_FP_REGS
    );
}

// Resets the registers of the FPU.
static inline void fninit()
{
    asm volatile
    (
        "fninit"
        :
        :
        : __ALL_FP_REGS
    );  
}

// Clears the status flags of the FPU (including pending exceptions).
static inline void fnclex()
{
    asm volatile
    (
        "fnclex"
        :
        :
        : "memory"
    );
}

// Saves the FPU state and %xmm0-%xmm7/%xmm15 (for 32-/64-bit mode).
static inline void fxsave(fxsave_t& save_area)
{
    asm volatile
    (
        "fxsave %0"
        : "=m" (save_area)
        :
        :
    );
}

// Restores the FPU state and %xmm0-%xmm7/%xmm15 (for 32-/64-bit mode).
[[gnu::target("sse")]]
static inline void fxrstor(const fxsave_t& save_area)
{
    asm volatile
    (
        "fxrstor %0"
        :
        : "m" (save_area)
        : 
#if !defined(__XVI_KERNEL)
          "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)",
#endif
          "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
#ifdef __x86_64__
          , "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
#endif
    );
}


__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fnstenv(fenv_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Fnstenv);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fnstenv_fldenv(fenv_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(FnstenvFldenv);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fldenv(const fenv_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Fldenv);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fnsave(fsave_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Fnsave);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fnsave_fldenv(fsave_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(FnsaveFldenv);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fninit() __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Fninit);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fnclex() __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Fnclex);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _frstor(const fsave_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Frstor);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fxsave(fxsave_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Fxsave);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _fxrstor(const fxsave_t*) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Fxrstor);


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_FLOATINGPOINT_H */
