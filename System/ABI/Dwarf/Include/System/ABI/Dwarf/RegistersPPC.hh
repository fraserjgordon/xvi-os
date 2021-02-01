#pragma once
#ifndef __SYSTEM_ABI_DWARF_REGISTERS_PPC_H
#define __SYSTEM_ABI_DWARF_REGISTERS_PPC_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/ExecContext/Arch/PowerPC/Frame.hh>


namespace System::ABI::Dwarf
{


// Forward declarations.
enum class reg_ppc : std::int16_t;
template <std::size_t> struct FrameTraitsPPC;


// Alias for PowerPC targets.
#if defined(_ARCH_PPC)
#  if defined(_ARCH_PPC64)
using FrameTraitsNative = FrameTraitsPPC<64>;
#  else
using FrameTraitsNative = FrameTraitsPPC<32>;
#  endif
#endif


// The DWARF register numbers for PowerPC.
//! @todo: seems to be incorrect for (at least) PPC64...
enum class reg_ppc : std::int16_t
{
    // Not defined by the PowerPC DWARF supplement.
    pc          = -1,
    
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
    r16         = 16,
    r17         = 17,
    r18         = 18,
    r19         = 19,
    r20         = 20,
    r21         = 21,
    r22         = 22,
    r23         = 23,
    r24         = 24,
    r25         = 25,
    r26         = 26,
    r27         = 27,
    r28         = 28,
    r29         = 29,
    r30         = 30,
    r31         = 31,

    // Floating-point registers.
    f0          = 32,
    f1          = 33,
    f2          = 34,
    f3          = 35,
    f4          = 36,
    f5          = 37,
    f6          = 38,
    f7          = 39,
    f8          = 40,
    f9          = 41,
    f10         = 42,
    f11         = 43,
    f12         = 44,
    f13         = 45,
    f14         = 46,
    f15         = 47,
    f16         = 48,
    f17         = 49,
    f18         = 50,
    f19         = 51,
    f20         = 52,
    f21         = 53,
    f22         = 54,
    f23         = 55,
    f24         = 56,
    f25         = 57,
    f26         = 58,
    f27         = 59,
    f28         = 60,
    f29         = 61,
    f30         = 62,
    f31         = 63,

    // Special registers.
    lr          = 65,
    ctr         = 66,

    // Condition registers.
    cr0         = 68,
    cr1         = 69,
    cr2         = 70,
    cr3         = 71,
    cr4         = 72,
    cr5         = 73,
    cr6         = 74,
    cr7         = 75,

    // Exception registers.
    xer         = 76,

    // AltiVec registers.
    v0          = 77,
    v1          = 78,
    v2          = 79,
    v3          = 80,
    v4          = 81,
    v5          = 82,
    v6          = 83,
    v7          = 84,
    v8          = 85,
    v9          = 86,
    v10         = 87,
    v11         = 88,
    v12         = 89,
    v13         = 90,
    v14         = 91,
    v15         = 92,
    v16         = 93,
    v17         = 94,
    v18         = 95,
    v19         = 96,
    v20         = 97,
    v21         = 98,
    v22         = 99,
    v23         = 100,
    v24         = 101,
    v25         = 102,
    v26         = 103,
    v27         = 104,
    v28         = 105,
    v29         = 106,
    v30         = 107,
    v31         = 108,

    // AltiVec control registers.
    vrsave      = 109,
};

constexpr auto operator<=>(const reg_ppc& a, const reg_ppc& b)
{
    using U = std::underlying_type_t<reg_ppc>;
    return static_cast<U>(a) <=> static_cast<U>(b);
}


// Traits class for PowerPC.
template <std::size_t Bits>
struct FrameTraitsPPC
{
    using reg_enum_t    = reg_ppc;

    // Register save slots are 32-bit.
    using reg_t         = std::conditional_t<Bits == 32, std::uint32_t, std::conditional_t<Bits == 64, std::uint64_t, void>>;

    //! @todo: implement floating-point register save/restore.
    using float_reg_t   = void;

    // Which registers are used for special purposes.
    static constexpr auto kStackPointerReg          = reg_ppc::r1;
    static constexpr auto kFramePointerReg          = reg_ppc::r31;
    static constexpr auto kInstructionPointerReg    = reg_ppc::pc;

    // Stack grows downwards.
    static constexpr std::ptrdiff_t kStackDirection = -1;

    // Callee-saved registers.
    static constexpr auto kCalleeSaveRegisters =
    { 
        reg_ppc::r14,
        reg_ppc::r15,
        reg_ppc::r16,
        reg_ppc::r17,
        reg_ppc::r18,
        reg_ppc::r19,
        reg_ppc::r20,
        reg_ppc::r21,
        reg_ppc::r22,
        reg_ppc::r23,
        reg_ppc::r24,
        reg_ppc::r25,
        reg_ppc::r26,
        reg_ppc::r27,
        reg_ppc::r28,
        reg_ppc::r29,
        reg_ppc::r30,
        reg_ppc::r31,
    };

    // Offset to apply when converting the CFA into the stack pointer.
    static constexpr std::ptrdiff_t kStackPointerOffset = 0;

    // All core integer registers, LR, CTR, CR0-8, XER plus the CFA and return address.
    static constexpr std::size_t kUnwindRegisterCount = 36;
    static constexpr std::size_t kUnwindStorageCount  = kUnwindRegisterCount + 2;

    //! @todo: floating point save and resture.

    // Structure suitable for holding the preserved registers.
    struct reg_storage_t
    {
        reg_t gp[kUnwindStorageCount];

        constexpr bool IsConditionRegister(reg_enum_t which) const
        {
            return (reg_ppc::cr0 <= which && which <= reg_ppc::cr7);
        }

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
            return gp[1];
        }
        void SetReturnAddress(reg_t ra)
        {
            gp[1] = ra;
        }

        reg_t GetConditionRegister(reg_enum_t which) const
        {
            auto offset = std::size_t(which) - std::size_t(reg_ppc::cr0);
            auto cr = gp[RegisterToIndex(reg_ppc::cr0)];
            auto shift = offset * 4;
            return (cr >> shift) & 0x0F;
        }
        void SetConditionRegister(reg_enum_t which, reg_t value)
        {
            auto index = RegisterToIndex(reg_ppc::cr0);
            auto offset = std::size_t(which) - std::size_t(reg_ppc::cr0);
            auto shift = offset * 4;
            auto mask = (0x0F << shift);

            auto cr = gp[index];
            cr = (cr & ~mask) | ((value << shift) & mask);
            gp[index] = cr;
        }

        // General-purpose register get/set methods.
        reg_t GetGPRegister(reg_enum_t which) const
        {
            // Assumes: IsValidGPRegister(which).
            if (IsConditionRegister(which))
                return GetConditionRegister(which);

            return gp[RegisterToIndex(which)];
        }
        void SetGPRegister(reg_enum_t which, reg_t value)
        {
            // Assumes: IsValidGPRegister(which).
            if (IsConditionRegister(which))
                return SetConditionRegister(which, value);

            gp[RegisterToIndex(which)] = value;
        }

        using frame_t = std::conditional_t<Bits == 64, ExecContext::ppc64_frame_t, ExecContext::ppc32_frame_t>;
        using full_frame_t = std::conditional_t<Bits == 64, ExecContext::ppc64_full_frame_t, ExecContext::ppc32_full_frame_t>;

        void CaptureFrame(const frame_t& frame)
        {
            SetGPRegister(reg_ppc::r14, frame.r14);
            SetGPRegister(reg_ppc::r15, frame.r15);
            SetGPRegister(reg_ppc::r16, frame.r16);
            SetGPRegister(reg_ppc::r17, frame.r17);
            SetGPRegister(reg_ppc::r18, frame.r18);
            SetGPRegister(reg_ppc::r19, frame.r19);
            SetGPRegister(reg_ppc::r20, frame.r20);
            SetGPRegister(reg_ppc::r21, frame.r21);
            SetGPRegister(reg_ppc::r22, frame.r22);
            SetGPRegister(reg_ppc::r23, frame.r23);
            SetGPRegister(reg_ppc::r24, frame.r24);
            SetGPRegister(reg_ppc::r25, frame.r25);
            SetGPRegister(reg_ppc::r26, frame.r26);
            SetGPRegister(reg_ppc::r27, frame.r27);
            SetGPRegister(reg_ppc::r28, frame.r28);
            SetGPRegister(reg_ppc::r29, frame.r29);
            SetGPRegister(reg_ppc::r30, frame.r30);
            SetGPRegister(reg_ppc::r31, frame.r31);

            SetGPRegister(reg_ppc::cr0, frame.cr);
            SetGPRegister(reg_ppc::xer, frame.xer);
            SetReturnAddress(frame.pc);
        }

        void CaptureFullFrame(const full_frame_t& frame)
        {
            auto r0 = static_cast<std::underlying_type_t<reg_ppc>>(reg_ppc::r0);
            for (int i = 0; i < 32; ++i)
                SetGPRegister(static_cast<reg_ppc>(r0 + i), frame.r[i]);

            SetGPRegister(reg_ppc::lr, frame.lr);
            SetGPRegister(reg_ppc::ctr, frame.ctr);
            SetGPRegister(reg_ppc::cr0, frame.cr);
            SetGPRegister(reg_ppc::xer, frame.xer);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(frame_t& frame)
        {
            frame.r14 = GetGPRegister(reg_ppc::r14);
            frame.r15 = GetGPRegister(reg_ppc::r15);
            frame.r16 = GetGPRegister(reg_ppc::r16);
            frame.r17 = GetGPRegister(reg_ppc::r17);
            frame.r18 = GetGPRegister(reg_ppc::r18);
            frame.r19 = GetGPRegister(reg_ppc::r19);
            frame.r20 = GetGPRegister(reg_ppc::r20);
            frame.r21 = GetGPRegister(reg_ppc::r21);
            frame.r22 = GetGPRegister(reg_ppc::r22);
            frame.r23 = GetGPRegister(reg_ppc::r23);
            frame.r24 = GetGPRegister(reg_ppc::r24);
            frame.r25 = GetGPRegister(reg_ppc::r25);
            frame.r26 = GetGPRegister(reg_ppc::r26);
            frame.r27 = GetGPRegister(reg_ppc::r27);
            frame.r28 = GetGPRegister(reg_ppc::r28);
            frame.r29 = GetGPRegister(reg_ppc::r29);
            frame.r30 = GetGPRegister(reg_ppc::r30);
            frame.r31 = GetGPRegister(reg_ppc::r31);

            frame.r1 = GetCFA();
            frame.cr = GetGPRegister(reg_ppc::cr0);
            frame.xer = GetGPRegister(reg_ppc::xer);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(full_frame_t& frame)
        {
            auto r0 = static_cast<std::underlying_type_t<reg_ppc>>(reg_ppc::r0);
            for (int i = 0; i <32; ++i)
                frame.r[i] = GetGPRegister(static_cast<reg_ppc>(r0 + i));

            frame.lr = GetGPRegister(reg_ppc::lr);
            frame.ctr = GetGPRegister(reg_ppc::ctr);
            frame.cr = GetGPRegister(reg_ppc::cr0);
            frame.xer = GetGPRegister(reg_ppc::xer);
            frame.r[1] = GetCFA();
            frame.pc = GetReturnAddress();
        }

        // Validates the given general-purpose register number.
        static constexpr bool IsValidGPRegister(std::size_t n)
        {
            return (0 <= n && n <= 31);
        }

        // Validates the given floating-point register number.
        static constexpr bool IsValidFPRegister(std::size_t)
        {
            //! @todo implement
            return false;
        }
    };

    static reg_enum_t IndexToRegister(int i)
    {
        switch (i)
        {
            case 2:
                return reg_ppc::lr;

            case 3:
                return reg_ppc::ctr;

            case 4:
                return reg_ppc::cr0;

            case 5:
                return reg_ppc::xer;

            default:
                return reg_enum_t(std::size_t(reg_ppc::r0) + i - 6);
        }
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        switch (reg)
        {
            case reg_ppc::lr:
                return 2;

            case reg_ppc::ctr:
                return 3;

            case reg_ppc::cr0:
                return 4;

            case reg_ppc::xer:
                return 5;

            default:
                return std::ptrdiff_t(reg) - std::ptrdiff_t(reg_ppc::r0) + 6;
        }
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
            case reg_enum_t::r16:
                return "r16";
            case reg_enum_t::r17:
                return "r17";
            case reg_enum_t::r18:
                return "r18";
            case reg_enum_t::r19:
                return "r19";
            case reg_enum_t::r20:
                return "r20";
            case reg_enum_t::r21:
                return "r21";
            case reg_enum_t::r22:
                return "r22";
            case reg_enum_t::r23:
                return "r23";
            case reg_enum_t::r24:
                return "r24";
            case reg_enum_t::r25:
                return "r25";
            case reg_enum_t::r26:
                return "r26";
            case reg_enum_t::r27:
                return "r27";
            case reg_enum_t::r28:
                return "r28";
            case reg_enum_t::r29:
                return "r29";
            case reg_enum_t::r30:
                return "r30";
            case reg_enum_t::r31:
                return "r31";
            case reg_enum_t::lr:
                return "lr";
            case reg_enum_t::ctr:
                return "ctr";
            case reg_enum_t::cr0:
                return "cr0";
            case reg_enum_t::cr1:
                return "cr1";
            case reg_enum_t::cr2:
                return "cr2";
            case reg_enum_t::cr3:
                return "cr3";
            case reg_enum_t::cr4:
                return "cr4";
            case reg_enum_t::cr5:
                return "cr5";
            case reg_enum_t::cr6:
                return "cr6";
            case reg_enum_t::cr7:
                return "cr7";
            case reg_enum_t::xer:
                return "xer";
            default:
                return "<unknown>";
        }
    }
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_ARM_H */
