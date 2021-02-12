#ifndef __SYSTEM_ABI_DWARF_REGISTERS_SPARC_H
#define __SYSTEM_ABI_DWARF_REGISTERS_SPARC_H


#include <System/ABI/ExecContext/Arch/Sparc/Frame.hh>

#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::Dwarf
{


// Forward declarations.
enum class reg_sparc : std::int16_t;


// The DWARF register numbers for Sparc.
enum class reg_sparc : std::int16_t
{
    // Not defined by the ABI; used internally.
    pc          = -1,

    // Integer registers.
    g0      = 0,
    g1      = 1,
    g2      = 2,
    g3      = 3,
    g4      = 4,
    g5      = 5,
    g6      = 6,
    g7      = 7,
    o0      = 8,
    o1      = 9,
    o2      = 10,
    o3      = 11,
    o4      = 12,
    o5      = 13,
    o6      = 14,
    o7      = 15,
    l0      = 16,
    l1      = 17,
    l2      = 18,
    l3      = 19,
    l4      = 20,
    l5      = 21,
    l6      = 22,
    l7      = 23,
    i0      = 24,
    i1      = 25,
    i2      = 26,
    i3      = 27,
    i4      = 28,
    i5      = 29,
    i6      = 30,
    i7      = 31,

    // Floating-point registers.
    f0      = 32,
    f1      = 33,
    f2      = 34,
    f3      = 35,
    f4      = 36,
    f5      = 37,
    f6      = 38,
    f7      = 39,
    f8      = 40,
    f9      = 41,
    f10     = 42,
    f11     = 43,
    f12     = 44,
    f13     = 45,
    f14     = 46,
    f15     = 47,
    f16     = 48,
    f17     = 49,
    f18     = 50,
    f19     = 51,
    f20     = 52,
    f21     = 53,
    f22     = 54,
    f23     = 55,
    f24     = 56,
    f25     = 57,
    f26     = 58,
    f27     = 59,
    f28     = 60,
    f29     = 61,
    f30     = 62,
    f31     = 63,     

    // Special registers.
    y       = 64,
};


// Traits class for Sparc.
template <std::size_t Bits>
struct FrameTraitsSparc
{
    using reg_enum_t    = reg_sparc;

    // Register save slots are 32-bit.
    using reg_t         = std::conditional_t<Bits == 32, std::uint32_t, std::conditional_t<Bits == 64, std::uint64_t, void>>;

    //! @todo: implement floating-point register save/restore.
    using float_reg_t   = void;

    // Which registers are used for special purposes.
    static constexpr auto kStackPointerReg          = reg_sparc::o6;
    static constexpr auto kFramePointerReg          = reg_sparc::i6;
    static constexpr auto kInstructionPointerReg    = reg_sparc::pc;

    // Stack grows downwards.
    static constexpr std::ptrdiff_t kStackDirection = -1;

    // Callee-saved registers.
    static constexpr auto kCalleeSaveRegisters =
    { 
        reg_sparc::i0,
        reg_sparc::i1,
        reg_sparc::i2,
        reg_sparc::i3,
        reg_sparc::i4,
        reg_sparc::i5,
        reg_sparc::i6,
        reg_sparc::i7,
        reg_sparc::l0,
        reg_sparc::l1,
        reg_sparc::l2,
        reg_sparc::l3,
        reg_sparc::l4,
        reg_sparc::l5,
        reg_sparc::l6,
        reg_sparc::l7,
    };

    // Offset to apply when converting the CFA into the stack pointer.
    static constexpr std::ptrdiff_t kStackPointerOffset = (Bits == 64) ? 2047 : 0;

    // All core integer registers plus the CFA and return address.
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

        using frame_t = std::conditional_t<Bits == 64, ExecContext::sparc64_frame_t, ExecContext::sparc32_frame_t>;
        using full_frame_t = std::conditional_t<Bits == 64, ExecContext::sparc64_full_frame_t, ExecContext::sparc32_full_frame_t>;

        void CaptureFrame(const frame_t& frame)
        {
            SetGPRegister(reg_sparc::i0, frame.i[0]);
            SetGPRegister(reg_sparc::i1, frame.i[1]);
            SetGPRegister(reg_sparc::i2, frame.i[2]);
            SetGPRegister(reg_sparc::i3, frame.i[3]);
            SetGPRegister(reg_sparc::i4, frame.i[4]);
            SetGPRegister(reg_sparc::i5, frame.i[5]);
            SetGPRegister(reg_sparc::i6, frame.i[6]);
            SetGPRegister(reg_sparc::i7, frame.i[7]);
            SetGPRegister(reg_sparc::l0, frame.l[0]);
            SetGPRegister(reg_sparc::l1, frame.l[1]);
            SetGPRegister(reg_sparc::l2, frame.l[2]);
            SetGPRegister(reg_sparc::l3, frame.l[3]);
            SetGPRegister(reg_sparc::l4, frame.l[4]);
            SetGPRegister(reg_sparc::l5, frame.l[5]);
            SetGPRegister(reg_sparc::l6, frame.l[6]);
            SetGPRegister(reg_sparc::l7, frame.l[7]);

            SetCFA(frame.sp + kStackPointerOffset);
            SetReturnAddress(frame.pc);
        }

        void ConfigureFrame(frame_t& frame)
        {
            frame.i[0] = GetGPRegister(reg_sparc::i0);
            frame.i[1] = GetGPRegister(reg_sparc::i1);
            frame.i[2] = GetGPRegister(reg_sparc::i2);
            frame.i[3] = GetGPRegister(reg_sparc::i3);
            frame.i[4] = GetGPRegister(reg_sparc::i4);
            frame.i[5] = GetGPRegister(reg_sparc::i5);
            frame.i[6] = GetGPRegister(reg_sparc::i6);
            frame.i[7] = GetGPRegister(reg_sparc::i7);
            frame.l[0] = GetGPRegister(reg_sparc::l0);
            frame.l[1] = GetGPRegister(reg_sparc::l1);
            frame.l[2] = GetGPRegister(reg_sparc::l2);
            frame.l[3] = GetGPRegister(reg_sparc::l3);
            frame.l[4] = GetGPRegister(reg_sparc::l4);
            frame.l[5] = GetGPRegister(reg_sparc::l5);
            frame.l[6] = GetGPRegister(reg_sparc::l6);
            frame.l[7] = GetGPRegister(reg_sparc::l7);

            frame.sp = GetCFA() - kStackPointerOffset;
            frame.pc = GetReturnAddress();
        }

        void ConfigureFullFrame(full_frame_t& frame)
        {
            frame.g[1] = GetGPRegister(reg_sparc::g1);
            frame.g[2] = GetGPRegister(reg_sparc::g2);
            frame.g[3] = GetGPRegister(reg_sparc::g3);
            frame.g[4] = GetGPRegister(reg_sparc::g4);
            frame.g[5] = GetGPRegister(reg_sparc::g5);
            frame.g[6] = GetGPRegister(reg_sparc::g6);
            frame.g[7] = GetGPRegister(reg_sparc::g7);
            frame.i[0] = GetGPRegister(reg_sparc::i0);
            frame.i[1] = GetGPRegister(reg_sparc::i1);
            frame.i[2] = GetGPRegister(reg_sparc::i2);
            frame.i[3] = GetGPRegister(reg_sparc::i3);
            frame.i[4] = GetGPRegister(reg_sparc::i4);
            frame.i[5] = GetGPRegister(reg_sparc::i5);
            //frame.i[6] = GetGPRegister(reg_sparc::i6);
            frame.i[7] = GetGPRegister(reg_sparc::i7);
            frame.l[0] = GetGPRegister(reg_sparc::l0);
            frame.l[1] = GetGPRegister(reg_sparc::l1);
            frame.l[2] = GetGPRegister(reg_sparc::l2);
            frame.l[3] = GetGPRegister(reg_sparc::l3);
            frame.l[4] = GetGPRegister(reg_sparc::l4);
            frame.l[5] = GetGPRegister(reg_sparc::l5);
            frame.l[6] = GetGPRegister(reg_sparc::l6);
            frame.l[7] = GetGPRegister(reg_sparc::l7);
            frame.o[0] = GetGPRegister(reg_sparc::o0);
            frame.o[1] = GetGPRegister(reg_sparc::o1);
            frame.o[2] = GetGPRegister(reg_sparc::o2);
            frame.o[3] = GetGPRegister(reg_sparc::o3);
            frame.o[4] = GetGPRegister(reg_sparc::o4);
            frame.o[5] = GetGPRegister(reg_sparc::o5);
            frame.o[6] = GetGPRegister(reg_sparc::o6);
            frame.o[7] = GetGPRegister(reg_sparc::o7);

            frame.i[6] = GetCFA() - kStackPointerOffset;
            frame.pc = GetReturnAddress();
        }

        // Validates the given general-purpose register number.
        static constexpr bool IsValidGPRegister(std::size_t n)
        {
            // Only registers in the range %rax-%r15 are valid.
            return (n >= std::size_t(reg_sparc::g0) && n <= std::size_t(reg_sparc::o7));
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
        return reg_enum_t(std::ptrdiff_t(reg_sparc::g0) + i);
    }

    static std::ptrdiff_t RegisterToIndex(reg_enum_t reg)
    {
        return std::ptrdiff_t(reg) - std::ptrdiff_t(reg_sparc::g0);
    }

    // Returns the name of the given register.
    static constexpr const char* GetRegisterName(reg_enum_t r)
    {
        switch (r)
        {
            case reg_enum_t::g0:
                return "%g0";
            case reg_enum_t::g1:
                return "%g1";
            case reg_enum_t::g2:
                return "%g2";
            case reg_enum_t::g3:
                return "%g3";
            case reg_enum_t::g4:
                return "%g4";
            case reg_enum_t::g5:
                return "%g5";
            case reg_enum_t::g6:
                return "%g6";
            case reg_enum_t::g7:
                return "%g7";
            case reg_enum_t::i0:
                return "%i0";
            case reg_enum_t::i1:
                return "%i1";
            case reg_enum_t::i2:
                return "%i2";
            case reg_enum_t::i3:
                return "%i3";
            case reg_enum_t::i4:
                return "%i4";
            case reg_enum_t::i5:
                return "%i5";
            case reg_enum_t::i6:
                return "%i6";
            case reg_enum_t::i7:
                return "%i7";
            case reg_enum_t::l0:
                return "%l0";
            case reg_enum_t::l1:
                return "%l1";
            case reg_enum_t::l2:
                return "%l2";
            case reg_enum_t::l3:
                return "%l3";
            case reg_enum_t::l4:
                return "%l4";
            case reg_enum_t::l5:
                return "%l5";
            case reg_enum_t::l6:
                return "%l6";
            case reg_enum_t::l7:
                return "%l7";
            case reg_enum_t::o0:
                return "%o0";
            case reg_enum_t::o1:
                return "%o1";
            case reg_enum_t::o2:
                return "%o2";
            case reg_enum_t::o3:
                return "%o3";
            case reg_enum_t::o4:
                return "%o4";
            case reg_enum_t::o5:
                return "%o5";
            case reg_enum_t::o6:
                return "%o6";
            case reg_enum_t::o7:
                return "%o7";
            default:
                return "<unknown>";
        }
    }
};


using FrameTraitsSparc32 = FrameTraitsSparc<32>;
using FrameTraitsSparc64 = FrameTraitsSparc<64>;

// Alias for SPARC targets.
#if defined(__sparc__)
#  if defined(__arch64__)
using FrameTraitsNative = FrameTraitsSparc64;
#  else
using FrameTraitsNative = FrameTraitsSparc32;
#  endif
#endif



} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_ARM_H */
