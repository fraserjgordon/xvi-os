#pragma once
#ifndef __SYSTEM_CXX_LOCALE_STDLIB_H
#define __SYSTEM_CXX_LOCALE_STDLIB_H


#include <System/C++/Locale/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>


namespace __XVI_STD_LOCALE_NS
{


__SYSTEM_CXX_LOCALE_EXPORT int mblen(const char* __s, size_t __n);
__SYSTEM_CXX_LOCALE_EXPORT int mbtowc(wchar_t* __pwc, const char* __s, size_t __n);
__SYSTEM_CXX_LOCALE_EXPORT int wctomb(char* __s, wchar_t __wc);
__SYSTEM_CXX_LOCALE_EXPORT size_t mbstowcs(wchar_t* __pwcs, const char* __s, size_t __n);
__SYSTEM_CXX_LOCALE_EXPORT size_t wcstombs(char* __s, const wchar_t* __pwcs, size_t __n);


} // namspace __XVI_STD_LOCALE_NS


#endif /* ifndef __SYSTEM_CXX_LOCALE_STDLIB_H */
