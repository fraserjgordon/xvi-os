#ifndef __SYSTEM_CXX_COROUTINE_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_COROUTINE_PRIVATE_CONFIG_H


#include <System/C++/Coroutine/Private/Namespace.hh>


#if __BUILD_SYSTEM_CXX_COROUTINE_SHARED
#  define __SYSTEM_CXX_COROUTINE_EXPORT    [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_CXX_COROUTINE_EXPORT    /**/
#endif


#endif /* ifndef __SYSTEM_CXX_COROUTINE_PRIVATE_CONFIG_H */
