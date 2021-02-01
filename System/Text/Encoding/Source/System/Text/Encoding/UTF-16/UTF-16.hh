#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_UTF16_H
#define __SYSTEM_TEXT_ENCODING_UTF16_H


#include <System/Text/Encoding/EncodingInfo.hh>


namespace System::Text::Encoding
{


constexpr EncodingInfo EncodingInfoUTF16 =
{
    .id     = EncodingID::UTF16,
    .names  = { "UTF-16", "csUTF16" },
};

constexpr EncodingInfo EncodingInfoUTF16LE =
{
    .id     = EncodingID::UTF16LE,
    .names  = { "UTF-16LE", "csUTF16LE" },
};

constexpr EncodingInfo EncodingInfoUTF16BE =
{
    .id     = EncodingID::UTF16BE,
    .names  = { "UTF16-BE", "csUTF16BE" },
};

constexpr EncodingInfo EncodingInfoUCS2 =
{
    .id     = EncodingID::UCS2,
    .names  = { "ISO-10646-UCS-2", "csUnicode" },
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_UTF16_H */
