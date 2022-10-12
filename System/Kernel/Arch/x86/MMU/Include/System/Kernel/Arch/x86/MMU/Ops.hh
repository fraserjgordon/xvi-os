#ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_OPS_H
#define __SYSTEM_KERNEL_ARCH_X86_MMU_OPS_H


#include <cstdint>


namespace System::Kernel::X86::MMU
{


struct invpcid_descriptor_t
{ 
    std::uint64_t   pcid:12 = 0;
    std::uint64_t   reserved:54 = 0;
    std::uint64_t   address = 0;
};


inline void cldemote(std::uintptr_t address)
{
    asm volatile ("cldemote (%0)" : : "r" (address) : "memory");
}

inline void clflush(std::uintptr_t address)
{
    asm volatile ("clflush (%0)" : : "r" (address) : "memory");
}

inline void clflushsopt(std::uintptr_t address)
{
    asm volatile ("clflushopt (%0)" : : "r" (address) : "memory");
}

inline void clwb(std::uintptr_t address)
{
    asm volatile ("clwb (%0)" : : "r" (address) : "memory");
}

inline void clzero(std::uintptr_t address)
{
    asm volatile ("clzero" : : "a" (address) : "memory");
}

inline void invd()
{
    asm volatile ("invd" ::: "memory");
}

inline void invlpg(std::uintptr_t address)
{
    asm volatile ("invlpg (%0)" : : "r" (address) : "memory");
}

inline void invlpga(std::uintptr_t address, std::uint32_t asid)
{
    asm volatile ("invlpga" : : "a" (address), "c" (asid) : "memory");
}

inline void invlpgb(std::uintptr_t descriptor, std::uint32_t details)
{
    asm volatile ("invlpgb" : : "a" (descriptor), "d" (details) : "memory");
}

inline void invpcid(std::uint64_t address, std::uint32_t pcid, std::uint32_t op)
{
    invpcid_descriptor_t descriptor { pcid, address };
    asm volatile ("invpcid %0, %1" : : "r" (op), "m" (descriptor) : "memory");
}

inline void lfence()
{
    asm volatile ("lfence" ::: "memory");
}

inline bool mcommit()
{
    bool result;
    asm volatile ("mcommit" : "=@ccc" (result) :: "memory");
    return result; 
}

inline void mfence()
{
    asm volatile ("mfence" ::: "memory");
}

inline void sfence()
{
    asm volatile ("sfence" ::: "memory");
}

inline std::uint32_t rdpkru()
{
    std::uint64_t retval;
    asm ("rdpkru" : "=A" (retval) : "c" (0));
    return retval;
}

inline void tlbsync()
{
    asm volatile ("tlbsync" ::: "memory");
}

inline void wbinvd()
{
    asm volatile ("wbinvd" ::: "memory");
}

inline void wbnoinvd()
{
    asm volatile ("wbnoinvd" ::: "memory");
}

inline void wrpkru(std::uint32_t keys)
{
    asm volatile ("wrpkru" : : "A" (keys), "c" (0) : "memory");
}


} // namespace System::Kernel::X86::MMU


#endif /* ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_OPS_H */
