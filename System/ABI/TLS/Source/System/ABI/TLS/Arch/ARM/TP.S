#include <System/ABI/TLS/Private/Config.hh>

#if (__ARM_ARCH == 7 || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6KZ__))
#  define HAVE_TID_REG 1
#endif

.global __aeabi_tp_read
.type __aeabi_tp_read, "function"   
.size __aeabi_tp_read, __aeabi_tp_read_end - __aeabi_tp_read

#ifdef __SYSTEM_ABI_TLS_BUILD_SHARED
.protected __aeabi_tp_read
#else
.hidden __aeabi_tp_read
#endif

__aeabi_tp_read:

#if HAVE_TID_REG
    @ We have a dedicated CP15 register for the thread pointer.
    mrc     p15, 0, r0, c13, c0, 2
    bx      lr
#else
    @ We need to make a syscall.
    b       Syscall.Arch.ARM.GetThreadPointer@plt
#endif

__aeabi_tp_read_end = .


.global __aeabi_tp_write
.type __aeabi_tp_write, "function"   
.size __aeabi_tp_write, __aeabi_tp_write_end - __aeabi_tp_write
.hidden __aeabi_tp_write

__aeabi_tp_write:

#if HAVE_TID_REG
    @ We have a dedicated CP15 register for the thread pointer.
    mcr     p15, 0, r0, c13, c0, 2
    bx      lr
#else
    @ We need to make a syscall.
    b       Syscall.Arch.ARM.SetThreadPointer@plt
#endif

__aeabi_tp_write_end = .
