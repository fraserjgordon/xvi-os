#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_EXPORT_H
#define __SYSTEM_ALLOCATOR_GALLOC_EXPORT_H


#define __GALLOC_SYMBOL(name)   __GALLOC_SYMBOL_(name)
#define __GALLOC_SYMBOL_(name)  __asm__("_$System_Allocator_GAlloc$v0$" #name)

#if defined(__GALLOC_BUILD_SHARED)
#  define __GALLOC_EXPORT __attribute__((__visibility__("protected")))
#else
#  define __GALLOC_EXPORT /* empty */
#endif


#define __GALLOC_CXX_API_NS     ::inline api_v0

#define __GALLOC_CXX_EXPORT     extern "C" __GALLOC_EXPORT


#endif /* ifndef __SYSTEM_ALLOCATOR_GALLOC_EXPORT_H */
