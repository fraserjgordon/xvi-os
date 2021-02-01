#pragma once
#ifndef __SYSTEM_ABI_ATOMIC_ATOMICX86_H
#define __SYSTEM_ABI_ATOMIC_ATOMICX86_H


#include <System/ABI/Atomic/Atomic.hh>
#include <System/ABI/Atomic/Config.hh>


// __ATOMIC_ASSUME_CMPXCHG8B
// __ATOMIC_ASSUME_CMPXCHG16B


#if defined(__x86_64__)
#  if !defined(__ATOMIC_ASSUME_CMPXCHG8B)
#    define __ATOMIC_ASSUME_CMPXCHG8B 1
#  endif
#  if !defined(__ATOMIC_ASSUME_CMPXCHG16B) && __ATOMIC_TUNE_NATIVE
#    if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16)
#      define __ATOMIC_ASSUME_CMPXCHG16B 1
#    endif
#  endif
#endif


#endif /* ifndef __SYSTEM_ABI_ATOMIC_ATOMICX86_H */
