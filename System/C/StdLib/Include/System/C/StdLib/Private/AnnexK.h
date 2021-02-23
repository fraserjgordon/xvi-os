#ifndef __SYSTEM_C_STDLIB_PRIVATE_ANNEXK_H
#define __SYSTEM_C_STDLIB_PRIVATE_ANNEXK_H


#include <System/C/BaseHeaders/Types.h>


#if !defined(__STDC_WANT_LIB_EXT1__)
#  define __STDC_WANT_LIB_EXT1__    1
#endif


#if defined(__SYSTEM_C_STDLIB_ANNEXK_ENABLED)
#  if __SYSTEM_C_STDLIB_ANNEXK_ENABLED != __STDC_WANT_LIB_EXT1__
#    error inconsistent value for __STDC_WANT_LIB_EXT1__
#  endif
#else
#  if __STDC_WANT_LIB_EXT1__
#    define __SYSTEM_C_STDLIB_ANNEXK_ENABLED    1
#  else
#    define __SYSTEM_C_STDLIB_ANNEXK_ENABLED    0
#  endif
#endif


typedef int         errno_t;
typedef __size_t    rsize_t;


#endif /* ifndef __SYSTEM_C_STDLIB_PRIVATE_ANNEXK_H */
