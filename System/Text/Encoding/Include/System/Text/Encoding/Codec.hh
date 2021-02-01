#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_CODEC_H
#define __SYSTEM_TEXT_ENCODING_CODEC_H


#include <System/Text/Encoding/Private/Config.hh>

#include <System/Text/Encoding/EncodingInfo.hh>

#include <cstddef>
#include <optional>
#include <span>


namespace System::Text::Encoding
{


class Codec
{
public:

    //! Returns the information object for this codec (name, id, etc).
    const EncodingInfo& getInfo() const
    {
        return *codecGetInfo(this);
    }

    //! Validates that the given sequence of bytes conforms to the encoding's specification. For some encodings, this
    //! may always return true (e.g. non-multibyte 8-bit encodings with no invalid byte values).
    bool isValid(std::span<const std::byte> encoded) const
    {
        return codecIsValid(this, encoded.data(), encoded.size());
    }

    //! Returns true if the given codepoint can be represented using this encoding.
    bool canRepresent(char32_t codepoint) const
    {
        return codecCanRepresent(this, codepoint);
    }

    //! Returns the recommended replacement character for this encoding. The returned codepoint is guaranteed to be
    //! representable in this encoding.
    //!
    //! The replacement character will not necessarily be used uniquely as a replacement character; it could be used for
    //! other purposes, too (for example, U'?' is a common choice of replacement).
    char32_t recommendedReplacementCharacter() const
    {
        return codecRecommendedReplacementCharacter(this);
    }

    //! Returns an estimate of the space required to transcode from the given encoding to this encoding. This method
    //! does not look at the specific string to be encoded; it estimates only from the length of the string.
    //!
    //! For an exact estimate, see the calulcateTranscodeLength method.
    std::size_t estimatedTranscodeLength(EncodingID encoding, std::size_t length) const
    {
        return codecEstimatedTranscodeLength(this, encoding, length);
    }

    //! Returns the number of bytes required to represent the given string when transcoded into this encoding. The given
    //! string is decoded in order to give an exact result.
    //!
    //! A replacement character needs to be specified so unrepresentable characters can be handled appropriately.
    std::size_t calculateTranscodeLength(EncodingID encoding, std::span<const std::byte> encoded, char32_t replacement) const
    {
        return codecCalculateTranscodeLengthWithReplacement(this, encoding, encoded.data(), encoded.size(), replacement);
    }

    //! Overload of calculateTranscodeLength but assuming the recommended replacement character.
    std::size_t calculateTranscodeLength(EncodingID encoding, std::span<const std::byte> encoded) const
    {
        return codecCalculateTranscodeLength(this, encoding, encoded.data(), encoded.size());
    }


private:

    __SYSTEM_TEXT_ENCODING_EXPORT static const EncodingInfo*    codecGetInfo(const Codec*) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.GetInfo);
    __SYSTEM_TEXT_ENCODING_EXPORT static bool                   codecIsValid(const Codec*, const std::byte*, std::size_t) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.IsValid);
    __SYSTEM_TEXT_ENCODING_EXPORT static bool                   codecCanRepresent(const Codec*, char32_t) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.CanRepresent);
    __SYSTEM_TEXT_ENCODING_EXPORT static char32_t               codecRecommendedReplacementCharacter(const Codec*) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.RecommendedReplacementCharacter);
    __SYSTEM_TEXT_ENCODING_EXPORT static std::size_t            codecEstimatedTranscodeLength(const Codec*, EncodingID, std::size_t) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.EstimatedTranscodeLength);
    __SYSTEM_TEXT_ENCODING_EXPORT static std::size_t            codecCalculateTranscodeLengthWithReplacement(const Codec*, EncodingID, const std::byte*, std::size_t, char32_t) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.CalculateTranscodeLengthWithReplacement);
    __SYSTEM_TEXT_ENCODING_EXPORT static std::size_t            codecCalculateTranscodeLength(const Codec*, EncodingID, const std::byte*, std::size_t) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.CalculateTranscodeLength);
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_CODEC_H */
