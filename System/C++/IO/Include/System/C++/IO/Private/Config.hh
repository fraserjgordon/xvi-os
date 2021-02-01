#ifndef __SYSTEM_CXX_IO_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_IO_PRIVATE_CONFIG_H


#if !defined(__XVI_STD_IO_NS)
#  if defined (__XVI_STD_NS)
#   define __XVI_STD_IO_NS         __XVI_STD_NS
#  else
#    define __XVI_STD_NS                std/*::inline __xvi_cxx_v0*/
#    define __XVI_STD_IO_NS        __XVI_STD_NS
#  endif
#endif


#if defined(__XVI_CXX_IO_BUILD_SHARED)
#  define __XVI_CXX_IO_EXPORT                      [[gnu::visibility("protected")]]
#elif defined(__XVI_CXX_IO_BUILD_STATIC)
#  define __XVI_CXX_IO_EXPORT                      /**/
#else
#  define __XVI_CXX_IO_EXPORT                      /**/
#endif

#define __XVI_CXX_IO_SYMBOL(x)                   asm("System.Cxx.IO." #x)


#endif /* ifndef __SYSTEM_CXX_IO_PRIVATE_CONFIG_H */
