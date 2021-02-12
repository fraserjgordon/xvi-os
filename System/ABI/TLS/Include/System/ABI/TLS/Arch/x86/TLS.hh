#pragma once
#ifndef __SYSTEM_ABI_TLS_ARCH_X86_TLS_H
#define __SYSTEM_ABI_TLS_ARCH_X86_TLS_H


#include <System/ABI/TLS/Private/Config.hh>


struct __tls_index
{
    unsigned long int ti_module;
    unsigned long int ti_offset;
};


// The x86 GNU ABI for this has a different name (three underscores) and calling convention.
#if !defined(__x86_64__)
extern "C" __SYSTEM_ABI_TLS_EXPORT [[gnu::regparm(1)]] void* ___tls_get_addr(__tls_index*);
#else
extern "C" __SYSTEM_ABI_TLS_EXPORT void*   __tls_get_addr(__tls_index*);
#endif


#endif /* ifndef __SYSTEM_ABI_TLS_ARCH_X86_TLS_H */
