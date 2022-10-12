#ifndef __SYSTEM_KERNEL_X86_MMU_TLB_H
#define __SYSTEM_KERNEL_X86_MMU_TLB_H


#include <cstdint>

#include <System/Kernel/Arch/x86/MMU/Runpatch.hh>
#include <System/Kernel/Arch/x86/MMU/Types.hh>


namespace System::Kernel::X86::MMU
{


inline void tlbInvalidatePage(vaddr_t address)
{
    asm volatile
    (
        RUNPATCH_START(PATCH_TLBINVALIDATEPAGE_METHOD)

        // 1: invlpg instruction
        RUNPATCH_DEFINE_ALTERNATIVE(
            "invlpg (%0)"
        )

        // Base case: write to %cr3 to flush entire TLB.
        RUNPATCH_DEFINE_DEFAULT(
            "mov    %%cr3, %0 ; \
             mov    %0, %%cr3 "
        )

        : "=r" (address)
        : "0" (address)
        : "memory"
    );
}

inline void tlbFlushAll()
{
    std::uintptr_t temp;
    asm volatile
    (
        RUNPATCH_START(PATCH_TLBFLUSHALL_METHOD)

        // 1: global pages are enabled.
        // Simply writing %cr3 is not enough; we need to toggle the PGE bit in %cr4.
        RUNPATCH_DEFINE_ALTERNATIVE(
            "mov     %%cr4, %0 ; \
             xor     $0x80, %0 ; \
             mov     %0, %%cr4 ; \
             xor     $0x80, %0 ; \
             mov     %0, %%cr4 ;"
        )

        // 2: invpcid is available
        // We can use invpcid to flush all TLB entries by using the right op.
#ifdef __x86_64__
        RUNPATCH_DEFINE_ALTERNATIVE(
            "xorq    %0, %0 ; \
             pushq   %0 ; \
             pushq   %0 ; \
             addq    $2, %0 ; \
             invpcid 0(%%rsp), %0 ; \
             addq    $16, %%rsp"
        )
#else
        RUNPATCH_DEFINE_ALTERNATIVE(
            ".arch push ; \
             .arch .invpcid ; \
             xorl    %0, %0 ; \
             pushl   %0 ; \
             pushl   %0 ; \
             pushl   %0 ; \
             pushl   %0 ; \
             addl    $2, %0 ; \
             invpcid 0(%%esp), %0 ; \
             addl    $16, %%esp ; \
             .arch pop"
        )
#endif

        // Base case (no enhanced features supported): a write to %cr3 fill flush the entire TLB.
        RUNPATCH_DEFINE_DEFAULT(
            "mov     %%cr3, %0 ; \
             mov     %0, %%cr3"
        )

        RUNPATCH_FINISH()

        : "=&r" (temp)
        :
        : "memory"
    );
}

inline void tlbFlushNonGlobal()
{
    std::uintptr_t temp;
    asm volatile
    (
        RUNPATCH_START(PATCH_TLBFLUSHNONGLOBAL_METHOD)

        // 1: invpcid is available and PCID is enabled.
        // We can use invpcid to flush all non-global TLB entries by using the right op.
#ifdef __x86_64__
        RUNPATCH_DEFINE_ALTERNATIVE(
            "xorq    %0, %0 ; \
             pushq   %0 ; \
             pushq   %0 ; \
             addq    $3, %0 ; \
             invpcid 0(%%rsp), %0 ; \
             addq    $16, %%rsp"
        )
#endif

        // Base case: a write to %cr3 fill flush the entire TLB.
        RUNPATCH_DEFINE_DEFAULT(
            "mov     %%cr3, %0 ; \
             mov     %0, %%cr3"
        )

        RUNPATCH_FINISH()
        : "=&r" (temp)
        :
        : "memory"
    );
}

inline void tlbFlushPCID(std::uint32_t pcid)
{
    std::uintptr_t temp;
    asm volatile
    (
        RUNPATCH_START(PATCH_TLBFLUSHPCID_METHOD)

        // 1: invpcid is available and PCID is enabled.
        // We can flush all TLB entries corresponding to the given PCID.
#ifdef __x86_64__
        RUNPATCH_DEFINE_ALTERNATIVE(
            "xorq    %0, %0 ; \
             pushq   %0 ; \
             pushq   %q1 ; \
             addq    $1, %0 ; \
             invpcid 0(%%rsp), %0 ; \
             addq    $16, %%rsp"
        )
#endif

        // Base case: PCIDs aren't supported; write to %cr3 to flush all non-global entries.
        RUNPATCH_DEFINE_DEFAULT(
            "mov     %%cr3, %0 ; \
             mov     %0, %%cr3"
        )

        RUNPATCH_FINISH()
        : "=&r" (temp)
        : "r" (pcid)
        : "memory"
    );
}

inline void tlbFlushThisPCID()
{
    // Regardless of PCID status, we can flush the TLB for the current PCID by writing to %cr3.
    std::uintptr_t temp;
    asm volatile
    (
        "mov     %%cr3, %0 ; \
         mov     %0, %%cr3"
    
        : "=&r" (temp)
        :
        : "memory"
    );
}


} // namespace System::Kernel::X86::MMU


#endif /* ifndef __SYSTEM_KERNEL_X86_MMU_TLB_H */
