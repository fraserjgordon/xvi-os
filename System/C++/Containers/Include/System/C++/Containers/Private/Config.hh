#pragma once
#ifndef __SYSTEM_CXX_CONTAINERS_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_CONTAINERS_PRIVATE_CONFIG_H


#include <System/C++/Containers/Private/Namespace.hh>


#if __BUILD_SYSTEM_CXX_CONTAINERS_SHARED
#  define __SYSTEM_CXX_CONTAINERS_EXPORT    [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_CXX_CONTAINERS_EXPORT    /**/
#endif


#endif /* ifndef __SYSTEM_CXX_LOCALE_PRIVATE_CONFIG_H */
