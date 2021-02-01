#pragma once
#ifndef __SYSTEM_CXX_LOCALE_PRIVATE_NAMESPACE_H
#define __SYSTEM_CXX_LOCALE_PRIVATE_NAMESPACE_H



#if !defined(__XVI_STD_LOCALE_NS)
#  if defined (__XVI_STD_NS)
#   define __XVI_STD_LOCALE_NS          __XVI_STD_NS
#  else
#    define __XVI_STD_NS                std/*::inline __xvi_cxx_v0*/
#    define __XVI_STD_LOCALE_NS         __XVI_STD_NS
#  endif
#endif

#if !defined(__XVI_STD_IOS_NS)
#  define __XVI_STD_IOS_NS              __XVI_STD_NS
#endif

#if !defined(__XVI_STD_STRING_NS)
#  define __XVI_STD_STRING_NS           __XVI_STD_NS
#endif

#if !defined(__XVI_STD_CONTAINERS_NS)
#  define __XVI_STD_CONTAINERS_NS       __XVI_STD_NS
#endif




#endif /* ifndef __SYSTEM_CXX_LOCALE_PRIVATE_NAMESPACE_H */
