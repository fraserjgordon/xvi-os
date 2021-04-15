#ifndef __SYSTEM_UNICODE_UTF_PRIVATE_CONFIG_H
#define __SYSTEM_UNICODE_UTF_PRIVATE_CONFIG_H


#ifdef __SYSTEM_UNICODE_UTF_BUILD_SHARED
#  define __SYSTEM_UNICODE_UTF_EXPORT       [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_UNICODE_UTF_EXPORT       /**/
#endif

#define __SYSTEM_UNICODE_UTF_SYMBOL(x)      asm("System.Unicode.UTF." #x)


#endif /* ifndef __SYSTEM_UNICODE_UTF_PRIVATE_CONFIG_H */
