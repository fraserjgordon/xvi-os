#pragma once
#ifndef __SYSTEM_ABI_CXX_PRIVATE_CONFIG_H
#define __SYSTEM_ABI_CXX_PRIVATE_CONFIG_H


#if __SYSTEM_ABI_CXX_BUILD_SHARED
#  define __SYSTEM_ABI_CXX_EXPORT                 [[gnu::visibility("protected")]]
#  define __SYSTEM_ABI_CXX_CTORDTOR_EXPORT        __SYSTEM_ABI_CXX_EXPORT
#  define __SYSTEM_ABI_CXX_DEMANGLE_EXPORT        __SYSTEM_ABI_CXX_EXPORT
#  define __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT   __SYSTEM_ABI_CXX_EXPORT
#  define __SYSTEM_ABI_CXX_EXCEPTION_EXPORT       __SYSTEM_ABI_CXX_EXPORT
#  define __SYSTEM_ABI_CXX_GUARD_EXPORT           __SYSTEM_ABI_CXX_EXPORT
#  define __SYSTEM_ABI_CXX_RTTI_EXPORT            __SYSTEM_ABI_CXX_EXPORT
#  define __SYSTEM_ABI_CXX_UNWIND_EXPORT          __SYSTEM_ABI_CXX_EXPORT
#  define __SYSTEM_ABI_CXX_VIRTUAL_EXPORT         __SYSTEM_ABI_CXX_EXPORT
#else
#  define __SYSTEM_ABI_CXX_EXPORT       /**/

#  if __SYSTEM_ABI_CXX_CTORDTOR_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_CTORDTOR_EXPORT  [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_CTORDTOR_EXPORT  /**/
#  endif

#  if __SYSTEM_ABI_CXX_DEMANGLE_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_DEMANGLE_EXPORT  [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_DEMANGLE_EXPORT  /**/
#  endif

#  if __SYSTEM_ABI_CXX_EHPERSONALITY_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT /**/
#  endif

#  if __SYSTEM_ABI_CXX_EXCEPTION_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_EXCEPTION_EXPORT  [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_EXCEPTION_EXPORT /**/
#  endif

#  if __SYSTEM_ABI_CXX_GUARD_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_GUARD_EXPORT     [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_GUARD_EXPORT     /**/
#  endif

#  if __SYSTEM_ABI_CXX_RTTI_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_RTTI_EXPORT      [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_RTTI_EXPORT      /**/
#  endif

#  if __SYSTEM_ABI_CXX_UNWIND_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_UNWIND_EXPORT    [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_UNWIND_EXPORT    /**/
#  endif

#  if __SYSTEM_ABI_CXX_VIRTUAL_BUILD_SHARED
#    define __SYSTEM_ABI_CXX_VIRTUAL_EXPORT   [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_CXX_VIRTUAL_EXPORT   /**/
#  endif
#endif

#if defined(__XVI_HOSTED)
#  define __SYSTEM_ABI_CXX_SYMBOL_(x)       asm(#x)
#  define __SYSTEM_ABI_CXX_SYMBOL(x)        __SYSTEM_ABI_CXX_SYMBOL_(hostedabi_##x)
#else
#  define __SYSTEM_ABI_CXX_SYMBOL_(x)       asm(#x)
#  define __SYSTEM_ABI_CXX_SYMBOL(x)        __SYSTEM_ABI_CXX_SYMBOL_(x)
#endif

#if defined(__XVI_HOSTED)
#  define __SYSTEM_ABI_CXX_NS               hosted_cxxabiv1
#else
#  define __SYSTEM_ABI_CXX_NS               __cxxabiv1
#endif


#endif /* ifndef __SYSTEM_ABI_CXX_PRIVATE_CONFIG_H */
