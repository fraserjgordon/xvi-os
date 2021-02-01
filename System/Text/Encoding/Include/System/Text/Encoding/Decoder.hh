#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_DECODER_H
#define __SYSTEM_TEXT_ENCODING_DECODER_H


#include <System/Text/Encoding/Private/Config.hh>


namespace System::Text::Encoding
{


enum class DecoderState
{
    Initial     = 0,    //!< Decoder hasn't processed any input data yet.
    Incomplete  = 1,    //!< Decoder expects more input (contains partially processed input). 
};


class Decoder
{
public:

    //! Calculates the length (in bytes) of the encoded codepoint at the beginning of the span.
    //!
    //! If the byte stream is not a valid encoding, the returned length is a value that may allow future bytes in the
    //! stream to be decoded (but recovery is not always possible).
    //!
    //! The decoder state is not updated when calling this method.
    std::size_t sizeOfNextCharacter(std::span<const std::byte> encoded) const
    {
        return decoderSizeOfNextCharacter(this, encoded.data(), encoded.size());
    }

    //! Attempts to decode a single codepoint from the byte stream.
    std::optional<char32_t> decodeNextCharacter(std::span<const std::byte>& encoded)
    {
        const std::byte* ptr = encoded.data();
        std::size_t len = encoded.size();
        bool success = false;

        auto result = decoderDecodeNextCharacter(this, &ptr, &len, &success);
        encoded = {ptr, len};

        if (success)
            return result;
        else
            return std::nullopt;
    }


private:

    __SYSTEM_TEXT_ENCODING_EXPORT static std::size_t            decoderSizeOfNextCharacter(const Decoder*, const std::byte*, std::size_t) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.SizeOfNextCharacter);
    __SYSTEM_TEXT_ENCODING_EXPORT static char32_t               decoderDecodeNextCharacter(Decoder*, const std::byte**, std::size_t*, bool*) __SYSTEM_TEXT_ENCODING_SYMBOL(Codec.DecodeNextCharacter);
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_DECODER_H */
