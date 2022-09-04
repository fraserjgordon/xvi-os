#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_MMU_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_MMU_H


#include <cstdint>


namespace System::Boot::Igniter
{


// Flags for the addEarlyMap method.
namespace EarlyMapFlag
{
    constexpr std::uint32_t     R   = 0;
    constexpr std::uint32_t     W   = 0x01;
    constexpr std::uint32_t     X   = 0x02;
    constexpr std::uint32_t     U   = 0x04;
    constexpr std::uint32_t     C   = 0x08;

    constexpr std::uint32_t     RWXC    = R|W|X|C;
    constexpr std::uint32_t     RC      = R|C;
    constexpr std::uint32_t     RW      = R|W;
    constexpr std::uint32_t     RXC     = R|X|C;
    constexpr std::uint32_t     RWXUC   = R|W|X|U|C;
    constexpr std::uint32_t     RXUC    = R|X|U|C;
}

using early_map_flag_t = std::uint32_t;


void enablePaging();

// Called by the probe code to set up the initial set of (identity-mapped) page tables.
void addEarlyMap(std::uint32_t address, std::uint32_t size, early_map_flag_t flags);


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_MMU_H */
