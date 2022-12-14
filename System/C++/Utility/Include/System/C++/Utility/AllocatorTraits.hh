#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ALLOCATORTRAITS_H
#define __SYSTEM_CXX_UTILITY_ALLOCATORTRAITS_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>


#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/Concepts.hh>
#include <System/C++/Utility/MemoryAlgorithms.hh>
#include <System/C++/Utility/PointerTraits.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{

template <class _A> using __pointer_detector = typename _A::pointer;
template <class _A> using __const_pointer_detector = typename _A::const_pointer;
template <class _A> using __void_pointer_detector = typename _A::void_pointer;
template <class _A> using __const_void_pointer_detector = typename _A::const_void_pointer;

template <class _A> using __size_type_detector = typename _A::size_type;

template <class _A> using __propagate_on_copy_assign_detector = typename _A::propagage_on_container_copy_assignment;
template <class _A> using __propagate_on_move_assign_detector = typename _A::propagate_on_container_move_assignment;
template <class _A> using __propagate_on_swap_detector = typename _A::propagate_on_container_swap;

template <class _A> using __is_always_equal_detector = typename _A::is_always_equal;

template <class _A, class _T> using __rebind_other_detector = typename _A::template rebind<_T>::other;

template <class _T, class> struct __allocator_traits_rebind_helper; // Not defined.
template <class _T, template <class, class...> class _Alloc, class _U, class... _Args>
    struct __allocator_traits_rebind_helper<_T, _Alloc<_U, _Args...>> { using type = _Alloc<_T, _Args...>; };

template <class _T, class _Alloc> struct __allocator_traits_rebind_detected
    { using type = typename _Alloc::template rebind<_T>::other; };

template <class _T, class _Alloc> struct __allocator_traits_rebind
    : conditional_t<is_detected_v<__rebind_other_detector, _Alloc, _T>,
                    __allocator_traits_rebind_detected<_T, _Alloc>,
                    __allocator_traits_rebind_helper<_T, _Alloc>> {};

template <class _A, class _ST, class _CVP> using __allocate_hint_detector = decltype(declval<_A>().allocate(declval<_ST>(), declval<_CVP>()));
template <class _A, class _T, class... _Args> using __construct_detector = decltype(declval<_A>().construct(declval<_T*>(), declval<_Args>()...));
template <class _A, class _T> using __destroy_detector = decltype(declval<_A>().destroy(declval<_T*>()));

template <class _A> using __max_size_detector = decltype(declval<_A>().max_size());
template <class _A> using __select_on_copy_construct_detector = decltype(declval<_A>().select_on_container_copy_construction());

template <class _T> using __difference_type_detector = typename _T::difference_type;

} // namespace __detail


template <class _Alloc>
struct allocator_traits
{
    using allocator_type    = _Alloc;
    using value_type        = typename _Alloc::value_type;

    using pointer           = __detail::detected_or_t<value_type*, __detail::__pointer_detector, _Alloc>;
    using const_pointer     = __detail::detected_or_t<typename pointer_traits<pointer>::template rebind<const value_type>, __detail::__const_pointer_detector, _Alloc>;
    using void_pointer      = __detail::detected_or_t<typename pointer_traits<pointer>::template rebind<void>, __detail::__void_pointer_detector, _Alloc>;
    using const_void_pointer= __detail::detected_or_t<typename pointer_traits<pointer>::template rebind<const void>, __detail::__const_void_pointer_detector, _Alloc>; 

    using difference_type   = __detail::detected_or_t<typename pointer_traits<pointer>::difference_type, __detail::__difference_type_detector, _Alloc>;
    using size_type         = __detail::detected_or_t<make_unsigned_t<difference_type>, __detail::__size_type_detector, _Alloc>;

    using propagate_on_container_copy_assignment = __detail::detected_or_t<false_type, __detail::__propagate_on_copy_assign_detector, _Alloc>;
    using propagate_on_container_move_assignment = __detail::detected_or_t<false_type, __detail::__propagate_on_move_assign_detector, _Alloc>;
    using propagate_on_container_swap            = __detail::detected_or_t<false_type, __detail::__propagate_on_swap_detector, _Alloc>;

    using is_always_equal   = __detail::detected_or_t<typename is_empty<_Alloc>::type, __detail::__is_always_equal_detector, _Alloc>;

    template <class _T>
    using rebind_alloc      = typename __detail::__allocator_traits_rebind<_T, _Alloc>::type;

    template <class _T>
    using rebind_traits     = allocator_traits<rebind_alloc<_T>>;

    [[nodiscard]] static constexpr pointer allocate(_Alloc& __a, size_type __n)
    {
        return __a.allocate(__n);
    }

    [[nodiscard]] static constexpr pointer allocate(_Alloc& __a, size_type __n, const_void_pointer __hint)
    {
        if constexpr (__detail::is_detected_v<__detail::__allocate_hint_detector, _Alloc, size_type, const_void_pointer>)
            return __a.allocate(__n, __hint);
        else
            return __a.allocate(__n);
    }

    static constexpr void deallocate(_Alloc& __a, pointer __p, size_type __n)
    {
        return __a.deallocate(__p, __n);
    }

    template <class _T, class... _Args>
    static constexpr void construct(_Alloc& __a, _T* __p, _Args&&... __args)
    {
        if constexpr (__detail::is_detected_v<__detail::__construct_detector, _Alloc, _T, _Args...>)
            __a.construct(__p, std::forward<_Args>(__args)...);
        else
            construct_at(__p, std::forward<_Args>(__args)...);
    }

    template <class _T>
    static constexpr void destroy(_Alloc& __a, _T* __p)
    {
        if constexpr (__detail::is_detected_v<__detail::__destroy_detector, _Alloc, _T>)
            __a.destroy(__p);
        else
            destroy_at(__p);
    }

    static constexpr size_type max_size(const _Alloc& __a) noexcept
    {
        if constexpr (__detail::is_detected_v<__detail::__max_size_detector, _Alloc>)
            return __a.max_size();
        else
            return numeric_limits<size_type>::max() / sizeof(value_type);
    }

    static constexpr _Alloc select_on_container_copy_construction(const _Alloc& __a)
    {
        if constexpr (__detail::is_detected_v<__detail::__select_on_copy_construct_detector, _Alloc>)
            return __a.select_on_container_copy_construction();
        else
            return __a;
    }
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ALLOCATORTRAITS_H */
