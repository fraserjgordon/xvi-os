#pragma once
#ifndef __SYSTEM_CXX_UTILITY_BIT_H
#define __SYSTEM_CXX_UTILITY_BIT_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/TypeTraits/Concepts.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/AddressOf.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


enum class endian
{
    little      = __ORDER_LITTLE_ENDIAN__,
    big         = __ORDER_BIG_ENDIAN__,
    native      = __BYTE_ORDER__,
};


namespace __detail
{


template <class _T>
constexpr _T __rotl(_T __x, int __s) noexcept
{
    constexpr auto _N = std::numeric_limits<_T>::digits;

    auto __r = __s % _N;
    return (__x << __r) | (__x >> (_N - __r));
}

template <class _T>
constexpr _T __rotr(_T __x, int __s)
{
    constexpr auto _N = std::numeric_limits<_T>::digits;

    auto __r = __s % _N;
    return (__x >> __r) | (__x << (_N - __r));
}


} // namespace __detail


template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
[[nodiscard]] constexpr _T rotl(_T __x, int __s) noexcept
{
    if (__s < 0)
        return __detail::__rotr(__x, -__s);

    return __detail::__rotl(__x, __s);
}

template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
[[nodiscard]] constexpr _T rotr(_T __x, int __s) noexcept
{
    if (__s < 0)
        return __detail::__rotl(__x, -__s);

    return __detail::__rotr(__x, __s);
}


template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
constexpr int countl_zero(_T __x) noexcept
{
    using std::numeric_limits;
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

template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
constexpr int countl_one(_T __x) noexcept
{
    return countl_zero(static_cast<_T>(~__x));
}

template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
constexpr int countr_zero(_T __x) noexcept
{
    using std::numeric_limits;
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

template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
constexpr int countr_one(_T __x) noexcept
{
    return countr_zero(static_cast<_T>(~__x));
}


template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
constexpr int popcount(_T __x) noexcept
{
    if constexpr (sizeof(_T) <= sizeof(unsigned int))
        return __builtin_popcount(__x);
    else if constexpr (sizeof(_T) <= sizeof(unsigned long))
        return __builtin_popcountl(__x);
    else
        return __builtin_popcountll(__x);
}


template <class _To, class _From>
    requires (sizeof(_To) == sizeof(_From)
          && is_trivially_copyable_v<_To>
          && is_trivially_copyable_v<_From>)
constexpr _To bit_cast(const _From& __from) noexcept
{
#if __has_builtin(__builtin_bit_cast)
    return __builtin_bit_cast(_To, __from);
#else
    _To __to = {};
    __builtin_memcpy(std::addressof(__to), std::addressof(__from), sizeof(_To));
    return __to;
#endif
}


template <class _T>
    requires std::integral<_T>
constexpr _T byteswap(_T __value) noexcept
{
    if constexpr (sizeof(_T) == sizeof(std::uint8_t))
        return __value;
    else if constexpr (sizeof(_T) == sizeof(std::uint16_t))
        return __builtin_bswap16(__value);
    else if constexpr (sizeof(_T) == sizeof(std::uint32_t))
        return __builtin_bswap32(__value);
    else if constexpr (sizeof(_T) == sizeof(std::uint64_t))
        return __builtin_bswap64(__value);
}


template <class _T>
    requires std::is_unsigned_v<_T>
constexpr bool has_single_bit(_T __x) noexcept
{
    return (__x & (__x - 1)) == 0;
}

template <class _T,
          class = enable_if_t<is_unsigned_v<_T>, void>>
constexpr _T bit_width(_T __x) noexcept
{
    if (__x == 0)
        return 0;

    if constexpr (sizeof(_T) <= sizeof(int))
        return static_cast<_T>(static_cast<int>(std::numeric_limits<unsigned int>::digits - __builtin_clz(__x)));
    else if constexpr (sizeof(_T) <= sizeof(long))
        return static_cast<_T>(static_cast<long>(std::numeric_limits<unsigned long>::digits - __builtin_clzl(__x)));
    else if constexpr (sizeof(_T) <= sizeof(long long))
        return static_cast<_T>(static_cast<long long>(std::numeric_limits<unsigned long long>::digits - __builtin_clzll(__x)));
}

template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
constexpr _T bit_floor(_T __x) noexcept
{
    if (__x == 0)
        return 0;

    return static_cast<_T>(_T(1) << (std::bit_width(__x) - 1));
}

template <class _T>
    requires std::is_integral_v<_T> && std::is_unsigned_v<_T>
constexpr _T bit_ceil(_T __x) noexcept
{
    // (x + (x - 1)) increases the bit_width of x by 1 iff x is not already a power of two. Then, the bit_floor of that
    // is the bit_ceil of x. We need to special-case for zero, though.

    return __x == 0 ? 1 : bit_floor(static_cast<_T>(__x + (__x - 1)));
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_BIT_H */
