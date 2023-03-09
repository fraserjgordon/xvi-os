#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONDITIONAL_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONDITIONAL_H


#include <System/C++/TypeTraits/Private/IntegralConstant.hh>


namespace __XVI_STD_TYPETRAITS_NS_DECL
{


namespace __detail
{

template <bool _B, class _T> struct enable_if { using type = _T; };
template <class _T> struct enable_if<false, _T> {};

template <bool _B, class _Then, class _Else> struct conditional { using type = _Then; };
template <class _Then, class _Else> struct conditional<false, _Then, _Else> { using type = _Else; };

template <bool _Value, class _BFirst, class... _BRest> struct __conjunction_helper
    { using type = _BFirst; };
template <bool _Value, class _B> struct __conjunction_helper<_Value, _B>
    { using type = _B; };
template <class _BFirst, class _BSecond, class... _BRest> struct __conjunction_helper<true, _BFirst, _BSecond, _BRest...>
    { using type = typename __conjunction_helper<bool(_BSecond::value), _BSecond, _BRest...>::type; };

template <class _BFirst, class... _BRest> struct conjunction
    : __conjunction_helper<bool(_BFirst::value), _BFirst, _BRest...>::type {};


template <bool _Value, class _BFirst, class... _BRest> struct __disjunction_helper
    { using type = _BFirst; };
template <bool _Value, class _B> struct __disjunction_helper<_Value, _B>
    { using type = _B; };
template <class _BFirst, class _BSecond, class... _BRest> struct __disjunction_helper<false, _BFirst, _BSecond, _BRest...>
    { using type = typename __disjunction_helper<bool(_BSecond::value), _BSecond, _BRest...>::type; };

template <class _BFirst, class... _BRest> struct disjunction
    : __disjunction_helper<bool(_BFirst::value), _BFirst, _BRest...>::type {};


template <class _B> struct negation : bool_constant<!bool(_B::value)> {};


} // namespace __detail


template <bool _B, class _T> struct enable_if : __detail::enable_if<_B, _T> {};
template <bool _B, class _T> using enable_if_t = typename enable_if<_B, _T>::type;

template <bool _B, class _Then, class _Else> struct conditional : __detail::conditional<_B, _Then, _Else> {};
template <bool _B, class _Then, class _Else> using conditional_t = typename conditional<_B, _Then, _Else>::type;

template <class... _B> struct conjunction : __detail::conjunction<true_type, _B...> {};
template <class... _B> inline constexpr bool conjunction_v = conjunction<_B...>::value;

template <class... _B> struct disjunction : __detail::disjunction<false_type, _B...> {};
template <class... _B> inline constexpr bool disjunction_v = disjunction<_B...>::value;

template <class _B> struct negation : __detail::negation<_B> {};
template <class _B> inline constexpr bool negation_v = negation<_B>::value;


} // namespace __XVI_STD_TYPETRAITS_NS_DECL


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONDITIONAL_H */
