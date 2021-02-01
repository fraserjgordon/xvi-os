#pragma once
#ifndef __SYSTEM_ABI_TLS_WEAKMALLOC_H
#define __SYSTEM_ABI_TLS_WEAKMALLOC_H


#include <System/C++/LanguageSupport/StdDef.hh>


extern "C" [[gnu::weak]] void* __malloc(std::size_t) asm("System.Allocator.C.Malloc");
extern "C" [[gnu::weak]] void  __free(void*) asm("System.Allocator.C.Free");


#endif /* ifndef __SYSTEM_ABI_TLS_WEAKMALLOC_H */
