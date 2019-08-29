#pragma once
#ifndef __SYSTEM_C_BASEHEADERS_LIMITS_H
#define __SYSTEM_C_BASEHEADERS_LIMITS_H


#define __CHAR_BIT      __CHAR_BIT__
#define __SCHAR_MIN     (-__SCHAR_MAX_-1)
#define __SCHAR_MAX     __SCHAR_MAX__
#define __UCHAR_MAX     (2U*__SCHAR_MAX__+1)
#define __SHRT_MIN      (-__SHRT_MAX__-1)
#define __SHRT_MAX      __SHRT_MAX__
#define __USHRT_MAX     (2U*__SHRT_MAX__+1)
#define __INT_MIN       (-__INT_MAX-1)
#define __INT_MAX       __INT_MAX__
#define __UINT_MAX      (2U*__INT_MAX__+1)
#define __LONG_MIN      (-__LONG_MAX__-1)
#define __LONG_MAX      __LONG_MAX__
#define __ULONG_MAX     (2U*__LONG_MAX__+1)
#define __LLONG_MIN     (-__LONG_LONG_MAX__-1)
#define __LLONG_MAX     __LONG_LONG_MAX__
#define __ULLONG_MAX    (2U*__LONG_LONG_MAX__+1)

#if defined(__UNSIGNED_CHAR__)
#  define __CHAR_MIN    0
#  define __CHAR_MAX    __UCHAR_MAX
#else
#  define __CHAR_MIN    __SCHAR_MIN
#  define __CHAR_MAX    __SCHAR_MAX
#endif

#define __INT8_MIN      (-__INT8_MAX__-1)
#define __INT8_MAX      __INT8_MAX__
#define __UINT8_MAX     (2U*__INT8_MAX__+1)
#define __INT16_MIN     (-__INT16_MAX__-1)
#define __INT16_MAX     __INT16_MAX__
#define __UINT16_MAX    (2U*__INT16_MAX__+1)
#define __INT32_MIN     (-__INT32_MAX__-1)
#define __INT32_MAX     __INT32_MAX__
#define __UINT32_MAX    (2U*__INT32_MAX__+1)
#define __INT64_MIN     (-__INT64_MAX__-1)
#define __INT64_MAX     __INT64_MAX__
#define __UINT64_MAX    (2U*__INT64_MAX__+1)

#define __INT_LEAST8_MIN   (-__INT_LEAST8_MAX__-1)
#define __INT_LEAST8_MAX    __INT_LEAST8_MAX__
#define __UINT_LEAST8_MAX  (2U*__INT_LEAST8_MAX__+1)
#define __INT_LEAST16_MIN  (-__INT_LEAST16_MAX__-1)
#define __INT_LEAST16_MAX   __INT_LEAST16_MAX__
#define __UINT_LEAST16_MAX (2U*__INT_LEAST32_MAX__+1)
#define __INT_LEAST32_MIN  (-__INT_LEAST32_MAX__-1)
#define __INT_LEAST32_MAX   __INT_LEAST32_MAX__
#define __UINT_LEAST32_MAX (2U*__INT_LEAST32_MAX__+1)
#define __INT_LEAST64_MIN  (-__INT_LEAST64_MAX__-1)
#define __INT_LEAST64_MAX   __INT_LEAST64_MAX__
#define __UINT_LEAST64_MAX (2U*__INT_LEAST64_MAX__+1)

#define __INT_FAST8_MIN     (-__INT_FAST8_MAX__-1)
#define __INT_FAST8_MAX     __INT_FAST8_MAX__
#define __UINT_FAST8_MAX    (2U*__INT_FAST8_MAX__+1)
#define __INT_FAST16_MIN    (-__INT_FAST16_MAX__-1)
#define __INT_FAST16_MAX    __INT_FAST16_MAX__
#define __UINT_FAST16_MAX   (2U*__INT_FAST16_MAX__+1)
#define __INT_FAST32_MIN    (-__INT_FAST32_MAX__-1)
#define __INT_FAST32_MAX    __INT_FAST32_MAX__
#define __UINT_FAST32_MAX   (2U*__INT_FAST32_MAX__+1)
#define __INT_FAST64_MIN    (-__INT_FAST64_MAX__-1)
#define __INT_FAST64_MAX    __INT_FAST64_MAX__
#define __UINT_FAST64_MAX   (2U*__INT_FAST64_MAX__+1)

#define __INTPTR_MIN        (-__INTPTR_MAX__-1)
#define __INTPTR_MAX        __INTPTR_MAX__
#define __UINTPTR_MAX       (2U*__INTPTR_MAX__+1)

#define __INTMAX_MIN        (-__INTMAX_MAX__-1)
#define __INTMAX_MAX        __INTMAX_MAX__
#define __UINTMAX_MAX       (2U*__INTMAX_MAX__+1)

#define __PTRDIFF_MIN       (-__PTRDIFF_MAX__-1)
#define __PTRDIFF_MAX       __PTRDIFF_MAX__

#define __WCHAR_MIN         (-__WCHAR_MAX__-1)
#define __WCHAR_MAX         __WCHAR_MAX__

#define __WINT_MIN          (-__WINT_MAX__-1)
#define __WINT_MAX          __WINT_MAX__

#define __SIZE_MAX          __SIZE_MAX__

#define __CHAR8_MIN         0
#define __CHAR8_MAX         __UINT_LEAST8_MAX

#define __CHAR16_MIN        0
#define __CHAR16_MAX        __UINT_LEAST16_MAX

#define __CHAR32_MIN        0
#define __CHAR32_MAX        __UINT_LEAST32_MAX

// GCC pre-defines these; Clang does not.
#ifndef __INT8_C
#  define __INT8_C(x)       x
#  define __INT16_C(x)      x
#  define __INT32_C(x)      x
#  define __UINT8_C(x)      x
#  define __UINT16_C(x)     x
#  define __UINT32_C(x)     x ## U
#  if __LONG_MAX >= __INT32_MAX
#    define __INT64_C(x)    x ## L
#    define __UINT64_C(x)   x ## UL
#  else
#    define __INT64_C(x)    x # LL
#    define __UINT64_C(x)   x # ULL
#  endif
#endif

// Not strictly limit-related but they do depend on the underlying types.
#define __INT8_FMTd         "hhd"
#define __INT8_FMTi         "hhi"
#define __UINT8_FMTo        "hho"
#define __UINT8_FMTu        "hhu"
#define __UINT8_FMTx        "hhx"
#define __UINT8_FMTX        "hhX"
#define __INT16_FMTd        "hd"
#define __INT16_FMTi        "hi"
#define __UINT16_FMTo       "ho"
#define __UINT16_FMTu       "hu"
#define __UINT16_FMTx       "hx"
#define __UINT16_FMTX       "hX"
#define __INT32_FMTd        "d"
#define __INT32_FMTi        "i"
#define __UINT32_FMTo       "o"
#define __UINT32_FMTu       "u"
#define __UINT32_FMTx       "x"
#define __UINT32_FMTX       "X"
#if __LONG_MAX >= __INT64_MAX
#  define __INT64_FMTd      "ld"
#  define __INT64_FMTi      "li"
#  define __UINT64_FMTo     "lo"
#  define __UINT64_FMTu     "lu"
#  define __UINT64_FMTx     "lx"
#  define __UINT64_FMTX     "lX"
#else
#  define __INT64_FMTd      "lld"
#  define __INT64_FMTi      "lli"
#  define __UINT64_FMTo     "llo"
#  define __UINT64_FMTu     "llu"
#  define __UINT64_FMTx     "llx"
#  define __UINT64_FMTX     "llX"
#endif


#endif /* ifndef __SYSTEM_C_BASEHEADERS_LIMITS_H */
