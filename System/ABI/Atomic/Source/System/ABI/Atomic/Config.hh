#pragma once
#ifndef __SYSTEM_ABI_ATOMIC_CONFIG_H
#define __SYSTEM_ABI_ATOMIC_CONFIG_H


#ifdef __SYSTEM_ABI_ATOMIC_BUILD_SHARED
#  if 0 && !defined(ATOMIC_NO_IFUNC) && __has_attribute(ifunc) 
#    define ATOMIC_IFUNC 1
#    define ATOMIC_IFUNC_RESOLVER(resolver)  [[gnu::ifunc(resolver)]]
#  endif
#endif

#if 0 && !defined(ATOMIC_IFUNC) && !defined(ATOMIC_NO_RUNTIME_CHECKS)
#  define ATOMIC_RUNTIME_CHECKS 1
#endif


#endif /* ifndef __SYSTEM_ABI_ATOMIC_CONFIG_H */
