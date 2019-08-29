#pragma once
#ifndef __SYSTEM_CXX_UTILITY_MEMORY_H
#define __SYSTEM_CXX_UTILITY_MEMORY_H


#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Allocator.hh>
#include <System/C++/Utility/AllocatorTraits.hh>
#include <System/C++/Utility/DefaultDelete.hh>
#include <System/C++/Utility/MemoryAlgorithms.hh>
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
    uintptr_t __original_p = uintptr_t(__ptr);
    uintptr_t __aligned_p = (__original_p + __alignment - 1) & ~(__alignment - 1);
    uintptr_t __padding_size = __aligned_p - __original_p;

    if ((__size + __padding_size) < __space)
        return nullptr;

    __ptr = reinterpret_cast<void*>(__aligned_p + __size);
    __space -= (__size + __padding_size);

    return reinterpret_cast<void*>(__aligned_p);
}

template <size_t _N, class _T>
[[nodiscard, gnu::assume_aligned(_N)]] constexpr _T* assume_aligned(_T* __ptr)
{
    return __ptr;
}


struct allocator_arg_t { explicit allocator_arg_t() = default; };

inline constexpr allocator_arg_t allocator_arg {};


namespace __detail
{

template <class _T, class _Alloc, class = void_t<>>
struct __uses_allocator : false_type {};

template <class _T, class _Alloc>
struct __uses_allocator<_T, _Alloc, void_t<typename _T::allocator_type>>
    : is_convertible<_Alloc, typename _T::allocator_type> {};

template <class _T>
struct __is_pair_specialization : false_type {};

template <class _T, class _U>
struct __is_pair_specialization<pair<_T, _U>> : true_type {};

}


template <class _T, class _Alloc>
struct uses_allocator : __detail::__uses_allocator<_T, _Alloc> {};

template <class _T, class _Alloc>
inline constexpr bool uses_allocator_v = uses_allocator<_T, _Alloc>::value;


template <class _T, class _Alloc, class... _Args,
          class = enable_if_t<!__detail::__is_pair_specialization<_T>::value, void>>
auto uses_allocator_construction_args(const _Alloc& __alloc, _Args&&... __args)
{
    if constexpr (!uses_allocator_v<_T, _Alloc>
                  && is_constructible_v<_T, _Args...>)
    {
        return forward_as_tuple(__XVI_STD_NS::forward<_Args>(__args)...);
    }
    else if constexpr (uses_allocator_v<_T, _Alloc>
                       && is_constructible_v<_T, allocator_arg_t, _Alloc, _Args...>)
    {
        return tuple<allocator_arg_t, const _Alloc&, _Args&&...>(allocator_arg, __alloc, __XVI_STD_NS::forward<_Args>(__args)...);
    }
    else if constexpr (uses_allocator_v<_T, _Alloc>
                       && is_constructible_v<_T, _Args..., _Alloc>)
    {
        return forward_as_tuple(__XVI_STD_NS::forward<_Args>(__args)..., __alloc);
    }
}

template <class _T, class _Alloc, class _Tuple1, class _Tuple2,
          class = enable_if_t<__detail::__is_pair_specialization<_T>::value, void>>
auto uses_allocator_construction_args(const _Alloc& __alloc, piecewise_construct_t, _Tuple1&& __x, _Tuple2&& __y)
{
    using _T1 = typename _T::first_type;
    using _T2 = typename _T::second_type;

    return make_tuple
    (
        piecewise_construct,
        apply([&__alloc](auto&&... __args1)
            {
                return uses_allocator_construction_args<_T1>(__alloc, __XVI_STD_NS::forward<decltype(__args1)>(__args1)...);
            }, __XVI_STD_NS::forward<_Tuple1>(__x)),
        apply([&__alloc](auto&&... __args2)
            {
                return uses_allocator_construction_args<_T2>(__alloc, __XVI_STD_NS::forward<decltype(__args2)>(__args2)...);
            }, __XVI_STD_NS::forward<_Tuple2>(__y))
    );
}

template <class _T, class _Alloc,
          class = enable_if_t<__detail::__is_pair_specialization<_T>::value, void>>
auto uses_allocator_construction_args(const _Alloc& __alloc)
{
    return uses_allocator_construction_args<_T>(__alloc, piecewise_construct, tuple<>{}, tuple<>{});
}

template <class _T, class _Alloc, class _U, class _V,
          class = enable_if_t<__detail::__is_pair_specialization<_T>::value, void>>
auto uses_allocator_construction_args(const _Alloc& __alloc, _U&& __u, _V&& __v)
{
    return uses_allocator_construction_args<_T>(__alloc, piecewise_construct, forward_as_tuple(__XVI_STD_NS::forward<_U>(__u)), forward_as_tuple(__XVI_STD_NS::forward<_V>(__v)));
}

template <class _T, class _Alloc, class _U, class _V,
          class = enable_if_t<__detail::__is_pair_specialization<_T>::value, void>>
auto uses_allocator_construction_args(const _Alloc& __alloc, const pair<_U, _V>& __pr)
{
    return uses_allocator_construction_args(__alloc, piecewise_construct, forward_as_tuple(__pr.first), forward_as_tuple(__pr.second));
}

template <class _T, class _Alloc, class _U, class _V,
          class = enable_if_t<__detail::__is_pair_specialization<_T>::value, void>>
auto uses_allocator_construction_args(const _Alloc& __alloc, pair<_U, _V>&& __pr)
{
    return uses_allocator_construction_args(__alloc, piecewise_construct, forward_as_tuple(__XVI_STD_NS::move(__pr.first)), forward_as_tuple(__XVI_STD_NS::move(__pr.second)));
}


template <class _T, class _Alloc, class... _Args>
_T make_obj_using_allocator(const _Alloc& __alloc, _Args&&... __args)
{
    return make_from_tuple<_T>(uses_allocator_construction_args<_T>(__alloc, __XVI_STD_NS::forward<_Args>(__args)...));
}

template <class _T, class _Alloc, class... _Args>
_T* uninitialized_construct_using_allocator(_T* __p, const _Alloc& __alloc, _Args&&... __args)
{
    return ::new(static_cast<void*>(__p)) _T(make_obj_using_allocator<_T>(__alloc, __XVI_STD_NS::forward<_Args>(__args)...));
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_MEMORY_H */
