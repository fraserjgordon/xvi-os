#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_RELATIONS_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_RELATIONS_H


#include <System/C++/TypeTraits/Private/Detect.hh>
#include <System/C++/TypeTraits/Private/IntegralConstant.hh>
#include <System/C++/TypeTraits/Private/Invoke.hh>
#include <System/C++/TypeTraits/Private/Namespace.hh>


namespace __XVI_STD_TYPETRAITS_NS
{


// Forward declarations.
template <class> struct is_array;
template <class> struct is_function;
template <class> struct is_void;
template <class...> struct conjunction;


namespace __detail
{

template <class _T, class _U> struct is_same : false_type {};
template <class _T> struct is_same<_T, _T> : true_type {};

template <class _T, class _U> struct __is_not_same : bool_constant<!is_same<_T, _U>::value> {};

template <class _From, class _To, bool = is_void<_From>::value>
    struct __is_convertible_helper2 : is_void<_To> {};
template <class _From, class _To>
struct __is_convertible_helper2<_From, _To, false>
{
    template <class _T> static void __test_fn(_T);

    template <class _F, class _T> using __convertible_detector = decltype(__test_fn<_T>(declval<_F>()));
    using type = is_detected<__convertible_detector, _From, _To>;
};

template <class _From, class _To, bool = is_array<_To>::value || is_function<_To>::value>
    struct __is_convertible_helper : __is_convertible_helper2<_From, _To> {};
template <class _From, class _To>
    struct __is_convertible_helper<_From, _To, true> : false_type {};

template <class _From, class _To> struct is_convertible : __is_convertible_helper<_From, _To>::type {};


template <class _From, class _To>
struct __is_nothrow_convertible_helper
{
    static void __test_fn(_To) noexcept;
    static constexpr bool value = noexcept(__test_fn(declval<_From>()));
    using type = bool_constant<value>;
};

template <class _From, class _To> struct is_nothrow_convertible
    : conjunction<is_convertible<_From, _To>, __is_nothrow_convertible_helper<_From, _To>>::type {};


template <class _Fn, class... _Args> struct is_invocable : is_detected<__is_invocable_detector, _Fn, _Args...> {};

template <class _R, class _Fn, class... _Args>
struct __is_invocable_r
{
    using type = __is_invocable_detector<_Fn, _Args...>;
    static constexpr bool value = is_convertible<type, _R>::value;
};

template <class _R, class _Fn, class... _Args>
    struct is_invocable_r
        : bool_constant<conjunction_v<is_invocable<_Fn, _Args...>, __is_invocable_r<_R, _Fn, _Args...>>> {};

template <class _Fn, class... _Args>
struct __is_nothrow_invocable
{
    static constexpr bool value = noexcept(_INVOKE(declval<_Fn>(), declval<_Args>()...));
};

template <class _Fn, class... _Args>
    struct is_nothrow_invocable
        : bool_constant<conjunction_v<is_invocable<_Fn, _Args...>, __is_nothrow_invocable<_Fn, _Args...>>> {};

template <class _R, class _Fn, class... _Args>
struct __is_nothrow_invocable_r
{
    using type = __is_invocable_detector<_Fn, _Args...>;
    static constexpr bool value = is_nothrow_convertible<type, _R>::value;
};

template <class _R, class _Fn, class... _Args>
    struct is_nothrow_invocable_r
        : bool_constant<conjunction_v<is_nothrow_invocable<_Fn, _Args...>, __is_nothrow_invocable_r<_R, _Fn, _Args...>>> {};

} // namespace __detail


template <class _T, class _U> struct is_same : __detail::is_same<_T, _U> {};
template <class _T, class _U> inline constexpr bool is_same_v = is_same<_T, _U>::value;

template <class _T, class _U> struct is_base_of : __detail::is_base_of<_T, _U> {};
template <class _T, class _U> inline constexpr bool is_base_of_v = is_base_of<_T, _U>::value;

template <class _From, class _To> struct is_convertible : __detail::is_convertible<_From, _To> {};
template <class _From, class _To> inline constexpr bool is_convertible_v = is_convertible<_From, _To>::value;

template <class _From, class _To> struct is_nothrow_convertible : __detail::is_nothrow_convertible<_From, _To> {};
template <class _From, class _To> inline constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<_From, _To>::value;

template <class _Fn, class... _Args> struct is_invocable : __detail::is_invocable<_Fn, _Args...> {};
template <class _Fn, class... _Args> inline constexpr bool is_invocable_v = is_invocable<_Fn, _Args...>::value;

template <class _R, class _Fn, class... _Args> struct is_invocable_r : __detail::is_invocable_r<_R, _Fn, _Args...> {};
template <class _R, class _Fn, class... _Args> inline constexpr bool is_invocable_r_v = is_invocable_r<_R, _Fn, _Args...>::value;

template <class _Fn, class... _Args> struct is_nothrow_invocable : __detail::is_nothrow_invocable<_Fn, _Args...> {};
template <class _Fn, class... _Args> inline constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<_Fn, _Args...>::value;

template <class _R, class _Fn, class... _Args> struct is_nothrow_invocable_r : __detail::is_nothrow_invocable_r<_R, _Fn, _Args...> {};
template <class _R, class _Fn, class... _Args> inline constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<_R, _Fn, _Args...>::value;


} // namespace __XVI_STD_TYPETRAITS_NS


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_RELATIONS_H */