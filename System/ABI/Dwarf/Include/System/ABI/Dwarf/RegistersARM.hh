#pragma once
#ifndef __SYSTEM_ABI_DWARF_REGISTERS_ARM_H
#define __SYSTEM_ABI_DWARF_REGISTERS_ARM_H


#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/ExecContext/Arch/ARM/Frame.hh>


namespace System::ABI::Dwarf
{


// Forward declarations.
enum class reg_arm : std::uint16_t;
enum class reg_arm64 : std::uint16_t;
struct FrameTraitsARM;
struct FrameTraitsARM64;


// Alias for ARM targets.
#if defined(__arm__)
using FrameTraitsNative = FrameTraitsARM;
#elif defined(__aarch64__)
using FrameTraitsNative = FrameTraitsARM64;
#endif


// The DWARF register numbers for ARM.
enum class reg_arm : std::uint16_t
{
    // Core registers.
    r0          = 0,
    r1          = 1,
    r2          = 2,
    r3          = 3,
    r4          = 4,
    r5          = 5,
    r6          = 6,
    r7          = 7,
    r8          = 8,
    r9          = 9,
    r10         = 10,
    r11         = 11,
    r12         = 12,
    r13         = 13,
    r14         = 14,
    r15         = 15,

    // Obsolete encodings for FPA registers.
    // Note: overlaps with obsolete VFP encodings.
    obs_fpa_f0  = 16,
    obs_fpa_f1  = 17,
    obs_fpa_f2  = 18,
    obs_fpa_f3  = 19,
    obs_fpa_f4  = 20,
    obs_fpa_f5  = 21,
    obs_fpa_f6  = 22,
    obs_fpa_f7  = 23,

    // Obsolete encodings for VPF registers.
    // Note: overlaps with obsolete FPA encodings.
    obs_vfp_s0  = 16,
    obs_vfp_s1  = 17,
    obs_vfp_s2  = 18,
    obs_vfp_s3  = 19,
    obs_vfp_s4  = 20,
    obs_vfp_s5  = 21,
    obs_vfp_s6  = 22,
    obs_vfp_s7  = 23,
    obs_vfp_s8  = 24,
    obs_vfp_s9  = 25,
    obs_vfp_s10 = 26,
    obs_vfp_s11 = 27,
    obs_vfp_s12 = 28,
    obs_vfp_s13 = 29,
    obs_vfp_s14 = 30,
    obs_vfp_s15 = 31,
    obs_vfp_s16 = 32,
    obs_vfp_s17 = 33,
    obs_vfp_s18 = 34,
    obs_vfp_s19 = 35,
    obs_vfp_s20 = 36,
    obs_vfp_s21 = 37,
    obs_vfp_s22 = 38,
    obs_vfp_s23 = 39,
    obs_vfp_s24 = 40,
    obs_vfp_s25 = 41,
    obs_vfp_s26 = 42,
    obs_vfp_s27 = 43,
    obs_vfp_s28 = 44,
    obs_vfp_s29 = 45,
    obs_vfp_s30 = 46,
    obs_vfp_s31 = 47,

    // VFPv2 registers.
    vfpv2_s0    = 64,
    vfpv2_s1    = 65,
    vfpv2_s2    = 66,
    vfpv2_s3    = 67,
    vfpv2_s4    = 68,
    vfpv2_s5    = 69,
    vfpv2_s6    = 70,
    vfpv2_s7    = 71,
    vfpv2_s8    = 72,
    vfpv2_s9    = 73,
    vfpv2_s10   = 74,
    vfpv2_s11   = 75,
    vfpv2_s12   = 76,
    vfpv2_s13   = 77,
    vfpv2_s14   = 78,
    vfpv2_s15   = 79,
    vfpv2_s16   = 80,
    vfpv2_s17   = 81,
    vfpv2_s18   = 82,
    vfpv2_s19   = 83,
    vfpv2_s20   = 84,
    vfpv2_s21   = 85,
    vfpv2_s22   = 86,
    vfpv2_s23   = 87,
    vfpv2_s24   = 88,
    vfpv2_s25   = 89,
    vfpv2_s26   = 90,
    vfpv2_s27   = 91,
    vfpv2_s28   = 92,
    vfpv2_s29   = 93,
    vfpv2_s30   = 94,
    vfpv2_s31   = 95,

    // FPA registers.
    fpa_f0      = 96,
    fpa_f1      = 97,
    fpa_f2      = 98,
    fpa_f3      = 99,
    fpa_f4      = 100,
    fpa_f5      = 101,
    fpa_f6      = 102,
    fpa_f7      = 103,

    // IWMMX general-purpose registers.
    // Note: overlaps XScale accumulator registers.
    iwmmx_wcgr0 = 104,
    iwmmx_wcgr1 = 105,
    iwmmx_wcgr2 = 106,
    iwmmx_wcgr3 = 107,
    iwmmx_wcgr4 = 108,
    iwmmx_wcgr5 = 109,
    iwmmx_wcgr6 = 110,
    iwmmx_wcgr7 = 111,

    // XScale accumulator registers.
    // Note: overlaps IWMMX general-purpose registers.
    xscale_acc0 = 104,
    xscale_acc1 = 105,
    xscale_acc2 = 106,
    xscale_acc3 = 107,
    xscale_acc4 = 108,
    xscale_acc5 = 109,
    xscale_acc6 = 110,
    xscale_acc7 = 111,

    // IWMMX data registers.
    iwmmx_wr0   = 112,
    iwmmx_wr1   = 113,
    iwmmx_wr2   = 114,
    iwmmx_wr3   = 115,
    iwmmx_wr4   = 116,
    iwmmx_wr5   = 117,
    iwmmx_wr6   = 118,
    iwmmx_wr7   = 119,
    iwmmx_wr8   = 120,
    iwmmx_wr9   = 121,
    iwmmx_wr10  = 122,
    iwmmx_wr11  = 123,
    iwmmx_wr12  = 124,
    iwmmx_wr13  = 125,
    iwmmx_wr14  = 126,
    iwmmx_wr15  = 127, 

    // System registers.
    spsr        = 128,
    spsr_fiq    = 129,
    spsr_irq    = 130,
    spsr_abt    = 131,
    spsr_und    = 132,
    spsr_svc    = 133,

    // Banked registers.
    r8_usr      = 144,
    r9_usr      = 145,
    r10_usr     = 146,
    r11_usr     = 147,
    r12_usr     = 148,
    r13_usr     = 149,
    r14_usr     = 150,
    r8_fiq      = 151,
    r9_fiq      = 152,
    r10_fiq     = 153,
    r11_fiq     = 154,
    r12_fiq     = 155,
    r13_fiq     = 156,
    r14_fiq     = 157,
    r13_irq     = 158,
    r14_irq     = 159,
    r13_abt     = 160,
    r14_abt     = 161,
    r13_und     = 162,
    r14_und     = 163,
    r13_svc     = 164,
    r14_svc     = 165,

    // IWMMX control registers.
    iwmmx_wc0   = 192,
    iwmmx_wc1   = 193,
    iwmmx_wc2   = 194,
    iwmmx_wc3   = 195,
    iwmmx_wc4   = 196,
    iwmmx_wc5   = 197,
    iwmmx_wc6   = 198,
    iwmmx_wc7   = 199,

    // VFPv3 registers.
    vfpv3_d0    = 256,
    vfpv3_d1    = 257,
    vfpv3_d2    = 258,
    vfpv3_d3    = 259,
    vfpv3_d4    = 260,
    vfpv3_d5    = 261,
    vfpv3_d6    = 262,
    vfpv3_d7    = 263,
    vfpv3_d8    = 264,
    vfpv3_d9    = 265,
    vfpv3_d10   = 266,
    vfpv3_d11   = 267,
    vfpv3_d12   = 268,
    vfpv3_d13   = 269,
    vfpv3_d14   = 270,
    vfpv3_d15   = 271,
    vfpv3_d16   = 272,
    vfpv3_d17   = 273,
    vfpv3_d18   = 274,
    vfpv3_d19   = 275,
    vfpv3_d20   = 276,
    vfpv3_d21   = 277,
    vfpv3_d22   = 278,
    vfpv3_d23   = 279,
    vfpv3_d24   = 280,
    vfpv3_d25   = 281,
    vfpv3_d26   = 282,
    vfpv3_d27   = 283,
    vfpv3_d28   = 284,
    vfpv3_d29   = 285,
    vfpv3_d30   = 286,
    vfpv3_d31   = 287,
};

enum class reg_arm64 : std::uint16_t
{
    // Core integer registers.
    x0          = 0,
    x1          = 1,
    x2          = 2,
    x3          = 3,
    x4          = 4,
    x5          = 5,
    x6          = 6,
    x7          = 7,
    x8          = 8,
    x9          = 9,
    x10         = 10,
    x11         = 11,
    x12         = 12,
    x13         = 13,
    x14         = 14,
    x15         = 15,
    x16         = 16,
    x17         = 17,
    x18         = 18,
    x19         = 19,
    x20         = 20,
    x21         = 21,
    x22         = 22,
    x23         = 23,
    x24         = 24,
    x25         = 25,
    x26         = 26,
    x27         = 27,
    x28         = 28,
    x29         = 29,
    x30         = 30,
    x31         = 31,

    // Frame poiner, link register and stack pointer aliases.
    fp          = x29,
    lr          = x30,
    sp          = x31,

    // Program counter.
    pc          = 32,

    // Special registers.
    elr             = 33,
    ra_sign_state   = 34,
    tpidrro_el0     = 35,
    tpidr_el0       = 36,
    tpidr_el1       = 37,
    tpdir_el2       = 38,
    tpidr_el3       = 39,

    // Vector special registers.
    vg              = 46,
    ffr             = 47,

    // Vector predicate registers.
    p0              = 48,
    p1              = 49,
    p2              = 50,
    p3              = 51,
    p4              = 52,
    p5              = 53,
    p6              = 54,
    p7              = 55,
    p8              = 56,
    p9              = 57,
    p10             = 58,
    p11             = 59,
    p12             = 60,
    p13             = 61,
    p14             = 62,
    p15             = 63,

    // SIMD registers. Note that the vX and zX registers alias each other.
    v0              = 64,
    v1              = 65,
    v2              = 66,
    v3              = 67,
    v4              = 68,
    v5              = 69,
    v6              = 70,
    v7              = 71,
    v8              = 72,
    v9              = 73,
    v10             = 74,
    v11             = 75,
    v12             = 76,
    v13             = 77,
    v14             = 78,
    v15             = 79,
    v16             = 80,
    v17             = 81,
    v18             = 82,
    v19             = 83,
    v20             = 84,
    v21             = 85,
    v22             = 86,
    v23             = 87,
    v24             = 88,
    v25             = 89,
    v26             = 90,
    v27             = 91,
    v28             = 92,
    v29             = 93,
    v30             = 94,
    v31             = 95,
    z0              = 96,
    z1              = 97,
    z2              = 98,
    z3              = 99,
    z4              = 100,
    z5              = 101,
    z6              = 102,
    z7              = 103,
    z8              = 104,
    z9              = 105,
    z10             = 106,
    z11             = 107,
    z12             = 108,
    z13             = 109,
    z14             = 110,
    z15             = 111,
    z16             = 112,
    z17             = 113,
    z18             = 114,
    z19             = 115,
    z20             = 116,
    z21             = 117,
    z22             = 118,
    z23             = 119,
    z24             = 120,
    z25             = 121,
    z26             = 122,
    z27             = 123,
    z28             = 124,
    z29             = 125,
    z30             = 126,
    z31             = 127,
};


// Traits class for ARM.
struct FrameTraitsARM
{
    using reg_enum_t    = reg_arm;

    // Register save slots are 32-bit.
    using reg_t         = std::uint32_t;

    //! @todo: implement floating-point register save/restore.
    using float_reg_t   = void;

    // Which registers are used for special purposes.
    static constexpr auto kStackPointerReg          = reg_arm::r13;
    static constexpr auto kInstructionPointerReg    = reg_arm::r15;

    // Stack grows downwards.
    static constexpr std::ptrdiff_t kStackDirection = -1;

    // Callee-saved registers.
    static constexpr auto kCalleeSaveRegisters =
    { 
        reg_arm::r4,
        reg_arm::r5,
        reg_arm::r6,
        reg_arm::r7,
        reg_arm::r8,
        reg_arm::r9,
        reg_arm::r10,
        reg_arm::r11,
        reg_arm::r13,
    };

    // Offset to apply when converting the CFA into the stack pointer.
    static constexpr std::ptrdiff_t kStackPointerOffset = 0;

    // All core integer registers.
    static constexpr std::size_t kUnwindRegisterCount = 16;
    static constexpr std::size_t kUnwindStorageCount  = kUnwindRegisterCount;

    //! @todo: floating point save and resture.

    // Structure suitable for holding the preserved registers.
    struct reg_storage_t
    {
        reg_t gp[kUnwindStorageCount];

        // CFA get/set methods.
        reg_t GetCFA() const
        {
            return GetGPRegister(reg_arm::r13);
        }
        void SetCFA(reg_t cfa)
        {
            SetGPRegister(reg_arm::r13, cfa);
        }

        // Return address get/set methods.
        reg_t GetReturnAddress() const
        {
            return GetGPRegister(reg_arm::r15);
        }
        void SetReturnAddress(reg_t ra)
        {
            SetGPRegister(reg_arm::r15, ra);
        }

        // General-purpose register get/set methods.
        reg_t GetGPRegister(reg_enum_t which) const
        {
            // Assumes: IsValidGPRegister(which).
            return gp[std::size_t(which)];
        }
        void SetGPRegister(reg_enum_t which, reg_t value)
        {
            // Assumes: IsValidGPRegister(which).
            gp[std::size_t(which)] = value;
        }

        /*void CaptureFrame(const ExecContext::sysv_x64_frame_t& frame)
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
        }*/

        // Validates the given general-purpose register number.
        static constexpr bool IsValidGPRegister(std::size_t n)
        {
            // Only registers in the range %rax-%r15 are valid.
            return (n >= std::size_t(reg_arm::r0) && n <= std::size_t(reg_arm::r15));
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
        return reg_enum_t(std::size_t(reg_arm::r0) + i);
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        return std::ptrdiff_t(reg) - std::ptrdiff_t(reg_arm::r0);
    }

    // Returns the name of the given register.
    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        switch (r)
        {
            case reg_enum_t::r0:
                return "r0";
            case reg_enum_t::r1:
                return "r1";
            case reg_enum_t::r2:
                return "r2";
            case reg_enum_t::r3:
                return "r3";
            case reg_enum_t::r4:
                return "r4";
            case reg_enum_t::r5:
                return "r5";
            case reg_enum_t::r6:
                return "r6";
            case reg_enum_t::r7:
                return "r7";
            case reg_enum_t::r8:
                return "r8";
            case reg_enum_t::r9:
                return "r9";
            case reg_enum_t::r10:
                return "r10";
            case reg_enum_t::r11:
                return "r11";
            case reg_enum_t::r12:
                return "r12";
            case reg_enum_t::r13:
                return "r13";
            case reg_enum_t::r14:
                return "r14";
            case reg_enum_t::r15:
                return "r15";
            default:
                return "<unknown>";
        }
    }
};


// Traits class for ARM64.
struct FrameTraitsARM64
{
    using reg_enum_t    = reg_arm64;

    // Register save slots are 32-bit.
    using reg_t         = std::uint64_t;

    //! @todo: implement floating-point register save/restore.
    using float_reg_t   = void;

    // Which registers are used for special purposes.
    static constexpr auto kFramePointerReg          = reg_arm64::x29;
    static constexpr auto kStackPointerReg          = reg_arm64::sp;
    static constexpr auto kInstructionPointerReg    = reg_arm64::pc;

    // Stack grows downwards.
    static constexpr std::ptrdiff_t kStackDirection = -1;

    // Callee-saved registers.
    static constexpr auto kCalleeSaveRegisters =
    { 
        reg_arm64::x19,
        reg_arm64::x20,
        reg_arm64::x21,
        reg_arm64::x22,
        reg_arm64::x23,
        reg_arm64::x24,
        reg_arm64::x25,
        reg_arm64::x26,
        reg_arm64::x27,
        reg_arm64::x28,
        reg_arm64::x29,
    };

    // Offset to apply when converting the CFA into the stack pointer.
    static constexpr std::ptrdiff_t kStackPointerOffset = 0;

    // All core integer registers plus the CFA and PC.
    static constexpr std::size_t kUnwindRegisterCount = 32;
    static constexpr std::size_t kUnwindStorageCount  = kUnwindRegisterCount + 2;

    //! @todo: floating point save and resture.

    // Structure suitable for holding the preserved registers.
    struct reg_storage_t
    {
        reg_t gp[kUnwindStorageCount];

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
            return GetGPRegister(reg_arm64::pc);
        }
        void SetReturnAddress(reg_t ra)
        {
            SetGPRegister(reg_arm64::pc, ra);
        }

        // General-purpose register get/set methods.
        reg_t GetGPRegister(reg_enum_t which) const
        {
            // Assumes: IsValidGPRegister(which).
            return gp[RegisterToIndex(which)];
        }
        void SetGPRegister(reg_enum_t which, reg_t value)
        {
            // Assumes: IsValidGPRegister(which).
            gp[RegisterToIndex(which)] = value;
        }

        void CaptureFrame(const ExecContext::arm64_frame_t& frame)
        {
            SetGPRegister(reg_arm64::x19, frame.x19);
            SetGPRegister(reg_arm64::x20, frame.x20);
            SetGPRegister(reg_arm64::x21, frame.x21);
            SetGPRegister(reg_arm64::x22, frame.x22);
            SetGPRegister(reg_arm64::x23, frame.x23);
            SetGPRegister(reg_arm64::x24, frame.x24);
            SetGPRegister(reg_arm64::x25, frame.x25);
            SetGPRegister(reg_arm64::x26, frame.x26);
            SetGPRegister(reg_arm64::x27, frame.x27);
            SetGPRegister(reg_arm64::x28, frame.x28);
            SetGPRegister(reg_arm64::x29, frame.x29);
            SetCFA(frame.x29);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(ExecContext::arm64_frame_t& frame)
        {
            frame.x19 = GetGPRegister(reg_arm64::x19);
            frame.x20 = GetGPRegister(reg_arm64::x20);
            frame.x21 = GetGPRegister(reg_arm64::x21);
            frame.x22 = GetGPRegister(reg_arm64::x22);
            frame.x23 = GetGPRegister(reg_arm64::x23);
            frame.x24 = GetGPRegister(reg_arm64::x24);
            frame.x25 = GetGPRegister(reg_arm64::x25);
            frame.x26 = GetGPRegister(reg_arm64::x26);
            frame.x27 = GetGPRegister(reg_arm64::x27);
            frame.x28 = GetGPRegister(reg_arm64::x28);
            frame.x29 = GetGPRegister(reg_arm64::x29);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::arm64_full_frame_t& frame)
        {
            for (int i = 0; i < 32; ++i)
                frame.x[i] = GetGPRegister(static_cast<reg_arm64>(static_cast<std::uint16_t>(reg_arm64::x0) + i));

            frame.pc = GetReturnAddress();
        }

        // Validates the given general-purpose register number.
        static constexpr bool IsValidGPRegister(std::size_t n)
        {
            // Only registers in the range x0-pc are valid.
            return (n >= std::size_t(reg_arm64::x0) && n <= std::size_t(reg_arm64::pc));
        }

        // Validates the given floating-point register number.
        static constexpr bool IsValidFPRegister(std::size_t)
        {
            //! @todo implement.
            return false;
        }
    };

    static reg_enum_t IndexToRegister(int i)
    {
        return reg_enum_t(std::size_t(reg_arm64::x0) + i - 1);
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        return 1 + std::ptrdiff_t(reg) - std::ptrdiff_t(reg_arm64::x0);
    }

    // Returns the name of the given register.
    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        switch (r)
        {
            case reg_enum_t::x0:
                return "x0";
            case reg_enum_t::x1:
                return "x1";
            case reg_enum_t::x2:
                return "x2";
            case reg_enum_t::x3:
                return "x3";
            case reg_enum_t::x4:
                return "x4";
            case reg_enum_t::x5:
                return "x5";
            case reg_enum_t::x6:
                return "x6";
            case reg_enum_t::x7:
                return "x7";
            case reg_enum_t::x8:
                return "x8";
            case reg_enum_t::x9:
                return "x9";
            case reg_enum_t::x10:
                return "x10";
            case reg_enum_t::x11:
                return "x11";
            case reg_enum_t::x12:
                return "x12";
            case reg_enum_t::x13:
                return "x13";
            case reg_enum_t::x14:
                return "x14";
            case reg_enum_t::x15:
                return "x15";
            case reg_enum_t::x16:
                return "x16";
            case reg_enum_t::x17:
                return "x17";
            case reg_enum_t::x18:
                return "x18";
            case reg_enum_t::x19:
                return "x19";
            case reg_enum_t::x20:
                return "x20";
            case reg_enum_t::x21:
                return "x21";
            case reg_enum_t::x22:
                return "x22";
            case reg_enum_t::x23:
                return "x23";
            case reg_enum_t::x24:
                return "x24";
            case reg_enum_t::x25:
                return "x25";
            case reg_enum_t::x26:
                return "x26";
            case reg_enum_t::x27:
                return "x27";
            case reg_enum_t::x28:
                return "x28";
            case reg_enum_t::x29:
                return "x29";
            case reg_enum_t::x30:
                return "x30";
            case reg_enum_t::x31:
                return "sp";
            case reg_enum_t::pc:
                return "pc";
            default:
                return "<unknown>";
        }
    }
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_ARM_H */
