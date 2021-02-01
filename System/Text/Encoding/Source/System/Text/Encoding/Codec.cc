#include <System/Text/Encoding/Codec.hh>

#include <System/Text/Encoding/CodecImpl.hh>


namespace System::Text::Encoding
{


const EncodingInfo* Codec::codecGetInfo(const Codec* c)
{
    return &CodecImpl::fromCodec(c)->getInfo();
}

bool Codec::codecIsValid(const Codec* c, const std::byte* ptr, std::size_t len)
{
    return CodecImpl::fromCodec(c)->isValid({ptr, len});
}

bool Codec::codecCanRepresent(const Codec* c, char32_t codepoint)
{
    return CodecImpl::fromCodec(c)->canRepresent(codepoint);
}

char32_t Codec::codecRecommendedReplacementCharacter(const Codec* c)
{
    return CodecImpl::fromCodec(c)->recommendedReplacementCharacter();
}

std::size_t Codec::codecEstimatedTranscodeLength(const Codec* c, EncodingID encid, std::size_t len)
{
    return CodecImpl::fromCodec(c)->estimatedTranscodeLength(encid, len);
}

std::size_t Codec::codecCalculateTranscodeLengthWithReplacement(const Codec* c, EncodingID encid, const std::byte* ptr, std::size_t len, char32_t replacement)
{
    return CodecImpl::fromCodec(c)->calculateTranscodeLength(encid, {ptr, len}, replacement);
}

std::size_t Codec::codecCalculateTranscodeLength(const Codec* c, EncodingID encid, const std::byte* ptr, std::size_t len)
{
    return CodecImpl::fromCodec(c)->calculateTranscodeLength(encid, {ptr, len});
}


} // namespace System::Text::Encoding
