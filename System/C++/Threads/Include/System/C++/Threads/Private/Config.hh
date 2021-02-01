#ifndef __SYSTEM_CXX_THREADS_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_THREADS_PRIVATE_CONFIG_H


#if !defined(__XVI_STD_THREADS_NS)
#  if defined (__XVI_STD_NS)
#   define __XVI_STD_THREADS_NS         __XVI_STD_NS
#  else
#    define __XVI_STD_NS                std/*::inline __xvi_cxx_v0*/
#    define __XVI_STD_THREADS_NS        __XVI_STD_NS
#  endif
#endif


#if defined(__XVI_CXX_THREADS_BUILD_SHARED)
#  define __XVI_CXX_THREADS_EXPORT                      [[gnu::visibility("protected")]]
#elif defined(__XVI_CXX_THREADS_BUILD_STATIC)
#  define __XVI_CXX_THREADS_EXPORT                      /**/
#else
#  define __XVI_CXX_THREADS_EXPORT                      /**/
#endif

#define __XVI_CXX_THREADS_SYMBOL(x)                   asm("System.Cxx.Threads." #x)


#endif /* ifndef __SYSTEM_CXX_THREADS_PRIVATE_CONFIG_H */
