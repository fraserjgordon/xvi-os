#ifndef __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_CR_H
#define __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_CR_H


#include <cstdint>


namespace System::HW::CPU::X86
{


namespace CR0
{

enum CR0Bits : std::uint32_t
{
    None    = 0,
    PE      = 0x00000001,   // Protection Enabled
    MP      = 0x00000002,   // Monitor (co-)Processor
    EM      = 0x00000004,   // Emulation
    TS      = 0x00000008,   // Task Switched
    ET      = 0x00000010,   // Extension Type
    NE      = 0x00000020,   // Numeric Error
    WP      = 0x00010000,   // Write Protect
    AM      = 0x00040000,   // Alignment Mask
    NW      = 0x20000000,   // Not Writethrough
    CD      = 0x40000000,   // Cache Disable
    PG      = 0x80000000,   // Paging
};

constexpr CR0Bits operator|(CR0Bits x, CR0Bits y)
{
    return static_cast<CR0Bits>(static_cast<std::uint32_t>(x) | static_cast<std::uint32_t>(y));
}

constexpr CR0Bits operator&(CR0Bits x, CR0Bits y)
{
    return static_cast<CR0Bits>(static_cast<std::uint32_t>(x) & static_cast<std::uint32_t>(y));
}

constexpr CR0Bits operator~(CR0Bits x)
{
    return static_cast<CR0Bits>(~static_cast<std::uint32_t>(x));
}

constexpr CR0Bits& operator|=(CR0Bits& x, CR0Bits y)
{
    x = x | y;
    return x;
}

constexpr CR0Bits& operator&=(CR0Bits& x, CR0Bits y)
{
    x = x & y;
    return x;
}

inline void write(CR0Bits value)
{
    asm volatile
    (
        "mov   %0, %%cr0"
        :
        : "r" (static_cast<std::uintptr_t>(value))
        : "memory"
    );
}

inline CR0Bits read()
{
    std::uintptr_t value;
    asm volatile
    (
        "mov   %%cr0, %0"
        : "=r" (value)
        :
        :
    );
    return static_cast<CR0Bits>(value);
}

inline void set(CR0Bits bits)
{
    write(static_cast<CR0Bits>(read() | bits));
}

inline void unset(CR0Bits bits)
{
    write(static_cast<CR0Bits>(read() & ~bits));
}

} // namespace CR0

using cr0_t = CR0::CR0Bits;


namespace CR1
{

// Not defined. Do not use.

} // namespace CR1


namespace CR2
{

#if __x86_64__
inline std::uint64_t read()
{
    std::uint64_t value;
    asm volatile
    (
        "movq   %%cr2, %0"
        : "=r" (value)
        :
        :
    );
    return value;
}
#else
inline std::uint32_t read()
{
    std::uint32_t value;
    asm volatile
    (
        "movl   %%cr2, %0"
        : "=r" (value)
        :
        :
    );
    return value;
}
#endif

} // namespace CR2



namespace CR3
{


// Mask covering the flags/PCID component of CR3.
constexpr std::uint32_t FlagsMask32 = 0x00000fff;
constexpr std::uint64_t FlagsMask64 = FlagsMask32;

// These flags are present when the PCID feature is disabled.
constexpr std::uint32_t PCD     = 0x00000010;   // Cache Disable for page directory.
constexpr std::uint32_t PWT     = 0x00000008;   // Write-through for page directory.

#if __x86_64__
inline std::uint64_t read()
{
    std::uint64_t value;
    asm volatile
    (
        "movq   %%cr3, %0"
        : "=r" (value)
        :
        :
    );
    return value;
}

inline void write(std::uint64_t value)
{
    asm volatile
    (
        "movq   %0, %%cr3"
        :
        : "r" (value)
        :  
    );
}
#else
inline std::uint32_t read()
{
    std::uint32_t value;
    asm volatile
    (
        "movl   %%cr2, %0"
        : "=r" (value)
        :
        :
    );
    return value;
}

inline void write(std::uint32_t value)
{
    asm volatile
    (
        "movl   %0, %%cr3"
        :
        : "r" (value)
        : "memory"
    );
}
#endif

} // namespace CR3


namespace CR4
{

enum CR4Bits : std::uint32_t
{
    None        = 0,
    VME         = 0x00000001,   // Virtual Mode Extensions
    PVI         = 0x00000002,   // Protected Virtual Interrupts
    TSD         = 0x00000004,   // Time Stamp Disable
    DE          = 0x00000008,   // Debugging Extensions
    PSE         = 0x00000010,   // Page Size Extensions
    PAE         = 0x00000020,   // Physical Address Extension
    MCE         = 0x00000040,   // Machine Check Enable
    PGE         = 0x00000080,   // Page-Global Enable
    PCE         = 0x00000100,   // Performance-monitoring Counter Enable
    OSFXSR      = 0x00000200,   // OS FXSAVE/FXRSTOR support
    OSXMMEXCPT  = 0x00000400,   // OS unmasked SSE exception support
    UMIP        = 0x00000800,   // User-Mode Instruction Prevention
    LA57        = 0x00001000,   // 5-level paging (57-bit virtual addresses).
    VMXE        = 0x00002000,   // VMX Enable
    SMXE        = 0x00004000,   // SMX Enable
    FSGSBASE    = 0x00010000,   // Enable {RD,WR}{FS,GS}BASE instructions
    PCIDE       = 0x00020000,   // Process Context ID Enable
    OSXSAVE     = 0x00040000,   // XSAVE enabled
    KL          = 0x00080000,   // Key Locker
    SMEP        = 0x00100000,   // Supervisor Mode Execution Prevention
    SMAP        = 0x00200000,   // Supervisor Mode Access Protection
    PKE         = 0x00400000,   // Protection Key Enable
    CET         = 0x00800000,   // Control-flow Enforcement Technology
    PKS         = 0x01000000,   // Protection Key for Supervisor mode
};

constexpr CR4Bits operator|(CR4Bits x, CR4Bits y)
{
    return static_cast<CR4Bits>(static_cast<std::uint32_t>(x) | static_cast<std::uint32_t>(y));
}

constexpr CR4Bits operator&(CR4Bits x, CR4Bits y)
{
    return static_cast<CR4Bits>(static_cast<std::uint32_t>(x) & static_cast<std::uint32_t>(y));
}

constexpr CR4Bits operator~(CR4Bits x)
{
    return static_cast<CR4Bits>(~static_cast<std::uint32_t>(x));
}

constexpr CR4Bits& operator|=(CR4Bits& x, CR4Bits y)
{
    x = x | y;
    return x;
}

constexpr CR4Bits& operator&=(CR4Bits& x, CR4Bits y)
{
    x = x & y;
    return x;
}

inline void write(CR4Bits value)
{
    asm volatile
    (
        "mov    %0, %%cr4"
        :
        : "r" (static_cast<std::uintptr_t>(value))
        : "memory"
    );
}

inline CR4Bits read()
{
    std::uintptr_t value;
    asm volatile
    (
        "mov    %%cr4, %0"
        : "=r" (value)
        :
        :
    );
    return static_cast<CR4Bits>(value);
}

inline void set(CR4Bits bits)
{
    write(static_cast<CR4Bits>(read() | bits));
}

inline void unset(CR4Bits bits)
{
    write(static_cast<CR4Bits>(read() & ~bits));
}

} // namespace CR4

using cr4_t = CR4::CR4Bits;


#if __x86_64__
namespace CR8
{

inline std::uint64_t read()
{
    std::uint64_t value;
    asm volatile
    (
        "movq   %%cr8, %0"
        : "=r" (value)
        :
        :
    );
    return value;
}

inline void write(std::uint64_t value)
{
    asm volatile
    (
        "movq   %0, %%cr8"
        :
        : "r" (value)
        : "memory"
    );
}

} // namespace CR8
#endif // if __x86_64__


// x86 CPUs have an opcode specifically for clearing the TS flag of CR0.
inline void clts()
{
    asm volatile
    (
        "clts"
        :
        :
        : "memory"
    );
}


} // namespace System::HW::CPU::X86


#endif // ifndef __SYSTEM_HW_CPU_ARCH_X86_CONTROLREGS_CR_H
