#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_UTILITY_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_UTILITY_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS_DECL
{


namespace __detail
{


template <class _A, class _B> using __copy_const = __XVI_STD_NS::conditional_t<__XVI_STD_NS::is_const_v<_A>, const _B, _B>;

template <class _A, class _B> using __override_ref = __XVI_STD_NS::conditional_t<__XVI_STD_NS::is_rvalue_reference_v<_A>, __XVI_STD_NS::remove_reference_t<_B>&&, _B&>;


} // namespace __detail


template <class _T> constexpr _T&& forward(remove_reference_t<_T>& __t) noexcept
    { return static_cast<_T&&>(__t); }
template <class _T> constexpr _T&& forward(remove_reference_t<_T>&& __t) noexcept
    { return static_cast<_T&&>(__t); }

template <class _T, class _U>
[[nodiscard]] constexpr auto forward_like(_U&& __x) noexcept
{
    using _V = __detail::__override_ref<_T&&, __detail::__copy_const<__XVI_STD_NS::remove_reference_t<_T>, __XVI_STD_NS::remove_reference_t<_U>>>;

    return static_cast<_V>(__x);
}

template <class _T> constexpr remove_reference_t<_T>&& move(_T&& __t) noexcept
    { return static_cast<remove_reference_t<_T>&&>(__t); }

template <class _T>
constexpr conditional_t<!is_nothrow_move_constructible_v<_T> && is_copy_constructible_v<_T>, const _T&, _T&&>
move_if_noexcept(_T& __x)
{
    if constexpr (!is_nothrow_move_constructible_v<_T> && is_copy_constructible_v<_T>)
        return __x;
    else
        return __XVI_STD_NS::move(__x);
}

template <class _T>
constexpr add_const_t<_T>& as_const(_T& __t) noexcept
{
    return __t;
}

template <class _T>
constexpr underlying_type_t<_T> to_underlying(_T __value) noexcept
{
    return static_cast<underlying_type_t<_T>>(__value);
}

[[noreturn]] inline void unreachable()
{
#if __has_builtin(__builtin_unreachable)
    __builtin_unreachable();
#endif
}


} // namespace __XVI_STD_LANGSUPPORT_NS_DECL


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_UTILITY_H */