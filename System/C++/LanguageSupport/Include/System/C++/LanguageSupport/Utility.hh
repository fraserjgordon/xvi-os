#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_UTILITY_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_UTILITY_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


template <class _T> constexpr _T&& forward(remove_reference_t<_T>& __t) noexcept
    { return static_cast<_T&&>(__t); }
template <class _T> constexpr _T&& forward(remove_reference_t<_T>&& __t) noexcept
    { return static_cast<_T&&>(__t); }

template <class _T> constexpr remove_reference_t<_T>&& move(_T&& __t) noexcept
    { return static_cast<remove_reference_t<_T>&&>(__t); }

template <class _T>
constexpr conditional_t<!is_nothrow_move_constructible_v<_T> && is_copy_constructible_v<_T>, const _T&, _T&&>
move_if_noexcept(_T& __x)
{
    if constexpr (!is_nothrow_move_constructible_v<_T> && is_copy_constructible_v<_T>)
        return __x;
    else
        return std::move(__x);
}


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_UTILITY_H */