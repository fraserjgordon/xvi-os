#ifndef __SYSTEM_CXX_LOCALE_CSTRING_H
#ifndef __SYSTEM_CXX_LOCALE_CSTRING_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/C++/Locale/Private/Config.hh>


namespace __XVI_STD_LOCALE_NS
{


__SYSTEM_CXX_LOCALE_EXPORT std::size_t strxfrm(char * __restrict __s1, const char * __restrict __s2, std::size_t __n);

__SYSTEM_CXX_LOCALE_EXPORT char* strerror(int __errnum);


} // namespace __XVI_STD_LOCALE_NS


#endif /* ifndef __SYSTEM_CXX_LOCALE_CSTRING_H */
