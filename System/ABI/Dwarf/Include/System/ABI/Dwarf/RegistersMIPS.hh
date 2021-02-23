#pragma once
#ifndef __SYSTEM_ABI_DWARF_REGISTERS_MIPS_H
#define __SYSTEM_ABI_DWARF_REGISTERS_MIPS_H


#include <System/ABI/ExecContext/Arch/MIPS/EABI32.h>
#include <System/ABI/ExecContext/Arch/MIPS/EABI64.h>
#include <System/ABI/ExecContext/Arch/MIPS/N32.h>
#include <System/ABI/ExecContext/Arch/MIPS/N64.h>
#include <System/ABI/ExecContext/Arch/MIPS/O32.h>
#include <System/ABI/ExecContext/Arch/MIPS/O64.h>

#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::Dwarf
{


// Forward declarations.
enum class reg_mips : std::uint16_t;
struct FrameTraitsMIPS_O32;
struct FrameTraitsMIPS_O64;
struct FrameTraitsMIPS_N32;
struct FrameTraitsMIPS_N64;
struct FrameTraitsMIPS_EABI32;
struct FrameTraitsMIPS_EABI64;


// Alias for MIPS targets.
#if defined(__mips__)
#  if defined(_ABIO32)
using FrameTraitsNative = FrameTraitsMIPS_O32;
#  elif defined(_ABIO64)
using FrameTraitsNative = FrameTraitsMIPS_O64;
#  elif defined(_ABIN32)
using FrameTraitsNative = FrameTraitsMIPS_N32;
#  elif defined(_ABIN64)
using FrameTraitsNative = FrameTraitsMIPS_N64;
#  elif defined(__mips_eabi)
#    if defined (__mips64)
using FrameTraitsNative = FrameTraitsMIPS_EABI64;
#    else
using FrameTraitsNative = FrameTraitsMIPS_EABI32;
#    endif
#  else
#    error "Unknown MIPS ABI"
#  endif
#endif


// The DWARF register numbers for MIPS.
enum class reg_mips : std::uint16_t
{
    // Core integer registers.
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

    //! @todo: floating point regs, control regs, special regs etc.
};


// Common traits across all MIPS ABIs.
struct FrameTraitsMIPS_Common
{
    using reg_enum_t    = reg_mips;

    // Register sizes vary across ABIs.
    //using reg_t         = ...;
    //using float_reg_t   = ...;

    // Which registers are used for special purposes.
    static constexpr auto kStackPointerReg          = reg_mips::r29;
    static constexpr auto kFramePointerReg          = reg_mips::r30;
    static constexpr auto kInstructionPointerReg    = reg_mips::r31;

    // Stack grows downwards.
    static constexpr std::ptrdiff_t kStackDirection = -1;

    // Offset to apply when converting the CFA into the stack pointer.
    static constexpr std::ptrdiff_t kStackPointerOffset = 0;

    // All core integer registers plus the return address and CFA.
    static constexpr std::size_t kUnwindRegisterCount = 32;
    static constexpr std::size_t kUnwindStorageCount  = kUnwindRegisterCount + 2;

    //! @todo: floating point save and resture.

    // Structure suitable for holding the preserved registers.
    template <class reg_t>
    struct reg_storage
    {
        reg_t gp[kUnwindStorageCount];

        /// CFA get/set methods.
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

        // General-purpose register get/set methods.
        reg_t GetGPRegister(reg_enum_t which) const
        {
            // Assumes: IsValidGPRegister(which).
            return gp[std::size_t(which) + 2];
        }
        void SetGPRegister(reg_enum_t which, reg_t value)
        {
            // Assumes: IsValidGPRegister(which).
            gp[std::size_t(which) + 2] = value;
        }

        // Validates the given general-purpose register number.
        static constexpr bool IsValidGPRegister(std::size_t n)
        {
            // Only registers in the range r0-r31
            return (n >= std::size_t(reg_mips::r0) && n <= std::size_t(reg_mips::r31));
        }

        // Validates the given floating-point register number.
        static constexpr bool IsValidFPRegister(std::size_t)
        {
            //! @todo: floating-point support.
            return false;
        }
    };

    static reg_enum_t IndexToRegister(int i)
    {
        return reg_enum_t(std::ptrdiff_t(reg_mips::r0) + i);
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        return std::ptrdiff_t(reg) - std::ptrdiff_t(reg_mips::r0);
    }

    // Register names depend on the ABI.
    // static constexpr const char* GetRegisterName(reg_enum_t r);
};


struct FrameTraitsMIPS_O32 : FrameTraitsMIPS_Common
{
    using reg_t         = std::uint32_t;

    static constexpr auto kCalleeSaveRegisters =
    {
        reg_mips::r16,
        reg_mips::r17,
        reg_mips::r18,
        reg_mips::r19,
        reg_mips::r20,
        reg_mips::r21,
        reg_mips::r22,
        reg_mips::r23,

        reg_mips::r28,
        reg_mips::r29,
        reg_mips::r30,
    };

    struct reg_storage_t : reg_storage<reg_t>
    {
        void CaptureFrame(const ExecContext::mips_o32_frame_t& frame)
        {
            SetGPRegister(reg_mips::r16, frame.s[0]);
            SetGPRegister(reg_mips::r17, frame.s[1]);
            SetGPRegister(reg_mips::r18, frame.s[2]);
            SetGPRegister(reg_mips::r19, frame.s[3]);
            SetGPRegister(reg_mips::r20, frame.s[4]);
            SetGPRegister(reg_mips::r21, frame.s[5]);
            SetGPRegister(reg_mips::r22, frame.s[6]);
            SetGPRegister(reg_mips::r23, frame.s[7]);
            SetGPRegister(reg_mips::r28, frame.gp);
            SetGPRegister(reg_mips::r29, frame.sp);
            SetGPRegister(reg_mips::r30, frame.s[9]);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(ExecContext::mips_o32_frame_t& frame)
        {
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::mips_o32_full_frame_t& frame)
        {
            frame.at = GetGPRegister(reg_mips::r1);
            frame.v[0] = GetGPRegister(reg_mips::r2);
            frame.v[1] = GetGPRegister(reg_mips::r3);
            frame.a[0] = GetGPRegister(reg_mips::r4);
            frame.a[1] = GetGPRegister(reg_mips::r5);
            frame.a[2] = GetGPRegister(reg_mips::r6);
            frame.a[3] = GetGPRegister(reg_mips::r7);
            frame.t[0] = GetGPRegister(reg_mips::r8);
            frame.t[1] = GetGPRegister(reg_mips::r9);
            frame.t[2] = GetGPRegister(reg_mips::r10);
            frame.t[3] = GetGPRegister(reg_mips::r11);
            frame.t[4] = GetGPRegister(reg_mips::r12);
            frame.t[5] = GetGPRegister(reg_mips::r13);
            frame.t[6] = GetGPRegister(reg_mips::r14);
            frame.t[7] = GetGPRegister(reg_mips::r15);
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.t[8] = GetGPRegister(reg_mips::r24);
            frame.t[9] = GetGPRegister(reg_mips::r25);
            frame.k[0] = GetGPRegister(reg_mips::r26);
            frame.k[1] = GetGPRegister(reg_mips::r27);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.ra = GetGPRegister(reg_mips::r31);
            frame.pc = GetReturnAddress();
        }
    };

    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        switch (r)
        {
            case reg_mips::r0:
                return "$zero";

            case reg_mips::r1:
                return "$at";

            case reg_mips::r2:
                return "$v0";

            case reg_mips::r3:
                return "$v1";

            case reg_mips::r4:
                return "$a0";

            case reg_mips::r5:
                return "$a1";

            case reg_mips::r6:
                return "$a2";

            case reg_mips::r7:
                return "$a3";

            case reg_mips::r8:
                return "$t0";

            case reg_mips::r9:
                return "$t1";

            case reg_mips::r10:
                return "$t2";

            case reg_mips::r11:
                return "$t3";

            case reg_mips::r12:
                return "$t4";

            case reg_mips::r13:
                return "$t5";

            case reg_mips::r14:
                return "$t6";

            case reg_mips::r15:
                return "$t7";

            case reg_mips::r16:
                return "$s0";

            case reg_mips::r17:
                return "$s1";

            case reg_mips::r18:
                return "$s2";

            case reg_mips::r19:
                return "$s3";

            case reg_mips::r20:
                return "$s4";

            case reg_mips::r21:
                return "$s5";

            case reg_mips::r22:
                return "$s6";

            case reg_mips::r23:
                return "$s7";

            case reg_mips::r24:
                return "$t8";

            case reg_mips::r25:
                return "$t9";

            case reg_mips::r26:
                return "$k0";

            case reg_mips::r27:
                return "$k1";

            case reg_mips::r28:
                return "$gp";

            case reg_mips::r29:
                return "$sp";

            case reg_mips::r30:
                return "$fp";

            case reg_mips::r31:
                return "$ra";

            default:
                return "<unknown>";
        }
    }
};

struct FrameTraitsMIPS_O64 : FrameTraitsMIPS_O32
{
    using reg_t         = std::uint64_t;

    struct reg_storage_t : reg_storage<reg_t>
    {
        void CaptureFrame(const ExecContext::mips_o64_frame_t& frame)
        {
            SetGPRegister(reg_mips::r16, frame.s[0]);
            SetGPRegister(reg_mips::r17, frame.s[1]);
            SetGPRegister(reg_mips::r18, frame.s[2]);
            SetGPRegister(reg_mips::r19, frame.s[3]);
            SetGPRegister(reg_mips::r20, frame.s[4]);
            SetGPRegister(reg_mips::r21, frame.s[5]);
            SetGPRegister(reg_mips::r22, frame.s[6]);
            SetGPRegister(reg_mips::r23, frame.s[7]);
            SetGPRegister(reg_mips::r29, frame.sp);
            SetGPRegister(reg_mips::r30, frame.fp);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(ExecContext::mips_o64_frame_t& frame)
        {
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.fp = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::mips_o64_full_frame_t& frame)
        {
            frame.at = GetGPRegister(reg_mips::r1);
            frame.v[0] = GetGPRegister(reg_mips::r2);
            frame.v[1] = GetGPRegister(reg_mips::r3);
            frame.a[0] = GetGPRegister(reg_mips::r4);
            frame.a[1] = GetGPRegister(reg_mips::r5);
            frame.a[2] = GetGPRegister(reg_mips::r6);
            frame.a[3] = GetGPRegister(reg_mips::r7);
            frame.t[0] = GetGPRegister(reg_mips::r8);
            frame.t[1] = GetGPRegister(reg_mips::r9);
            frame.t[2] = GetGPRegister(reg_mips::r10);
            frame.t[3] = GetGPRegister(reg_mips::r11);
            frame.t[4] = GetGPRegister(reg_mips::r12);
            frame.t[5] = GetGPRegister(reg_mips::r13);
            frame.t[6] = GetGPRegister(reg_mips::r14);
            frame.t[7] = GetGPRegister(reg_mips::r15);
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.t[8] = GetGPRegister(reg_mips::r24);
            frame.t[9] = GetGPRegister(reg_mips::r25);
            frame.k[0] = GetGPRegister(reg_mips::r26);
            frame.k[1] = GetGPRegister(reg_mips::r27);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.ra = GetGPRegister(reg_mips::r31);
            frame.pc = GetReturnAddress();
        }
    };
};

struct FrameTraitsMIPS_N64 : FrameTraitsMIPS_Common
{
    using reg_t         = std::uint64_t;

    static constexpr auto kCalleeSaveRegisters =
    {
        reg_mips::r16,
        reg_mips::r17,
        reg_mips::r18,
        reg_mips::r19,
        reg_mips::r20,
        reg_mips::r21,
        reg_mips::r22,
        reg_mips::r23,

        reg_mips::r28,
        reg_mips::r29,
        reg_mips::r30,
    };

    struct reg_storage_t : reg_storage<reg_t>
    {
        void CaptureFrame(const ExecContext::mips_n64_frame_t& frame)
        {
            SetGPRegister(reg_mips::r16, frame.s[0]);
            SetGPRegister(reg_mips::r17, frame.s[1]);
            SetGPRegister(reg_mips::r18, frame.s[2]);
            SetGPRegister(reg_mips::r19, frame.s[3]);
            SetGPRegister(reg_mips::r20, frame.s[4]);
            SetGPRegister(reg_mips::r21, frame.s[5]);
            SetGPRegister(reg_mips::r22, frame.s[6]);
            SetGPRegister(reg_mips::r23, frame.s[7]);
            SetGPRegister(reg_mips::r28, frame.gp);
            SetGPRegister(reg_mips::r29, frame.sp);
            SetGPRegister(reg_mips::r30, frame.s[8]);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(ExecContext::mips_n64_frame_t& frame)
        {
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::mips_n64_full_frame_t& frame)
        {
            frame.at = GetGPRegister(reg_mips::r1);
            frame.v[0] = GetGPRegister(reg_mips::r2);
            frame.v[1] = GetGPRegister(reg_mips::r3);
            frame.a[0] = GetGPRegister(reg_mips::r4);
            frame.a[1] = GetGPRegister(reg_mips::r5);
            frame.a[2] = GetGPRegister(reg_mips::r6);
            frame.a[3] = GetGPRegister(reg_mips::r7);
            frame.a[4] = GetGPRegister(reg_mips::r8);
            frame.a[5] = GetGPRegister(reg_mips::r9);
            frame.a[6] = GetGPRegister(reg_mips::r10);
            frame.a[7] = GetGPRegister(reg_mips::r11);
            frame.t[0] = GetGPRegister(reg_mips::r12);
            frame.t[1] = GetGPRegister(reg_mips::r13);
            frame.t[2] = GetGPRegister(reg_mips::r14);
            frame.t[3] = GetGPRegister(reg_mips::r15);
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.t[4] = GetGPRegister(reg_mips::r24);
            frame.t[5] = GetGPRegister(reg_mips::r25);
            frame.k[0] = GetGPRegister(reg_mips::r26);
            frame.k[1] = GetGPRegister(reg_mips::r27);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }
    };

    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        switch (r)
        {
            case reg_mips::r8:
                return "$a4";

            case reg_mips::r9:
                return "$a5";

            case reg_mips::r10:
                return "$a6";

            case reg_mips::r11:
                return "$a7";

            default:
                return FrameTraitsMIPS_O32::GetRegisterName(r);
        }
    }
};

struct FrameTraitsMIPS_N32 : FrameTraitsMIPS_N64
{
       struct reg_storage_t : reg_storage<reg_t>
    {
        void CaptureFrame(const ExecContext::mips_n32_frame_t& frame)
        {
            SetGPRegister(reg_mips::r16, frame.s[0]);
            SetGPRegister(reg_mips::r17, frame.s[1]);
            SetGPRegister(reg_mips::r18, frame.s[2]);
            SetGPRegister(reg_mips::r19, frame.s[3]);
            SetGPRegister(reg_mips::r20, frame.s[4]);
            SetGPRegister(reg_mips::r21, frame.s[5]);
            SetGPRegister(reg_mips::r22, frame.s[6]);
            SetGPRegister(reg_mips::r23, frame.s[7]);
            SetGPRegister(reg_mips::r28, frame.gp);
            SetGPRegister(reg_mips::r29, frame.sp);
            SetGPRegister(reg_mips::r30, frame.s[8]);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(ExecContext::mips_n32_frame_t& frame)
        {
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::mips_n32_full_frame_t& frame)
        {
            frame.at = GetGPRegister(reg_mips::r1);
            frame.v[0] = GetGPRegister(reg_mips::r2);
            frame.v[1] = GetGPRegister(reg_mips::r3);
            frame.a[0] = GetGPRegister(reg_mips::r4);
            frame.a[1] = GetGPRegister(reg_mips::r5);
            frame.a[2] = GetGPRegister(reg_mips::r6);
            frame.a[3] = GetGPRegister(reg_mips::r7);
            frame.a[4] = GetGPRegister(reg_mips::r8);
            frame.a[5] = GetGPRegister(reg_mips::r9);
            frame.a[6] = GetGPRegister(reg_mips::r10);
            frame.a[7] = GetGPRegister(reg_mips::r11);
            frame.t[0] = GetGPRegister(reg_mips::r12);
            frame.t[1] = GetGPRegister(reg_mips::r13);
            frame.t[2] = GetGPRegister(reg_mips::r14);
            frame.t[3] = GetGPRegister(reg_mips::r15);
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.t[4] = GetGPRegister(reg_mips::r24);
            frame.t[5] = GetGPRegister(reg_mips::r25);
            frame.k[0] = GetGPRegister(reg_mips::r26);
            frame.k[1] = GetGPRegister(reg_mips::r27);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }
    };
};

struct FrameTraitsMIPS_EABI32 : FrameTraitsMIPS_Common
{
    using reg_t         = std::uint32_t;
    
    static constexpr auto kCalleeSaveRegisters =
    {
        reg_mips::r16,
        reg_mips::r17,
        reg_mips::r18,
        reg_mips::r19,
        reg_mips::r20,
        reg_mips::r21,
        reg_mips::r22,
        reg_mips::r23,

        reg_mips::r28,
        reg_mips::r29,
        reg_mips::r30,
    };

    struct reg_storage_t : reg_storage<reg_t>
    {
        void CaptureFrame(const ExecContext::mips_eabi32_frame_t& frame)
        {
            SetGPRegister(reg_mips::r16, frame.s[0]);
            SetGPRegister(reg_mips::r17, frame.s[1]);
            SetGPRegister(reg_mips::r18, frame.s[2]);
            SetGPRegister(reg_mips::r19, frame.s[3]);
            SetGPRegister(reg_mips::r20, frame.s[4]);
            SetGPRegister(reg_mips::r21, frame.s[5]);
            SetGPRegister(reg_mips::r22, frame.s[6]);
            SetGPRegister(reg_mips::r23, frame.s[7]);
            SetGPRegister(reg_mips::r28, frame.gp);
            SetGPRegister(reg_mips::r29, frame.sp);
            SetGPRegister(reg_mips::r30, frame.s[8]);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(ExecContext::mips_eabi32_frame_t& frame)
        {
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::mips_eabi32_full_frame_t& frame)
        {
            frame.at = GetGPRegister(reg_mips::r1);
            frame.v[0] = GetGPRegister(reg_mips::r2);
            frame.v[1] = GetGPRegister(reg_mips::r3);
            frame.a[0] = GetGPRegister(reg_mips::r4);
            frame.a[1] = GetGPRegister(reg_mips::r5);
            frame.a[2] = GetGPRegister(reg_mips::r6);
            frame.a[3] = GetGPRegister(reg_mips::r7);
            frame.a[4] = GetGPRegister(reg_mips::r8);
            frame.a[5] = GetGPRegister(reg_mips::r9);
            frame.a[6] = GetGPRegister(reg_mips::r10);
            frame.a[7] = GetGPRegister(reg_mips::r11);
            frame.t[0] = GetGPRegister(reg_mips::r12);
            frame.t[1] = GetGPRegister(reg_mips::r13);
            frame.t[2] = GetGPRegister(reg_mips::r14);
            frame.t[3] = GetGPRegister(reg_mips::r15);
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.t[4] = GetGPRegister(reg_mips::r24);
            frame.t[5] = GetGPRegister(reg_mips::r25);
            frame.k[0] = GetGPRegister(reg_mips::r26);
            frame.k[1] = GetGPRegister(reg_mips::r27);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.s[8] = GetGPRegister(reg_mips::r30);
            frame.ra = GetGPRegister(reg_mips::r31);
            frame.pc = GetReturnAddress();
        }
    };

    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        return FrameTraitsMIPS_N64::GetRegisterName(r);
    }
};

struct FrameTraitsMIPS_EABI64 : FrameTraitsMIPS_Common
{
    using reg_t         = std::uint64_t;

    static constexpr auto kCalleeSaveRegisters =
    {
        reg_mips::r16,
        reg_mips::r17,
        reg_mips::r18,
        reg_mips::r19,
        reg_mips::r20,
        reg_mips::r21,
        reg_mips::r22,
        reg_mips::r23,

        reg_mips::r28,
        reg_mips::r29,
        reg_mips::r30,
    };

    struct reg_storage_t : reg_storage<reg_t>
    {
        void CaptureFrame(const ExecContext::mips_eabi64_frame_t& frame)
        {
            SetGPRegister(reg_mips::r16, frame.s[0]);
            SetGPRegister(reg_mips::r17, frame.s[1]);
            SetGPRegister(reg_mips::r18, frame.s[2]);
            SetGPRegister(reg_mips::r19, frame.s[3]);
            SetGPRegister(reg_mips::r20, frame.s[4]);
            SetGPRegister(reg_mips::r21, frame.s[5]);
            SetGPRegister(reg_mips::r22, frame.s[6]);
            SetGPRegister(reg_mips::r23, frame.s[7]);
            SetGPRegister(reg_mips::r28, frame.gp);
            SetGPRegister(reg_mips::r29, frame.sp);
            SetGPRegister(reg_mips::r30, frame.fp);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(ExecContext::mips_eabi64_frame_t& frame)
        {
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.fp = GetGPRegister(reg_mips::r30);
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(ExecContext::mips_eabi64_full_frame_t& frame)
        {
            frame.at = GetGPRegister(reg_mips::r1);
            frame.v[0] = GetGPRegister(reg_mips::r2);
            frame.v[1] = GetGPRegister(reg_mips::r3);
            frame.a[0] = GetGPRegister(reg_mips::r4);
            frame.a[1] = GetGPRegister(reg_mips::r5);
            frame.a[2] = GetGPRegister(reg_mips::r6);
            frame.a[3] = GetGPRegister(reg_mips::r7);
            frame.a[4] = GetGPRegister(reg_mips::r8);
            frame.a[5] = GetGPRegister(reg_mips::r9);
            frame.a[6] = GetGPRegister(reg_mips::r10);
            frame.a[7] = GetGPRegister(reg_mips::r11);
            frame.t[0] = GetGPRegister(reg_mips::r12);
            frame.t[1] = GetGPRegister(reg_mips::r13);
            frame.t[2] = GetGPRegister(reg_mips::r14);
            frame.t[3] = GetGPRegister(reg_mips::r15);
            frame.s[0] = GetGPRegister(reg_mips::r16);
            frame.s[1] = GetGPRegister(reg_mips::r17);
            frame.s[2] = GetGPRegister(reg_mips::r18);
            frame.s[3] = GetGPRegister(reg_mips::r19);
            frame.s[4] = GetGPRegister(reg_mips::r20);
            frame.s[5] = GetGPRegister(reg_mips::r21);
            frame.s[6] = GetGPRegister(reg_mips::r22);
            frame.s[7] = GetGPRegister(reg_mips::r23);
            frame.t[4] = GetGPRegister(reg_mips::r24);
            frame.t[5] = GetGPRegister(reg_mips::r25);
            frame.k[0] = GetGPRegister(reg_mips::r26);
            frame.k[1] = GetGPRegister(reg_mips::r27);
            frame.gp = GetGPRegister(reg_mips::r28);
            frame.sp = GetGPRegister(reg_mips::r29);
            frame.fp = GetGPRegister(reg_mips::r30);
            frame.ra = GetGPRegister(reg_mips::r31);
            frame.pc = GetReturnAddress();
        }
    };

    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        return FrameTraitsMIPS_N64::GetRegisterName(r);
    }
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_ARM_H */
