#pragma once
#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SSE_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SSE_H


namespace System::ABI::ExecContext
{


#define __SSE_NONE      0
#define __SSE1          100
#define __SSE2          200
#define __SSE3          300
#define __SSSE3         301
#define __SSE4          400
#define __AVX           1000
#define __AVX2          2000
#define __AVX512        3000


// The size of SSE registers (in bits) depends on the SSE version.
#define __SSE_REG_BITS_FOR(level) \
    (level >= __AVX512 ? 512 : \
     level >= __AVX    ? 256 : \
     level >= __SSE1   ? 128 : \
     -1)

// The number of SSE registers depends on the SSE version for 64-bit code but is always 8 for 32-bit.
#define __SSE_REG_COUNT_X86_FOR(level)  8
#define __SSE_REG_COUNT_X64_FOR(level)  \
    (level >= __AVX512 ? 32 : \
     level >= __AVX    ? 16 : \
     level >= __SSE1   ? 8  : \
     -1)

// The size of SSE mask registers depends on the SSE version.
#define __SSE_MASK_REG_BITS_FOR(level) \
    (level >= __AVX512 ? 64 : \
     -1)

// The number of SSE mask registers depends on the SSE version.
#define __SSE_MASK_REG_COUNT_FOR(level) \
    (level >= __AVX512 ? 8 : \
     -1)

// If not told otherwise, support all SSE/AVX features.
#if !defined(__SSE_LEVEL)
#  define __SSE_LEVEL   __AVX512
#endif

#define __SSE_REG_BITS  __SSE_REG_BITS_FOR(__SSE_LEVEL)

#define __SSE_REG_COUNT_X86 __SSE_REG_COUNT_X86_FOR(__SSE_LEVEL)
#define __SSE_REG_COUNT_X64 __SSE_REG_COUNT_X64_FOR(__SSE_LEVEL)

#define __SSE_MASK_REG_BITS     __SSE_MASK_REG_BITS_FOR(__SSE_LEVEL)
#define __SSE_MASK_REG_COUNT    __SSE_MASK_REG_COUNT_FOR(__SSE_LEVEL)


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SSE_H */
