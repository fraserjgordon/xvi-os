#pragma once
#ifndef __SYSTEM_UNICODE_UTF_UTF16CODEC_H
#define __SYSTEM_UNICODE_UTF_UTF16CODEC_H


namespace System::Unicode::UTF
{


class UTF16Codec
{
public:

    // The codeunit ranges reserved as surrogates.
    static constexpr char16_t LeadingSurrogateStart     = 0xD800;
    static constexpr char16_t LeadingSurrogateEnd       = 0xDBFF;
    static constexpr char16_t TrailingSurrogateStart    = 0xDC00;
    static constexpr char16_t TrailingSurrogateEnd      = 0xDFFF;

    //! @brief Indicates whether the given codeunit is a leading surrogate.
    static constexpr
    bool
    isLeadingSurrogate(char16_t c)
    {
        return LeadingSurrogateStart <= c && c <= LeadingSurrogateEnd;
    }

    //! @brief Indicates whether the given codeunit is a trailing surrogate.
    static constexpr
    bool
    isTrailingSurrogate(char16_t c)
    {
        return TrailingSurrogateStart <= c && c <= TrailingSurrogateEnd;
    }

    //! @brief Indicates whether the given codeunit is a surrogate.
    static constexpr
    bool
    isSurrogate(char16_t c)
    {
        return isLeadingSurrogate(c) || isTrailingSurrogate(c);
    }

    //! @brief Calculates the codepoint represented by the given surrogate pair.
    //!
    //! @warning The given codeunits are not checked for validity as a surrogate pair; the return value is undefined if
    //!          they are not a leading and trailing surrogates in the correct order.
    static constexpr
    char32_t
    combineSurrogates(char16_t lead, char16_t trail)
    {
        return 0x10000 + ((lead - LeadingSurrogateStart) << 10) + (trail - TrailingSurrogateStart);
    }
};


} // namespace System::Unicode::UTF


#endif /* ifndef __SYSTEM_UNICODE_UTF_UTF16CODEC_H */
