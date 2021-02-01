#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ASCII_H
#define __SYSTEM_TEXT_ENCODING_ASCII_H


#include <System/Text/Encoding/CodecImpl.hh>
#include <System/Text/Encoding/EncodingFactoryImpl.hh>
#include <System/Text/Encoding/EncodingInfo.hh>


namespace System::Text::Encoding
{


class ASCIICodec;
using ASCIIEncodingFactory = EncodingFactoryImpl<ASCIICodec>;


__SYSTEM_TEXT_ENCODING_EXPORT constexpr EncodingInfo EncodingInfoASCII __SYSTEM_TEXT_ENCODING_SYMBOL(EncodingInfoASCII) =
{
    .id         = EncodingID::US_ASCII,
    .names      = { "US-ASCII", "iso-ir-6", "ANSI_X3.4-1968", "ANSI_X3.4-1986", "ISO_646.irv:1991", "ISO646-US", "us", "IBM367", "cp367", "csASCII" },
    .factory    = &ASCIIEncodingFactory::instance()
};


class ASCIICodec final
    : public CodecImpl
{
public:

    // Inherited from CodecImpl.
    const EncodingInfo& getInfo() const final;
    bool isValid(std::span<const std::byte>) const final;
    bool canRepresent(char32_t) const final;
    char32_t recommendedReplacementCharacter() const final;
    std::size_t estimatedTranscodeLength(EncodingID, std::size_t) const final;
    std::size_t calculateTranscodeLength(EncodingID, std::span<const std::byte>, char32_t) const final;
    std::size_t calculateTranscodeLength(EncodingID, std::span<const std::byte>) const final;
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_UTF8_H */
