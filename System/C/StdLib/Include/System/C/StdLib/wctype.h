#ifndef __SYSTEM_C_STDLIB_WCTYPE_H
#define __SYSTEM_C_STDLIB_WCTYPE_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Types.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define WEOF    ((wint_t)-1)


typedef __wint_t wint_t;

typedef __uint32_t  wctrans_t;
typedef __uint32_t  wctype_t;


__SYSTEM_C_STDLIB_EXPORT int iswalnum(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswalpha(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswblank(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswcntrl(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswdigit(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswgraph(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswlower(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswprint(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswpunct(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswspace(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswupper(wint_t);
__SYSTEM_C_STDLIB_EXPORT int iswxdigit(wint_t);

__SYSTEM_C_STDLIB_EXPORT int iswctype(wint_t, wctype_t);
__SYSTEM_C_STDLIB_EXPORT wctype_t wctype(const char*);

__SYSTEM_C_STDLIB_EXPORT wint_t towlower(wint_t);
__SYSTEM_C_STDLIB_EXPORT wint_t towupper(wint_t);

__SYSTEM_C_STDLIB_EXPORT wint_t towctrans(wint_t, wctrans_t);
__SYSTEM_C_STDLIB_EXPORT wctrans_t wctrans(const char*);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_WCTYPE_H */
