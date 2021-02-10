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


using ExecContext::ppc32_sysv_frame_t;
using ExecContext::ppc64_elfv1_frame_t;
using ExecContext::ppc64_elfv2_frame_t;
using ExecContext::ppc32_sysv_full_frame_t;
using ExecContext::ppc64_elfv1_full_frame_t;
using ExecContext::ppc64_elfv2_full_frame_t;


// Forward declarations.
struct FrameTraitsPPC32SysV;
struct FrameTraitsPPC64ElfV1;
struct FrameTraitsPPC64ElfV2;


// Alias for PowerPC targets.
#if defined(_ARCH_PPC)
#  if defined(_CALL_SYSV)
using FrameTraitsNative = FrameTraitsPPC32SysV;
#  elif _CALL_ELF == 1
using FrameTraitsNative = FrameTraitsPPC64ElfV1;
#  elif _CALL_ELF == 2
using FrameTraitsNative = FrameTraitsPPC64ElfV2;
#  else
#    error unknown PowerPC ABI
#  endif
#endif


// Register mapping for the 32-bit SysV PowerPC ABI.
enum class reg_ppc32_sysv : std::int16_t
{
    // Not defined by the ABI; used internally.
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

    cr          = 64,   // Condition register.
    fpscr       = 65,   // Floating-point status and control register.
    msr         = 66,   // Machine state register.
    acc         = 99,   // SPE accumulator.

    // Special-purpose registers.
    xer         = 101,  // SPR1 - fixed-point exception register.
    lr          = 108,  // SPR8 - link register.
    ctr         = 109,  // SPR9 - count register.

    // Vector registers.
    vr0         = 1124,
    vr1         = 1125,
    vr2         = 1126,
    vr3         = 1127,
    vr4         = 1128,
    vr5         = 1129,
    vr6         = 1130,
    vr7         = 1131,
    vr8         = 1132,
    vr9         = 1133,
    vr10        = 1134,
    vr11        = 1135,
    vr12        = 1136,
    vr13        = 1137,
    vr14        = 1138,
    vr15        = 1139,
    vr16        = 1140,
    vr17        = 1141,
    vr18        = 1142,
    vr19        = 1143,
    vr20        = 1144,
    vr21        = 1145,
    vr22        = 1146,
    vr23        = 1147,
    vr24        = 1148,
    vr25        = 1149,
    vr26        = 1150,
    vr27        = 1151,
    vr28        = 1152,
    vr29        = 1153,
    vr30        = 1154,
    vr31        = 1155,

    // High part of the SPE GPRs.
    spegpr0     = 1200,
    spegpr1     = 1201,
    spegpr2     = 1202,
    spegpr3     = 1203,
    spegpr4     = 1204,
    spegpr5     = 1205,
    spegpr6     = 1206,
    spegpr7     = 1207,
    spegpr8     = 1208,
    spegpr9     = 1209,
    spegpr10    = 1210,
    spegpr11    = 1211,
    spegpr12    = 1212,
    spegpr13    = 1213,
    spegpr14    = 1214,
    spegpr15    = 1215,
    spegpr16    = 1216,
    spegpr17    = 1217,
    spegpr18    = 1218,
    spegpr19    = 1219,
    spegpr20    = 1220,
    spegpr21    = 1221,
    spegpr22    = 1222,
    spegpr23    = 1223,
    spegpr24    = 1224,
    spegpr25    = 1225,
    spegpr26    = 1226,
    spegpr27    = 1227,
    spegpr28    = 1228,
    spegpr29    = 1229,
    spegpr30    = 1230,
    spegpr31    = 1231,
};


// Register mapping for the ELFv1 PowerPC ABI.
enum class reg_ppc64_elfv1 : std::int16_t
{
    // Not defined by the ABI; used internally.
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

    cr          = 64,   // Condition register.
    fpscr       = 65,   // Floating-point status and control register.
    msr         = 66,   // Machine state register.

    // Segment registers.
    sr0         = 70,
    sr1         = 71,
    sr2         = 72,
    sr3         = 73,
    sr4         = 74,
    sr5         = 75,
    sr6         = 76,
    sr7         = 77,
    sr8         = 78,
    sr9         = 79,
    sr10        = 80,
    sr11        = 81,
    sr12        = 82,
    sr13        = 83,
    sr14        = 84,
    sr15        = 85,   
    
    // Special-purpose registers.
    xer         = 101,  // SPR1 - fixed-point exception register.
    lr          = 108,  // SPR8 - link register.
    ctr         = 109,  // SPR9 - count register.
    dsisr       = 118,  // SPR18 - data storage interrupt status register.
    dar         = 119,  // SPR19 - data address register.
    dec         = 122,  // SPR22 - decrementer.
    sdr1        = 125,  // SPR25 - storage description register 1.
    srr0        = 126,  // SPR26 - machine status save/restore register 0.
    srr1        = 127,  // SPR27 - machine status save/restore register 1.
    vrsave      = 356,  // SPR256 - vector save/restore register.
    sprg0       = 372,  // SPR272 - special-purpose register 0.
    sprg1       = 373,  // SPR273 - special-purpose register 1.
    sprg2       = 374,  // SPR274 - special-purpose register 2.
    sprg3       = 375,  // SPR275 - special-purpose register 3.
    asr         = 380,  // SPR280 - address space register.
    ear         = 382,  // SPR282 - external access register.
    tb          = 384,  // SPR284 - time base.
    tbu         = 385,  // SPR285 - time base upper.
    pvr         = 387,  // SPR287 - processor version register.
    ibat0u      = 628,  // SPR528 - instruction BAT register 0 upper.
    ibat0l      = 629,  // SPR529 - instruction BAT register 0 lower.
    ibat1u      = 630,  // SPR530 - instruction BAT register 1 upper.
    ibat1l      = 631,  // SPR531 - instruction BAT register 1 lower.
    ibat2u      = 632,  // SPR532 - instruction BAT register 2 upper.
    ibat2l      = 633,  // SPR533 - instruction BAT register 2 lower.
    ibat3u      = 634,  // SPR534 - instruction BAT register 3 upper.
    ibat3l      = 635,  // SPR535 - instruction BAT register 3 lower.
    dbat0u      = 636,  // SPR536 - data BAT register 0 upper.
    dbat0l      = 637,  // SPR537 - data BAT register 0 lower.
    dbat1u      = 638,  // SPR538 - data BAT register 1 upper.
    dbat1l      = 639,  // SPR539 - data BAT register 1 lower.
    dbat2u      = 640,  // SPR540 - data BAT register 2 upper.
    dbat2l      = 641,  // SPR541 - data BAT register 2 lower.
    dbat3u      = 642,  // SPR542 - data BAT register 3 upper.
    dbat3l      = 643,  // SPR543 - data BAT register 3 lower.

    // Vector registers.
    vr0         = 1124,
    vr1         = 1125,
    vr2         = 1126,
    vr3         = 1127,
    vr4         = 1128,
    vr5         = 1129,
    vr6         = 1130,
    vr7         = 1131,
    vr8         = 1132,
    vr9         = 1133,
    vr10        = 1134,
    vr11        = 1135,
    vr12        = 1136,
    vr13        = 1137,
    vr14        = 1138,
    vr15        = 1139,
    vr16        = 1140,
    vr17        = 1141,
    vr18        = 1142,
    vr19        = 1143,
    vr20        = 1144,
    vr21        = 1145,
    vr22        = 1146,
    vr23        = 1147,
    vr24        = 1148,
    vr25        = 1149,
    vr26        = 1150,
    vr27        = 1151,
    vr28        = 1152,
    vr29        = 1153,
    vr30        = 1154,
    vr31        = 1155,
};


// Register mapping for the ELFv2 PowerPC ABI.
enum class reg_ppc64_elfv2 : std::int16_t
{
    // Not defined by the ABI; used internally.
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

    // Special-purpose registers.
    lr          = 65,
    ctr         = 66,

    // Condition register subfields.
    cr0         = 68,
    cr1         = 69,
    cr2         = 70,
    cr3         = 71,
    cr4         = 72,
    cr5         = 73,
    cr6         = 74,
    cr7         = 75,

    // Fixed-point exception register.
    xer         = 76,

    // Vector registers.
    vr0         = 77,
    vr1         = 78,
    vr2         = 79,
    vr3         = 80,
    vr4         = 81,
    vr5         = 82,
    vr6         = 83,
    vr7         = 84,
    vr8         = 85,
    vr9         = 86,
    vr10        = 87,
    vr11        = 88,
    vr12        = 89,
    vr13        = 90,
    vr14        = 91,
    vr15        = 92,
    vr16        = 93,
    vr17        = 94,
    vr18        = 95,
    vr19        = 96,
    vr20        = 97,
    vr21        = 98,
    vr22        = 99,
    vr23        = 100,
    vr24        = 101,
    vr25        = 102,
    vr26        = 103,
    vr27        = 104,
    vr28        = 105,
    vr29        = 106,
    vr30        = 107,
    vr31        = 108,

    vscr        = 110,
    tfhar       = 114,
    tfiar       = 115,
    texasr      = 116,
};

constexpr auto operator<=>(const reg_ppc64_elfv2& a, const reg_ppc64_elfv2& b)
{
    using T = std::underlying_type_t<reg_ppc64_elfv2>;
    return static_cast<T>(a) <=> static_cast<T>(b);
}


// Shared traits for all PowerPC frame types.
template <std::size_t Bits, typename RegEnum, typename Frame, typename FullFrame>
struct FrameTraitsPPCCommon
{
    //! @todo: implement floating-point register save/restore.
    using reg_enum_t        = RegEnum;
    using reg_t             = std::conditional_t<Bits == 32, std::uint32_t, std::uint64_t>;
    using frame_t           = Frame;
    using full_frame_t      = FullFrame;

    // Registers with special purposes that a DWARF user needs to know about.
    static constexpr auto kStackPointerReg          = reg_enum_t::r1;
    static constexpr auto kInstructionPointerReg    = reg_enum_t::pc;

    // Downward-growing stack with no offset.
    static constexpr std::ptrdiff_t kStackDirection     = -1;
    static constexpr std::ptrdiff_t kStackPointerOffset = 0;

    // The core integer registers and CR0-8 plus the CFA and PC.
    static constexpr std::size_t kUnwindRegisterCount = 33;
    static constexpr std::size_t kUnwindStorageCount  = kUnwindRegisterCount + 2;

    // All frames save the same set of integer registers.
    static constexpr auto kCalleeSaveRegisters =
    { 
        reg_enum_t::r1,
        reg_enum_t::r2,
        reg_enum_t::r13,
        reg_enum_t::r14,
        reg_enum_t::r15,
        reg_enum_t::r16,
        reg_enum_t::r17,
        reg_enum_t::r18,
        reg_enum_t::r19,
        reg_enum_t::r20,
        reg_enum_t::r21,
        reg_enum_t::r22,
        reg_enum_t::r23,
        reg_enum_t::r24,
        reg_enum_t::r25,
        reg_enum_t::r26,
        reg_enum_t::r27,
        reg_enum_t::r28,
        reg_enum_t::r29,
        reg_enum_t::r30,
        reg_enum_t::r31,
    };


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
            return gp[1];
        }
        void SetReturnAddress(reg_t ra)
        {
            gp[1] = ra;
        }

        // Condition register get/set methods.
        reg_t GetConditionRegister() const
        {
            return gp[2];
        }
        void SetConditionRegister(reg_t cr)
        {
            gp[2] = cr;
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

        void CaptureFrame(const frame_t& frame)
        {
            SetGPRegister(reg_enum_t::r1, frame.r1);
            SetGPRegister(reg_enum_t::r2, frame.r2);
            SetGPRegister(reg_enum_t::r13, frame.r13);
            SetGPRegister(reg_enum_t::r14, frame.r14);
            SetGPRegister(reg_enum_t::r15, frame.r15);
            SetGPRegister(reg_enum_t::r16, frame.r16);
            SetGPRegister(reg_enum_t::r17, frame.r17);
            SetGPRegister(reg_enum_t::r18, frame.r18);
            SetGPRegister(reg_enum_t::r19, frame.r19);
            SetGPRegister(reg_enum_t::r20, frame.r20);
            SetGPRegister(reg_enum_t::r21, frame.r21);
            SetGPRegister(reg_enum_t::r22, frame.r22);
            SetGPRegister(reg_enum_t::r23, frame.r23);
            SetGPRegister(reg_enum_t::r24, frame.r24);
            SetGPRegister(reg_enum_t::r25, frame.r25);
            SetGPRegister(reg_enum_t::r26, frame.r26);
            SetGPRegister(reg_enum_t::r27, frame.r27);
            SetGPRegister(reg_enum_t::r28, frame.r28);
            SetGPRegister(reg_enum_t::r29, frame.r29);
            SetGPRegister(reg_enum_t::r30, frame.r30);
            SetGPRegister(reg_enum_t::r31, frame.r31);

            SetCFA(frame.r1);
            SetConditionRegister(frame.cr);
            SetReturnAddress(frame.pc);
        }

        void CaptureFullFrame(const full_frame_t& frame)
        {
            SetGPRegister(reg_enum_t::r0, frame.r0);
            SetGPRegister(reg_enum_t::r1, frame.r1);
            SetGPRegister(reg_enum_t::r2, frame.r2);
            SetGPRegister(reg_enum_t::r3, frame.r3);
            SetGPRegister(reg_enum_t::r4, frame.r4);
            SetGPRegister(reg_enum_t::r5, frame.r5);
            SetGPRegister(reg_enum_t::r6, frame.r6);
            SetGPRegister(reg_enum_t::r7, frame.r7);
            SetGPRegister(reg_enum_t::r8, frame.r8);
            SetGPRegister(reg_enum_t::r9, frame.r9);
            SetGPRegister(reg_enum_t::r10, frame.r10);
            SetGPRegister(reg_enum_t::r11, frame.r11);
            SetGPRegister(reg_enum_t::r12, frame.r12);
            SetGPRegister(reg_enum_t::r13, frame.r13);
            SetGPRegister(reg_enum_t::r14, frame.r14);
            SetGPRegister(reg_enum_t::r15, frame.r15);
            SetGPRegister(reg_enum_t::r16, frame.r16);
            SetGPRegister(reg_enum_t::r17, frame.r17);
            SetGPRegister(reg_enum_t::r18, frame.r18);
            SetGPRegister(reg_enum_t::r19, frame.r19);
            SetGPRegister(reg_enum_t::r20, frame.r20);
            SetGPRegister(reg_enum_t::r21, frame.r21);
            SetGPRegister(reg_enum_t::r22, frame.r22);
            SetGPRegister(reg_enum_t::r23, frame.r23);
            SetGPRegister(reg_enum_t::r24, frame.r24);
            SetGPRegister(reg_enum_t::r25, frame.r25);
            SetGPRegister(reg_enum_t::r26, frame.r26);
            SetGPRegister(reg_enum_t::r27, frame.r27);
            SetGPRegister(reg_enum_t::r28, frame.r28);
            SetGPRegister(reg_enum_t::r29, frame.r29);
            SetGPRegister(reg_enum_t::r30, frame.r30);
            SetGPRegister(reg_enum_t::r31, frame.r31);

            SetCFA(frame.r1);
            SetConditionRegister(frame.cr);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(frame_t& frame)
        {
            frame.r1 = GetGPRegister(reg_enum_t::r1);
            frame.r2 = GetGPRegister(reg_enum_t::r2);
            frame.r13 = GetGPRegister(reg_enum_t::r13);
            frame.r14 = GetGPRegister(reg_enum_t::r14);
            frame.r15 = GetGPRegister(reg_enum_t::r15);
            frame.r16 = GetGPRegister(reg_enum_t::r16);
            frame.r17 = GetGPRegister(reg_enum_t::r17);
            frame.r18 = GetGPRegister(reg_enum_t::r18);
            frame.r19 = GetGPRegister(reg_enum_t::r19);
            frame.r20 = GetGPRegister(reg_enum_t::r20);
            frame.r21 = GetGPRegister(reg_enum_t::r21);
            frame.r22 = GetGPRegister(reg_enum_t::r22);
            frame.r23 = GetGPRegister(reg_enum_t::r23);
            frame.r24 = GetGPRegister(reg_enum_t::r24);
            frame.r25 = GetGPRegister(reg_enum_t::r25);
            frame.r26 = GetGPRegister(reg_enum_t::r26);
            frame.r27 = GetGPRegister(reg_enum_t::r27);
            frame.r28 = GetGPRegister(reg_enum_t::r28);
            frame.r29 = GetGPRegister(reg_enum_t::r29);
            frame.r30 = GetGPRegister(reg_enum_t::r30);
            frame.r31 = GetGPRegister(reg_enum_t::r31);

            frame.cr = GetConditionRegister();
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(full_frame_t& frame)
        {
            frame.r0 = GetGPRegister(reg_enum_t::r0);
            frame.r1 = GetGPRegister(reg_enum_t::r1);
            frame.r2 = GetGPRegister(reg_enum_t::r2);
            frame.r3 = GetGPRegister(reg_enum_t::r3);
            frame.r4 = GetGPRegister(reg_enum_t::r4);
            frame.r5 = GetGPRegister(reg_enum_t::r5);
            frame.r6 = GetGPRegister(reg_enum_t::r6);
            frame.r7 = GetGPRegister(reg_enum_t::r7);
            frame.r8 = GetGPRegister(reg_enum_t::r8);
            frame.r9 = GetGPRegister(reg_enum_t::r9);
            frame.r10 = GetGPRegister(reg_enum_t::r10);
            frame.r11 = GetGPRegister(reg_enum_t::r11);
            frame.r12 = GetGPRegister(reg_enum_t::r12);
            frame.r13 = GetGPRegister(reg_enum_t::r13);
            frame.r14 = GetGPRegister(reg_enum_t::r14);
            frame.r15 = GetGPRegister(reg_enum_t::r15);
            frame.r16 = GetGPRegister(reg_enum_t::r16);
            frame.r17 = GetGPRegister(reg_enum_t::r17);
            frame.r18 = GetGPRegister(reg_enum_t::r18);
            frame.r19 = GetGPRegister(reg_enum_t::r19);
            frame.r20 = GetGPRegister(reg_enum_t::r20);
            frame.r21 = GetGPRegister(reg_enum_t::r21);
            frame.r22 = GetGPRegister(reg_enum_t::r22);
            frame.r23 = GetGPRegister(reg_enum_t::r23);
            frame.r24 = GetGPRegister(reg_enum_t::r24);
            frame.r25 = GetGPRegister(reg_enum_t::r25);
            frame.r26 = GetGPRegister(reg_enum_t::r26);
            frame.r27 = GetGPRegister(reg_enum_t::r27);
            frame.r28 = GetGPRegister(reg_enum_t::r28);
            frame.r29 = GetGPRegister(reg_enum_t::r29);
            frame.r30 = GetGPRegister(reg_enum_t::r30);
            frame.r31 = GetGPRegister(reg_enum_t::r31);

            frame.cr = GetConditionRegister();
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


    static reg_enum_t IndexToRegister(std::size_t i)
    {
        return reg_enum_t(std::size_t(reg_enum_t::r0) + i - 3);
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        return std::ptrdiff_t(reg) - std::ptrdiff_t(reg_enum_t::r0) + 3;
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
            default:
                return "<unknown>";
        }
    }
};


struct FrameTraitsPPC32SysV :
    public FrameTraitsPPCCommon<32, reg_ppc32_sysv, ppc32_sysv_frame_t, ppc32_sysv_full_frame_t>
{
};


struct FrameTraitsPPC64ElfV1 :
    public FrameTraitsPPCCommon<64, reg_ppc64_elfv1, ppc64_elfv1_frame_t, ppc64_elfv1_full_frame_t>
{
};


struct FrameTraitsPPC64ElfV2 :
    public FrameTraitsPPCCommon<64, reg_ppc64_elfv2, ppc64_elfv2_frame_t, ppc64_elfv2_full_frame_t>
{
    using Base = FrameTraitsPPCCommon<64, reg_ppc64_elfv2, ppc64_elfv2_frame_t, ppc64_elfv2_full_frame_t>;

    struct reg_storage_t :
        public Base::reg_storage_t
    {
        // General-purpose register get/set methods.
        reg_t GetGPRegister(reg_ppc64_elfv2 which) const
        {
            if (IsConditionRegister(which))
                return GetConditionRegister(which);
            
            // Assumes: IsValidGPRegister(which).
            return gp[RegisterToIndex(which)];
        }
        void SetGPRegister(reg_ppc64_elfv2 which, reg_t value)
        {
            if (IsConditionRegister(which))
                return SetConditionRegister(which, value);
            
            // Assumes: IsValidGPRegister(which).
            gp[RegisterToIndex(which)] = value;
        }

        // Condition register get/set methods.
        using Base::reg_storage_t::GetConditionRegister;
        using Base::reg_storage_t::SetConditionRegister;
        reg_t GetConditionRegister(reg_ppc64_elfv2 which) const
        {
            auto offset = std::size_t(which) - std::size_t(reg_ppc64_elfv2::cr0);
            auto cr = GetConditionRegister();
            auto shift = offset * 4;
            return (cr >> shift) & 0x0F;
        }
        void SetConditionRegister(reg_ppc64_elfv2 which, reg_t value)
        {
            auto offset = std::size_t(which) - std::size_t(reg_ppc64_elfv2::cr0);
            auto shift = offset * 4;
            auto mask = (reg_t(0x0F) << shift);

            auto cr = GetConditionRegister();
            cr = (cr & ~mask) | ((value << shift) & mask);
            SetConditionRegister(cr);
        }


        static constexpr bool IsConditionRegister(reg_ppc64_elfv2 reg)
        {
            return reg_ppc64_elfv2::cr0 <= reg && reg <= reg_ppc64_elfv2::cr7;
        }
    };
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_ARM_H */
