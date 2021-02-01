#pragma once
#ifndef __SYSTEM_ABI_INTEGER_TYPES_H
#define __SYSTEM_ABI_INTEGER_TYPES_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/Integer/Arch/Capabilities.hh>


namespace System::ABI::Integer
{


using int8_t = std::int8_t;
using int16_t = std::int16_t;
using int32_t = std::int32_t;
using int64_t = std::int64_t;

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;


template <class T, class U> struct is_same_t { static constexpr bool value = false; };
template <class T> struct is_same_t<T, T> { static constexpr bool value = true; };
template <class T, class U> static constexpr bool is_same = is_same_t<T, U>::value;

template <bool C, class T, class U> struct conditional { using type = T; };
template <class T, class U> struct conditional<false, T, U> { using type = U; };
template <bool C, class T, class U> using conditional_t = typename conditional<C, T, U>::type;

template <class T> struct make_signed;

// Emulated wide integer type (required when emulating e.g. 128-bit division using 256-bit temporaries).
template <class UHalfT, class SHalfT = UHalfT>
struct emulated_integer
{
    // The two parts of the integer. The order is arbitrary but matches endianness for consistency.
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    struct parts_t { SHalfT high = {}; UHalfT low = {}; };
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    struct parts_t { UHalfT low = {}; SHalfT high = {}; };
#endif
    parts_t parts = {};

    // Implicit conversion to the corresponding signed/unsigned partner.
    using counterpart_t = conditional_t<is_same<UHalfT, SHalfT>,
                                        emulated_integer<UHalfT, typename make_signed<UHalfT>::type>,
                                        emulated_integer<UHalfT>>;
    constexpr operator counterpart_t() const
    {
        typename counterpart_t::parts_t p;
        p.low = parts.low;
        p.high = static_cast<decltype(p.high)>(parts.high);
        return counterpart_t{.parts = p};
    }
};


#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
using int128_t = __int128_t;
using uint128_t = __uint128_t;
using int256_t = emulated_integer<uint128_t, int128_t>;
using uint256_t = emulated_integer<uint128_t>;
#else
using int128_t = emulated_integer<uint64_t, int64_t>;
using uint128_t = emulated_integer<uint64_t>;
#endif


template <class T> struct zero_t { static constexpr T value = 0; };
template <class T> static constexpr T zero_value = zero_t<T>::value;
template <class T, class U> struct zero_t<emulated_integer<T, U>> { static constexpr emulated_integer<T, U> value {{0, 0}}; };


template <class T> struct double_bits;
template <class T> using double_bits_t = typename double_bits<T>::type;
template <> struct double_bits<int32_t> { using type = int64_t; };
template <> struct double_bits<int64_t> { using type = int128_t; };
template <> struct double_bits<uint32_t> { using type = uint64_t; };
template <> struct double_bits<uint64_t> { using type = uint128_t; };
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
template <> struct double_bits<int128_t> { using type = int256_t; };
template <> struct double_bits<uint128_t> { using type = uint256_t; };
#endif

template <class T> struct half_bits;
template <class T> using half_bits_t = typename half_bits<T>::type;
template <> struct half_bits<int32_t> { using type = int16_t; };
template <> struct half_bits<int64_t> { using type = int32_t; };
template <> struct half_bits<int128_t> { using type = int64_t; };
template <> struct half_bits<uint32_t> { using type = uint16_t; };
template <> struct half_bits<uint64_t> { using type = uint32_t; };
template <> struct half_bits<uint128_t> { using type = uint64_t; };
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
template <> struct half_bits<int256_t> { using type = int128_t; };
template <> struct half_bits<uint256_t> { using type = uint128_t; };
#endif

template <class T> struct make_signed;
template <class T> using make_signed_t = typename make_signed<T>::type;
template <> struct make_signed<int16_t> { using type = int16_t; };
template <> struct make_signed<int32_t> { using type = int32_t; };
template <> struct make_signed<int64_t> { using type = int64_t; };
template <> struct make_signed<int128_t> { using type = int128_t; };
template <> struct make_signed<uint16_t> { using type = int16_t; };
template <> struct make_signed<uint32_t> { using type = int32_t; };
template <> struct make_signed<uint64_t> { using type = int64_t; };
template <> struct make_signed<uint128_t> { using type = int128_t; };
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
template <> struct make_signed<int256_t> { using type = int256_t; };
template <> struct make_signed<uint256_t> { using type = int256_t; };
#endif

template <class T> struct make_unsigned;
template <class T> using make_unsigned_t = typename make_unsigned<T>::type;
template <> struct make_unsigned<int16_t> { using type = uint16_t; };
template <> struct make_unsigned<int32_t> { using type = uint32_t; };
template <> struct make_unsigned<int64_t> { using type = uint64_t; };
template <> struct make_unsigned<int128_t> { using type = uint128_t; };
template <> struct make_unsigned<uint16_t> { using type = uint16_t; };
template <> struct make_unsigned<uint32_t> { using type = uint32_t; };
template <> struct make_unsigned<uint64_t> { using type = uint64_t; };
template <> struct make_unsigned<uint128_t> { using type = uint128_t; };
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
template <> struct make_unsigned<int256_t> { using type = uint256_t; };
template <> struct make_unsigned<uint256_t> { using type = uint256_t; };
#endif

template <class T> struct split_int_signed;
template <class T> struct split_int_unsigned;
template <class T> struct split_int_terminal;

template <class T> struct split_int;
template <> struct split_int<int16_t> { using type = split_int_terminal<int16_t>; };
template <> struct split_int<int32_t> { using type = split_int_signed<int32_t>; };
template <> struct split_int<int64_t> { using type = split_int_signed<int64_t>; };
template <> struct split_int<int128_t> { using type = split_int_signed<int128_t>; };
template <> struct split_int<uint16_t> { using type = split_int_terminal<uint16_t>; };
template <> struct split_int<uint32_t> { using type = split_int_unsigned<uint32_t>; };
template <> struct split_int<uint64_t> { using type = split_int_unsigned<uint64_t>; };
template <> struct split_int<uint128_t> { using type = split_int_unsigned<uint128_t>; };
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
template <> struct split_int<int256_t> { using type = split_int_signed<int256_t>; };
template <> struct split_int<uint256_t> { using type = split_int_unsigned<uint256_t>; };
#endif
template <class T> using split_int_t = typename split_int<T>::type;


template <class T>
struct split_int_signed
{
    using half_t = half_bits_t<T>;
    using uhalf_t = make_unsigned_t<half_t>;
    using shalf_t = half_t;
    using uhalf_split_t = split_int_t<uhalf_t>;
    using shalf_split_t = split_int_t<shalf_t>;

    using upper_half_t = shalf_t;
    using lower_half_t = uhalf_t;

    struct parts_t
    {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        union { shalf_t high; shalf_split_t high_s; uhalf_t uhigh; };
        union { uhalf_t low; uhalf_split_t low_s; };
    #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        union { uhalf_t low; uhalf_split_t low_s; };
        union { shalf_t high; shalf_split_t high_s; uhalf_t uhigh; };
    #else
    #  error Unsupported endianness
    #endif
    };

    union
    {
        T       whole = {};
        parts_t split;
    };

    constexpr split_int_signed() = default;
    constexpr split_int_signed(const split_int_signed&) = default;
    constexpr split_int_signed(split_int_signed&&) = default;
    constexpr split_int_signed(T t) : whole{t} {}
    constexpr split_int_signed(upper_half_t u, lower_half_t l)
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      : split{.high = u, .low = l} {}
    #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      : split{.low = l, .high = u} {}
    #endif

    split_int_signed& operator= (const split_int_signed&) = default;
    split_int_signed& operator= (split_int_signed&&) = default;

    ~split_int_signed() = default;

    constexpr operator T() const { return whole; }
};

template <class T>
struct split_int_unsigned
{
    using half_t = half_bits_t<T>;
    using half_split_t = split_int_t<half_t>;

    using upper_half_t = half_t;
    using lower_half_t = half_t;

    struct parts_t
    {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        union { half_t high; half_split_t high_s; half_t uhigh; };
        union { half_t low; half_split_t low_s; };
    #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        union { half_t low; half_split_t low_s; };
        union { half_t high; half_split_t high_s; half_t uhigh; };
    #else
    #  error Unsupported endianness
    #endif
    };

    union
    {
        T       whole = {};
        parts_t split;
    };

    constexpr split_int_unsigned() = default;
    constexpr split_int_unsigned(const split_int_unsigned&) = default;
    constexpr split_int_unsigned(split_int_unsigned&&) = default;
    constexpr split_int_unsigned(T t) : whole{t} {}
    constexpr split_int_unsigned(upper_half_t u, lower_half_t l)
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
      : split{.high = u, .low = l} {}
    #elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
      : split{.low = l, .high = u} {}
    #endif

    split_int_unsigned& operator= (const split_int_unsigned&) = default;
    split_int_unsigned& operator= (split_int_unsigned&&) = default;

    ~split_int_unsigned() = default;

    constexpr operator T() const { return whole; }
};

template <class T>
struct split_int_terminal
{
    T whole;

    constexpr split_int_terminal() = default;
    constexpr split_int_terminal(const split_int_terminal&) = default;
    constexpr split_int_terminal(split_int_terminal&&) = default;
    constexpr split_int_terminal(T t) : whole{t} {}

    split_int_terminal& operator= (const split_int_terminal&) = default;
    split_int_terminal& operator= (split_int_terminal&&) = default;

    ~split_int_terminal() = default;

    operator T() const { return whole; }
};


} // namespace System::ABI::Integer


#if !__SYSTEM_ABI_INTEGER_PROVIDE_128BIT
using __int128_t = System::ABI::Integer::int128_t;
using __uint128_t = System::ABI::Integer::uint128_t;
#endif


#endif /* ifndef __SYSTEM_ABI_INTEGER_TYPES_H */
