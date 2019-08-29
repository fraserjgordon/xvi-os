#pragma once
#ifndef __SYSTEM_CXX_UTILITY_PRIVATE_TUPLETRAITS_H
#define __SYSTEM_CXX_UTILITY_PRIVATE_TUPLETRAITS_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Declared-but-not-defined classes.
template <class> struct tuple_size;
template <size_t, class> struct tuple_element;


namespace __detail
{

template <size_t _I, class... _Types> struct __nth_type;
template <size_t _I, class _T, class... _Types> struct __nth_type<_I, _T, _Types...>
    { using type = typename __nth_type<_I-1, _Types...>::type; };
template <class _T, class... _Types> struct __nth_type<0, _T, _Types...>
    { using type = _T; };

template <class _T, bool _Seen, class... _Types> struct __type_unique_in_pack_helper;
template <class _T, class... _Types> struct __type_unique_in_pack_helper<_T, true, _T, _Types...>
    : false_type {};
template <class _T, bool _Seen, class _U, class... _Types> struct __type_unique_in_pack_helper<_T, _Seen, _U, _Types...>
    : bool_constant<__type_unique_in_pack_helper<_T, _Seen || is_same_v<_T, _U>, _Types...>::value> {};
template <class _T, bool _Seen> struct __type_unique_in_pack_helper<_T, _Seen>
    : true_type {};

template <class _T, class... _Types> struct __type_unique_in_pack : __type_unique_in_pack_helper<_T, false, _Types...> {};
template <class _T, class... _Types> inline constexpr bool __type_unique_in_pack_v = __type_unique_in_pack<_T, _Types...>::value;


template <class _T> using __value_detector = decltype(_T::value);
template <class _T> using __type_detector = decltype(declval<typename _T::type>());

template <class _T, bool = is_detected_v<__value_detector, tuple_size<_T>>> struct __tuple_size_or_empty {};
template <class _T> struct __tuple_size_or_empty<_T, true> : integral_constant<decltype(_T::value), _T::value> {};

template <size_t _I, class _T, template <class> class _Modifier = type_identity, bool = is_detected_v<__type_detector, tuple_element<_I, _T>>> struct __tuple_element_or_empty {};
template <size_t _I, class _T, template <class> class _Modifier> struct __tuple_element_or_empty<_I, _T, _Modifier, true> { using type = typename _Modifier<typename tuple_element<_I, _T>::type>::type; };

} // namespace __detail


template <class _T> struct tuple_size<const _T> : __detail::__tuple_size_or_empty<_T> {};
template <class _T> struct tuple_size<volatile _T> : __detail::__tuple_size_or_empty<_T> {};
template <class _T> struct tuple_size<const volatile _T> : __detail::__tuple_size_or_empty  <_T> {};

template <size_t _I, class _T> struct tuple_element<_I, const _T> : __detail::__tuple_element_or_empty<_I, _T, add_const> {};
template <size_t _I, class _T> struct tuple_element<_I, volatile _T> : __detail::__tuple_element_or_empty<_I, _T, add_volatile> {};
template <size_t _I, class _T> struct tuple_element<_I, const volatile _T> : __detail::__tuple_element_or_empty<_I, _T, add_cv> {};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_PRIVATE_TUPLETRAITS_H */
