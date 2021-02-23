#ifndef __SYSTEM_C_STDLIB_TIME_H
#define __SYSTEM_C_STDLIB_TIME_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Limits.h>
#include <System/C/BaseHeaders/Null.h>
#include <System/C/LanguageSupport/StdDef.h>

#include <System/C/StdLib/Private/AnnexK.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define CLOCKS_PER_SEC (__get_clocks_per_sec())

#define TIME_UTC    1


typedef __int64_t   clock_t;
typedef __int64_t   time_t;

struct timespec
{
    time_t  tv_sec;
    long    tv_nanosec;
};

struct tm
{
    int     tm_sec;
    int     tm_min;
    int     tm_hour;
    int     tm_mday;
    int     tm_mon;
    int     tm_year;
    int     tm_wday;
    int     tm_yday;
    int     tm_isdst;
};


__SYSTEM_C_STDLIB_EXPORT clock_t __cstdlib_get_clocks_per_sec(void);

__SYSTEM_C_STDLIB_EXPORT clock_t clock(void);

__SYSTEM_C_STDLIB_EXPORT double difftime(time_t, time_t);
__SYSTEM_C_STDLIB_EXPORT time_t mktime(struct tm*);
__SYSTEM_C_STDLIB_EXPORT time_t time(time_t*);
__SYSTEM_C_STDLIB_EXPORT int timespec_get(struct timespec*, int);
__SYSTEM_C_STDLIB_EXPORT int timespec_getres(struct timespec*, int);
__SYSTEM_C_STDLIB_EXPORT char* asctime(const struct tm*);
__SYSTEM_C_STDLIB_EXPORT char* asctime_r(const struct tm*, char*);
__SYSTEM_C_STDLIB_EXPORT char* ctime(const time_t*);
__SYSTEM_C_STDLIB_EXPORT char* ctime_r(const time_t*, char*);
__SYSTEM_C_STDLIB_EXPORT struct tm* gmtime(const time_t*);
__SYSTEM_C_STDLIB_EXPORT struct tm* gmtime_r(const time_t*, struct tm*);
__SYSTEM_C_STDLIB_EXPORT struct tm* localtime(const time_t*);
__SYSTEM_C_STDLIB_EXPORT struct tm* localtime_r(const time_t*, struct tm*);
__SYSTEM_C_STDLIB_EXPORT size_t strftime(char* __restrict, size_t, const char* __restrict, const struct tm* __restrict);


#if __SYSTEM_C_STDLIB_ANNEXK_ENABLED


__SYSTEM_C_STDLIB_EXPORT errno_t asctime_s(char*, rsize_t, const struct tm*);
__SYSTEM_C_STDLIB_EXPORT errno_t ctime_s(char*, rsize_t, const time_t*);
__SYSTEM_C_STDLIB_EXPORT struct tm* gmtime_s(const time_t* __restrict, struct tm* __restrict);
__SYSTEM_C_STDLIB_EXPORT struct tm* localtime_s(const time_t* __restrict, struct tm* __restrict);


#endif // __SYSTEM_C_STDLIB_ANNEXK_ENABLED


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_TIME_H */
