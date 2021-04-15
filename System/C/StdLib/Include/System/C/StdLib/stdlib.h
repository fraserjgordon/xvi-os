#ifndef __SYSTEM_C_STDLIB_STDLIB_H
#define __SYSTEM_C_STDLIB_STDLIB_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Limits.h>
#include <System/C/BaseHeaders/Null.h>
#include <System/C/LanguageSupport/StdDef.h>

#include <System/C/StdLib/Private/AnnexK.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define EXIT_FAILURE    -1
#define EXIT_SUCCESS    0

#define RAND_MAX        __INT_MAX

#define MB_CUR_MAX      (__cstdlib_get_mb_cur_max())


typedef struct div_t
{
    int quot;
    int rem;
} div_t;

typedef struct ldiv_t
{
    long quot;
    long rem;
} ldiv_t;

typedef struct lldiv_t
{
    long long quot;
    long long rem;
} lldiv_t;


__SYSTEM_C_STDLIB_EXPORT size_t __cstdlib_get_mb_cur_max(void);

__SYSTEM_C_STDLIB_EXPORT double atof(const char*);

__SYSTEM_C_STDLIB_EXPORT int atoi(const char*);
__SYSTEM_C_STDLIB_EXPORT long atol(const char*);
__SYSTEM_C_STDLIB_EXPORT long long atoll(const char*);

__SYSTEM_C_STDLIB_EXPORT int strfromd(char* __restrict, size_t, const char* __restrict, double);
__SYSTEM_C_STDLIB_EXPORT int strfromf(char* __restrict, size_t, const char* __restrict, float);
__SYSTEM_C_STDLIB_EXPORT int strfroml(char* __restrict, size_t, const char* __restrict, long double);

#ifdef __STDC_IEC_60559_DFP__
__SYSTEM_C_STDLIB_EXPORT int strfromd32(char* __restrict, size_t, const char* __restrict, _Decimal32);
__SYSTEM_C_STDLIB_EXPORT int strfromd64(char* __restrict, size_t, const char* __restrict, _Decimal64);
__SYSTEM_C_STDLIB_EXPORT int strfromd128(char* __restrict, size_t, const char* __restrict, _Decimal128);
#endif

__SYSTEM_C_STDLIB_EXPORT double strtod(const char* __restrict, char** __restrict);
__SYSTEM_C_STDLIB_EXPORT float strtof(const char* __restrict, char** __restrict);
__SYSTEM_C_STDLIB_EXPORT long double strtold(const char* __restrict, char** __restrict);

#ifdef __STDC_IEC_60559_DFP__
__SYSTEM_C_STDLIB_EXPORT _Decimal32 strtod32(const char* __restrict, char** __restrict);
__SYSTEM_C_STDLIB_EXPORT _Decimal64 strtod64(const char* __restrict, char** __restrict);
__SYSTEM_C_STDLIB_EXPORT _Decimal128 strtod128(const char* __restrict, char** __restrict);
#endif

__SYSTEM_C_STDLIB_EXPORT long strtol(const char* __restrict, char** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT long long strtoll(const char* __restrict, char** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT unsigned long strtoul(const char* __restrict, char** __restrict, int);
__SYSTEM_C_STDLIB_EXPORT unsigned long long strtoull(const char* __restrict, char** __restrict, int);

__SYSTEM_C_STDLIB_EXPORT int rand(void);
__SYSTEM_C_STDLIB_EXPORT void srand(unsigned int);

__SYSTEM_C_STDLIB_EXPORT void* aligned_alloc(size_t, size_t);
__SYSTEM_C_STDLIB_EXPORT void* calloc(size_t, size_t);
__SYSTEM_C_STDLIB_EXPORT void free(void*);
__SYSTEM_C_STDLIB_EXPORT void* malloc(size_t);
__SYSTEM_C_STDLIB_EXPORT void* realloc(void*, size_t);

__SYSTEM_C_STDLIB_EXPORT __noreturn void abort(void);
__SYSTEM_C_STDLIB_EXPORT int atexit(void (*)(void));
__SYSTEM_C_STDLIB_EXPORT int at_quick_exit(void (*)(void));
__SYSTEM_C_STDLIB_EXPORT __noreturn void exit(int);
__SYSTEM_C_STDLIB_EXPORT __noreturn void _Exit(int);
__SYSTEM_C_STDLIB_EXPORT char* getenv(const char*);
__SYSTEM_C_STDLIB_EXPORT __noreturn void quick_exit(int);
__SYSTEM_C_STDLIB_EXPORT int system(const char*);

__SYSTEM_C_STDLIB_EXPORT void* bsearch(const void*, const void*, size_t, size_t, int (*)(const void*, const void*));
__SYSTEM_C_STDLIB_EXPORT void qsort(void*, size_t, size_t, int (*)(const void*, const void*));

__SYSTEM_C_STDLIB_EXPORT int abs(int);
__SYSTEM_C_STDLIB_EXPORT long labs(long);
__SYSTEM_C_STDLIB_EXPORT long long llabs(long long);

__SYSTEM_C_STDLIB_EXPORT div_t div(int, int);
__SYSTEM_C_STDLIB_EXPORT ldiv_t ldiv(long, long);
__SYSTEM_C_STDLIB_EXPORT lldiv_t lldiv(long long, long long);

__SYSTEM_C_STDLIB_EXPORT int mblen(const char*, size_t);
__SYSTEM_C_STDLIB_EXPORT int mbtowc(wchar_t* __restrict, const char* __restrict, size_t);
__SYSTEM_C_STDLIB_EXPORT int wctomb(char*, wchar_t);
__SYSTEM_C_STDLIB_EXPORT size_t mbstowcs(wchar_t* __restrict, const char* __restrict, size_t);
__SYSTEM_C_STDLIB_EXPORT size_t wcstombs(char* __restrict, const wchar_t* __restrict, size_t);


#if __SYSTEM_C_STDLIB_ANNEXK_ENABLED


typedef void (*constraint_handler_t)(const char* __restrict, void* __restrict, errno_t);


__SYSTEM_C_STDLIB_EXPORT constraint_handler_t set_constraint_handler_s(constraint_handler_t);
__SYSTEM_C_STDLIB_EXPORT void abort_handler_s(const char* __restrict, void* __restrict, errno_t);
__SYSTEM_C_STDLIB_EXPORT void ignore_handler_s(const char* __restrict, void* __restrict, errno_t);

__SYSTEM_C_STDLIB_EXPORT errno_t getenv_s(size_t* __restrict, char* __restrict, rsize_t, const char* __restrict);

__SYSTEM_C_STDLIB_EXPORT void* bsearch_s(const void*, const void*, rsize_t, rsize_t, int (*)(const void*, const void*, void*), void*);
__SYSTEM_C_STDLIB_EXPORT errno_t qsort_s(void*, rsize_t, rsize_t, int (*)(const void*, const void*, void*), void*);

__SYSTEM_C_STDLIB_EXPORT errno_t wctomb_s(int* __restrict, char* __restrict, rsize_t, wchar_t);
__SYSTEM_C_STDLIB_EXPORT errno_t mbstowcs_s(size_t* __restrict, wchar_t* __restrict, rsize_t, const char* __restrict, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t wcstombs_s(size_t* __restrict, char* __restrict, rsize_t, const wchar_t* __restrict, rsize_t);


#endif // __SYSTEM_C_STDLIB_ANNEXK_ENABLED


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_STDLIB_H */
