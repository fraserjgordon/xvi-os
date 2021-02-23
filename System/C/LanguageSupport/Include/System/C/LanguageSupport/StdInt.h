#ifndef __SYSTEM_C_LANGUAGESUPPORT_STDINT_H
#define __SYSTEM_C_LANGUAGESUPPORT_STDINT_H


#include <System/C/BaseHeaders/Limits.h>
#include <System/C/BaseHeaders/Types.h>


#define INT8_MIN        __INT8_MIN
#define INT8_MAX        __INT8_MAX
#define INT8_WIDTH      __INT8_WIDTH
#define UINT8_MAX       __UINT8_MAX
#define UINT8_WIDTH     __UINT8_WIDTH
#define INT16_MIN       __INT16_MIN
#define INT16_MAX       __INT16_MAX
#define INT16_WIDTH     __INT16_WIDTH
#define UINT16_MAX      __UINT16_MAX
#define UINT16_WIDTH    __UINT16_WIDTH
#define INT32_MIN       __INT32_MIN
#define INT32_MAX       __INT32_MAX
#define INT32_WIDTH     __INT32_WIDTH
#define UINT32_MAX      __UINT32_MAX
#define UINT32_WIDTH    __UINT32_WIDTH
#define INT64_MIN       __INT64_MIN
#define INT64_MAX       __INT64_MAX
#define INT64_WIDTH     __INT64_WIDTH
#define UINT64_MAX      __UINT64_MAX
#define UINT64_WIDTH    __UINT64_WIDTH

#define INT_LEAST8_MIN      __INT_LEAST8_MIN
#define INT_LEAST8_MAX      __INT_LEAST8_MAX
#define INT_LEAST8_WIDTH    __INT_LEAST8_WIDTH
#define UINT_LEAST8_MAX     __UINT_LEAST8_MAX
#define UINT_LEAST8_WIDTH   __UINT_LEAST8_WIDTH
#define INT_LEAST16_MIN     __INT_LEAST16_MIN
#define INT_LEAST16_MAX     __INT_LEAST16_MAX
#define INT_LEAST16_WIDTH   __INT_LEAST16_WIDTH
#define UINT_LEAST16_MAX    __UINT_LEAST16_MAX
#define UINT_LEAST16_WIDTH  __UINT_LEAST16_WIDTH
#define INT_LEAST32_MIN     __INT_LEAST32_MIN
#define INT_LEAST32_MAX     __INT_LEAST32_MAX
#define INT_LEAST32_WIDTH   __INT_LEAST32_WIDTH
#define UINT_LEAST32_MAX    __UINT_LEAST32_MAX
#define UINT_LEAST32_WIDTH  __UINT_LEAST32_WIDTH
#define INT_LEAST64_MIN     __INT_LEAST64_MIN
#define INT_LEAST64_MAX     __INT_LEAST64_MAX
#define INT_LEAST64_WIDTH   __INT_LEAST64_WIDTH
#define UINT_LEAST64_MAX    __UINT_LEAST64_MAX
#define UINT_LEAST64_WIDTH  __UINT_LEAST64_WIDTH

#define INT_FAST8_MIN       __INT_FAST8_MIN
#define INT_FAST8_MAX       __INT_FAST8_MAX
#define INT_FAST8_WIDTH     __INT_FAST8_WIDTH
#define UINT_FAST8_MAX      __UINT_FAST8_MAX
#define UINT_FAST8_WIDTH    __UINT_FAST8_WIDTH
#define INT_FAST16_MIN      __INT_FAST16_MIN
#define INT_FAST16_MAX      __INT_FAST16_MAX
#define INT_FAST16_WIDTH    __INT_FAST16_WIDTH
#define UINT_FAST16_MAX     __UINT_FAST16_MAX
#define UINT_FAST16_WIDTH   __UINT_FAST16_WIDTH
#define INT_FAST32_MIN      __INT_FAST32_MIN
#define INT_FAST32_MAX      __INT_FAST32_MAX
#define INT_FAST32_WIDTH    __INT_FAST32_WIDTH
#define UINT_FAST32_MAX     __UINT_FAST32_MAX
#define UINT_FAST32_WIDTH   __UINT_FAST32_WIDTH
#define INT_FAST64_MIN      __INT_FAST64_MIN
#define INT_FAST64_MAX      __INT_FAST64_MAX
#define INT_FAST64_WIDTH    __INT_FAST64_WIDTH
#define UINT_FAST64_MAX     __UINT_FAST64_MAX
#define UINT_FAST64_WIDTH   __UINT_FAST64_WIDTH

#define INTPTR_MIN          __INTPTR_MIN
#define INTPTR_MAX          __INTPTR_MAX
#define UINTPTR_MAX         __UINTPTR_MAX

#define INTMAX_MIN          __INTMAX_MIN
#define INTMAX_MAX          __INTMAX_MAX
#define INTMAX_WITH         __INTMAX_WIDTH
#define UINTMAX_MAX         __UINTMAX_MAX
#define UINTMAX_WIDTH       __UINTMAX_WIDTH

#define PTRDIFF_MIN         __PTRDIFF_MIN
#define PTRDIFF_MAX         __PTRDIFF_MAX
#define PTRDIFF_WIDTH       __PTRDIFF_WIDTH

#define SIZE_MAX            __SIZE_MAX
#define SIZE_WIDTH          __SIZE_WIDTH

#define WCHAR_MIN           __WCHAR_MIN
#define WCHAR_MAX           __WCHAR_MAX
#define WCHAR_WIDTH         __WCHAR_WIDTH

#define WINT_MIN            __WINT_MIN
#define WINT_MAX            __WINT_MAX
#define WINT_WIDTH          __WINT_WIDTH


#define INT8_C(x)           __INT8_C(x)
#define INT16_C(x)          __INT16_C(x)
#define INT32_C(x)          __INT32_C(x)
#define INT64_C(x)          __INT64_C(x)

#define UINT8_C(x)          __UINT8_C(x)
#define UINT16_C(x)         __UINT16_C(x)
#define UINT32_C(x)         __UINT32_C(x)
#define UINT64_C(x)         __UINT64_C(x)

#define INTMAX_C(x)         __INTMAX_C(x)
#define UINTMAX_C(x)        __UINTMAX_C(x)


typedef __int8_t    int8_t;
typedef __int16_t   int16_t;
typedef __int32_t   int32_t;
typedef __int64_t   int64_t;

typedef __uint8_t   uint8_t;
typedef __uint16_t  uint16_t;
typedef __uint32_t  uint32_t;
typedef __uint64_t  uint64_t;

typedef __int_least8_t      int_least8_t;
typedef __int_least16_t     int_least16_t;
typedef __int_least32_t     int_least32_t;
typedef __int_least64_t     int_least64_t;

typedef __uint_least8_t     uint_least8_t;
typedef __uint_least16_t    uint_least16_t;
typedef __uint_least32_t    uint_least32_t;
typedef __uint_least64_t    uint_least64_t;

typedef __int_fast8_t       int_fast8_t;
typedef __int_fast16_t      int_fast16_t;
typedef __int_fast32_t      int_fast32_t;
typedef __int_fast64_t      int_fast64_t;

typedef __uint_fast8_t      uint_fast8_t;
typedef __uint_fast16_t     uint_fast16_t;
typedef __uint_fast32_t     uint_fast32_t;
typedef __uint_fast64_t     uint_fast64_t;

typedef __intptr_t  intptr_t;
typedef __uintptr_t uintptr_t;

typedef __intmax_t  intmax_t;
typedef __uintmax_t uintmax_t;


#endif /* ifndef __SYSTEM_C_LANGUAGESUPPORT_STDINT_H */
