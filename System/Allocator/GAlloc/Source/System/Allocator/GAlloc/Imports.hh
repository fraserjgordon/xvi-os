#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_IMPORTS_H
#define __SYSTEM_ALLOCATOR_GALLOC_IMPORTS_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/Allocator/GAlloc/Config.hh>


namespace __GALLOC_NS_DECL
{


void* AllocateVirtual(std::size_t size, std::size_t alignment);
bool  CommitVirtual(void* where, std::size_t size);


} // namespace __GALLOC_NS


#endif // ifndef __SYSTEM_ALLOCATOR_GALLOC_IMPORTS_H
