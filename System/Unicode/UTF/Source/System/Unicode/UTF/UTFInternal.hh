#ifndef __SYSTEM_UNICODE_UTF_UTFINTERNAL_H
#define __SYSTEM_UNICODE_UTF_UTFINTERNAL_H


#if __SYSTEM_UNICODE_UTF_MINIUTF
#  include <System/C++/Utility/Bit.hh>
#  include <System/C++/Utility/Optional.hh>
#else

#endif


namespace System::Unicode::UTF
{


constexpr char16_t LeadSurrogateBegin   = 0xD800;
constexpr char16_t LeadSurrogateEnd     = 0xDBFF;
constexpr char16_t TrailSurrogateBegin  = 0xDC00;
constexpr char16_t TrailSurrogateEnd    = 0xDFFF;
constexpr char32_t LastValidCodepoint   = 0x10FFFF;

constexpr std::size_t MaxUTF8EncodedLength = 4;
constexpr std::size_t MaxUTF16EncodedLength = 2;


constexpr bool isValidCodepoint(char32_t c)
{
    if (c > LastValidCodepoint)
        return false;

    if (LeadSurrogateBegin <= c and c <= TrailSurrogateEnd)
        return false;

    return true;
}

constexpr std::size_t utf8LengthFromCodepoint(char32_t c)
{
    if (c < 0x80)
        return 1;
    else if (c < 0x800)
        return 2;
    else if (c < 0x10000)
        return 3;
    else if (c <= LastValidCodepoint)
        return 4;
    else
        return 0;
}

constexpr std::size_t utf8LengthFromLeadByte(char8_t c)
{
    auto leading = std::countl_one(static_cast<std::uint8_t>(c));

    // Only zero, two, three or four leading ones are permissable. 
    if (leading == 1 or leading > 4)
        return 0;

    // Zero leading ones is a single-byte encoding. All others have the same length in bytes as the number of leading
    // ones.
    if (leading == 0)
        return 1;
    else
        return static_cast<std::size_t>(leading);
}

constexpr bool isUTF8LeadByte(char8_t c)
{
    auto length = utf8LengthFromLeadByte(c);

    return (length > 0);
}

constexpr bool isUTF8ContinuationByte(char8_t c)
{
    return (c & 0xC0) == 0x80;
}

constexpr std::optional<char32_t> utf8Decode(char8_t lead, char8_t t1 = 0, char8_t t2 = 0, char8_t t3 = 0)
{
    // Validate that the bytes are of the expected form.
    if (!isUTF8LeadByte(lead))
        return {};

    auto len = utf8LengthFromLeadByte(lead);
    switch (len)
    {
        case 4:
            if (!isUTF8ContinuationByte(t3))
                return {};
            [[fallthrough]];

        case 3:
            if (!isUTF8ContinuationByte(t2))
                return {};
            [[fallthrough]];
            
        case 2:
            if (!isUTF8ContinuationByte(t1))
                return {};
            [[fallthrough]];

        case 1:
            break;
    }

    // Calculate the codepoint represented by these bytes.
    std::int32_t code;
    switch (len)
    {
        case 1:
            code = (lead & 0x7f);
            break;

        case 2:
            code = ((lead & 0x1f) << 6) | (t1 & 0x3f);
            break;

        case 3:
            code = ((lead & 0x0f) << 12) | ((t1 & 0x3f) << 6) | (t2 & 0x3f);
            break;

        case 4:
            code = ((lead & 0x07) << 18) | ((t1 & 0x3f) << 12) | ((t2 & 0x3f) << 6) | (t3 & 0x3f);
            break;
    }

    // Check that the codepoint is valid.
    auto codepoint = static_cast<char32_t>(code);
    if (!isValidCodepoint(codepoint))
        return {};

    // Check that this wasn't an over-long encoding of the codepoint.
    if (utf8LengthFromCodepoint(codepoint) != len)
        return {};

    // Decode complete and validation checks passed.
    return codepoint;
}

constexpr bool isUTF16LeadSurrogate(char16_t c)
{
    return (LeadSurrogateBegin <= c and c <= LeadSurrogateEnd);
}

constexpr bool isUTF16TrailSurrogate(char16_t c)
{
    return (TrailSurrogateBegin <= c and c <= TrailSurrogateEnd);
}

constexpr std::optional<char32_t> utf16Decode(char16_t lead, char16_t trail)
{
    if (!isUTF16LeadSurrogate(lead) or !isUTF16TrailSurrogate(trail))
        return {};

    return 0x10000 + (((static_cast<char32_t>(lead) & 0x3FF) << 10) | (static_cast<char32_t>(trail) & 0x3FF));
}


} // namespace System::Unicode::UTF


#endif /* ifndef __SYSTEM_UNICODE_UTF_UTFINTERNAL_H */
