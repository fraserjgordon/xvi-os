#include <algorithm>
#include <array>
#include <cstddef>
#include <string_view>


namespace System::Compress::Deflate
{


template <char... Cs, std::size_t... Idx>
static consteval auto BitsToBytesImpl(std::index_sequence<Idx...>)
{
    constexpr auto BitLen = sizeof...(Cs);
    constexpr auto ByteLen = (BitLen + 7) / 8;

    using array_t = std::array<std::byte, ByteLen>;

    array_t arr {};
    auto setbit = [&arr](char c, std::size_t idx)
    {
        // Pack from the least significant bit upwards.
        arr[idx / 8] |= static_cast<std::byte>((c == '0' ? 0 : 1) << (idx % 8));
    };

    (setbit(Cs, Idx), ...);

    return arr;
}

template <char... Cs>
static consteval auto BitsToBytes()
{
    return BitsToBytesImpl<Cs...>(std::make_index_sequence<sizeof...(Cs)>());
}

template <char... Cs>
static consteval auto operator""_bits ()
{
    return BitsToBytes<Cs...>();
}


template <std::size_t N>
struct BitsStruct
{
    consteval BitsStruct(const char (&str)[N])
    {
        std::ranges::copy(str, m_str.data());
    }

    std::array<char, N> m_str = {};

    static constexpr auto Size = N;
};

template <BitsStruct Bits>
static consteval auto operator""_bits ()
{
    constexpr auto BitLen = Bits.Size;
    constexpr auto ByteLen = (BitLen + 7) / 8;

    using array_t = std::array<std::byte, ByteLen>;

    array_t arr {};
    auto setbit = [&arr](char c, std::size_t idx)
    {
        // Pack from the least significant bit upwards.
        arr[idx / 8] |= static_cast<std::byte>((c == '0' ? 0 : 1) << (idx % 8));
    };

    for (std::size_t i = 0; i < BitLen; ++i)
        setbit(Bits.m_str[i], i);

    return arr;
}


// Bitpattern for "Hello, World!" using the static Huffman tree.
constexpr auto HelloWorldStaticHuffman =
    "01111000" // H
    "10010101" // e
    "10011100" // l
    "10011100" // l
    "10001111" // o
    "01011100" // ,
    "01010000" //
    "10000111" // W
    "10001111" // o
    "10100010" // r
    "10011100" // l
    "10010100" // d
    "01010001" // !
    "00110000"_bits;

// Bitpattern for "Hello, hello!" using LZ77 + the static Huffman tree.
constexpr auto HelloHelloDeflated =
    "01111000"  // H
    "10010101"  // e
    "10011100"  // l
    "10011100"  // l
    "10001111"  // o
    "01011100"  // ,
    "01010000"  //
    "10011000"  // h
    "0000010"   // Backref: length 4...
    "00100" "1" // ...distance 5 + 1 extra bit = 6 bytes
    "01010001"  // !
    "00110000"_bits;

} // namespace System::Compress::Deflate
