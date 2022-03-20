#include <System/Compress/Deflate/LZ77.hh>


namespace System::Compress::Deflate
{


bool LZ77Decoder::isLengthCode(Symbol s) noexcept
{
    return (s >= Symbol::FirstLengthCode) && (s <= Symbol::LastLengthCode);
}


unsigned int LZ77Decoder::lengthCodeExtraBits(Symbol s) noexcept
{
    if (!isLengthCode(s)) [[unlikely]]
        return 0;

    // The number of extra bits can be calculated without a lookup table as:
    //
    //      extra = /  0                n <  4
    //              |  floor(n/4) - 1   4 <= n < 28
    //              \  0                n = 28
    //
    //  where n is (s - Symbol::FirstLengthCode).
    unsigned int n = static_cast<std::uint16_t>(s) - static_cast<std::uint16_t>(Symbol::FirstLengthCode);
    if (n < 4 || n == 28)
        return 0;

    return (n / 4) - 1;
}


unsigned int LZ77Decoder::lengthCodeBaseLength(Symbol s) noexcept
{
    if (!isLengthCode(s)) [[unlikely]]
        return 0;

    // The pattern of the base lengths for the length codes is that the base length minus 3 doubles every four codes
    // (termed a "group" of 4 here). In particular, the base lengths for the 1st-4th codes in group G are:
    //
    //      l_G_1 = 2**(G + 1) + 3
    //      l_G_2 = 2**(G + 1) + 2**(G - 1) + 3
    //      l_G_3 = 2**(G + 1) + 2**G + 3
    //      l_G_4 = 2**(G + 1) + 2**G + 2**(G - 1) + 3
    //
    //      where G = floor(n / 4)
    //            n = s - Symbol::FirstLengthCode
    //
    // Strictly speaking, this pattern does not hold for group 0.
    //
    // The code below is the above equations converted into bit operations: the powers of two can
    // be converted into left-shifts by (G - 1) where the value to be shifted is 0b100, 0b101, 0b110, 0b111 for l_G_1,
    // l_G_2, l_G_3, l_G_4 respectively.
    unsigned int n = static_cast<std::uint16_t>(s) - static_cast<std::uint16_t>(Symbol::FirstLengthCode);
    if (n < 4)
        return n;

    auto g = n / 4;
    auto topBits = 0b100 | (n & 3);
    auto l = (topBits << (g - 1)) + 3;

    return l;
}


unsigned int LZ77Decoder::lengthCodeLength(Symbol s, unsigned int extra) noexcept
{
    return lengthCodeBaseLength(s) + extra;
}


unsigned int LZ77Decoder::distanceCodeExtraBits(DistanceSymbol s) noexcept
{
    // The number of extra bits can be calculated without a lookup table as:
    //
    //      extra = /  0                n <  2
    //              \  floor(n/2) - 1   n >= 2
    //
    //  where n is int{s}.
    unsigned int n = static_cast<std::uint8_t>(s) - static_cast<std::uint8_t>(Symbol::FirstLengthCode);
    if (n >= 2)
        return (n / 2) - 1;
    else
        return 0;
}


unsigned int LZ77Decoder::distanceCodeBaseDistance(DistanceSymbol s) noexcept
{
    // The base distances for each distance symbol go up by a factor of two per two symbols:
    //
    //      0-1     ->      1-2
    //      2-3     ->      3-4
    //      4-5     ->      5-8
    //      6-7     ->      9-16
    //              ...
    //     28-29    ->  16385-32768
    //
    // Using this, we can calculate the base distance for each member 1 and 2 of each group G as:
    //
    //      d_G_1 = 2**G + 1
    //      d_G_2 = 2**G + 2**(G-1) + 1
    //
    //      where G = floor(n / 2),
    //            n = int{s}
    //
    //      exception: d_0_1 = 1 (i.e when n = 0).
    //
    // The code below is the above equations converted into bit operations. In particular, the
    // calculation for d_G_1 can be expressed as (1 << G) + 1 while d_G_2 is (3 << (G - 1)) + 1.
    // Those are then reworked further to avoid a conditional in testing the low bit of n.
    unsigned int n = static_cast<std::uint16_t>(s) - static_cast<std::uint16_t>(Symbol::FirstLengthCode);
    if (n < 2)
        return n + 1;

    auto g = n / 2;
    auto topBits = 0b10 | (n & 1);
    auto d = (topBits << (g - 1)) + 1;

    return d;
}


unsigned int LZ77Decoder::distanceCodeDistance(DistanceSymbol s, unsigned int extra) noexcept
{
    return distanceCodeBaseDistance(s) + extra;
}


} // namespace System::Compress::Deflate
