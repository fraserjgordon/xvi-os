#ifndef __SYSTEM_CXX_LOCALE_UCHAR_H
#define __SYSTEM_CXX_LOCALE_UCHAR_H


#include <System/C++/Locale/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/MBState.hh>


namespace __XVI_STD_LOCALE_NS
{


__SYSTEM_CXX_LOCALE_EXPORT size_t mbrtoc8(char8_t* __pc8, const char* __s, size_t __n, mbstate_t* __ps);
__SYSTEM_CXX_LOCALE_EXPORT size_t c8rtomb(char* __s, const char8_t* __pc8, mbstate_t* __ps);
__SYSTEM_CXX_LOCALE_EXPORT size_t mbrtoc16(char16_t* __pc16, const char* __s, size_t __n, mbstate_t* __ps);
__SYSTEM_CXX_LOCALE_EXPORT size_t c16rtomb(char* __s, const char16_t* __pc16, mbstate_t* __ps);
__SYSTEM_CXX_LOCALE_EXPORT size_t mbrtoc32(char32_t* __pc32, const char* __s, size_t __n, mbstate_t* __ps);
__SYSTEM_CXX_LOCALE_EXPORT size_t c32rtomb(char* __s, const char32_t* __pc32, mbstate_t* __ps);


} // namespace __XVI_STD_LOCALE_NS


#endif /* ifndef __SYSTEM_CXX_LOCALE_UCHAR_H */
