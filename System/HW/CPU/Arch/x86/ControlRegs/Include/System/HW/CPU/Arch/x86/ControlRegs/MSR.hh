#ifndef __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_MSR_H
#define __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_MSR_H


#include <cstdint>


namespace System::HW::CPU::X86
{


namespace MSR
{

// Architectural and/or generic MSRs.
constexpr std::uint32_t TSC                 = 0x0000'0010;      // Time stamp counter.
constexpr std::uint32_t APIC_BASE           = 0x0000'001B;      // APIC MMIO base address.
constexpr std::uint32_t MTRR_CAP            = 0x0000'00FE;      // MTRR capabilities.
constexpr std::uint32_t MTRR_BASE_0         = 0x0000'0200;      // Variable MTRR 0 base register.
constexpr std::uint32_t MTRR_MASK_0         = 0x0000'0201;      // Variable MTRR 0 mask register.
constexpr std::uint32_t MTRR_FIX_64K_00000  = 0x0000'0250;      // Fixed MTRR for 00000-80000.
constexpr std::uint32_t MTRR_FIX_16K_80000  = 0x0000'0258;      // Fixed MTRR for 80000-A0000.
constexpr std::uint32_t MTRR_FIX_16K_A0000  = 0x0000'0259;      // Fixed MTRR for A0000-C0000.
constexpr std::uint32_t MTRR_FIX_4K_C0000   = 0x0000'0268;      // Fixed MTRR for C0000-C8000.
constexpr std::uint32_t MTRR_FIX_4K_C8000   = 0x0000'0269;      // Fixed MTRR for C8000-D0000.
constexpr std::uint32_t MTRR_FIX_4K_D0000   = 0x0000'026A;      // Fixed MTRR for D0000-D8000.
constexpr std::uint32_t MTRR_FIX_4K_D8000   = 0x0000'026B;      // Fixed MTRR for D8000-E0000.
constexpr std::uint32_t MTRR_FIX_4K_E0000   = 0x0000'026C;      // Fixed MTRR for E0000-E8000.
constexpr std::uint32_t MTRR_FIX_4K_E8000   = 0x0000'026D;      // Fixed MTRR for E8000-F0000.
constexpr std::uint32_t MTRR_FIX_4K_F0000   = 0x0000'026E;      // Fixed MTRR for F0000-F8000.
constexpr std::uint32_t MTRR_FIX_4K_F8000   = 0x0000'026F;      // Fixed MTRR for F8000-100000.
constexpr std::uint32_t PAT                 = 0x0000'0277;      // Page attribute table.
constexpr std::uint32_t MTRR_DEFAULT_TYPE   = 0x0000'02FF;      // Attributes for memory not covered by MTRRs.
constexpr std::uint32_t SYSENTER_CS         = 0x0000'0174;      // Target CS for sysenter.
constexpr std::uint32_t SYSENTER_ESP        = 0x0000'0175;      // Stack pointer for sysenter.
constexpr std::uint32_t SYSENTER_EIP        = 0x0000'0176;      // Target EIP for sysenter.  
constexpr std::uint32_t EFER                = 0xC000'0080;      // Extended Features Enable Register.
constexpr std::uint32_t STAR                = 0xC000'0081;      // Syscall/sysret target CS/SS/EIP (32-bit).
constexpr std::uint32_t LSTAR               = 0xC000'0082;      // Target RIP for syscall from long mode.
constexpr std::uint32_t CSTAR               = 0xC000'0083;      // Target RIP for syscall from compatibility mode.
constexpr std::uint32_t SFMASK              = 0xC000'0084;      // Flags to clear from RFLAGS on syscall.
constexpr std::uint32_t FSBASE              = 0xC000'0100;      // Base address for %fs segment.
constexpr std::uint32_t GSBASE              = 0xC000'0101;      // Base address for %gs segment.
constexpr std::uint32_t KERNELGSBASE        = 0xC000'0102;      // Base address for kernel %gs segment.
constexpr std::uint32_t TSC_AUX             = 0xC000'0103;      // Returned by the RDTSCP instruction.
constexpr std::uint32_t TSC_RATIO           = 0xC000'0104;      // TSC scale ratio.

constexpr std::uint32_t MTRR_BASE(unsigned int n)
{
    return MTRR_BASE_0 + 2U*n;
}

constexpr std::uint32_t MTRR_MASK(unsigned int n)
{
    return MTRR_MASK_0 + 2U*n;
}

// AMD-specific MSRs.
namespace AMD
{

constexpr std::uint32_t SYSCFG              = 0xC001'0010;
constexpr std::uint32_t IORR_BASE_0         = 0xC001'0016;
constexpr std::uint32_t IORR_MASK_0         = 0xC001'0017;
constexpr std::uint32_t IORR_BASE_1         = 0xC001'0018;
constexpr std::uint32_t IORR_MASK_1         = 0xC001'0019;
constexpr std::uint32_t TOP_MEM             = 0xC001'001A;
constexpr std::uint32_t TOP_MEM2            = 0xC001'001D;

}

// Intel-specific MSRs.
namespace Intel
{

} // namspace Intel

}   // namespace MSR


inline void wrmsr(std::uint32_t msr, std::uint64_t value)
{
    asm volatile
    (
        "wrmsr"
        :
        : "a" (static_cast<std::uint32_t>(value & 0xFFFFFFFF)),
          "d" (static_cast<std::uint32_t>((value >> 32) & 0xFFFFFFFF)),
          "c" (msr)
        : "memory"
    );
}

inline std::uint64_t rdmsr(std::uint32_t msr)
{
    std::uint32_t low, high;
    asm volatile
    (
        "rdmsr"
        : "=a" (low),
          "=d" (high)
        : "c" (msr)
        :
    );
    return (std::uint64_t(high) << 32) | low;
}


namespace EFER
{

enum EFERBits : std::uint64_t
{
    None    = 0,
    SCE     = 0x00000001,       // System Call Extension.
    LME     = 0x00000100,       // Long Mode Enable.
    LMA     = 0x00000400,       // Long Mode Active.
    NXE     = 0x00000800,       // No-Execute Enable.
    SVME    = 0x00001000,       // Secure Virtual Machine Enable.
    LMSLE   = 0x00002000,       // Long Mode Segment Limit Enable.
    FFXSR   = 0x00004000,       // Fast FXSAVE/FXRSTOR.
    TCE     = 0x00008000,       // Translation Cache Extension.
};

constexpr EFERBits operator|(EFERBits x, EFERBits y)
{
    return static_cast<EFERBits>(static_cast<std::uint64_t>(x) | static_cast<std::uint64_t>(y));
}

constexpr EFERBits operator&(EFERBits x, EFERBits y)
{
    return static_cast<EFERBits>(static_cast<std::uint64_t>(x) & static_cast<std::uint64_t>(y));
}

constexpr EFERBits operator~(EFERBits x)
{
    return static_cast<EFERBits>(~static_cast<std::uint32_t>(x));
}

constexpr EFERBits& operator|=(EFERBits& x, EFERBits y)
{
    x = x | y;
    return x;
}

constexpr EFERBits& operator&=(EFERBits& x, EFERBits y)
{
    x = x & y;
    return x;
}

inline void write(EFERBits value)
{
    wrmsr(MSR::EFER, value);
}

inline EFERBits read()
{
    return static_cast<EFERBits>(rdmsr(MSR::EFER));
}

inline void set(EFERBits bits)
{
    write(static_cast<EFERBits>(read() | bits));
}

inline void unset(EFERBits bits)
{
    write(static_cast<EFERBits>(read() & ~bits));
}

} // namespace EFER

using efer_t = EFER::EFERBits;


#if __x86_64__
// Dedicated instruction for reading the FSBASE MSR.
inline std::uint64_t rdfsbase()
{
    std::uint64_t value;
    asm
    (
        "rdfsbaseq  %0"
        : "=r" (value)
        :
        :
    );
    return value;
}

// Dedicated instruction for writing the FSBASE MSR.
inline void wrfsbase(std::uint64_t value)
{
    asm volatile
    (
        "wrfsbaseq %0"
        :
        : "r" (value)
        : "memory"
    );
}

// Dedicated instruction for reading the GSBASE MSR.
inline std::uint64_t rdgsbase()
{
    std::uint64_t value;
    asm
    (
        "rdgsbaseq  %0"
        : "=r" (value)
        :
        :
    );
    return value;
}

// Dedicated instruction for writing the GSBASE MSR.
inline void wrgsbase(std::uint64_t value)
{
    asm volatile
    (
        "wrgsbaseq %0"
        :
        : "r" (value)
        : "memory"
    );
}

// Dedicated instruction for swapping the GSBASE and KERNELGSBASE MSRs.
inline void swapgs()
{
    asm volatile
    (
        "swapgs"
        :
        :
        : "memory"
    );
}
#endif // if __x86_64__


} // namespace System::HW::CPU::X86


#endif // ifndef __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_MSR_H
