#pragma once
#ifndef __SYSTEM_CXX_LOCALE_CTYPE_H
#define __SYSTEM_CXX_LOCALE_CTYPE_H


#include <System/C++/Locale/Private/Config.hh>


namespace __XVI_STD_LOCALE_NS
{


__SYSTEM_CXX_LOCALE_EXPORT int isalnum(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isalpha(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isblank(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int iscntrl(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isdigit(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isgraph(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int islower(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isprint(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int ispunct(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isspace(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isupper(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int isxdigit(int __c);

__SYSTEM_CXX_LOCALE_EXPORT int tolower(int __c);
__SYSTEM_CXX_LOCALE_EXPORT int toupper(int __c);


} // namespace __XVI_STD_LOCALE_NS


#endif /* ifndef __SYSTEM_CXX_LOCALE_CTYPE_H */
