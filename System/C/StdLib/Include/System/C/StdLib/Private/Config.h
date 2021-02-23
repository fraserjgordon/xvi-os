#ifndef __SYSTEM_C_STDLIB_PRIVATE_CONFIG_H
#define __SYSTEM_C_STDLIB_PRIVATE_CONFIG_H


#ifdef __SYSTEM_C_STDLIB_BUILD_SHARED
#  define __SYSTEM_C_STDLIB_EXPORT      [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_C_STDLIB_EXPORT      /**/
#endif


#if defined(__cplusplus)
#  define __noreturn                    [[noreturn]]
#else
#  define __noreturn                    _Noreturn
#endif

#if !defined(__cplusplus)
#  define __restrict                    restrict
#elif defined(__GNUC__)
#  define __restrict                    __restrict
#else
#  define __restrict                    /**/
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_PRIVATE_CONFIG_H */
