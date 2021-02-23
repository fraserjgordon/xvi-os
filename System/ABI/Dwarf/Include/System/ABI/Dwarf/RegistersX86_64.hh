#pragma once
#ifndef __SYSTEM_ABI_DWARF_REGISTERS_X86_64_H
#define __SYSTEM_ABI_DWARF_REGISTERS_X86_64_H


#include <System/ABI/ExecContext/Arch/x86/SysV_x64.h>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::Dwarf
{


// Forward declarations.
enum class reg_x86_64 : std::uint8_t;
struct FrameTraitsX86_64;


// Alias for x86_64 targets.
#if defined(__x86_64__)
using FrameTraitsNative = FrameTraitsX86_64;
#endif


// The DWARF register numbers for AMD64 (aka x86_64).
enum class reg_x86_64 : std::uint8_t
{
    // General-purpose integer registers.
    rax         = 0,
    rdx         = 1,
    rcx         = 2,
    rbx         = 3,
    rsi         = 4,
    rdi         = 5,
    rbp         = 6,
    rsp         = 7,
    r8          = 8,
    r9          = 9,
    r10         = 10,
    r11         = 11,
    r12         = 12,
    r13         = 13,
    r14         = 14,
    r15         = 15,

    // Instruction pointer.
    return_addr = 16,

    // SSE registers (%xmmX/%ymmX/%zmmX, depending on SSE/AVX flavour). Note that registers 16-31 do not have numbers
    // contiguous with the lower registers.
    xmm0        = 17,
    xmm1        = 18,
    xmm2        = 19,
    xmm3        = 20,
    xmm4        = 21,
    xmm5        = 22,
    xmm6        = 23,
    xmm7        = 24,
    xmm8        = 25,
    xmm9        = 26,
    xmm10       = 27,
    xmm11       = 28,
    xmm12       = 29,
    xmm13       = 30,
    xmm14       = 31,
    xmm15       = 32,

    // x87 floating-point registers.
    st0         = 33,
    st1         = 34,
    st2         = 35,
    st3         = 36,
    st4         = 37,
    st5         = 38,
    st6         = 39,
    st7         = 40,

    // MMX registers - these are the same physical registers as %st(0) to %st(7).
    mm0         = 41,
    mm1         = 42,
    mm2         = 43,
    mm3         = 44,
    mm4         = 45,
    mm5         = 46,
    mm6         = 47,
    mm7         = 48,

    // Flags register.
    rflags      = 49,

    // Segment selector registers.
    es          = 50,
    cs          = 51,
    ss          = 52,
    ds          = 53,
    fs          = 54,
    gs          = 55,

    // Segment base address registers.
    fsbase      = 58,
    gsbase      = 59,

    // Misc. system registers.
    tr          = 62,       // Task register.
    ldtr        = 63,       // Local descriptor table register.
    mxcsr       = 64,       // SSE control/status register.
    fcw         = 65,       // x87 control word.
    fsw         = 66,       // x87 status word.

    // Additional SSE registers.
    xmm16       = 67,
    xmm17       = 68,
    xmm18       = 69,
    xmm19       = 70,
    xmm20       = 71,
    xmm21       = 72,
    xmm22       = 73,
    xmm23       = 74,
    xmm24       = 75,
    xmm25       = 76,
    xmm26       = 77,
    xmm27       = 78,
    xmm28       = 79,
    xmm29       = 80,
    xmm30       = 81,
    xmm31       = 82,

    // Vector mask registers.
    k0          = 118,
    k1          = 119,
    k2          = 120,
    k3          = 121,
    k4          = 122,
    k5          = 123,
    k6          = 124,
    k7          = 125,
};


// Traits class for x86_64.
struct FrameTraitsX86_64
{
    using reg_enum_t    = reg_x86_64;

    // Register save slots are 64-bit.
    using reg_t         = std::uint64_t;

    // Floating-point registers are not preserved.
    using float_reg_t   = void;

    // Which registers are used for special purposes.
    static constexpr auto kStackPointerReg          = reg_x86_64::rsp;
    static constexpr auto kInstructionPointerReg    = reg_x86_64::return_addr;

    // Stack grows downwards.
    static constexpr std::ptrdiff_t kStackDirection = -1;

    // Callee-saved registers.
    static constexpr auto kCalleeSaveRegisters =
    { 
        reg_x86_64::rbx,
        reg_x86_64::rbp,
        reg_x86_64::r11,
        reg_x86_64::r12,
        reg_x86_64::r13,
        reg_x86_64::r14,
        reg_x86_64::r15,
    };

    // Offset to apply when converting the CFA into the stack pointer.
    static constexpr std::ptrdiff_t kStackPointerOffset = 0;

    // The saved registers are the 16 general-purpose integer registers, the CFA and the return address. The only other
    // registers that the SysV AMD64 ABI requires to be preserved are the floating-point control registers; we can omit
    // these as long as the unwinder code does not change them.
    static constexpr std::size_t kUnwindRegisterCount = 16; // Excluding the CFA and return "register".
    static constexpr std::size_t kUnwindStorageCount  = kUnwindRegisterCount + 2;

    // None of the floating-point or SSE registers are saved; they are all volatile across calls. The control words do
    // need to be preserved, however.

    // Structure suitable for holding the preserved registers.
    struct reg_storage_t
    {
        reg_t gp[kUnwindStorageCount];

    #if __SYSTEM_ABI_DWARF_FLOAT_SUPPORT
        __uint16_t fcw;
        __uint16_t mxcsr;
    #endif

        // CFA get/set methods.
        reg_t GetCFA() const
        {
            return gp[0];
        }
        void SetCFA(reg_t cfa)
        {
            gp[0] = cfa;
        }

        // Return address get/set methods.
        reg_t GetReturnAddress() const
        {
            return gp[std::size_t(reg_x86_64::return_addr)];
        }
        void SetReturnAddress(reg_t ra)
        {
            gp[std::size_t(reg_x86_64::return_addr)] = ra;
        }

        // General-purpose register get/set methods.
        reg_t GetGPRegister(reg_enum_t which) const
        {
            // Assumes: IsValidGPRegister(which).
            return gp[std::size_t(which) + 1];
        }
        void SetGPRegister(reg_enum_t which, reg_t value)
        {
            // Assumes: IsValidGPRegister(which).
            gp[std::size_t(which) + 1] = value;
        }

        void CaptureFrame(const ExecContext::sysv_x64_frame_t& frame)
        {
            SetGPRegister(reg_x86_64::rsp, frame.rsp);
            SetGPRegister(reg_x86_64::r12, frame.r12);
            SetGPRegister(reg_x86_64::r13, frame.r13);
            SetGPRegister(reg_x86_64::r14, frame.r14);
            SetGPRegister(reg_x86_64::r15, frame.r15);
            SetGPRegister(reg_x86_64::rbx, frame.rbx);
            SetGPRegister(reg_x86_64::rbp, frame.rbp);
            SetGPRegister(reg_x86_64::rsp, frame.rsp);
            SetReturnAddress(frame.rip);
        }

        void ConfigureFrame(ExecContext::sysv_x64_frame_t& frame)
        {
            frame.r12 = GetGPRegister(reg_x86_64::r12);
            frame.r13 = GetGPRegister(reg_x86_64::r13);
            frame.r14 = GetGPRegister(reg_x86_64::r14);
            frame.r15 = GetGPRegister(reg_x86_64::r15);
            frame.rbx = GetGPRegister(reg_x86_64::rbx);
            frame.rbp = GetGPRegister(reg_x86_64::rbp);
            frame.rsp = GetCFA();
            frame.rip = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::sysv_x64_integer_t& frame)
        {
            ConfigureFrame(frame);
            frame.rax = GetGPRegister(reg_x86_64::rax);
            frame.rcx = GetGPRegister(reg_x86_64::rcx);
            frame.rdx = GetGPRegister(reg_x86_64::rdx);
            frame.rsi = GetGPRegister(reg_x86_64::rsi);
            frame.rdi = GetGPRegister(reg_x86_64::rdi);
            frame.rbp = GetGPRegister(reg_x86_64::rbp);
            frame.r8  = GetGPRegister(reg_x86_64::r8);
            frame.r9  = GetGPRegister(reg_x86_64::r9);
            frame.r10 = GetGPRegister(reg_x86_64::r10);
            frame.r11 = GetGPRegister(reg_x86_64::r11);
        }

        // Validates the given general-purpose register number.
        static constexpr bool IsValidGPRegister(std::size_t n)
        {
            // Only registers in the range %rax-%r15 are valid.
            return (n >= std::size_t(reg_x86_64::rax) && n <= std::size_t(reg_x86_64::r15));
        }

        // Validates the given floating-point register number.
        static constexpr bool IsValidFPRegister(std::size_t)
        {
            // None of the FP or SSE registers are preserved so none are valid.
            return false;
        }
    };

    static reg_enum_t IndexToRegister(int i)
    {
        return reg_enum_t(std::size_t(reg_x86_64::rax) + i);
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        return std::ptrdiff_t(reg) - std::ptrdiff_t(reg_x86_64::rax);
    }

    // Returns the name of the given register.
    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        switch (r)
        {
            case reg_enum_t::rax:
                return "%rax";
            case reg_enum_t::rdx:
                return "%rdx";
            case reg_enum_t::rcx:
                return "%rcx";
            case reg_enum_t::rbx:
                return "%rbx";
            case reg_enum_t::rsi:
                return "%rsi";
            case reg_enum_t::rdi:
                return "%rdi";
            case reg_enum_t::rbp:
                return "%rbp";
            case reg_enum_t::rsp:
                return "%rsp";
            case reg_enum_t::r8:
                return "%r8";
            case reg_enum_t::r9:
                return "%r9";
            case reg_enum_t::r10:
                return "%r10";
            case reg_enum_t::r11:
                return "%r11";
            case reg_enum_t::r12:
                return "%r12";
            case reg_enum_t::r13:
                return "%r13";
            case reg_enum_t::r14:
                return "%r14";
            case reg_enum_t::r15:
                return "%r15";
            case reg_enum_t::return_addr:
                return "%rip";
            default:
                return "<unknown>";
        }
    }
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_X86_64_H */
