#ifndef __SYSTEM_IO_HANDLE_PRIVATE_CONFIG_H
#define __SYSTEM_IO_HANDLE_PRIVATE_CONFIG_H


#ifdef __SYSTEM_IO_HANDLE_BUILD_SHARED
#  define __SYSTEM_IO_HANDLE_EXPORT     [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_IO_HANDLE_EXPORT     [[gnu::visibility("hidden")]]
#endif

#define __SYSTEM_IO_HANDLE_SYMBOL(x)    __SYSTEM_IO_HANDLE_SYMBOL_(x)
#define __SYSTEM_IO_HANDLE_SYMBOL_(x)   asm("System.IO.Handle." #x)


#endif /* ifndef __SYSTEM_IO_HANDLE_PRIVATE_CONFIG_H */
