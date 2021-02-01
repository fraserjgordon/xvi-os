#pragma once
#ifndef __SYSTEM_CXX_LOCALE_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_LOCALE_PRIVATE_CONFIG_H


#include <System/C++/Locale/Private/Namespace.hh>


#if __BUILD_SYSTEM_CXX_LOCALE_SHARED
#  define __SYSTEM_CXX_LOCALE_EXPORT    [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_CXX_LOCALE_EXPORT    /**/
#endif


#endif /* ifndef __SYSTEM_CXX_LOCALE_PRIVATE_CONFIG_H */
