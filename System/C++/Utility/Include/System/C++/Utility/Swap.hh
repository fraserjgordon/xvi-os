#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SWAP_H
#define __SYSTEM_CXX_UTILITY_SWAP_H


#include <System/C++/Core/Swap.hh>
#include <System/C++/LanguageSupport/Utility.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace ranges
{


namespace __detail
{


// Forward declarations.
struct __swap;
struct __swap_ranges;


// We use an extraneous template that depends on _T here to delay the evaluation of the type until swap_ranges is
// actually available.
template <class _T>
using __delayed_swap_ranges = ranges::__detail::__swap_ranges;





} // namespace __detail


inline namespace __swap
{

inline constexpr __detail::__swap swap = {};

} // inline namespace __swap


} // namespace ranges


template <class _T, size_t _N>
    requires std::is_swappable_v<_T>
constexpr void swap(_T (&__a)[_N], _T (&__b)[_N])
    noexcept(std::is_nothrow_swappable_v<_T>)
{
    ranges::__detail::__delayed_swap_ranges<_T>{}(__a, __b);   
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SWAP_H */
