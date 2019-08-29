#pragma once
#ifndef __SYSTEM_CXX_UTILITY_BIT_H
#define __SYSTEM_CXX_UTILITY_BIT_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/AddressOf.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class _T, class = enable_if_t<is_unsigned_v<_T>, void>> constexpr _T rotl(_T, int) noexcept;
template <class _T, class = enable_if_t<is_unsigned_v<_T>, void>> constexpr _T rotr(_T, int) noexcept;


template <class _T, class>
[[nodiscard]] constexpr _T rotl(_T __x, int __s) noexcept
{
    if (__s < 0)
        return rotr(__x, -__s);

    constexpr auto _N = __XVI_STD_LANGSUPPORT_NS::numeric_limits<_T>::digits;

    auto __r = __s % _N;
    return (__x << __r) | (__x >> (_N - __r));
}

template <class _T, class>
[[nodiscard]] constexpr _T rotr(_T __x, int __s) noexcept
{
    if (__s < 0)
        return rotl(__x, -__s);

    constexpr auto _N = __XVI_STD_LANGSUPPORT_NS::numeric_limits<_T>::digits;

    auto __r = __s % _N;
    return (__x >> __r) | (__x << (_N - __r));
}


template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr int countl_zero(_T __x) noexcept
{
    using __XVI_STD_LANGSUPPORT_NS::numeric_limits;
    constexpr auto _N = numeric_limits<_T>::digits;

    int __lz = 0;
    int __len = 0;
    if constexpr (sizeof(_T) <= sizeof(unsigned int))
    {
        __lz = __builtin_clz(__x);
        __len = numeric_limits<unsigned int>::digits;
    }
    else if constexpr (sizeof(_T) <= sizeof(unsigned long))
    {
        __lz = __builtin_clzl(__x);
        __len = numeric_limits<unsigned long>::digits;
    }
    else
    {
        __lz = __builtin_clzll(__x);
        __len = numeric_limits<unsigned long long>::digits;
    }

    return __lz - (__len - _N);
}

template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr int countl_one(_T __x) noexcept
{
    return countl_zero(~__x);
}

template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr int countr_zero(_T __x) noexcept
{
    using __XVI_STD_LANGSUPPORT_NS::numeric_limits;
    constexpr auto _N = numeric_limits<_T>::digits;

    int __tz = 0;
    if constexpr (sizeof(_T) <= sizeof(unsigned int))
    {
        __tz = __builtin_ctz(__x);
    }
    else if constexpr (sizeof(_T) <= sizeof(unsigned long))
    {
        __tz = __builtin_ctzl(__x);
    }
    else
    {
        __tz = __builtin_ctzll(__x);
    }

    return (__tz > _N) ? _N : __tz;
}

template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr int countr_one(_T __x) noexcept
{
    return countr_zero(~__x);
}


template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr int popcount(_T __x) noexcept
{
    if constexpr (sizeof(_T) <= sizeof(unsigned int))
        return __builtin_popcount(__x);
    else if constexpr (sizeof(_T) <= sizeof(unsigned long))
        return __builtin_popcountl(__x);
    else
        return __builtin_popcountll(__x);
}


template <class _To, class _From,
          class = enable_if_t<sizeof(_To) == sizeof(_From)
                              && is_trivially_copyable_v<_To>
                              && is_trivially_copyable_v<_From>, void>>
constexpr _To bit_cast(const _From& __from) noexcept
{
    _To __to = {};
    __builtin_memcpy(__XVI_STD_NS::addressof(__to), __XVI_STD_NS::addressof(__from), sizeof(_To));
    return __to;
}


template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr bool ispow2(_T __x) noexcept
{
    return (__x & (__x - 1)) == 0;
}

template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr _T log2p1(_T __x) noexcept
{
    if (__x == 0)
        return 0;

    if constexpr (sizeof(_T) <= sizeof(int))
        return 8 * sizeof(int) - __builtin_clz(__x);
    else if constexpr (sizeof(_T) <= sizeof(long))
        return 8 * sizeof(long) - __builtin_clzl(__x);
    else if constexpr (sizeof(_T) <= sizeof(long long))
        return 8 * sizeof(long long) - __builtin_clzll(__x);
}

template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr _T ceil2(_T __x) noexcept
{
    _T __l = __XVI_STD_UTILITY_NS::log2p1(__x);
    if (__l == 0)
        return 0;

    _T __v = _T(1) << (__l - 1);
    return (__v < __x) ? (__v << 1) : __v;
}

template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr _T floor2(_T __x) noexcept
{
    _T __l = __XVI_STD_UTILITY_NS::log2p1(__x);
    if (__l == 0)
        return 0;

    _T __v = _T(1) << (__l - 1);
    return __v;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_BIT_H */
