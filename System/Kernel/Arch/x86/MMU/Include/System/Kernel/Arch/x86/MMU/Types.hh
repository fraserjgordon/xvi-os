#ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_TYPES_H
#define __SYSTEM_KERNEL_ARCH_X86_MMU_TYPES_H


#include <compare>
#include <cstdint>


namespace System::Kernel::X86::MMU
{


using vaddr_t = std::uintptr_t;
using vsize_t = std::size_t;
using vdiff_t = std::ptrdiff_t;

using paddr_t = std::uint64_t;
using psize_t = std::uint64_t;
using pdiff_t = std::int64_t;


enum class PagingMode
{
    Disabled        = 0,    // Paging is disabled.
    Legacy          = 1,    // 32-bit virtual addresses -> 32-bit physical addresses.
    PAE             = 2,    // 32-bit virtual addresses -> 36(+)-bit physical addresses.
    LongMode        = 3,    // 48-bit virtual addresses -> 52(+)-bit physical addresses.
    LongMode5Level  = 4,    // 57-bit virtual addresses -> 52(+)-bit physical addresses.
};

using paging_mode = PagingMode;


// Note: these values are intended to match the cache types as written into MTRR/PAT fields.
enum class CacheType
{
    Uncached        = 0,
    WriteCombine    = 1,
    WriteThrough    = 4,
    WriteProtect    = 5,
    WriteBack       = 6,
    UncachedMinus   = 7,    // Like Uncached but overwritable by MTRRs to WriteCombine.
};

using cache_type = CacheType;


using page_flags = std::uint8_t;

namespace PageFlag
{
    constexpr page_flags    P       = 0x01;     // Present.
    constexpr page_flags    W       = 0x02;     // Writeable.
    constexpr page_flags    X       = 0x04;     // Executable.
    constexpr page_flags    U       = 0x08;     // User.
    constexpr page_flags    G       = 0x10;     // Global.
    constexpr page_flags    A       = 0x20;     // Accessed.
    constexpr page_flags    D       = 0x40;     // Dirty.
}


struct mtrr_record
{
    std::uint32_t       msr;        // Which MSR this record corresponds to.
    std::uint64_t       value;      // Value to write into the MTRR.
};


union mtrr_fixed
{
    std::uint64_t       raw;
    std::uint8_t        entries[8];
};


struct mtrr_variable
{
    std::uint64_t       base;
    std::uint64_t       mask;
};


struct mapping_info
{
    vaddr_t             address;
    vsize_t             size;
    paddr_t             physical;
    page_flags          flags;
    cache_type          caching;
};


} // namespace System::Kernel::X86::MMU


#endif /* ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_TYPES_H */
