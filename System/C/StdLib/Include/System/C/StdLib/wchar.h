#ifndef __SYSTEM_C_STDLIB_WCHAR_H
#define __SYSTEM_C_STDLIB_WCHAR_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Limits.h>
#include <System/C/BaseHeaders/Null.h>
#include <System/C/BaseHeaders/Types.h>
#include <System/C/BaseHeaders/VarArgs.h>

#include <System/C/StdLib/MBStateT.h>
#include <System/C/StdLib/Private/AnnexK.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define WCHAR_MIN   __WCHAR_MIN
#define WCHAR_MAX   __WCHAR_MAX
#define WCHAR_WIDTH __WCHAR_WIDTH

#define WEOF        ((wint_t)-1)


typedef __size_t size_t;
typedef __wint_t wint_t;

#ifndef __cplusplus
typedef __wchar_t wchar_t;
#endif

struct FILE;
struct tm;


__SYSTEM_C_STDLIB_EXPORT int fwprintf(FILE* __restrict, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int fwscanf(FILE* __restrict, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int swprintf(wchar_t* __restrict, size_t, const wchar_t*, ...);
__SYSTEM_C_STDLIB_EXPORT int swscanf(const wchar_t* __restrict, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int vfwprintf(FILE* __restrict, const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vfwscanf(FILE* __restrict, const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vswprintf(wchar_t* __restrict, size_t, const wchar_t* __restric, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vswscanf(const wchar_t* __restrict, const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vwprintf(const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vwscanf(const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int wprintf(const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int wscanf(const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT wint_t fgetwc(FILE*);
__SYSTEM_C_STDLIB_EXPORT wchar_t* fgetws(wchar_t* __restrict, int, FILE* __restrict);
__SYSTEM_C_STDLIB_EXPORT wint_t fputwc(wchar_t, FILE*);
__SYSTEM_C_STDLIB_EXPORT int fputws(const wchar_t* __restrict, FILE* __restrict);
__SYSTEM_C_STDLIB_EXPORT int fwide(FILE*, int);
__SYSTEM_C_STDLIB_EXPORT wint_t getwc(FILE*);
__SYSTEM_C_STDLIB_EXPORT wint_t getwchar(void);
__SYSTEM_C_STDLIB_EXPORT wint_t putwc(wchar_t, FILE*);
__SYSTEM_C_STDLIB_EXPORT wint_t putwchar(wchar_t);
__SYSTEM_C_STDLIB_EXPORT wint_t ungetwc(wint_t, FILE*);

__SYSTEM_C_STDLIB_EXPORT double wcstod(const wchar_t* __restrict, wchar_t** __restrict);
__SYSTEM_C_STDLIB_EXPORT float wcstof(const wchar_t* __restrict, wchar_t** __restrict);
__SYSTEM_C_STDLIB_EXPORT long double wcstold(const wchar_t* __restrict, wchar_t** __restrict);

#ifdef __STDC_IEC_60559_DFP__
__SYSTEM_C_STDLIB_EXPORT _Decimal32 wcstod32(const wchar_t* __restrict, wchar_t** __restrict);
__SYSTEM_C_STDLIB_EXPORT _Decimal64 wcstod64(const wchar_t* __restrict, wchar_t** __restrict);
__SYSTEM_C_STDLIB_EXPORT _Decimal128 wcstod128(const wchar_t* __restrict, wchar_t** __restrict);
#endif

__SYSTEM_C_STDLIB_EXPORT long int wcstol(const wchar_t* __restrict, wchar_t** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT long long int wcstoll(const wchar_t* __restrict, wchar_t** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT unsigned long int wcstoul(const wchar_t* __restrict, wchar_t** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT unsigned long long int wcstoull(const wchar_t* __restrict, wchar_t** __restrict, int);

__SYSTEM_C_STDLIB_EXPORT wchar_t* wcscpy(wchar_t* __restrict, const wchar_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcsncpy(wchar_t* __restrict, const wchar_t* __restrict, size_t n);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wmemcpy(wchar_t* __restrict, const wchar_t* __restrict, size_t n);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wmemmove(wchar_t*, const wchar_t*, size_t);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcscat(wchar_t* __restrict, const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcsncat(wchar_t* __restrict, const wchar_t*, size_t);
__SYSTEM_C_STDLIB_EXPORT int wcscmp(const wchar_t*, const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT int wcscoll(const wchar_t*, const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT int wcsncmp(const wchar_t*, const wchar_t*, int);
__SYSTEM_C_STDLIB_EXPORT size_t wcsxfrm(wchar_t* __restrict, const wchar_t* __restrict, size_t);
__SYSTEM_C_STDLIB_EXPORT int wmemcmp(const wchar_t*, const wchar_t*, size_t);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcschr(const wchar_t*, wchar_t);
__SYSTEM_C_STDLIB_EXPORT size_t wcscspn(const wchar_t*, const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcspbrk(const wchar_t*, const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcsrchr(const wchar_t*, wchar_t);
__SYSTEM_C_STDLIB_EXPORT size_t wcsspn(const wchar_t*, const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcsstr(const wchar_t*, const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wcstok(wchar_t* __restrict, const wchar_t* __restrict, wchar_t** __restrict);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wmemchr(const wchar_t*, wchar_t, size_t);
__SYSTEM_C_STDLIB_EXPORT size_t wcslen(const wchar_t*);
__SYSTEM_C_STDLIB_EXPORT wchar_t* wmemset(wchar_t*, wchar_t, size_t);

__SYSTEM_C_STDLIB_EXPORT size_t wcsftime(wchar_t* __restrict, size_t, const wchar_t* __restrict, const struct tm* __restrict);

__SYSTEM_C_STDLIB_EXPORT wint_t btowc(int);
__SYSTEM_C_STDLIB_EXPORT int wctob(wint_t);
__SYSTEM_C_STDLIB_EXPORT int mbsinit(const mbstate_t*);
__SYSTEM_C_STDLIB_EXPORT size_t mbrlen(const char* __restrict, size_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t mbrtowc(wchar_t* __restrict, const char* __restrict, size_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t wcrtomb(char* __restrict, wchar_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t mbsrtowcs(wchar_t* __restrict, const char** __restrict, size_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t wcsrtombs(char* __restrict, const wchar_t** __restrict, size_t, mbstate_t* __restrict);


#if __SYSTEM_C_STDLIB_ANNEXK_ENABLED


__SYSTEM_C_STDLIB_EXPORT int fwprintf_s(FILE* __restrict, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int fwscanf_s(FILE* __restrict, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int snwprintf_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int swprintf_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int swscanf_s(const wchar_t* __restrict, const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int vfwprintf_s(FILE* __restrict, const wchar_t* __restrict, __varargs_va-list);
__SYSTEM_C_STDLIB_EXPORT int vfwscanf_s(FILE* __restrict, const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vsnwprintf_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vwsprintf_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vswscanf_s(const wchar_t* __restrict, const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vwprintf_s(const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vwscanf_s(const wchar_t* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int wprintf_s(const wchar_t* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int wscanf_s(const wchar_t* __restrict, ...);

__SYSTEM_C_STDLIB_EXPORT errno_t wcscpy_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t wcsncpy_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t wmemcpy_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t wmemmove_s(wchar_t*, rsize_t, const wchar_t*, rsize);
__SYSTEM_C_STDLIB_EXPORT errno_t wcscat_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t wcsncat_s(wchar_t* __restrict, rsize_t, const wchar_t* __restrict, rsize_t);
__SYSTEM_C_STDLIB_EXPORT whar_t* wcstok_s(wchar_t* __restrict, rise_t* __restrict, const wchar_t* __restrict, wchar_t** __restrict);
__SYSTEM_C_STDLIB_EXPORT size_t wcsnlen_s(const wchar_t*, size_t);

__SYSTEM_C_STDLIB_EXPORT errno_t wcrtomb_s(size_t* __restrict, char* __restrict, rsize_t, wchar_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t mbsrtowcs_s(size_t* __restrict, wchar_t* __restrict, rsize_t, const char** __restrict, rsize_t, mbstate_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t wcsrtombs_s(size_t* __restrict, char* __restrict, rsize_t, const wchar_t** __restrict, rsize_t, mbstate_t* __restrict);


#endif // __SYSTEM_C_STDLIB_ANNEXK_ENABLED


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_WCHAR_H */
