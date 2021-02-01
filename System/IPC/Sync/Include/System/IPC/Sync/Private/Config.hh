#ifndef __SYSTEM_IPC_SYNC_PRIVATE_CONFIG_H
#define __SYSTEM_IPC_SYNC_PRIVATE_CONFIG_H


#if __SYSTEM_IPC_SYNC_BUILD_SHARED
#  define __SYSTEM_IPC_SYNC_EXPORT  [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_IPC_SYNC_EXPORT  /**/
#endif

#define __SYSTEM_IPC_SYNC_SYMBOL(x)     __SYSTEM_IPC_SYNC_SYMBOL_(x)
#define __SYSTEM_IPC_SYNC_SYMBOL_(x)    asm("System.IPC.Sync." #x)


#endif /* ifndef __SYSTEM_IPC_SYNC_PRIVATE_CONFIG_H */
