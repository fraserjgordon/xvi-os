#pragma once
#ifndef __SYSTEM_ABI_TLS_PRIVATE_CONFIG_H
#define __SYSTEM_ABI_TLS_PRIVATE_CONFIG_H


#if __SYSTEM_ABI_TLS_BUILD_SHARED
#  define __SYSTEM_ABI_TLS_EXPORT       [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_ABI_TLS_EXPORT       /**/
#endif

#define __SYSTEM_ABI_TLS_SYMBOL_(x)     asm("System.ABI.TLS." #x)
#define __SYSTEM_ABI_TLS_SYMBOL(x)      __SYSTEM_ABI_TLS_SYMBOL_(x)


#endif /* ifndef __SYSTEM_ABI_TLS_PRIVATE_CONFIG_H */
