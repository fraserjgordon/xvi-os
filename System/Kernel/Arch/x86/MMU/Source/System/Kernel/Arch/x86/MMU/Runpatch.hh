#ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_RUNPATCH_H
#define __SYSTEM_KERNEL_ARCH_X86_MMU_RUNPATCH_H


#include <System/Kernel/Runpatch/Arch/x86/Runpatch.hh>


#define PATCH_PAGING_MODE   "x86.mmu.paging"
#if defined(__x86_64__)
#  define PAGING_LM         0
#  define PAGING_LM57       1
#else
#  define PAGING_LEGACY     0
#  define PAGING_PAE        1
#  define PAGING_LM         2
#  define PAGING_LM57       3
#endif

#define PATCH_SMAP_SUPPORT  "x86.mmu.smap"
#define SMAP_DISABLED       0
#define SMAP_ENABLED        1

#define PATCH_TLBINVALIDATEPAGE_METHOD  "x86.mmu.tlbInvalidatePage"
#define TLBINVALIDATEPAGE_MOV_CR3   0
#define TLBINVALIDATEPAGE_INVLPG    1

#define PATCH_TLBFLUSHALL_METHOD    "x86.mmu.tlbFlushAll"
#define TLBFLUSHALL_MOV_CR3         0
#define TLBFLUSHALL_TOGGLE_PGE      1
#define TLBFLUSHALL_INVPCID         2

#define PATCH_TLBFLUSHNONGLOBAL_METHOD  "x86.mmu.tlbFlushNonGlobal"
#define TLBFLUSHNONGLOBAL_MOV_CR3       0
#define TLBFLUSHNONGLOBAL_INVPCID       1

#define PATCH_TLBFLUSHPCID_METHOD   "x86.mmu.tlbFlushPCID"
#define TLBFLUSHPCID_MOV_CR3        0
#define TLBFLUSHPCID_INVPCID        1


#endif /* ifndef __SYSTEM_KERNEL_ARCH_X86_MMU_RUNPATCH_H */
