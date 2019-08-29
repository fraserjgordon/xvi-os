#pragma once
#ifndef __SYSTEM_CXX_UTILITY_PRIVATE_INPLACE_H
#define __SYSTEM_CXX_UTILITY_PRIVATE_INPLACE_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


struct piecewise_construct_t
    { explicit piecewise_construct_t() = default; };
inline constexpr piecewise_construct_t piecewise_construct {};

struct in_place_t
    { explicit in_place_t() = default; };
inline constexpr in_place_t in_place {};

template <class _T> struct in_place_type_t
    { explicit in_place_type_t() = default; };
template <class _T> inline constexpr in_place_type_t<_T> in_place_type {};

template <size_t _I> struct in_place_index_t
    { explicit in_place_index_t() = default; };
template <size_t _I> inline constexpr in_place_index_t<_I> in_place_index {};


namespace __detail
{

template <class _T> struct __is_in_place_type_specialization : false_type {};
template <class _T> struct __is_in_place_type_specialization<in_place_type_t<_T>> : true_type {};
template <class _T> inline constexpr bool __is_in_place_type_specialization_v = __is_in_place_type_specialization<_T>::value;

template <class _T> struct __is_in_place_index_specialization : false_type {};
template <size_t _I> struct __is_in_place_index_specialization<in_place_index_t<_I>> : true_type {};
template <class _T> inline constexpr bool __is_in_place_index_specialization_v = __is_in_place_index_specialization<_T>::value;

template <class _T> inline constexpr bool __is_in_place_specialization_v = __is_in_place_type_specialization_v<_T> || __is_in_place_index_specialization_v<_T>;

} // namespace __detail


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_PRIVATE_INPLACE_H */
