#ifndef __SYSTEM_UNICODE_UTF_ENCODESTATEINTERNAL_H
#define __SYSTEM_UNICODE_UTF_ENCODESTATEINTERNAL_H


#include <System/Unicode/UTF/EncodeState.hh>
#include <System/Unicode/UTF/UTFInternal.hh>


namespace System::Unicode::UTF
{


class EncodeStateInternal
{
public:

    bool isInitialState() const noexcept
    {
        return m_state == StateInitial;
    }

    bool consumeUTF8LeadByte(char8_t c) noexcept
    {
        if (!isInitialState())
            return false;

        if (!isUTF8LeadByte(c))
            return false;

        m_state = StateConsumeUTF8;
        m_in.utf8[0] = c;
        m_expectedLen = utf8LengthFromLeadByte(c);

        return true;
    }

    bool consumeUTF8ContinuationByte(char8_t c) noexcept
    {
        if (m_state != StateConsumeUTF8)
            return false;

        if (!isUTF8ContinuationByte(c))
            return false;

        m_in.utf8[m_len++] = c;

        if (m_len == m_expectedLen)
        {
            // We have a full codepoint so decode it.
            auto cp = utf8Decode(m_in.utf8[0], m_in.utf8[1], m_in.utf8[2], m_in.utf8[3]);
            if (!cp)
            {
                // Invalid encoding.
                m_state = StateInvalid;
                return false;
            }

            // Switch to an output state.
            m_out.utf32 = *cp;
            m_state = StateProduceAny;
        }

        return true;
    }

    bool consumeUTF16LeadingUnit(char16_t c) noexcept
    {
        if (!isInitialState())
            return false;

        // All code units except trailing surrogates are valid as leading code units.
        if (isUTF16TrailSurrogate(c))
            return false;

        // If this isn't a leading surrogate, we can go straight into the output state.
        if (!isUTF16LeadSurrogate(c)) [[likely]]
        {
            m_out.utf32 = c;
            m_state = StateProduceAny;
        }
        else
        {
            // We need a trailing surrogate to complete the codepoint.
            m_in.utf16[0] = c;
            m_state = StateConsumeUTF16;
            m_expectedLen = 2;
        }

        return true;
    }

    bool consumeUTF16TrailingUnit(char16_t c) noexcept
    {
        if (m_state != StateConsumeUTF16)
            return false;

        if (!isUTF16TrailSurrogate(c))
            return false;

        m_in.utf16[1] = c;
        auto cp = utf16Decode(m_in.utf16[0], m_in.utf16[1]);
        if (!cp)
        {
            // Invalid encoding.
            m_state = StateInvalid;
            return false;
        }

        m_state = StateProduceAny;
        m_out.utf32 = *cp;

        return true;
    }

    bool consumeUTF32(char32_t c)
    {
        if (!isInitialState())
            return false;

        if (!isValidCodepoint(c))
            return false;

        m_state = StateProduceAny;
        m_out.utf32 = c;

        return true;
    }


    static EncodeStateInternal* from(encode_state* state)
    {
        return reinterpret_cast<EncodeStateInternal*>(state);
    }

    static const EncodeStateInternal* from(const encode_state* state)
    {
        return reinterpret_cast<const EncodeStateInternal*>(state);
    }

private:

    static constexpr std::uint8_t StateInitial      = 0;
    static constexpr std::uint8_t StateConsumeUTF8  = 1;
    static constexpr std::uint8_t StateConsumeUTF16 = 2;
    static constexpr std::uint8_t StateProduceAny   = 3;
    static constexpr std::uint8_t StateProduceUTF8  = 4;
    static constexpr std::uint8_t StateProduceUTF16 = 5;
    static constexpr std::uint8_t StateInvalid      = 255;

    union
    {
        encode_state m_raw = {};

        struct
        {
            union
            {
                struct
                {
                    union
                    {
                        char8_t     utf8[3];
                        char16_t    utf16[1];
                    };
                } m_in;

                struct
                {
                    union
                    {
                        char8_t     utf8[4];
                        char16_t    utf16[2];
                        char32_t    utf32;
                    };
                } m_out;
            };

            std::uint8_t m_state;
            std::uint8_t m_len;
            std::uint8_t m_expectedLen;
        };
    };
};

static_assert(sizeof(EncodeStateInternal) == sizeof(encode_state));


} // namespace System::Unicode::UTF


#endif /* ifndef __SYSTEM_UNICODE_UTF_ENCODESTATEINTERNAL_H */
