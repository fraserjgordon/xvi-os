#ifndef __SYSTEM_CXX_UTILITY_RANGESUTILS_H
#define __SYSTEM_CXX_UTILITY_RANGESUTILS_H


#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Ranges.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{


template <class _R, class _T>
concept __container_compatible_range = ranges::input_range<_R>
    && std::convertible_to<ranges::range_reference_t<_R>, _T>;


} // namespace __detail


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_RANGESUTILS_H */
