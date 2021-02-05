#pragma once
#ifndef __SYSTEM_ABI_ATOMIC_ARCH_ARM_PRIMITIVES_H
#define __SYSTEM_ABI_ATOMIC_ARCH_ARM_PRIMITIVES_H


#include <System/ABI/Atomic/Atomic.hh>
#include <System/ABI/Atomic/Config.hh>


// __ATOMIC_ASSUME_LSE_ATOMICS


#if defined(__ARM_ARCH)
#  if !defined(__ATOMIC_ASSUME_LSE_ATOMICS)
#    if defined(__ARM_FEATURE_ATOMICS)
#      define __ATOMIC_ASSUME_LSE_ATOMICS 1
#    else
#      define __ATOMIC_ASSUME_LSE_ATOMICS 0
#    endif
#  endif
#endif


#endif /* ifndef __SYSTEM_ABI_ATOMIC_ARCH_ARM_PRIMITIVES_H */
