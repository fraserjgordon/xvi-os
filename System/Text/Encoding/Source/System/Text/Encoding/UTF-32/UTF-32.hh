#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_UTF32_H
#define __SYSTEM_TEXT_ENCODING_UTF32_H


#include <System/Text/Encoding/EncodingInfo.hh>


namespace System::Text::Encoding
{


constexpr EncodingInfo EncodingInfoUTF32 =
{
    .id     = EncodingID::UTF32,
    .names  = { "UTF-32", "csUTF32" },
};

constexpr EncodingInfo EncodingInfoUTF32LE =
{
    .id     = EncodingID::UTF32LE,
    .names  = { "UTF-32LE", "csUTF32LE" },
};

constexpr EncodingInfo EncodingInfoUTF32BE =
{
    .id     = EncodingID::UTF32BE,
    .names  = { "UTF-32BE", "csUTF32BE" },
};

constexpr EncodingInfo EncodingInfoUCS4 =
{
    .id     = EncodingID::UCS4,
    .names  = { "ISO-10646-UCS-4", "csUCS4" },
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_UTF32_H */
