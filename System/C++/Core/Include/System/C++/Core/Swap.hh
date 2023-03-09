#ifndef __SYSTEM_CXX_CORE_SWAP_H
#define __SYSTEM_CXX_CORE_SWAP_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


template <class _T>
    requires __detail::is_move_constructible<_T>::value && __detail::is_move_assignable<_T>::value
constexpr void swap(_T& __a, _T& __b)
    noexcept(__detail::is_nothrow_move_constructible<_T>::value && __detail::is_nothrow_move_assignable<_T>::value)
{
    _T __tmp = std::move(__a);
    __a = std::move(__b);
    __b = std::move(__tmp);
}


template <class _T, class _U = _T>
constexpr _T exchange(_T& __obj, _U&& __new_val)
{
    _T __old_val = std::move(__obj);
    __obj = std::forward<_U>(__new_val);
    return __old_val;
}


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_SWAP_H */