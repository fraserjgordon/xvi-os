#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_DETECT_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_DETECT_H


#include <System/C++/TypeTraits/Private/IntegralConstant.hh>
#include <System/C++/TypeTraits/Private/VoidT.hh>


namespace __XVI_STD_TYPETRAITS_NS
{


// Defined elsewhere.
template <class, class> struct is_same;
template <class, class> struct is_convertible;


} // namespace __XVI_STD_TYPETRAITS_NS


namespace __XVI_STD_TYPETRAITS_NS::__detail
{


struct nonesuch
{
    ~nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    void operator=(const nonesuch&) = delete;
};


template <class _Default, class _AlwaysVoid, template <class...> class _Op, class... _Args>
struct __detector
{
    using value_t = false_type;
    using type = _Default;
};

template <class _Default, template <class...> class _Op, class... _Args>
struct __detector<_Default, void_t<_Op<_Args...>>, _Op, _Args...>
{
    using value_t = true_type;
    using type = _Op<_Args...>;
};

template <class _Default, class _AlwaysVoid, template <class...> class _Op, class... _Args>
struct __detector_value_t
    : false_type {};

template <class _Default, template <class...> class _Op, class... _Args>
struct __detector_value_t<_Default, void_t<_Op<_Args...>>, _Op, _Args...>
    : true_type {};


template <template<class...> class _Op, class... _Args>
using is_detected = typename __detector<nonesuch, void, _Op, _Args...>::value_t;

template <template <class...> class _Op, class... _Args>
inline constexpr bool is_detected_v = is_detected<_Op, _Args...>::value;

template <template<class...> class _Op, class... _Args>
using __is_detected_delayed = __detector_value_t<nonesuch, void, _Op, _Args...>;

template <template<class...> class _Op, class... _Args>
using detected_t = typename __detector<nonesuch, void, _Op, _Args...>::type;

template <class _Default, template<class...> class _Op, class... _Args>
using detected_or = __detector<_Default, void, _Op, _Args...>;

template <class _Default, template <class...> class _Op, class... _Args>
using detected_or_t = typename detected_or<_Default, _Op, _Args...>::type;

template <class _Expected, template <class...> class _Op, class... _Args>
using is_detected_exact = is_same<_Expected, detected_t<_Op, _Args...>>;

template <class _Expected, template <class...> class _Op, class... _Args>
inline constexpr bool is_detected_exact_v = is_detected_exact<_Expected, _Op, _Args...>::value;

template <class _To, template <class...> class _Op, class... _Args>
using is_detected_convertible = is_convertible<detected_t<_Op, _Args...>, _To>;

template <class _To, template <class...> class _Op, class... _Args>
inline constexpr bool is_detected_convertible_v = is_detected_convertible<_To, _Op, _Args...>::value;


} // namespace __XVI_STD_TYPETRAITS_NS::__detail


namespace __XVI_STD_TYPETRAITS_NS::experimental
{
inline namespace fundamentals_v3
{


using ::__XVI_STD_TYPETRAITS_NS::__detail::nonesuch;
using ::__XVI_STD_TYPETRAITS_NS::__detail::is_detected;
using ::__XVI_STD_TYPETRAITS_NS::__detail::is_detected_v;
using ::__XVI_STD_TYPETRAITS_NS::__detail::detected_t;
using ::__XVI_STD_TYPETRAITS_NS::__detail::detected_or;
using ::__XVI_STD_TYPETRAITS_NS::__detail::detected_or_t;
using ::__XVI_STD_TYPETRAITS_NS::__detail::is_detected_exact;
using ::__XVI_STD_TYPETRAITS_NS::__detail::is_detected_exact_v;
using ::__XVI_STD_TYPETRAITS_NS::__detail::is_detected_convertible;
using ::__XVI_STD_TYPETRAITS_NS::__detail::is_detected_convertible_v;


} // inline namespace fundamentals_v3
} // namespace __XVI_STD_TYPETRAITS_NS::experimental


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_DETECT_H */
