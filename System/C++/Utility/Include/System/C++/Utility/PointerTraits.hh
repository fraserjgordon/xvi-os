#pragma once
#ifndef __SYSTEM_CXX_UTILITY_POINTERTRAITS_H
#define __SYSTEM_CXX_UTILITY_POINTERTRAITS_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/C++/Utility/Private/AddressOf.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class> struct pointer_traits;


namespace __detail
{

template <class _T> using __element_type_detector = typename _T::element_type;
template <class _T> using __difference_type_detector = typename _T::difference_type;
template <class _T, class _U> using __rebind_detector = typename _T::template rebind<_U>;

template <class _T> struct __pointer_traits_element_type_helper; // Not defined.
template <template <class, class...> class _Ptr, class _T, class... _Args>
struct __pointer_traits_element_type_helper<_Ptr<_T, _Args...>>
    { using type = _T; };

template <class _T> struct __pointer_traits_element_type_detected
    { using type = typename _T::element_type; };

template <class _T> struct __pointer_traits_element_type
    : conditional_t<is_detected_v<__element_type_detector, _T>,
                    __pointer_traits_element_type_detected<_T>,
                    __pointer_traits_element_type_helper<_T>> {};

template <class _T> struct __pointer_traits_difference_type
    { using type = detected_or_t<ptrdiff_t, __difference_type_detector, _T>; };

template <class _T, class _U> struct __pointer_traits_rebind_helper; // Not defined.
template <template <class, class...> class _Ptr, class _T, class... _Args, class _U>
struct __pointer_traits_rebind_helper<_Ptr<_T, _Args...>, _U>
    { using type = _Ptr<_U, _Args...>; };

template <class _T, class _U> struct __pointer_traits_rebind_detected
    { using type = typename _T::template rebind<_U>; };

template <class _T, class _U> struct __pointer_traits_rebind
    : conditional_t<is_detected_v<__rebind_detector, _T, _U>,
                    __pointer_traits_rebind_detected<_T, _U>,
                    __pointer_traits_rebind_helper<_T, _U>> {};


template <class _Ptr> using __to_address_detector = decltype(pointer_traits<_Ptr>::to_address(declval<_Ptr&>()));

} // namespace __detail


template <class _Ptr>
struct pointer_traits
{
    using pointer           = _Ptr;
    using element_type      = typename __detail::__pointer_traits_element_type<_Ptr>::type;
    using difference_type   = typename __detail::__pointer_traits_difference_type<_Ptr>::type;

    template <class _U> using rebind = typename __detail::__pointer_traits_rebind<_Ptr, _U>::type;

    using __element_ref_type = conditional_t<is_void_v<remove_cv_t<element_type>>,
                                             __detail::nonesuch,
                                             add_lvalue_reference_t<element_type>>;

    template <class _U = remove_cv_t<element_type>, class = enable_if_t<!is_void_v<_U>, void>>
    static pointer pointer_to(__element_ref_type __r) noexcept
    {
        return _Ptr::pointer_to(__r);
    }

    template <class _U, class = enable_if_t<is_void_v<remove_cv_t<element_type>>, void>>
    static pointer pointer_to(_U& __r) noexcept
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

    using __element_ref_type = conditional_t<is_void_v<remove_cv_t<element_type>>,
                                             __detail::nonesuch,
                                             add_lvalue_reference_t<element_type>>;

    template <class _U = remove_cv_t<element_type>, class = enable_if_t<!is_void_v<_U>, void>>
    static constexpr pointer pointer_to(__element_ref_type __r) noexcept
    {
        return addressof(__r);
    }

    template <class _U, class = enable_if_t<is_void_v<remove_cv_t<element_type>>, void>>
    static constexpr pointer pointer_to(_U& __r) noexcept
    {
        return addressof(__r);
    }
};


template <class _Ptr>
auto to_address(const _Ptr& __p) noexcept
{
    if constexpr (__detail::is_detected_v<__detail::__to_address_detector, _Ptr>)
        return pointer_traits<_Ptr>::to_address(__p);
    else
        return to_address(__p.operator->());
}

template <class _T>
constexpr _T* to_address(_T* __p) noexcept
{
    return __p;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_POINTERTRAITS_H */
