#pragma once
#ifndef __SYSTEM_CXX_UTILITY_EXECUTIONPOLICY_H
#define __SYSTEM_CXX_UTILITY_EXECUTIONPOLICY_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class> struct is_execution_policy : false_type {};
template <class _T> inline constexpr bool is_execution_policy_v = is_execution_policy<_T>::value;


namespace execution
{

class sequenced_policy {};
class parallel_policy {};
class parallel_unsequenced_policy {};
class unsequenced_policy {};

inline constexpr sequenced_policy               seq{};
inline constexpr parallel_policy                par{};
inline constexpr parallel_unsequenced_policy    par_unseq{};
inline constexpr unsequenced_policy             unseq{};

} // namespace execution


template <> struct is_execution_policy<execution::sequenced_policy> : true_type {};
template <> struct is_execution_policy<execution::parallel_policy> : true_type {};
template <> struct is_execution_policy<execution::parallel_unsequenced_policy> : true_type {};
template <> struct is_execution_policy<execution::unsequenced_policy> : true_type {};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_EXECUTIONPOLICY_H */
