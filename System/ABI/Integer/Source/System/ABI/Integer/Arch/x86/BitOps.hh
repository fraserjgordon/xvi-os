#pragma once
#ifndef __SYSTEM_ABI_INTEGER_ARCH_X86_BITOPS_H
#define __SYSTEM_ABI_INTEGER_ARCH_X86_BITOPS_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/Integer/Arch/x86/Capabilities.hh>


#if !defined(__INTEGER_POPCOUNT32_INTRINSIC) && __INTEGER_ASSUME_POPCNT
#  define __INTEGER_POPCOUNT32_INTRINSIC __asm_popcntl
static inline int __asm_popcntl(std::uint32_t u)
{
    int count;
    asm
    (
        "popcntl    %1, %0"
        : "=r" (count)
        : "g" (u)
    );
    return count;
}
#endif

#if !defined(__INTEGER_POPCOUNT64_INTRINSIC) && __INTEGER_ASSUME_POPCNT && __INTEGER_ASSUME_ISA64
#  define __INTEGER_POPCOUNT64_INTRINSIC __asm_popcntq
static inline int __asm_popcntq(std::uint64_t u)
{
    int count;
    asm
    (
        "popcntq    %1, %0"
        : "=r" (count)
        : "g" (u)
    );
    return count;
}
#endif


#if !defined(__INTEGER_CLZ32_INTRINSIC)
#  if __INTEGER_ASSUME_LZCNT
#    define __INTEGER_CLZ32_INTRINSIC __asm_lzcntl
static inline int __asm_lzcntl(std::uint32_t u)
{
    int count;
    asm
    (
        "lzcntl %1, %0"
        : "=r" (count)
        : "g" (u)
    );
    return count;
}
#  else
#    define __INTEGER_CLZ32_INTRINSIC __asm_bsrl
static inline int __asm_bsrl(std::uint32_t u)
{
    int count;
    asm
    (
        "bsrl   %1, %0"
        : "=r" (count)
        : "g" (u), "0" (32)
    );
    return count;
}
#  endif
#endif

#if !defined(__INTEGER_CLZ64_INTRINSIC) && __INTEGER_ASSUME_ISA64
#  if __INTEGER_ASSUME_LZCNT
#    define __INTEGER_CLZ64_INTRINSIC __asm_lzcntq
static inline int __asm_lzcntq(std::uint64_t u)
{
    std::uint64_t count;
    asm
    (
        "lzcntq %1, %0"
        : "=r" (count)
        : "g" (u)
    );
    return count;
}
#  else
#    define __INTEGER_CLZ64_INTRINSIC __asm_bsrq
static inline int __asm_bsrq(std::uint64_t u)
{
    std::uint64_t count;
    asm
    (
        "bsrq   %1, %0"
        : "=r" (count)
        : "g" (u), "0" (64)
    );
    return count;
}
#  endif
#endif


#if !defined(__INTEGER_CTZ32_INTRINSIC)
#  if __INTEGER_ASSUME_TZCNT
#    define __INTEGER_CTZ32_INTRINSIC __asm_tzcntl
static inline int __asm_tzcntl(std::uint32_t u)
{
    int count;
    asm
    (
        "tzcntl %1, %0"
        : "=r" (count)
        : "g" (u)
    );
    return count;
}
#  else
#    define __INTEGER_CTZ32_INTRINSIC __asm_bsfl
static inline int __asm_bsfl(std::uint32_t u)
{
    int count;
    asm
    (
        "bsfl   %1, %0"
        : "=r" (count)
        : "g" (u), "0" (32)
    );
    return count;
}
#  endif
#endif

#if !defined(__INTEGER_CTZ64_INTRINSIC) && __INTEGER_ASSUME_ISA64
#  if __INTEGER_ASSUME_TZCNT
#    define __INTEGER_CTZ64_INTRINSIC __asm_tzcntq
static inline int __asm_tzcntq(std::uint64_t u)
{
    std::uint64_t count;
    asm
    (
        "tzcntq %1, %0"
        : "=r" (count)
        : "g" (u)
    );
    return count;
}
#  else
#    define __INTEGER_CTZ64_INTRINSIC __asm_bsfq
static inline int __asm_bsfq(std::uint64_t u)
{
    std::uint64_t count;
    asm
    (
        "bsfq   %1, %0"
        : "=r" (count)
        : "g" (u), "0" (64)
    );
    return count;
}
#  endif
#endif


#if !defined(__INTEGER_PARITY32_INTRINSIC)
#  define __INTEGER_PARITY32_INTRINSIC __asm_parity32
static inline __int32_t __asm_parity32(std::uint32_t u)
{
    bool parity;
    asm
    (
        "addl   $0, %1\n\t"
    #if __GCC_ASM_FLAG_OUTPUTS__
        : "=@ccp" (parity)
    #else
        "setp   %0\n\t"
        : "=g" (parity)
    #endif
        : "g" (u)
    );
    return parity; 
}
#endif

#if !defined(__INTEGER_PARITY64_INTRINSIC) && __INTEGER_ASSUME_ISA64
#  define __INTEGER_PARITY64_INTRINSIC __asm_parity64
static inline __int32_t __asm_parity64(std::uint64_t u)
{
    bool parity;
    asm
    (
        "addq  $0, %1\n\t"
    #if __GCC_ASM_FLAG_OUTPUTS__
        : "=@ccp" (parity)
    #else
        "setp   %0\n\t"
        : "=g" (parity)
    #endif
        : "g" (u)
    );
    return parity;
}
#endif


#if !defined(__INTEGER_BSWAP32_INTRINSIC)
#  define __INTEGER_BSWAP32_INTRINSIC __asm_bswapl
static inline __int32_t __asm_bswapl(__int32_t i)
{
    __int32_t swapped;
    asm
    (
        "bswapl %0"
        : "=r" (swapped)
        : "0" (i)
    );
    return swapped;
}
#endif

#if !defined(__INTEGER_BSWAP64_INTRINSIC) && __INTEGER_ASSUME_ISA64
#  define __INTEGER_BSWAP64_INTRINSIC __asm_bswapq
static inline __int64_t __asm_bswapq(__int64_t i)
{
    __int64_t swapped;
    asm
    (
        "bswapq %0"
        : "=r" (swapped)
        : "0" (i)
    );
    return swapped;
}
#endif


#endif /* ifndef __SYSTEM_ABI_INTEGER_ARCH_X86_BITOPS_H */
