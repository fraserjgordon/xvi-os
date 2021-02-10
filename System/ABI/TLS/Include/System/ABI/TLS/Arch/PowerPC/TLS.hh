#pragma once
#ifndef __SYSTEM_ABI_TLS_ARCH_POWERPC_TLS_H
#define __SYSTEM_ABI_TLS_ARCH_POWERPC_TLS_H


#include <System/ABI/TLS/Private/Config.hh>


struct __tls_index
{
    unsigned long int ti_module;
    unsigned long int ti_offset;
};


extern "C" __SYSTEM_ABI_TLS_EXPORT void* __tls_get_addr(__tls_index*);


#endif /* ifndef __SYSTEM_ABI_TLS_ARH_X86_TLS_H */
