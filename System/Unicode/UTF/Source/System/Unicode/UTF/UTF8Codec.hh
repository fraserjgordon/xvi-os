#pragma once
#ifndef __SYSTEM_UNICODE_UTF_UTF8CODEC_H
#define __SYSTEM_UNICODE_UTF_UTF8CODEC_H


#include <cstdint>
#include <utility>

#include <System/Unicode/UTF/UTF16Codec.hh>
#include <System/Unicode/UTF/UTFCodec.hh>


#define char8_t char


namespace System::Unicode::UTF
{

//! @brief Flags used to select non-standard UTF-8 variants. 
namespace UTF8Variant
{
    static constexpr unsigned int None              = 0x00;     //!< No variant.
    static constexpr unsigned int OverlongNuls      = 0x01;     //!< Permit the overlong 2-byte encoding of U+0000.
    static constexpr unsigned int SMPSurrogates     = 0x02;     //!< Non-BMP codepoints encoded using UTF-16 surrogates.
    static constexpr unsigned int AllowSurrogates   = 0x04;     //!< Allow surrogates (unpaired if CESU8 also set).
    static constexpr unsigned int AllowOverlong     = 0x08;     //!< Allow any overlong encodings.
    static constexpr unsigned int AllowNonUnicode   = 0x10;     //!< Allow values above the Unicode U+10FFFF ceiling.

    // Do not use as a parameter.
    static constexpr unsigned int All               = 0x1F;
}


class UTF8Codec :
    public UTFCodec
{
public:

    static constexpr
    bool
    validateVariantFlags(unsigned int flags)
    {
        return (flags & ~UTF8Variant::All) == 0;
    }

    static constexpr
    std::pair<bool, std::size_t>
    decodeSeqLength(char8_t c, unsigned int variations = UTF8Variant::None)
    {
        // The length of the encoded codepoint can be determined from the number of leading one bits in the initial byte
        // of the encoded sequence.
        auto uc = std::uint_least8_t(c);
        auto len = __builtin_clz(~c) - __builtin_clz(0xff);
        if (uc < 0x80)
        {
            // This is part of the 7-bit ASCII subset and is encoded in a single byte.
            return {true, 1};
        }
        else if (len == 1)
        {
            // This is a continuation byte and should not start an encoded sequence. Declare it to have a length of one
            // so that recovery is possible.
            return {false, 1};
        }
        else if (2 <= len && len <= 4)
        {
            // This is (conditionally) valid as the beginning of a multibyte encoded sequence. It is still possible that
            // the sequence decodes to a non-codepoint (e.g. UTF-16 surrogate or above U+10FFFF) but that is not
            // diagnosed here.
            return {true, len};
        }
        else if (!(variations & UTF8Variant::AllowNonUnicode))
        {
            // We're requiring strictly-conforming UTF8 so this byte is invalid as a leading byte.
            return {false, 1};
        }
        else if (5 <= len && len <= 6)
        {
            // This leading byte specifies a sequence longer than permitted in UTF-8 (as it is necessarily either
            // overlong or encoding a codepoint outside the Unicode repertoire).
            return {true, len};
        }
        else
        {
            // Six bytes is sufficient for encoding any 31-bit codepoint so these leading bytes are always invalid.
            return {false, 1};
        }
    }

    static constexpr
    bool
    isContinuationByte(char8_t c)
    {
        // Continuation bytes have 0b10.. for the high bits.
        return (c & 0b11000000) == 0b10000000;
    }

    //! @brief Returns the expected length of the given codepoint under UTF-8 encoding.
    //!
    //! For a given codepoint, returns the minimal number of bytes required to encode that codepoint using UTF-8. This
    //! method returns a meaningful value for any 32-bit integer even though only a subset of them are valid codepoints
    //! (i.e sensible values are returned for values above U+10FFFF even though they are not valid in UTF-8).
    static constexpr
    std::size_t
    encodedSize(char32_t c, unsigned int variations = UTF8Variant::None)
    {
        if (c < 0x80)
            return 1;
        else if (c < 0x800)
            return 2;
        else if (c < 0x10000)
            return 3;
        else if (!(variations & UTF8Variant::AllowNonUnicode))
        {
            // This is 4 bytes if a valid codepoint or an error otherwise.
            return isInRange(c) ? 4 : 0;
        }
        else if (c < 0x200000)
            return 4;   // Not all of this range is valid; only up to U+10FFFF.
        else if (c < 0x4000000)
            return 5;   // Not valid UTF-8.
        else if (c < 0x80000000)
            return 6;   // Not valid UTF-8.
        else
            return 0;   // Not valid UTF-8, even before Unicode allowed up to U+7FFFFFFF.
    }

    static constexpr
    std::pair<char32_t, std::size_t> 
    decodeOneUnchecked(const char8_t* ptr)
    {
        // Get the length of the encoded sequence. Note that the initial byte is not validated as this method is
        // explicitly unchecked.
        auto len = decodeSeqLength(*ptr).second;

        // Is this a single byte or multi-byte encoding?
        if (len == 1)
        {
            // Retutn the byte unmodified. It'll do the wrong thing if the length decode was invalid.
            return {*ptr, 1};
        }

        // Otherwise, this is a multi-byte encoding and further processing is required.
        auto initial_mask = 0x7f >> len;
        std::size_t decoded = 1;
        char32_t result = (*ptr & initial_mask);
        for (; decoded < len; ++decoded)
        {
            // Each continuation byte supplies another 6 bits of the result.
            result = (result << 6) | (ptr[decoded] & 0x3f);
        }

        return {result, decoded};
    }

    template <unsigned int SupportedVariations = UTF8Variant::All>
    static constexpr
    std::pair<char32_t, std::size_t>
    decodeOneWithVariations(const char8_t* ptr, std::size_t len, unsigned int variations)
    {
        using namespace UTF8Variant;

        // Mask out all variations that this instantiation doesn't support.
        variations &= SupportedVariations;
        
        // Verify that we can read at least one byte.
        if (len < 1)
        {
            // Empty string, cannot decode anything.
            return {ErrorEmptyString, 0};
        }

        // Read the first byte so the expected encoded length can be determined.
        auto [initial_valid, encoded_len] = decodeSeqLength(*ptr);
        if (bool len_valid = (encoded_len <= len); !initial_valid || !len_valid)
        {
            // The byte was not a valid initial byte for a UTF-8 encoded sequence or the expected length is greater than
            // the remaining length of the string.
            //
            // An invalid lead might be ignored if an appropriate variation has been selected. In particular, an invalid
            // leading byte will be ignored if all of the following are true:
            //  - the string is at least as long as the sequence length;
            //  - the error is because the lead byte specifies a 5- or 6-byte sequence length (i.e it is not invalid
            //    because we read an orphaned continuation byte); and
            //  - we're permitting non-Unicode values to be decoded.
            //
            // If we do report the error, report a length of one so that the malformed lead byte may be skipped.
            bool ignore_error = len_valid && (variations & AllowNonUnicode) && (encoded_len > 1);
            if (!ignore_error)
                return {ErrorMalformed, 1};
        }

        // Fast-path return for ASCII-range characters.
        if (encoded_len == 1)
        {
            // No further processing is required to decode this codepoint - the byte has the same numeric value of as
            // the codepoint it encodes and it is not possible for it to fail any of the validity checks that we need to
            // run on the multi-byte encodings.
            return {*ptr, 1};
        }

        // For multi-byte encodings, the top few bits are stored in the leading byte (how many depends on the length of
        // the encoded sequence -- an n-byte sequence has fixed values for the top n+1 bits of the lead byte and the
        // rest store the top bits). Each continuation character adds another 6 bits in decreasing significance.
        auto initial_mask = 0x7f >> len;
        std::size_t decoded = 1;
        char32_t c = (*ptr & initial_mask);
        for (; decoded < encoded_len; ++decoded)
        {
            // Verify that the next byte is actually a continuation byte. If not, report that the input is malformed and
            // that the malformed subsequence is all the bytes we've decoded so far (because they consist of exactly one
            // leading byte and zero or more continuation bytes, there is no way for them to be validly decoded from any
            // point in the substring).
            if (!isContinuationByte(ptr[decoded]))
                return {ErrorMalformed, decoded - 1};
            
            // The continuation bits are inserted at the low end.
            c = (c << 6) | (ptr[decoded] & 0x3f);
        }

        // Now that we have the decoded value, check if it is actually a Unicode codepoint.
        if (!isInRange(c))
        {
            // Unless explicitly requested, this is an error.
            if (!(variations & AllowNonUnicode))
                return {ErrorNotACodepoint, decoded};
        }
        
        // Next, check for overlong encodings.
        if (encoded_len > encodedSize(c, variations))
        {
            // This overlong sequence might be permitted if the appropriate variation has been requested.
            bool ignore_overlong = (variations & AllowOverlong);
            bool ignore_overlong_nul = (variations & OverlongNuls) && c == 0 && encoded_len == 2;
            if (!ignore_overlong && !ignore_overlong_nul)
                return {ErrorOverlong, decoded};
        }

        // Check whether the decoded value represents a UTF-16 surrogate. This is normally invalid but may be permitted
        // if an appropriate variation has been selected.
        bool leading_surrogate = UTF16Codec::isLeadingSurrogate(c);
        bool trailing_surrogate = UTF16Codec::isTrailingSurrogate(c);
        if (leading_surrogate && (variations & SMPSurrogates))
        {
            // CESU8 encodes SMP codepoints by converting them to a UTF-16 surrogate pair and then encoding those
            // surrogates as UTF-16. We've just decoded the leading half so try the trailing half next.
            //
            // Because the trailing surrogate looks like an unpaired surrogate, we need to add the appropriate flag.
            auto [next_c, next_len] = decodeOneWithVariations<SupportedVariations|AllowSurrogates>(ptr + decoded, len - decoded, variations | AllowSurrogates);
            if (next_len > 0)
            {
                // The following sequence was successfully decoded. Is it the trailing surrogate we're looking for?
                if (UTF16Codec::isTrailingSurrogate(next_c))
                {
                    // We now have a valid surrogate pair; decode them. Because we've decoded these two sequences as a
                    // unit, the sequence length needs to be adjusted to cover both.
                    c = UTF16Codec::combineSurrogates(c, next_c);
                    encoded_len += next_len;
                } 
                else
                {
                    // The sequence we just decoded isn't a trailing surrogate. This is fine if we're allowing unpaired
                    // surrogates but is otherwise an error.
                    if (!(variations & AllowSurrogates))
                        return {ErrorSurrogate, decoded};
                }
            }
            else
            {
                // The attempt to decode a trailing surrogate failed. Unless we're allowing unpaired surrogates, this is
                // an error.
                if (!(variations & AllowSurrogates))
                    return {ErrorSurrogate, decoded};
            }
        }
        else if (leading_surrogate || trailing_surrogate)
        {
            // The values for the UTF-16 surrogates are not allowed in UTF-8. We'll allow them if the appropriate
            // variation has been requested, though.
            if (!(variations & AllowSurrogates))
                return {ErrorSurrogate, decoded};
        }

        // All of the validation tests have passed; the codepoint can now be returned.
        return {c, decoded};
    }

    static constexpr
    auto
    decodeOne(const char8_t* ptr, std::size_t len)
    {
        return decodeOneWithVariations<UTF8Variant::None>(ptr, len, UTF8Variant::None);
    }

    static constexpr
    auto
    decodeOne(const char8_t* ptr, std::size_t len, unsigned int variations)
    {
        return decodeOneWithVariations(ptr, len, variations);
    }

    //! @brief Validates that a given byte sequence is valid UTF-8.
    static constexpr
    bool
    isValid(const char8_t* ptr, std::size_t len)
    {
        // Attempt to decode each codepoint in the sequence -- if all decode successfully, the sequence is valid UTF-8.
        while (len > 0)
        {
            // The important field is the codepoint -- it may be a special error value.
            auto [c, increment] = decodeOne(ptr, len);
            if (isError(c))
                return false;
            ptr += increment;
            len -= increment;
        }

        return true;
    }

    //! @brief Validates that a given byte sequence is valid UTF-8.
    static constexpr
    bool
    isValid(const char8_t* ptr, std::size_t len, unsigned int variations)
    {
        // Attempt to decode each codepoint in the sequence -- if all decode successfully, the sequence is valid UTF-8.
        while (len > 0)
        {
            // The important field is the codepoint -- it may be a special error value.
            auto [c, increment] = decodeOneWithVariations<UTF8Variant::All>(ptr, len, variations);
            if (isError(c))
                return false;
            ptr += increment;
            len -= increment;
        }

        return true;
    }
};


} // namespace System::Unicode::UTF


#endif /* ifndef __SYSTEM_UNICODE_UTF_UTF8CODEC_H */
