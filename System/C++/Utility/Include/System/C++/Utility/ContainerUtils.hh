#ifndef __SYSTEM_CXX_UTILITY_CONTAINERUTILS_H
#define __SYSTEM_CXX_UTILITY_CONTAINERUTILS_H


#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS::__detail
{


template <class _T>
concept __maybe_iterator = (!std::integral<_T>);


template <class _T>
concept __maybe_allocator = requires
    {
        typename _T::value_type;
        std::declval<_T&>().allocate(std::size_t{});
    };


} // namespace __XVI_STD_UTILITY_NS::__detail


#endif /* ifndef __SYSTEM_CXX_UTILITY_CONTAINERUTILS_H */
