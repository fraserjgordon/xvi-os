#pragma once
#ifndef __SYSTEM_ABI_DWARF_REGISTERS_X86_H
#define __SYSTEM_ABI_DWARF_REGISTERS_X86_H


#include <System/ABI/ExecContext/Arch/x86/SysV_x86.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::Dwarf
{

// Forward declarations.
enum class reg_x86 : std::uint8_t;
struct FrameTraitsX86;


// Alias for x86 targets.
#if defined(__i386__)
using FrameTraitsNative = FrameTraitsX86;
#endif




enum class reg_x86 : std::uint8_t
{
    // Core integer registers.
    eax             = 0,
    ecx             = 1,
    edx             = 2,
    ebx             = 3,
    esp             = 4,
    ebp             = 5,
    esi             = 6,
    edi             = 7,
    return_addr     = 8,

    // CPU status flags.
    eflags          = 9,

    // 80-bit floating-point registers.
    st0             = 11,
    st1             = 12,
    st2             = 13,
    st3             = 14,
    st4             = 15,
    st5             = 16,
    st6             = 17,
    st7             = 18,

    // SSE registers.
    xmm0            = 21,
    xmm1            = 22,
    xmm2            = 23,
    xmm3            = 24,
    xmm4            = 25,
    xmm5            = 26,
    xmm6            = 27,
    xmm7            = 28,

    // 64-bit MMX registers (aliased to the floating point registers).
    mm0             = 29,
    mm1             = 30,
    mm2             = 31,
    mm3             = 32,
    mm4             = 33,
    mm5             = 34,
    mm6             = 35,
    mm7             = 36,

    // SSE control register.
    mxcsr           = 39,

    // Segment registers.
    es              = 40,
    cs              = 41,
    ss              = 42,
    ds              = 43,
    fs              = 44,
    gs              = 45,
    tr              = 48,
    ldtr            = 49,

    // AVX mask registers.
    k0              = 93,
    k1              = 94,
    k2              = 95,
    k3              = 96,
    k4              = 97,
    k5              = 98,
    k6              = 99,
    k7              = 100,
};


// Traits class for x86.
struct FrameTraitsX86
{
    using reg_enum_t    = reg_x86;

    // Register save slots are 32-bit.
    using reg_t         = std::uint32_t;

    // Floating-point registers are not preserved.
    using float_reg_t   = void;

    // Which registers are used for special purposes.
    static constexpr auto kStackPointerReg          = reg_x86::esp;
    static constexpr auto kInstructionPointerReg    = reg_x86::return_addr;

    // Stack grows downwards.
    static constexpr std::ptrdiff_t kStackDirection = -1;

    // Callee-saved registers.
    static constexpr auto kCalleeSaveRegisters =
    { 
        reg_x86::ebx,
        reg_x86::esi,
        reg_x86::edi,
        reg_x86::ebp,
    };

    // Offset to apply when converting the CFA into the stack pointer.
    static constexpr std::ptrdiff_t kStackPointerOffset = 0;

    // The saved registers are the 8 general-purpose integer registers, the CFA and the return address. The only other
    // registers that the SysV X86 ABI requires to be preserved are the floating-point control registers; we can omit
    // these as long as the unwinder code does not change them.
    static constexpr std::size_t kUnwindRegisterCount = 8; // Excluding the CFA and return "register".
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
            return gp[std::size_t(reg_x86::return_addr)];
        }
        void SetReturnAddress(reg_t ra)
        {
            gp[std::size_t(reg_x86::return_addr)] = ra;
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

        void CaptureFrame(const ExecContext::sysv_x86_frame_t& frame)
        {
            SetGPRegister(reg_x86::esp, frame.esp);
            SetGPRegister(reg_x86::esi, frame.esi);
            SetGPRegister(reg_x86::edi, frame.edi);
            SetGPRegister(reg_x86::ebx, frame.ebx);
            SetGPRegister(reg_x86::ebp, frame.ebp);
            SetReturnAddress(frame.eip);
        }

        void ConfigureFrame(ExecContext::sysv_x86_frame_t& frame)
        {
            frame.esi = GetGPRegister(reg_x86::esi);
            frame.edi = GetGPRegister(reg_x86::edi);
            frame.ebx = GetGPRegister(reg_x86::ebx);
            frame.ebp = GetGPRegister(reg_x86::ebp);
            frame.esp = GetCFA();
            frame.eip = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::sysv_x86_integer_t& frame)
        {
            ConfigureFrame(frame);
            frame.eax = GetGPRegister(reg_x86::eax);
            frame.ecx = GetGPRegister(reg_x86::ecx);
            frame.edx = GetGPRegister(reg_x86::edx);
        }

        // Validates the given general-purpose register number.
        static constexpr bool IsValidGPRegister(std::size_t n)
        {
            // Only registers in the range %rax-%r15 are valid.
            return (n >= std::size_t(reg_x86::eax) && n <= std::size_t(reg_x86::edi));
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
        return reg_enum_t(std::size_t(reg_x86::eax) + static_cast<std::size_t>(i));
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        return std::ptrdiff_t(reg) - std::ptrdiff_t(reg_x86::eax);
    }

    // Returns the name of the given register.
    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        switch (r)
        {
            case reg_enum_t::eax:
                return "%eax";
            case reg_enum_t::edx:
                return "%edx";
            case reg_enum_t::ecx:
                return "%ecx";
            case reg_enum_t::ebx:
                return "%ebx";
            case reg_enum_t::esi:
                return "%esi";
            case reg_enum_t::edi:
                return "%edi";
            case reg_enum_t::ebp:
                return "%ebp";
            case reg_enum_t::esp:
                return "%esp";
            case reg_enum_t::return_addr:
                return "%eip";
            default:
                return "<unknown>";
        }
    }
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_X86_H */
