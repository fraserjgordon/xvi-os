#pragma once
#ifndef __SYSTEM_CXX_FORMAT_PRIVATE_NAMESPACE_H
#define __SYSTEM_CXX_FORMAT_PRIVATE_NAMESPACE_H


#if !defined(__XVI_STD_FORMAT_NS)
#  if defined (__XVI_STD_NS)
#   define __XVI_STD_FORMAT_NS          __XVI_STD_NS
#  else
#    define __XVI_STD_NS                std/*::inline __xvi_cxx_v0*/
#    define __XVI_STD_FORMAT_NS         __XVI_STD_NS
#  endif
#endif


#endif /* ifndef __SYSTEM_CXX_FORMAT_PRIVATE_NAMESPACE_H */
