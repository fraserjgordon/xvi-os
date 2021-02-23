#ifndef __SYSTEM_C_STDLIB_INTTYPES_H
#define __SYSTEM_C_STDLIB_INTTYPES_H


#include <System/C/StdLib/Private/Config.hh>

#include <System/C/LanguageSupport/IntTypes.h>


#ifdef __cplusplus
extern "C"
{
#endif


__SYSTEM_C_STDLIB_EXPORT intmax_t imaxabs(intmax_t);
__SYSTEM_C_STDLIB_EXPORT imaxdiv_t imaxdiv(intmax_t, intmax_t);

__SYSTEM_C_STDLIB_EXPORT intmax_t strtoimax(const char* __restrict, char** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT uintmax_t strtoumax(const char* __restrict, char** __restrict, int);

__SYSTEM_C_STDLIB_EXPORT intmax_t wcstoimax(const __wchar_t* __restrict, __wchar_t** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT uintmax_t wcstoumax(const __wchar_t* __restrict, __wchar_t** __restrict, int);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_INTTYPES_H */
