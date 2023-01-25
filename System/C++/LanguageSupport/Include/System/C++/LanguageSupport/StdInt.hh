#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDINT_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_STDINT_H


#include <System/C/BaseHeaders/Limits.h>
#include <System/C/BaseHeaders/Types.h>

#include <System/C++/LanguageSupport/Private/Namespace.hh>


#if !defined(INT8_MIN)
#define INT8_MIN    __INT8_MIN
#define INT8_MAX    __INT8_MAX
#define UINT8_MAX   __UINT8_MAX
#define INT16_MIN   __INT16_MIN
#define INT16_MAX   __INT16_MAX
#define UINT16_MAX  __UINT16_MAX
#define INT32_MIN   __INT32_MIN
#define INT32_MAX   __INT32_MAX
#define UINT32_MAX  __UINT32_MAX
#define INT64_MIN   __INT64_MIN
#define INT64_MAX   __INT64_MAX
#define UINT64_MAX  __UINT64_MAX

#define INT_LEAST8_MIN      __INT_LEAST8_MIN
#define INT_LEAST8_MAX      __INT_LEAST8_MAX
#define UINT_LEAST8_MAX     __UINT_LEAST8_MAX
#define INT_LEAST16_MIN     __INT_LEAST16_MIN
#define INT_LEAST16_MAX     __INT_LEAST16_MAX
#define UINT_LEAST16_MAX    __UINT_LEAST16_MAX
#define INT_LEAST32_MIN     __INT_LEAST32_MIN
#define INT_LEAST32_MAX     __INT_LEAST32_MAX
#define UINT_LEAST32_MAX    __UINT_LEAST32_MAX
#define INT_LEAST64_MIN     __INT_LEAST64_MIN
#define INT_LEAST64_MAX     __INT_LEAST64_MAX
#define UINT_LEAST64_MAX    __UINT_LEAST64_MAX

#define INTPTR_MIN          __INTPTR_MIN
#define INTPTR_MAX          __INTPTR_MAX
#define UINTPTR_MAX         __UINTPTR_MAX

#define INTMAX_MIN          __INTMAX_MIN
#define INTMAX_MAX          __INTMAX_MAX
#define UINTMAX_MAX         __UINTMAX_MAX

#define PTRDIFF_MIN         __PTRDIFF_MIN
#define PTRDIFF_MAX         __PTRDIFF_MAX
#define SIZE_MAX            __SIZE_MAX

#define WCHAR_MIN           __WCHAR_MIN
#define WCHAR_MAX           __WCHAR_MAX

#define WINT_MIN            __WINT_MIN
#define WINT_MAX            __WINT_MAX
#endif // if !defined(INT8_MIN)


namespace __XVI_STD_LANGSUPPORT_NS
{


using int8_t = __int8_t;
using int16_t = __int16_t;
using int32_t = __int32_t;
using int64_t = __int64_t;

using uint8_t = __uint8_t;
using uint16_t = __uint16_t;
using uint32_t = __uint32_t;
using uint64_t = __uint64_t;

using int_least8_t = __int_least8_t;
using int_least16_t = __int_least16_t;
using int_least32_t = __int_least32_t;
using int_least64_t = __int_least64_t;

using uint_least8_t = __uint_least8_t;
using uint_least16_t = __uint_least16_t;
using uint_least32_t = __uint_least32_t;
using uint_least64_t = __uint_least64_t;

using int_fast8_t = __int_fast8_t;
using int_fast16_t = __int_fast16_t;
using int_fast32_t = __int_fast32_t;
using int_fast64_t = __int_fast64_t;

using uint_fast8_t = __uint_fast8_t;
using uint_fast16_t = __uint_fast16_t;
using uint_fast32_t = __uint_fast32_t;
using uint_fast64_t = __uint_fast64_t;

using intptr_t = __intptr_t;
using uintptr_t = __uintptr_t;

using intmax_t = __intmax_t;
using uintmax_t = __uintmax_t;


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDINT_H */
