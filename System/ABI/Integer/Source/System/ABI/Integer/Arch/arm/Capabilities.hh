#pragma once
#ifndef __SYSTEM_CRT_ARCH_ARM_CAPABILITIES_H
#define __SYSTEM_CRT_ARCH_ARM_CAPABILITIES_H


#if !defined(__arm__) && !defined(__aarch64__)
#  error Incompatible architecture
#endif

#ifndef __INTEGER_ASSUME_ISA64
#  if defined(__ARM_64BIT_STATE)
#    define __INTEGER_ASSUME_ISA64 1
#  endif
#endif

// Support for 128-bit arithmetic operations.
#if __INTEGER_ASSUME_ISA64
#  define __SYSTEM_ABI_INTEGER_PROVIDE_128BIT 1
#endif

// Features which compilers provide flags for.
#if __ARM_FEATURE_CLZ
#  define __INTEGER_ASSUME_CLZ 1
#  define __INTEGER_CLZ32_INTRINSIC __builtin_clz
#  define __INTEGER_CLZ64_INTRINSIC __builtin_clzll
#  define __INTEGER_CTZ32_INTRINSIC __builtin_ctz
#  define __INTEGER_CTZ64_INTRINSIC __builtin_ctzll
#  define __INTEGER_FFS32_INTRINSIC __builtin_ffs
#  define __INTEGER_FFS64_INTRINSIC __builtin_ffsll
#endif

// Features available from ARMv6T2 onwards.
#if __ARM_ARCH > 6 || defined(__ARM_ARCH_6T2__)
#  define __INTEGER_ASSUME_RBIT 1
#endif

// Arithmetic capabilities.
#if __INTEGER_ASSUME_ISA64
#  define __INTEGER_ARCH_SHIFT_CAP      __INTEGER_SHIFT_128
#  define __INTEGER_ARCH_MUL_CAP        __INTEGER_MUL_128
#  if __ARM_FEATURE_IDIV
#    define __INTEGER_ARCH_DIV_CAP      __INTEGER_DIV_64
#  else
#    define __INTEGER_ARCH_DIV_CAP      __INTEGER_DIV_NONE
#  endif
#else
#  define __INTEGER_ARCH_SHIFT_CAP      __INTEGER_SHIFT_64
#  define __INTEGER_ARCH_MUL_CAP        __INTEGER_MUL_64
#  if __ARM_FEATURE_IDIV
#    define __INTEGER_ARCH_DIV_CAP      __INTEGER_DIV_32  
#  else
#    define __INTEGER_ARCH_DIV_CAP      __INTEGER_DIV_NONE
#  endif
#endif

// Byte swaps are always supported natively (REV opcode).
#define __INTEGER_BSWAP32_INTRINSIC __builtin_bswap32
#define __INTEGER_BSWAP64_INTRINSIC __builtin_bswap64


#endif /* ifndef __SYSTEM_CRT_ARCH_X86_CAPABILITIES_H */
