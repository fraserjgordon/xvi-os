#include <System/Text/Encoding/ASCII/ASCII.hh>

#include <algorithm>


namespace System::Text::Encoding
{


const EncodingInfo& ASCIICodec::getInfo() const
{
    return EncodingInfoASCII;
}

bool ASCIICodec::isValid(std::span<const std::byte> bytes) const
{
    // Valid US-ASCII characters are limited to 7 bits.
    return std::all_of(bytes.begin(), bytes.end(), [this](std::byte b)
    {
        return canRepresent(static_cast<char32_t>(b));
    });
}

bool ASCIICodec::canRepresent(char32_t c) const
{
    return c <= 127;
}

char32_t ASCIICodec::recommendedReplacementCharacter() const
{
    return U'?';
}

std::size_t ASCIICodec::estimatedTranscodeLength(EncodingID, std::size_t length) const
{
    // THe maximum length is equal to the number of input bytes.
    return length;
}

std::size_t ASCIICodec::calculateTranscodeLength(EncodingID encid, std::span<const std::byte> bytes) const
{
    return calculateTranscodeLength(encid, bytes, recommendedReplacementCharacter());
}

std::size_t ASCIICodec::calculateTranscodeLength(EncodingID encid, std::span<const std::byte> bytes, char32_t replacement) const
{
    
}



} // System::Text::Encoding
