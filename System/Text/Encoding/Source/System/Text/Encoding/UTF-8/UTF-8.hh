#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_UTF8_H
#define __SYSTEM_TEXT_ENCODING_UTF8_H


#include <System/Text/Encoding/CodecImpl.hh>
#include <System/Text/Encoding/EncodingFactoryImpl.hh>
#include <System/Text/Encoding/EncodingInfo.hh>


namespace System::Text::Encoding
{


// UTF-8 variants that exist.
enum class UTF8Variant
{
    Standard,       // Standard UTF-8, as defined by the Unicode Consortium.
    CESU8,          // Encodes SMP codepoints as pairs of encoded surrogates.
    Modified,       // CESU-8 plus U+0000 is encoded using the overlong {0xC0, 0x80} encoding.
    WTF8,           // Standard except surrogates can be encoded (so malformed UTF-16 can be encoded).
};


template <UTF8Variant Variant>
class UTF8Codec final :
    public CodecImpl
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


using UTF8CodecStandard = UTF8Codec<UTF8Variant::Standard>;
using UTF8CodecCESU8 = UTF8Codec<UTF8Variant::CESU8>;
using UTF8CodecModified = UTF8Codec<UTF8Variant::Modified>;
using UTF8CodecWTF8 = UTF8Codec<UTF8Variant::WTF8>;


using UTF8EncodingFactory = EncodingFactoryImpl<UTF8CodecStandard>;
using CESU8EncodingFactory = EncodingFactoryImpl<UTF8CodecCESU8>;
using ModifiedUTF8EncodingFactory = EncodingFactoryImpl<UTF8CodecModified>;
using WTF8EncodingFactory = EncodingFactoryImpl<UTF8CodecWTF8>;

extern template class UTF8Codec<UTF8Variant::Standard>;
extern template class UTF8Codec<UTF8Variant::CESU8>;
extern template class UTF8Codec<UTF8Variant::Modified>;
extern template class UTF8Codec<UTF8Variant::WTF8>;


__SYSTEM_TEXT_ENCODING_EXPORT constexpr EncodingInfo EncodingInfoUTF8 __SYSTEM_TEXT_ENCODING_SYMBOL(UTF8.EncodingInfo) =
{
    .id         = EncodingID::UTF8,
    .names      = { "UTF-8", "csUTF8", "unicode-1-1-utf-8" },
    .factory    = &UTF8EncodingFactory::instance()
};

__SYSTEM_TEXT_ENCODING_EXPORT constexpr EncodingInfo EncodingInfoCESU8 __SYSTEM_TEXT_ENCODING_SYMBOL(UTF8.EncodingInfoCESU8) =
{
    .id         = EncodingID::CESU8,
    .names      = { "CESU-8", "csCESU8" },
    .factory    = &CESU8EncodingFactory::instance()
};

__SYSTEM_TEXT_ENCODING_EXPORT constexpr EncodingInfo EncodingInfoModifiedUTF8 __SYSTEM_TEXT_ENCODING_SYMBOL(UTF8.EncodingInfoModifiedUTF8) =
{
    .id         = EncodingID::ModifiedUTF8,
    .names      = { /* no IANA registered names */ },
    .factory    = &ModifiedUTF8EncodingFactory::instance()
};

__SYSTEM_TEXT_ENCODING_EXPORT constexpr EncodingInfo EncodingInfoWTF8 __SYSTEM_TEXT_ENCODING_SYMBOL(UTF8.EncodingInfoWTF8)
{
    .id         = EncodingID::WTF8,
    .names      = { /* no IANA registered names */ },
    .factory    = &WTF8EncodingFactory::instance()
};



} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_UTF8_H */
