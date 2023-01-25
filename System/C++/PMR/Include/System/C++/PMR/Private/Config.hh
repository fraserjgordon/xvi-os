#ifndef __SYSTEM_CXX_PMR_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_PMR_PRIVATE_CONFIG_H


#include <System/C++/PMR/Private/Namespace.hh>


#if defined(__XVI_CXX_PMR_BUILD_SHARED)
#  define __XVI_CXX_PMR_EXPORT                          [[gnu::visibility("protected")]]
#elif defined(__XVI_CXX_PMR_BUILD_STATIC)
#  define __XVI_CXX_PMR_EXPORT                          /**/
#else
#  define __XVI_CXX_PMR_EXPORT                          /**/
#endif

#define __XVI_CXX_PMR_SYMBOL(x)                         asm(#x)


#endif /* ifndef __SYSTEM_CXX_PMR_PRIVATE_CONFIG_H */
