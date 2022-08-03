#ifndef __SYSTEM_UTILITY_ENDIAN_ENDIAN_H
#define __SYSTEM_UTILITY_ENDIAN_ENDIAN_H


#if defined(__XVI_NO_STDLIB)
#  include <System/C++/LanguageSupport/StdInt.hh>
#  include <System/C++/TypeTraits/TypeTraits.hh>
#  include <System/C++/Utility/Bit.hh>
#else
#  include <bit>
#  include <cstdint>
#  include <type_traits>
#  include <utility>
#endif


namespace System::Utility::Endian
{


constexpr std::endian opposite_endian(std::endian e) noexcept
{
    return (e == std::endian::big) ? std::endian::little : std::endian::big;
}


constexpr std::uint16_t byte_swap_16(std::uint16_t x) noexcept
{
    if constexpr (__has_builtin(__builtin_bswap16))
        return __builtin_bswap16(x);
    else
        return ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
}

constexpr std::uint32_t byte_swap_32(std::uint32_t x) noexcept
{
    if constexpr (__has_builtin(__builtin_bswap32))
        return __builtin_bswap32(x);
    else
        return byte_swap_16((x & 0xFFFF0000U) >> 16) | (byte_swap_16(x & 0x0000FFFFU) << 16);
}

constexpr std::uint64_t byte_swap_64(std::uint64_t x) noexcept
{
    if constexpr (__has_builtin(__builtin_bswap64))
        return __builtin_bswap64(x);
    else
        return byte_swap_32((x & 0xFFFFFFFF'00000000U) >> 32) | (byte_swap_32(x & 0x00000000'FFFFFFFFU) << 32);
}

constexpr std::int16_t byte_swap_16(std::int16_t x) noexcept
{
    return static_cast<std::int16_t>(byte_swap_16(static_cast<std::uint16_t>(x)));
}

constexpr std::int32_t byte_swap_32(std::int32_t x) noexcept
{
    return static_cast<std::int32_t>(byte_swap_32(static_cast<std::uint32_t>(x)));
}

constexpr std::int64_t byte_swap_64(std::int64_t x) noexcept
{
    return static_cast<std::int64_t>(byte_swap_64(static_cast<std::uint64_t>(x)));
}


namespace detail
{


constexpr std::uint8_t byte_swap(std::uint8_t x) noexcept
{
    return x;
}

constexpr std::uint16_t byte_swap(std::uint16_t x) noexcept
{
    return byte_swap_16(x);
}

constexpr std::uint32_t byte_swap(std::uint32_t x) noexcept
{
    return byte_swap_32(x);
}

constexpr std::uint64_t byte_swap(std::uint64_t x) noexcept
{
    return byte_swap_64(x);
}

constexpr std::int8_t byte_swap(std::int8_t x) noexcept
{
    return x;
}

constexpr std::int16_t byte_swap(std::int16_t x) noexcept
{
    return byte_swap_16(x);
}

constexpr std::int32_t byte_swap(std::int32_t x) noexcept
{
    return byte_swap_32(x);
}

constexpr std::int64_t byte_swap(std::int64_t x) noexcept
{
    return byte_swap_64(x);
}

constexpr char16_t byte_swap(char16_t x) noexcept
{
    return static_cast<char16_t>(byte_swap_16(static_cast<std::uint16_t>(x)));
}

constexpr char32_t byte_swap(char32_t x) noexcept
{
    return static_cast<char32_t>(byte_swap_32(static_cast<std::uint32_t>(x)));
}

template <class T>
    requires std::is_enum_v<T>
constexpr T byte_swap(T x) noexcept
{
    return static_cast<T>(byte_swap(static_cast<std::underlying_type_t<T>>(x)));
}


struct byte_swap_t
{
    template <class T>
    constexpr auto operator()(T x) const noexcept
        requires requires (T x) { x.byte_swap(); }
    {
        return x.byte_swap();
    }

    template <class T>
    constexpr auto operator()(T x) const noexcept
        requires (!requires (T x) { x.byte_swap(); }
              &&   requires (T x) { byte_swap(x); })
    {
        return byte_swap(x);
    }
};


} // namespace detail


inline constexpr detail::byte_swap_t byte_swap = {};


template <class T, bool Swapped>
    requires std::is_standard_layout_v<T> 
          && std::is_trivially_destructible_v<T>
          && requires (T x) { byte_swap(x); }
class basic_endian_type
{
public:

    friend class basic_endian_type<T, !Swapped>;

    using underlying_native_endian_type = T;
    using opposite_endian_type          = basic_endian_type<T, !Swapped>;

    constexpr basic_endian_type() noexcept :
        m_store(bswap(T())) {}

    constexpr basic_endian_type(const basic_endian_type& x) noexcept = default;

    constexpr basic_endian_type(basic_endian_type&& x) noexcept = default;

    constexpr basic_endian_type(T x) noexcept :
        m_store(bswap(x)) {}

    constexpr basic_endian_type(const opposite_endian_type& x) noexcept :
        m_store(Endian::byte_swap(x.m_store)) {}

    constexpr basic_endian_type& operator=(const basic_endian_type& x) noexcept = default;

    constexpr basic_endian_type& operator=(basic_endian_type&& x) noexcept = default;

    constexpr basic_endian_type& operator=(T x) noexcept
    {
        m_store = bswap(x);
        return *this;
    }

    constexpr basic_endian_type& operator=(const opposite_endian_type& x) noexcept
    {
        m_store = Endian::byte_swap(x.m_store);
        return *this;
    }

    ~basic_endian_type() = default;

    constexpr operator T() const noexcept
    {
        return bswap(m_store);
    }

    friend constexpr auto operator<=>(const basic_endian_type& x, const basic_endian_type& y) noexcept
    {
        return bswap(x.m_store) <=> bswap(y.m_store);
    }

    friend constexpr auto operator<=>(const basic_endian_type& x, const T& y) noexcept
    {
        return bswap(x.m_store) <=> y;
    }

    constexpr auto byte_swap() const noexcept
    {
        return Endian::byte_swap(m_store);
    }

    static constexpr T bswap(T x) noexcept
    {
        if constexpr (Swapped)
            return Endian::byte_swap(x);
        else
            return x;
    }

private:

    T m_store;
};


template <class T, std::endian Endian>
using fixed_endian_type = basic_endian_type<T, Endian != std::endian::native>;

template <class T>
using big_endian_type = fixed_endian_type<T, std::endian::big>;

template <class T>
using little_endian_type = fixed_endian_type<T, std::endian::little>;

template <class T>
using native_endian_type = fixed_endian_type<T, std::endian::native>;


using uint16le_t = little_endian_type<std::uint16_t>;
using uint32le_t = little_endian_type<std::uint32_t>;
using uint64le_t = little_endian_type<std::uint64_t>;

using int16le_t = little_endian_type<std::int16_t>;
using int32le_t = little_endian_type<std::int32_t>;
using int64le_t = little_endian_type<std::int64_t>;

using uint16be_t = big_endian_type<std::uint16_t>;
using uint32be_t = big_endian_type<std::uint32_t>;
using uint64be_t = big_endian_type<std::uint64_t>;

using int16be_t = big_endian_type<std::int16_t>;
using int32be_t = big_endian_type<std::int32_t>;
using int64be_t = big_endian_type<std::int64_t>;

using uint16ne_t = native_endian_type<std::uint16_t>;
using uint32ne_t = native_endian_type<std::uint32_t>;
using uint64ne_t = native_endian_type<std::uint64_t>;

using int16ne_t = native_endian_type<std::int16_t>;
using int32ne_t = native_endian_type<std::int32_t>;
using int64ne_t = native_endian_type<std::int64_t>;

using char16le_t = little_endian_type<char16_t>;
using char32le_t = little_endian_type<char32_t>;

using char16be_t = big_endian_type<char16_t>;
using char32be_t = big_endian_type<char32_t>;

using char16ne_t = native_endian_type<char16_t>;
using char32ne_t = native_endian_type<char32_t>;


template <class T>
struct is_endian_type : std::false_type {};

template <class T, bool Swap>
struct is_endian_type<basic_endian_type<T, Swap>> : std::true_type {};

template <class T>
inline constexpr bool is_endian_type_v = is_endian_type<T>::value;


template <std::endian Endian>
struct endian_traits
{
    static constexpr std::endian order = Endian;

    using uint8_t       = std::uint8_t;
    using uint16_t      = fixed_endian_type<std::uint16_t, Endian>;
    using uint32_t      = fixed_endian_type<std::uint32_t, Endian>;
    using uint64_t      = fixed_endian_type<std::uint64_t, Endian>;

    using int8_t        = std::int8_t;
    using int16_t       = fixed_endian_type<std::int16_t, Endian>;
    using int32_t       = fixed_endian_type<std::int32_t, Endian>;
    using int64_t       = fixed_endian_type<std::int64_t, Endian>;
};

using little_endian_traits = endian_traits<std::endian::little>;
using big_endian_traits = endian_traits<std::endian::big>;
using native_endian_traits = endian_traits<std::endian::native>;


} // namespace System::Utility::Endian


#endif /* ifndef __SYSTEM_UTILITY_ENDIAN_ENDIAN_H */
