#ifndef __SYSTEM_C_BASEHEADERS_TYPES_H
#define __SYSTEM_C_BASEHEADERS_TYPES_H


// Some hosts define __size_t
#if __XVI_HOSTED && defined(__size_t)
#  define __XVI_REDEF_SIZE_T
#  undef __size_t
#endif

typedef __PTRDIFF_TYPE__    __ptrdiff_t;
typedef __SIZE_TYPE__       __size_t;

#ifdef __XVI_REDEF_SIZE_T
#  undef __XVI_REDEF_SIZE_T
#  define __size_t
#endif

#ifdef __cplusplus
typedef struct { alignas(__BIGGEST_ALIGNMENT__) char __c[1]; } __max_align_t;
#else
typedef struct { _Alignas(__BIGGEST_ALIGNMENT__) char __c[1]; } __max_align_t;
#endif

typedef __UINTPTR_TYPE__    __uintptr_t;
typedef __INTPTR_TYPE__     __intptr_t;

typedef __INTMAX_TYPE__     __intmax_t;
typedef __UINTMAX_TYPE__    __uintmax_t;

typedef __UINT8_TYPE__      __uint8_t;
typedef __UINT16_TYPE__     __uint16_t;
typedef __UINT32_TYPE__     __uint32_t;
typedef __UINT64_TYPE__     __uint64_t;

typedef __INT8_TYPE__       __int8_t;
typedef __INT16_TYPE__      __int16_t;
typedef __INT32_TYPE__      __int32_t;
typedef __INT64_TYPE__      __int64_t;

typedef __UINT_LEAST8_TYPE__    __uint_least8_t;
typedef __UINT_LEAST16_TYPE__   __uint_least16_t;
typedef __UINT_LEAST32_TYPE__   __uint_least32_t;
typedef __UINT_LEAST64_TYPE__   __uint_least64_t;

typedef __INT_LEAST8_TYPE__     __int_least8_t;
typedef __INT_LEAST16_TYPE__    __int_least16_t;
typedef __INT_LEAST32_TYPE__    __int_least32_t;
typedef __INT_LEAST64_TYPE__    __int_least64_t;

typedef __UINT_FAST8_TYPE__     __uint_fast8_t;
typedef __UINT_FAST16_TYPE__    __uint_fast16_t;
typedef __UINT_FAST32_TYPE__    __uint_fast32_t;
typedef __UINT_FAST64_TYPE__    __uint_fast64_t;

typedef __INT_FAST8_TYPE__      __int_fast8_t;
typedef __INT_FAST16_TYPE__     __int_fast16_t;
typedef __INT_FAST32_TYPE__     __int_fast32_t;
typedef __INT_FAST64_TYPE__     __int_fast64_t;

typedef __WINT_TYPE__           __wint_t;

#ifdef __SIG_ATOMIC_TYPE__
typedef __SIG_ATOMIC_TYPE__     __sig_atomic_t;
#elif __SIG_ATOMIC_WIDTH__ == 32
typedef __INT32_TYPE__          __sig_atomic_t;
#elif __SIG_ATOMIC_WIDTH__ == 64
typedef __INT64_TYPE__          __sig_atomic_t;
#endif

#ifndef __cplusplus
#define __WCHAR_TYPE__          __wchar_t;
typedef __CHAR16_TYPE__         __char16_t;
typedef __CHAR32_TYPE__         __char32_t;
#elif !defined(__llvm__)
typedef wchar_t                 __wchar_t;
typedef char16_t                __char16_t;
typedef char32_t                __char32_t;
#endif

#define __EOF                   (-1)
#ifdef __cplusplus
#  define __WEOF                (wint_t{0}-1)
#else
#  define __WEOF                ((wint_t)-1)
#endif


#endif /* ifndef __SYSTEM_C_BASEHEADERS_TYPES_H */
