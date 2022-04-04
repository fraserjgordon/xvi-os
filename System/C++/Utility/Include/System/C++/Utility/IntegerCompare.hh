#ifndef __SYSTEM_CXX_UTILITY_INTEGERCOMPARE_H
#define __SYSTEM_CXX_UTILITY_INTEGERCOMPARE_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _T, class _U>
constexpr bool cmp_equal(_T __t, _U __u) noexcept
{
    using _UT = std::make_unsigned_t<_T>;
    using _UU = std::make_unsigned_t<_U>;

    if constexpr (std::is_signed_v<_T> == std::is_signed_v<_U>)
        return __t == __u;
    else if constexpr (std::is_signed_v<_T>)
        return __t < 0 ? false : _UT(__t) == __u;
    else
        return __u < 0 ? false : __t == _UU(__u);
}

template <class _T, class _U>
constexpr bool cmp_not_equal(_T __t, _U __u) noexcept
{
    return !cmp_equal(__t, __u);
}

template <class _T, class _U>
constexpr bool cmp_less(_T __t, _U __u) noexcept
{
    using _UT = std::make_unsigned_t<_T>;
    using _UU = std::make_unsigned_t<_U>;

    if constexpr (std::is_signed_v<_T> == std::is_signed_v<_U>)
        return __t < __u;
    else if constexpr (std::is_signed_v<_T>)
        return __t < 0 ? true : _UT(__t) < __u;
    else
        return __u < 0 ? false : __t < _UU(__u);
}

template <class _T, class _U>
constexpr bool cmp_greater(_T __t, _U __u) noexcept
{
    return cmp_less(__u, __t);
}

template <class _T, class _U>
constexpr bool cmp_less_equal(_T __t, _U __u) noexcept
{
    return !cmp_greater(__t, __u);
}

template <class _T, class _U>
constexpr bool cmp_greater_equal(_T __t, _U __u) noexcept
{
    return !cmp_less(__t, __u);
}

template <class _R, class _T>
constexpr bool in_range(_T __t) noexcept
{
    return cmp_greater_equal(__t, std::numeric_limits<_R>::min())
        && cmp_less_equal(__t, std::numeric_limits<_R>::max());
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_INTEGERCOMPARE_H */
