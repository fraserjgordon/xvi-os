#ifndef __SYSTEM_CONCURRENCY_PRIVATE_CONFIG_H
#define __SYSTEM_CONCURRENCY_PRIVATE_CONFIG_H


#if defined(__SYSTEM_CONCURRENCY_BUILD_SHARED)
#  define __SYSTEM_CONCURRENCY_EXPORT       [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_CONCURRENCY_EXPORT       /**/
#endif

#define __SYSTEM_CONCURRENCY_SYMBOL_(x)     asm("System.Concurrency." #x)
#define __SYSTEM_CONCURRENCY_SYMBOL(x)      __SYSTEM_CONCURRENCY_SYMBOL_(x)


#endif /* ifndef __SYSTEM_CONCURRENCY_PRIVATE_CONFIG_H */
