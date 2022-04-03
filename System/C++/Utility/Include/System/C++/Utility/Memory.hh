#pragma once
#ifndef __SYSTEM_CXX_UTILITY_MEMORY_H
#define __SYSTEM_CXX_UTILITY_MEMORY_H


#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/AllocatorTraits.hh>
#include <System/C++/Utility/DefaultDelete.hh>
#include <System/C++/Utility/MemoryAlgorithms.hh>
#include <System/C++/Utility/OutPtr.hh>
#include <System/C++/Utility/PointerTraits.hh>
#include <System/C++/Utility/SharedPtr.hh>
#include <System/C++/Utility/Tuple.hh>
#include <System/C++/Utility/UniquePtr.hh>
#include <System/C++/Utility/Private/AddressOf.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{

enum class pointer_safety
{
    relaxed,
    preferred,
    strict,
};

// Only the declarations are present in this library. Implemented elsewhere.
void declare_reachable(void*);
void undeclare_reachable(void*);

template <class _T>
_T* undeclare_reachable(_T* __p)
{
    undeclare_reachable(static_cast<void*>(__p));
    return __p;
}

// Only the declarations are present in this library. Implemented elsewhere.
void declare_no_pointers(char*, size_t);
void undeclare_no_pointers(char*, size_t);

// Only the declaration is present in this library. Implemented elsewhere.
pointer_safety get_pointer_safety() noexcept;


inline void* align(size_t __alignment, size_t __size, void*& __ptr, size_t& __space)
{
    uintptr_t __original_p = reinterpret_cast<uintptr_t>(__ptr);
    uintptr_t __aligned_p = (__original_p + __alignment - 1) & ~(__alignment - 1);
    uintptr_t __padding_size = __aligned_p - __original_p;

    if ((__size + __padding_size) > __space)
        return nullptr;

    __ptr = reinterpret_cast<void*>(__aligned_p);
    __space -= (__size + __padding_size);

    return __ptr;
}

template <size_t _N, class _T>
[[nodiscard, gnu::assume_aligned(_N)]] constexpr _T* assume_aligned(_T* __ptr)
{
    static_assert((_N & (_N - 1) == 0), "N must be a power of two");    

    return __ptr;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_MEMORY_H */
