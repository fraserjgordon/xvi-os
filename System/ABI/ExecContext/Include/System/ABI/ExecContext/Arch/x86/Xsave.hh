#pragma once
#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_XSAVE_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_XSAVE_H

#if __XVI_NO_STDLIB
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#else
#  include <cstddef>
#  include <cstdint>
#endif

#include <System/ABI/ExecContext/Private/Config.hh>
#include <System/ABI/ExecContext/Arch/x86/FloatingPoint.hh>

#include <System/HW/CPU/CPUID/Arch/x86/CPUID.hh>


namespace System::ABI::ExecContext
{


using std::uint32_t;
using std::uint64_t;


enum class xsave_flag_t : std::uint64_t
{
    None        = 0,
    
    X87         = 0x0000'0000'0000'0001,    //!< Legacy x87 floating-point state.
    SSE         = 0x0000'0000'0000'0002,    //!< SSE registers %xmm0-%xmm7/%xmm15.
    AVX         = 0x0000'0000'0000'0004,    //!< AVX registers %ymm0-%ymm7/%ymm15.
    MpxBndRegs  = 0x0000'0000'0000'0008,    //!< MPX bounds registers.
    MpxBndCsr   = 0x0000'0000'0000'0010,    //!< MPX control and status registers,
    AvxOpmask   = 0x0000'0000'0000'0020,    //!< AVX-512 opmask registers (%k0-%k7).
    AvxZmmUpper = 0x0000'0000'0000'0040,    //!< Upper halves of AVX-512 %zmm0-%zmm15.
    AvxZmmExtra = 0x0000'0000'0000'0080,    //!< AVX-512 registers %zmm16-%zmm31.
    PTrace      = 0x0000'0000'0000'0100,    //!< Processor Trace state.
    PKRU        = 0x0000'0000'0000'0200,    //!< Protection Key state.

    X           = 0x8000'0000'0000'0000,    //!< Special functionality (depends on where bitmap is used).
};
using XsaveFlag = xsave_flag_t;

constexpr xsave_flag_t operator~(xsave_flag_t a)
{
    return xsave_flag_t(~std::uint64_t(a));
}

constexpr xsave_flag_t operator|(xsave_flag_t a, xsave_flag_t b)
{
    return xsave_flag_t(std::uint64_t(a) | std::uint64_t(b));
}

constexpr xsave_flag_t operator&(xsave_flag_t a, xsave_flag_t b)
{
    return xsave_flag_t(std::uint64_t(a) & std::uint64_t(b));
}

constexpr xsave_flag_t& operator|=(xsave_flag_t& a, xsave_flag_t b)
{
    a = a | b;
    return a;
}

constexpr xsave_flag_t& operator&=(xsave_flag_t& a, xsave_flag_t b)
{
    a = a & b;
    return a;
}


enum class xsave_feature_t
{
    X87 = 0,
    SSE,
    AVX,
    MpxBndRegs,
    MpxBndCsr,
    AvxOpmask,
    AvxZmmUpper,
    AvxZmmExtra,
    PTrace,
    PKRU,

    FeaturesCount = PKRU + 1,
};
using XsaveFeature = xsave_feature_t;

static constexpr std::size_t XsaveFeatureCount = static_cast<std::size_t>(XsaveFeature::FeaturesCount);


// Bits describing extended xsave capabilities (rather than state types).
struct xsave_caps_t
{
    union
    {
        struct
        {
            std::uint32_t   xsaveopt:1;     // Support for the xsaveopt instruction.
            std::uint32_t   compaction:1;   // Support for xsave area compaction.
            std::uint32_t   xinuse:1;       // Calling xgetbv with ecx=1 will report init optimisation state.
            std::uint32_t   supervisor:1;   // Supervisor state extensions are available.
            std::uint32_t   :0;
        };

        std::uint32_t as_u32;
    };
};

static_assert(sizeof(xsave_caps_t) == 4);


struct xsave_header_t
{
    xsave_flag_t saved_components;
    xsave_flag_t compacted_componenets;

    // Padding bytes that don't currently have a defined use.
    std::uint8_t    _padding[48];
};

static_assert(sizeof(xsave_header_t) == 64);

struct xsave_avx_t
{
    // Storage type for the upper halves of the %ymm AVX registers.
    struct ymm_hi_t { std::uint8_t bytes alignas(16) [16]; };

    // The upper parts of %ymm0-%ymm15 (i.e those parts not stored in %xmm0-%xmm15).
    ymm_hi_t ymm_hi[16];
};

static_assert(sizeof(xsave_avx_t) == 256);

struct xsave_avx512_opmask_t
{
    // Storage for the eight AVX512 opmask registers (%k0-%k7).
    std::uint64_t k[8];
};

static_assert(sizeof(xsave_avx512_opmask_t) == 64);

struct xsave_avx512_zmm_hi256_t
{
    // Storage type for the upper halves of the %zmm AVX registers.
    struct zmm_hi_t { std::uint8_t bytes alignas(32) [32]; };

    // The upper parts of %zmm0-%zmm15 (i.e those parts not stored in %ymm0-%ymm15).
    zmm_hi_t zmm_hi[16];
};

static_assert(sizeof(xsave_avx512_zmm_hi256_t) == 512);

struct xsave_avx512_zmm_hi16_t
{
    // Storage type for the %zmm AVX registers.
    struct zmm_t { std::uint8_t bytes alignas(64) [64]; };

    // The %zmm16-%zmm31 registers.
    zmm_t zmm16[16];
};

static_assert(sizeof(xsave_avx512_zmm_hi16_t) == 1024);


//! Caches information about the state saved by the xsave instruction.
struct xsave_info_t
{
    // Bitmap of the features that the CPU supports (whether enabled by the OS or not). Only features available in user-
    // mode will be reported here.
    xsave_flag_t    supported_user_features;

    // Bitmap of the features that the CPU supports (whether enabled by the OS or not). Only features exclusive to
    // supervisor mode will be reported here.
    xsave_flag_t    supported_supervisor_features;

    // Extended xsave capabilities that the processor supports.
    xsave_caps_t    capabilities;

    // Size of the xsave area if all supported components are enabled.
    std::size_t     max_size;

    // Size of the xsave area for all user-level components enabled in XCR0.
    std::size_t     current_user_size;

    // Size of the xsave area for all components enabled in XCR0 or the IA32_XSS MSR.
    std::size_t     current_supervisor_size;

    // Offsets to and sizes of each of the save areas. If compaction is enabled, the offsets no longer apply but the
    // sizes remain valid.
    std::ptrdiff_t  offset[XsaveFeatureCount] = {};
    std::size_t     size[XsaveFeatureCount] = {};
    bool            align_64[XsaveFeatureCount] = {};
};


//! @returns true if the xsave instruction is supported.
static inline bool isXsaveSupported()
{
    using namespace System::HW::CPU::CPUID;
    return HasFeature(Feature::Xsave);
}

//! @returns true if the xsave instruction is enabled by the kernel.
static inline bool isXsaveAvailable()
{
    using namespace System::HW::CPU::CPUID;
    return HasFeature(Feature::XsaveEnabled);
}

//! @returns information about the layout of the xsave save area.
xsave_info_t getXsaveInfo();

//! Reads a value from an extended control register (XCR).
static inline std::uint64_t xgetbv(std::uint32_t index)
{
    std::uint32_t lo, hi;
    asm
    (
        "xgetbv"
        : "=a" (lo), "=d" (hi)
        : "c" (index)
    );

    return (std::uint64_t(hi) << 32) | lo;
}

//! Writes the given value to an extended control register (XCR).
static inline void xsetbv(std::uint32_t index, std::uint64_t value)
{
    asm volatile
    (
        "xsetbv"
        :
        : "a" (static_cast<std::uint32_t>(value & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((value >> 32) & 0xFFFFFFFF)),
          "c" (index)
        : "memory"
    );
}


//! Restores processor state from the given save area according to the given bitmap.
//!
//! @warning it is the caller's responsibility to ensure that the save area is valid for this operation.
static inline void xrstor(xsave_flag_t states, const fxsave_t* save_area)
{
    asm volatile
    (
        "xrstor %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

//! Restores supervisor-level processor state from the given save area according to the given bitmap.
//!
//! @warning it is the caller's responsibility to ensure that the save area is valid for this operation.
static inline void xrstors(xsave_flag_t states, const fxsave_t* save_area)
{
    asm volatile
    (
        "xrstors %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsave(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsave %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsavec(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsavec %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsaveopt(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsaveopt %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsaves(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsaves %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

#if defined(__x86_64__)
//! Restores processor state from the given save area according to the given bitmap.
//!
//! @warning it is the caller's responsibility to ensure that the save area is valid for this operation.
static inline void xrstor64(xsave_flag_t states, const fxsave_t* save_area)
{
    asm volatile
    (
        "xrstor64 %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

//! Restores supervisor-level processor state from the given save area according to the given bitmap.
//!
//! @warning it is the caller's responsibility to ensure that the save area is valid for this operation.
static inline void xrstors64(xsave_flag_t states, const fxsave_t* save_area)
{
    asm volatile
    (
        "xrstors64 %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsave64(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsave64 %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsavec64(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsavec64 %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsaveopt64(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsaveopt64 %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}

static inline void xsaves64(xsave_flag_t states, fxsave_t* save_area)
{
    asm volatile
    (
        "xsaves64 %0"
        :
        : "m" (*save_area),
          "a" (static_cast<std::uint32_t>(std::uint64_t(states) & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((std::uint64_t(states) >> 32) & 0xFFFFFFFF))
        : "memory"
    );
}
#endif


__SYSTEM_ABI_EXECCONTEXT_EXPORT xsave_info_t getXsaveInfo() __SYSTEM_ABI_EXECCONTEXT_SYMBOL(GetXsaveInfo);
__SYSTEM_ABI_EXECCONTEXT_EXPORT bool _isXsaveSupported() __SYSTEM_ABI_EXECCONTEXT_SYMBOL(IsXsaveSupported);
__SYSTEM_ABI_EXECCONTEXT_EXPORT bool _isXsaveAvailable() __SYSTEM_ABI_EXECCONTEXT_SYMBOL(IsXsaveAvailable);

__SYSTEM_ABI_EXECCONTEXT_EXPORT uint64_t _xgetbv(uint32_t index) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xgetbv);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsetbv(uint32_t index, uint64_t value) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsetbv);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xrstor(uint64_t flags, const void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xrstor);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xrstors(uint64_t flags, const void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xrstors);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsave(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsave);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsavec(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsavec);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsaveopt(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsaveopt);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsaves(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsaves);

#if defined(__x86_64__)
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xrstor64(uint64_t flags, const void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xrstor64);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xrstors64(uint64_t flags, const void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xstors64);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsave64(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsave64);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsavec64(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsavec64);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsaveopt64(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsaveopt64);
__SYSTEM_ABI_EXECCONTEXT_EXPORT void _xsaves64(uint64_t flags, void* save_area) __SYSTEM_ABI_EXECCONTEXT_SYMBOL(Xsaves64);
#endif


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_XSAVE_H */
