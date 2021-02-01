#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_CODECIMPL_H
#define __SYSTEM_TEXT_ENCODING_CODECIMPL_H


#include <System/Text/Encoding/Codec.hh>


namespace System::Text::Encoding
{


class CodecImpl :
    public Codec
{
public:

    // Note: these shadow the non-virtual declarations in the base class deliberately.
    virtual const EncodingInfo& getInfo() const = 0;
    virtual bool isValid(std::span<const std::byte> encoded) const = 0;
    virtual bool canRepresent(char32_t codepoint) const = 0;
    virtual char32_t recommendedReplacementCharacter() const = 0;
    virtual std::size_t estimatedTranscodeLength(EncodingID encoding, std::size_t length) const = 0;
    virtual std::size_t calculateTranscodeLength(EncodingID encoding, std::span<const std::byte> encoded, char32_t replacement) const = 0;
    virtual std::size_t calculateTranscodeLength(EncodingID encoding, std::span<const std::byte> encoded) const = 0;


    static const CodecImpl* fromCodec(const Codec* c)
    {
        return static_cast<const CodecImpl*>(c);
    }
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_CODECIMPL_H */
