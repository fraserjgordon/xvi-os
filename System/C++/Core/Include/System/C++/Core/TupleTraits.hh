#ifndef __SYSTEM_CXX_CORE_TUPLETRAITS_H
#define __SYSTEM_CXX_CORE_TUPLETRAITS_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class, __XVI_STD_NS::size_t> class array;
template <class, class> class pair;
template <class...> class tuple;


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

template <std::size_t _I, class... _Types>
using __nth_type_t = typename __nth_type<_I, _Types...>::type;

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

template <class _T> inline constexpr size_t tuple_size_v = tuple_size<_T>::value;

template <size_t _I, class _T> struct tuple_element<_I, const _T> : __detail::__tuple_element_or_empty<_I, _T, add_const> {};
template <size_t _I, class _T> struct tuple_element<_I, volatile _T> : __detail::__tuple_element_or_empty<_I, _T, add_volatile> {};
template <size_t _I, class _T> struct tuple_element<_I, const volatile _T> : __detail::__tuple_element_or_empty<_I, _T, add_cv> {};

template <size_t _I, class _T> using tuple_element_t = typename tuple_element<_I, _T>::type;


namespace __detail
{


template <class _T> struct __is_pair_specialization : false_type {};
template <class _T, class _U> struct __is_pair_specialization<pair<_T, _U>> : true_type {};
template <class _T> inline constexpr bool __is_pair_specialization_v = __is_pair_specialization<_T>::value;

template <class _T> struct __is_tuple_specialisation : false_type {};
template <class... _Types> struct __is_tuple_specialisation<tuple<_Types...>> : true_type {};

template <class _T> struct __is_tuple_like : false_type {};
template <class _T, __XVI_STD_NS::size_t _N> struct __is_tuple_like<array<_T, _N>> : true_type {};
template <class _T, class _U> struct __is_tuple_like<pair<_T, _U>> : true_type {};
template <class... _Types> struct __is_tuple_like<tuple<_Types...>> : true_type {};

template <class _T>
concept __tuple_like = __is_tuple_like<remove_cvref_t<_T>>::value;

template <class _T>
concept __pair_like = __tuple_like<_T> && tuple_size_v<remove_cvref_t<_T>> == 2;


};


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_TUPLETRAITS_H */
