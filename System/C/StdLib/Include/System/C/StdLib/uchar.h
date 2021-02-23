#ifndef __SYSTEM_C_STDLIB_UCHAR_H
#define __SYSTEM_C_STDLIB_UCHAR_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Types.h>

#include <System/C/StdLib/MBStateT.h>


#ifdef __cplusplus
extern "C"
{
#endif


typedef __size_t size_t;

#ifndef __cplusplus
typedef __char16_t  char16_t;
typedef __char32_t  char32_t;
#endif


__SYSTEM_C_STDLIB_EXPORT size_t mbrtoc16(char16_t* __restrict, const char* __restrict, size_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t c16rtomb(char* __restrict, const char16_t* __restrict, size_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t mbrtoc32(char32_t* __restrict, const char* __restrict, size_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t c32rtomb(char* __restrict, const char32_t* __restrict, size_t, mbstate_t* __restrict);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_UCHAR_H */
