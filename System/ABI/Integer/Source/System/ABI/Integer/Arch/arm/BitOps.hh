#pragma once
#ifndef __SYSTEM_CRT_ARCH_ARM_BITOPS_H
#define __SYSTEM_CRT_ARCH_ARM_BITOPS_H


#include <System/CRT/Types.h>
#include <System/CRT/Arch/arm/Capabilities.hh>


#if !defined(__INTEGER_CLZ32_INTRINSIC) && __INTEGER_ASSUME_CLZ
#  define __INTEGER_CLZ32_INTRINSIC __asm_clz
static inline int __asm_clz(__uint32_t u)
{
    int result;
    asm
    (
        "CLZ %0, %1"
        : "=r" (result)
        : "r" (u)
    );
    return result;
}
#endif

#if !defined(__INTEGER_CTZ32_INTRINSIC) && __INTEGER_ASSUME_RBIT && defined(__INTEGER_CLZ32_INTRINSIC)
#  define __INTEGER_CTZ32_INTRINSIC __asm_rbit_clz
static inline int __asm_rbit_clz(__uint32_t u)
{
    __uint32_t reversed;
    asm
    (
        "RBIT   %0, %1"
        : "=r" (reversed)
        : "r" (u)
    );
    return __INTEGER_CLZ32_INTRINSIC(reversed);
}
#endif


#endif /* ifndef __SYSTEM_CRT_ARCH_ARM_BITOPS_H */
