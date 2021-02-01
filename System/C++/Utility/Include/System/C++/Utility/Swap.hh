#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SWAP_H
#define __SYSTEM_CXX_UTILITY_SWAP_H


#include <System/C++/LanguageSupport/Utility.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Note the extra class - comes from an enable_if in the TypeTraits lib. Noexcept conditions are specified there too.
template <class _T, class>
constexpr void swap(_T& __a, _T& __b)
    noexcept(__detail::is_nothrow_move_constructible<_T>::value && __detail::is_nothrow_move_assignable<_T>::value)
{
    _T __tmp = __XVI_STD_NS::move(__a);
    __a = __XVI_STD_NS::move(__b);
    __b = __XVI_STD_NS::move(__tmp);
}

template <class _T, class _U = _T>
constexpr _T exchange(_T& __obj, _U&& __new_val)
{
    _T __old_val = __XVI_STD_NS::move(__obj);
    __obj = std::forward<_U>(__new_val);
    return __old_val;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SWAP_H */
