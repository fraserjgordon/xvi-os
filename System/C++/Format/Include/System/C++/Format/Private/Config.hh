#pragma once
#ifndef __SYSTEM_CXX_FORMAT_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_FORMAT_PRIVATE_CONFIG_H


#include <System/C++/Format/Private/Namespace.hh>


#if __BUILD_SYSTEM_CXX_FORMAT_SHARED
#  define __SYSTEM_CXX_FORMAT_EXPORT    [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_CXX_FORMAT_EXPORT    /**/
#endif


#endif /* ifndef __SYSTEM_CXX_LOCALE_PRIVATE_CONFIG_H */
