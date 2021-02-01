#pragma once
#ifndef __SYSTEM_UNICODE_UTF_UTFCODEC_H
#define __SYSTEM_UNICODE_UTF_UTFCODEC_H


#include <cstddef>


namespace System::Unicode::UTF
{


class UTFCodec
{
public:

    using byte = std::byte;

    // Range of non-codepoint values used internally to signal errors.
    static constexpr char32_t ErrorRangeStart           = 0xFFFF0000;
    static constexpr char32_t ErrorRangeEnd             = 0xFFFF7FFF;
    static constexpr char32_t ErrorUnknown              = 0xFFFF0000;   //!< A non-specific error occurred.
    static constexpr char32_t ErrorSurrogate            = 0xFFFF0001;   //!< Encoded value is a surrogate (or unpaired).
    static constexpr char32_t ErrorNotACodepoint        = 0xFFFF0002;   //!< Encoded value is out of range.
    static constexpr char32_t ErrorOverlong             = 0xFFFF0003;   //!< An overlong encoding was used.
    static constexpr char32_t ErrorMalformed            = 0xFFFF0004;   //!< Input is malformed (e.g. invalid byte).
    static constexpr char32_t ErrorEmptyString          = 0xFFFF0005;   //!< Empty string was given.
    static constexpr char32_t ErrorEndOfInput           = 0xFFFF7FFF;   //!< Returned when no more data to decode.

    // The last valid codepoint for Unicode (UTF-8 and UTF-32 are capable of encoding values outside this boundary but
    // those values are not valid codepoints).
    static constexpr char32_t HighestCodepoint          = 0x10FFFF;

    // The range reserved as surrogates. It is possible to encode these values under the UTF-8 and UTF-32 encoding
    // schemes but they are not valid codepoints.
    static constexpr char32_t SurrogatesStart           = 0xD800;
    static constexpr char32_t SurrogatesEnd             = 0xDFFF;

    // The codepoint ranges reserved as non-characters. It is valid for these to be encoded but are only intended for
    // "private" use (they are not intended to be exchanged externally -- however one chooses to define that).
    static constexpr char32_t NonCharacterRangeStart    = 0xFDD0;
    static constexpr char32_t NonCharacterRangeEnd      = 0xFDEF;
    static constexpr char32_t NonCharacterMask          = 0xFFFE;

    // The Unicode byte order mark codepoint. This also used to be used as a zero-width non-breaking space but that
    // usage is now deprecated.
    static constexpr char32_t BOM                       = 0xFEFF;


    //! @brief Sets the input byte range for decoding.
    virtual void setInputBytes(const byte*, std::size_t) = 0;


    static constexpr
    bool
    isError(char32_t c)
    {
        return (ErrorRangeStart <= c && c <= ErrorRangeEnd);
    }

    static constexpr
    bool
    isInRange(char32_t c)
    {
        return (0 <= c && c <= HighestCodepoint);
    }

    static constexpr
    bool
    isSurrogate(char32_t c)
    {
        return (SurrogatesStart <= c && c <= SurrogatesEnd);
    }

    static constexpr
    bool
    isNonCharacter(char32_t c)
    {
        // There is a range of non-characters in the BMP. In addition, the last two codepoints of every plane are non-
        // characters.
        return ((c & NonCharacterMask) == NonCharacterMask)
            || (NonCharacterRangeStart <= c && c <= NonCharacterRangeEnd);
    }
};


} // namespace System::Unicode::UTF


#endif /* ifndef __SYSTEM_UNICODE_UTF_UTFCODEC_H */
