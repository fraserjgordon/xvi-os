#ifndef __SYSTEM_CXX_LOCALE_UCHAR_H
#define __SYSTEM_CXX_LOCALE_UCHAR_H


#include <System/C++/Locale/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/MBState.hh>


#define EOF     __EOF
#define WEOF    __WEOF


namespace __XVI_STD_LOCALE_NS
{


using wint_t = __wint_t;


__SYSTEM_CXX_LOCALE_EXPORT int mbsinit(const mbstate_t*);
__SYSTEM_CXX_LOCALE_EXPORT wint_t btowc(int);
__SYSTEM_CXX_LOCALE_EXPORT int wctob(wint_t);
__SYSTEM_CXX_LOCALE_EXPORT size_t mbrlen(const char * __restrict, size_t, mbstate_t * __restrict);
__SYSTEM_CXX_LOCALE_EXPORT size_t mbrtowc(wchar_t * __restrict, const char * __restrict, size_t, mbstate_t * __restrict);
__SYSTEM_CXX_LOCALE_EXPORT size_t wcrtomb(char * __restrict, wchar_t, mbstate_t * __restrict);
__SYSTEM_CXX_LOCALE_EXPORT size_t mbsrtowcs(wchar_t * __restrict, const char ** __restrict, size_t, mbstate_t * __restrict);
__SYSTEM_CXX_LOCALE_EXPORT size_t wcsrtombs(char * __restrict, const wchar_t ** __restrict, size_t, mbstate_t * __restrict);


} // namespace __XVI_STD_LOCALE_NS


#endif /* ifndef __SYSTEM_CXX_LOCALE_UCHAR_H */
