#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_GALLOC_HH
#define __SYSTEM_ALLOCATOR_GALLOC_GALLOC_HH


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/Pair.hh>

#include <System/Allocator/GAlloc/Export.h>


namespace System::Allocator::GAlloc __GALLOC_CXX_API_NS
{


__GALLOC_CXX_EXPORT
std::pair<void*, std::size_t> allocate(std::size_t)
    __GALLOC_SYMBOL(galloc_allocate);

__GALLOC_CXX_EXPORT
std::pair<void*, std::size_t> allocateAligned(std::size_t, std::size_t)
    __GALLOC_SYMBOL(galloc_allocate_aligned);

__GALLOC_CXX_EXPORT
std::pair<void*, std::size_t> resize(void*, std::size_t)
    __GALLOC_SYMBOL(galloc_resize);

__GALLOC_CXX_EXPORT
std::pair<void*, std::size_t> reallocate(void*, std::size_t)
    __GALLOC_SYMBOL(galloc_reallocate);

__GALLOC_CXX_EXPORT
std::pair<void*, std::size_t> reallocateAligned(void*, std::size_t, std::size_t)
    __GALLOC_SYMBOL(galloc_reallocate_aligned);

__GALLOC_CXX_EXPORT
void
free(void*)
    __GALLOC_SYMBOL(galloc_free);

__GALLOC_CXX_EXPORT
void
freeSized(void*, std::size_t)
    __GALLOC_SYMBOL(galloc_free_sized);


} // namespace __SYSTEM_ALLOCATOR_GALLOC


#endif /* ifndef __SYSTEM_ALLOCATOR_GALLOC_GALLOC_HH */
