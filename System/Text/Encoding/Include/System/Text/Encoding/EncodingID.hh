#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ENCODINGID_H
#define __SYSTEM_TEXT_ENCODING_ENCODINGID_H


#include <cstdint>


namespace System::Text::Encoding
{


// These IDs match the IANA character set IDs.
enum class EncodingID : std::int32_t
{
    // Special values.
    Unknown         = 0,    

    // Legacy 7-/8-bit encodings.
    US_ASCII        = 3,
    ISO_8859_1      = 4,
    
    // Unicode range, 1000-1999 (UTF-8 is an outlier).
    UTF8            = 106,
    UCS2            = 1000,
    UCS4            = 1001,
    UTF16BE         = 1013,
    UTF16LE         = 1014,
    UTF16           = 1015,
    CESU8           = 1016,
    UTF32           = 1017,
    UTF32BE         = 1018,
    UTF32LE         = 1019,

    // Encodings not in the IANA character set database.
    ModifiedUTF8    = -10000,   // Java's modified UTF-8 encoding.
    WTF8            = -10001,   // "Wobbly Translation Format"-8.
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ENCODINGID_H */
