#ifndef __SYSTEM_HW_CPU_X86_MMU_TLB_H
#define __SYSTEM_HW_CPU_X86_MMU_TLB_H


#include <cstdint>


namespace System::HW::CPU::X86::MMU
{


inline void invlpg(std::uintptr_t addr)
{
    asm volatile
    (
        "invlpg (%0)"
        :
        : "r" (addr)
        : "memory"
    );
}

inline void invlpga(std::uintptr_t addr, std::uint32_t asid)
{
    asm volatile
    (
        "invlpga"
        :
        : "a" (addr), "c" (asid)
        : "memory"
    );
}

inline void invlpgb(std::uintptr_t address_info, std::uint32_t attr, std::uint32_t count)
{
    asm volatile
    (
        "invlpgb"
        :
        : "a" (address_info), "b" (attr), "c" (count)
        : "memory"
    );
}

inline void invlpcid(std::uintptr_t address, std::uint32_t pcid, std::uint32_t scope)
{
    struct descriptor_t
    {
        std::uint64_t   pcid:12     = 0;
        std::uint64_t   :0          = 0;
        std::uint64_t   address     = 0;
    };

    descriptor_t descriptor { .pcid = pcid, .address = address };
    
    asm volatile
    (
        "invlpcid   %0, %1"
        :
        : "r" (scope), "m" (descriptor)
        : "memory"
    );
}

inline void tlbsync()
{
    asm volatile
    (
        "tlbsync"
        :
        :
        : "memory"
    );
}


} // namespace System::HW::CPU::X86::MMU


#endif /* ifndef __SYSTEM_HW_CPU_X86_MMU_TLB_H */
