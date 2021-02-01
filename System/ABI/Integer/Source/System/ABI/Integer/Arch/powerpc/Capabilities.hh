#pragma once
#ifndef __SYSTEM_ABI_INTEGER_ARCH_POWERPC_CAPABILITIES_H
#define __SYSTEM_ABI_INTEGER_ARCH_POWERPC_CAPABILITIES_H


#if !defined(_ARCH_PPC)
#  error Incompatible architecture
#endif

#ifndef __INTEGER_ASSUME_ISA64
#  if defined(_ARCH_PPC64)
#    define __INTEGER_ASSUME_ISA64 1
#  endif
#endif

// Support for 128-bit arithmetic operations.
#if __INTEGER_ASSUME_ISA64
#  define __SYSTEM_ABI_INTEGER_PROVIDE_128BIT 1
#endif

// Arithmetic capabilities.
#if __INTEGER_ASSUME_ISA64
#  define __INTEGER_ARCH_SHIFT_CAP      __INTEGER_SHIFT_128
#  define __INTEGER_ARCH_MUL_CAP        __INTEGER_MUL_64_128
#  define __INTEGER_ARCH_DIV_CAP        __INTEGER_DIV_64
#else
#  define __INTEGER_ARCH_SHIFT_CAP      __INTEGER_SHIFT_64
#  define __INTEGER_ARCH_MUL_CAP        __INTEGER_MUL_32_64
#  define __INTEGER_ARCH_DIV_CAP        __INTEGER_DIV_32
#endif


#endif /* ifndef __SYSTEM_ABI_INTEGER_ARCH_POWERPC_CAPABILITIES_H */
