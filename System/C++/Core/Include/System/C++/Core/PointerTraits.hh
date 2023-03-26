#ifndef __SYSTEM_CXX_CORE_POINTERTRAITS_H
#define __SYSTEM_CXX_CORE_POINTERTRAITS_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/CtorTags.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class> struct pointer_traits;


namespace __detail
{


template <class _T>
concept __has_member_value_type = requires { typename _T::value_type; };

template <class _T>
concept __has_member_element_type = requires { typename _T::element_type; };

template <class _T>
concept __has_member_difference_type = requires { typename _T::difference_type; };

template <class _T>
concept __has_member_pointer_type = requires { typename _T::pointer; };


template <class _T> struct __pointer_member_detected
    { using type = typename _T::pointer; };

template <class _T> struct __pointer_traits_element_type_detected
    { using type = typename _T::element_type; };

template <class _T> struct __pointer_traits_element_type_pointer_indirect
    { using type = typename pointer_traits<_T*>::element_type*; };

template <class _T> struct __pointer_of
    : conditional_t<__has_member_pointer_type<_T>,
                    __pointer_member_detected<_T>, 
                    conditional_t<__has_member_element_type<_T>,
                                  __pointer_traits_element_type_detected<_T>,
                                  __pointer_traits_element_type_pointer_indirect<_T>>> {};

template <class _T>
using __pointer_of_t = typename __pointer_of<_T>::type;

template <class _T, class _U>
struct __pointer_of_or { using type = _U; };

template <class _T, class _U>
    requires requires { typename __pointer_of_t<_T>; }
struct __pointer_of_or<_T, _U> { using type = __pointer_of_t<_T>; };

template <class _T, class _U>
using __pointer_of_or_t = typename __pointer_of_or<_T, _U>::type;


template <class _T, class _U>
concept __valid_rebind = requires { typename _T::template rebind<_U>; };

template <class _T> struct __pointer_traits_element_type_helper; // Not defined.
template <template <class, class...> class _Ptr, class _T, class... _Args>
struct __pointer_traits_element_type_helper<_Ptr<_T, _Args...>>
    { using type = _T; };

template <class _T> struct __pointer_traits_element_type
    : conditional_t<__has_member_element_type<_T>,
                    __pointer_traits_element_type_detected<_T>,
                    __pointer_traits_element_type_helper<_T>> {};

template <class _T> struct __pointer_traits_difference_type
    { using type = std::ptrdiff_t; };

template <class _T>
    requires __has_member_difference_type<_T>
struct __pointer_traits_difference_type<_T>
    { using type = typename _T::difference_type; };

template <class _T, class _U> struct __pointer_traits_rebind_helper; // Not defined.
template <template <class, class...> class _Ptr, class _T, class... _Args, class _U>
struct __pointer_traits_rebind_helper<_Ptr<_T, _Args...>, _U>
    { using type = _Ptr<_U, _Args...>; };

template <class _T, class _U> struct __pointer_traits_rebind_detected
    { using type = typename _T::template rebind<_U>; };

template <class _T, class _U> struct __pointer_traits_rebind
    : conditional_t<__valid_rebind<_T, _U>,
                    __pointer_traits_rebind_detected<_T, _U>,
                    __pointer_traits_rebind_helper<_T, _U>> {};


} // namespace __detail


template <class _Ptr>
struct pointer_traits
{
    using pointer           = _Ptr;
    using element_type      = typename __detail::__pointer_traits_element_type<_Ptr>::type;
    using difference_type   = typename __detail::__pointer_traits_difference_type<_Ptr>::type;

    template <class _U> using rebind = typename __detail::__pointer_traits_rebind<_Ptr, _U>::type;

    static pointer pointer_to(conditional_t<is_void_v<element_type>, int, element_type>& __r) noexcept
        requires (!is_void_v<element_type>)
    {
        return _Ptr::pointer_to(__r);
    }

    template <class _U>
    static pointer pointer_to(_U& __r) noexcept
        requires (std::is_void_v<element_type>)
    {
        return _Ptr::pointer_to(__r);
    }
};

template <class _T>
struct pointer_traits<_T*>
{
    using pointer           = _T*;
    using element_type      = _T;
    using difference_type   = ptrdiff_t;

    template <class _U> using rebind = _U*;

    static constexpr pointer pointer_to(conditional_t<is_void_v<element_type>, int, element_type>& __r) noexcept
        requires (!is_void_v<element_type>)
    {
        return addressof(__r);
    }

    template <class _U>
    static constexpr pointer pointer_to(_U& __r) noexcept
        requires is_void_v<element_type>
    {
        return addressof(__r);
    }
};


template <class _Ptr>
auto to_address(const _Ptr& __p) noexcept
{
    if constexpr (requires(const _Ptr& __p) { pointer_traits<_Ptr>::to_address(__p); })
        return pointer_traits<_Ptr>::to_address(__p);
    else
        return to_address(__p.operator->());
}

template <class _T>
constexpr _T* to_address(_T* __p) noexcept
{
    static_assert(!is_function_v<_T>, "cannot use to_address on function pointers");
    return __p;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_POINTERTRAITS_H */
