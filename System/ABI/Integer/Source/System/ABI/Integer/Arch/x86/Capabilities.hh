#pragma once
#ifndef __SYSTEM_ABI_INTEGER_ARCH_X86_CAPABILITIES_H
#define __SYSTEM_ABI_INTEGER_ARCH_X86_CAPABILITIES_H


#if !defined(__x86_64__) && !defined(__i386__)
#  error Incompatible architecture
#endif

#ifndef __INTEGER_ASSUME_ISA64
#  if defined(__x86_64__)
#    define __INTEGER_ASSUME_ISA64 1
#  endif
#endif

// Support for 128-bit arithmetic operations.
#if __INTEGER_ASSUME_ISA64
#  define __SYSTEM_ABI_INTEGER_PROVIDE_128BIT 1
#endif

// Additional instruction sets implied by x86_64.
#if __INTEGER_ASSUME_ISA64
#  define __INTEGER_ASSUME_XADD      1
#  define __INTEGER_ASSUME_CMPXCHG8B 1
#endif

// Arithmetic capabilities.
#if __INTEGER_ASSUME_ISA64
#  define __INTEGER_ARCH_SHIFT_CAP      __INTEGER_SHIFT_128
#  define __INTEGER_ARCH_MUL_CAP        __INTEGER_MUL_64_128
#  define __INTEGER_ARCH_DIV_CAP        __INTEGER_DIV_128_64
#else
#  define __INTEGER_ARCH_SHIFT_CAP      __INTEGER_SHIFT_64
#  define __INTEGER_ARCH_MUL_CAP        __INTEGER_MUL_32_64
#  define __INTEGER_ARCH_DIV_CAP        __INTEGER_DIV_64_32
#endif

// Parity detection is always supported natively.
#define __INTEGER_PARITY32_INTRINSIC __builtin_parity
#if __INTEGER_ASSUME_ISA64
#  define __INTEGER_PARITY64_INTRINSIC __builtin_parityll
#endif

// POPCNT instruction.
#if __INTEGER_TUNE_NATIVE && (defined(__ABM__) || defined(__SSE4_2__) || defined(__POPCNT__))
#  define __INTEGER_ASSUME_POPCNT 1
#  define __INTEGER_POPCOUNT32_INTRINSIC __builtin_popcount
#  define __INTEGER_POPCOUNT64_INTRINSIC __builtin_popcountll
#endif

// LZCNT instruction.
#if __INTEGER_TUNE_NATIVE && defined(__LZCNT__)
#  define __INTEGER_ASSUME_LZCNT 1
#  define __INTEGER_CLZ16_INTRINSIC __builtin_ia32_lzcnt_u16
#  define __INTEGER_CLZ32_INTRINSIC __builtin_ia32_lzcnt_u32
#  if __INTEGER_ASSUME_ISA64
#    define __INTEGER_CLZ64_INTRINSIC __builtin_ia32_lzcnt_u64
#  endif
#endif

// TZCNT instruction.
#if __INTEGER_TUNE_NATIVE && defined(__BMI__)
#  define __INTEGER_ASSUME_TZCNT 1
#  define __INTEGER_CTZ16_INTRINSIC __builtin_ia32_tzcnt_u16
#  define __INTEGER_CTZ32_INTRINSIC __builtin_ia32_tzcnt_u32
#  if __INTEGER_ASSUME_ISA64
#    define __INTEGER_CTZ64_INTRINSIC __builtin_ia32_tzcnt_u64
#  endif
#endif

// Byte swaps are always supported natively.
#define __INTEGER_BSWAP32_INTRINSIC __builtin_bswap32
#define __INTEGER_BSWAP64_INTRINSIC __builtin_bswap64


#endif /* ifndef __SYSTEM_ABI_INTEGER_ARCH_X86_CAPABILITIES_H */
