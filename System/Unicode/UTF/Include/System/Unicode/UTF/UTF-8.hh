#ifndef __SYSTEM_UNICODE_UTF_UTF8_HH
#define __SYSTEM_UNICODE_UTF_UTF8_HH


#include <System/Unicode/UTF/Private/Config.hh>

#ifdef __SYSTEM_UNICODE_UTF_MINIUTF
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#else
#  include <cstddef>
#  include <cstdint>
#  include <stdexcept>
#  include <ranges>
#endif

#include <System/Unicode/UTF/EncodeState.hh>


namespace System::Unicode::UTF
{

/*
namespace UTF8
{


constexpr std::size_t ByteLengthOfCodepoint(char32_t c);

constexpr bool IsValidLeadByte(char8_t c);

constexpr bool IsValidContinuationByte(char8_t c)
{
    return (c & 0xC0) == 0x80;
}

constexpr std::size_t ExpectedByteLengthFromLeadByte(char8_t c);


} // namespace UTF8


enum class SurrogateMode
{
    Reject,         // Follow the standard. Reject all surrogates.
    UCS2,           // Surrogates are decoded but pairs are not joined.
    CESU8,          // Surrogates are decoded, pairs are joined as in UTF-16. Lone surrogates are accepted.
};

enum class OverlongMode
{
    Reject,         // Follow the standard. No overlong encodings are permitted.
    NulOnly,        // Nul is encoded as two bytes. No other overlong encodings are permitted.
    Limited,        // Overlong encodings are allowed as long as they decode to a valid codepoint.
    Allow,          // Any overlong encodings are accepted.
};


struct utf8_decoder_options
{
    SurrogateMode   surrogates  = SurrogateMode::Reject;
    OverlongMode    overlong    = OverlongMode::Reject;
};


template <utf8_decoder_options Options>
class utf8_decoder_traits
{
public:

    using value_type = std::conditional_t<Options.overlong == OverlongMode::Allow, std::uint32_t, char32_t>;


    static constexpr bool isValidLeadByte(char8_t) noexcept;

    static constexpr bool isValidContinuationByte(char8_t) noexcept;

    static constexpr std::size_t expectedLengthFromLead(char8_t) noexcept;

    // Takes a uint32_t because we don't yet know the value is valid for storing as a char32_t.
    static constexpr bool isValidCodePoint(std::uint32_t) noexcept;

    static constexpr bool isOverlong(char32_t, std::size_t length) noexcept;
};


template <utf8_decoder_options Options>
class basic_utf8_input_iterator_base
{
public:

    using difference_type   = std::ptrdiff_t;
    using value_type        = std::conditional_t<Options.overlong == OverlongMode::Allow, std::uint32_t, char32_t>;
    using reference         = value_type;
    using iterator_category = std::input_iterator_tag;
    using pointer           = void;


    friend constexpr bool operator==(basic_utf8_input_iterator_base, basic_utf8_input_iterator_base) = default;

protected:

    constexpr void error(const char* why)
    {
        throw std::domain_error(why);
    }

    constexpr std::size_t codeUnitCount() const;
};


template <utf8_decoder_options Options>
class basic_utf8_input_iterator :
    public basic_utf8_input_iterator_base<Options>
{
public:

    using value_type = typename basic_utf8_input_iterator_base<Options>::value_type;

    constexpr basic_utf8_input_iterator() = default;
    constexpr basic_utf8_input_iterator(const basic_utf8_input_iterator&) = default;
    constexpr ~basic_utf8_input_iterator() = default;
    constexpr basic_utf8_input_iterator& operator=(const basic_utf8_input_iterator&) = default;

    constexpr basic_utf8_input_iterator(const char8_t* begin, const char8_t* end) :
        m_ptr{begin}, m_end{end}
    {
    }

    constexpr value_type operator*() const;

    constexpr basic_utf8_input_iterator& operator++();

    constexpr basic_utf8_input_iterator operator++(int);

    friend constexpr bool operator==(basic_utf8_input_iterator, basic_utf8_input_iterator) = default;

private:

    const char8_t*  m_ptr = nullptr;
    const char8_t*  m_end = nullptr;
};


template <utf8_decoder_options Options>
class basic_utf8_nul_terminated_input_iterator :
    public basic_utf8_input_iterator_base<Options>
{
public:

    using value_type = typename basic_utf8_input_iterator_base<Options>::value_type;

    constexpr basic_utf8_nul_terminated_input_iterator() = default;
    constexpr basic_utf8_nul_terminated_input_iterator(const basic_utf8_nul_terminated_input_iterator&) = default;
    constexpr ~basic_utf8_nul_terminated_input_iterator() = default;
    constexpr basic_utf8_nul_terminated_input_iterator& operator=(const basic_utf8_nul_terminated_input_iterator&) = default;

    explicit constexpr basic_utf8_nul_terminated_input_iterator(const char8_t* ptr) :
        m_ptr{ptr}
    {
    }

    constexpr value_type operator*() const;

    constexpr basic_utf8_nul_terminated_input_iterator& operator++();

    constexpr basic_utf8_nul_terminated_input_iterator operator++(int);

    friend constexpr bool operator==(basic_utf8_nul_terminated_input_iterator, basic_utf8_nul_terminated_input_iterator) = default;

private:

    const char8_t*  m_ptr = nullptr;
};


template <utf8_decoder_options Options>
class basic_utf8_nul_terminated_input_sentinel
{
public:

    friend constexpr bool operator==(basic_utf8_nul_terminated_input_sentinel, basic_utf8_nul_terminated_input_iterator<Options> i)
    {
        return (*i == U'\0');
    }
};


template <utf8_decoder_options Options>
class basic_utf8_input_view :
    public std::ranges::view_interface<basic_utf8_input_view<Options>>
{
public:

    constexpr basic_utf8_input_view() = default;
    constexpr basic_utf8_input_view(const basic_utf8_input_view&) = default;
    constexpr ~basic_utf8_input_view() = default;
    constexpr basic_utf8_input_view& operator=(const basic_utf8_input_view&) = default;

    constexpr basic_utf8_input_view(const char8_t* ptr, std::size_t len) :
        m_ptr{ptr}, m_len{len}
    {
    }

    constexpr auto begin() const noexcept
    {
        return basic_utf8_input_iterator<Options>{m_ptr, m_ptr + m_len};
    }

    constexpr auto end() const noexcept
    {
        return basic_utf8_input_iterator<Options>{m_ptr + m_len, m_ptr + m_len};
    }

private:

    const char8_t*  m_ptr = nullptr;
    std::size_t     m_len = 0;
};


template <utf8_decoder_options Options>
class basic_utf8_nul_terminated_input_view :
    public std::ranges::view_interface<basic_utf8_nul_terminated_input_view<Options>>
{
public:

    constexpr basic_utf8_nul_terminated_input_view() = default;
    constexpr basic_utf8_nul_terminated_input_view(const basic_utf8_nul_terminated_input_view&) = default;
    constexpr ~basic_utf8_nul_terminated_input_view() = default;
    constexpr basic_utf8_nul_terminated_input_view& operator=(const basic_utf8_nul_terminated_input_view&) = default;

    explicit constexpr basic_utf8_nul_terminated_input_view(const char8_t* ptr) :
        m_ptr{ptr}
    {
    }

    constexpr auto begin() const noexcept
    {
        return basic_utf8_nul_terminated_input_iterator<Options>{m_ptr};
    }

    constexpr auto end() const noexcept
    {
        return basic_utf8_nul_terminated_input_sentinel<Options>{};
    }

private:

    const char8_t*  m_ptr = nullptr;
};


using utf8_input_iterator = basic_utf8_input_iterator<{}>;
using utf8_nul_terminated_input_iterator = basic_utf8_nul_terminated_input_iterator<{}>;
using utf8_nul_terminated_input_sentinel = basic_utf8_nul_terminated_input_sentinel<{}>;

using utf8_input_view = basic_utf8_input_view<{}>;
using utf8_nul_terminated_input_view = basic_utf8_nul_terminated_input_view<{}>;
*/

__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8ByteLengthOfCodepoint(char32_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.ByteLengthOfCodepoint);

__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidLeadByte(char8_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidLeadByte);
__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidContinuationByte(char8_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidContinuationByte);

__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8ExpectedByteLengthFromLeadByte(char8_t) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.ExpectedByteLengthFromLeadByte);

__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidSequenceSingle(const char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidSequenceSingle);
__SYSTEM_UNICODE_UTF_EXPORT bool UTF8IsValidSequenceAll(const char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.IsValidSequenceAll);

__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8Consume(encode_state*, const char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.Consume);
__SYSTEM_UNICODE_UTF_EXPORT std::size_t UTF8Produce(encode_state*, char8_t*, std::size_t len) __SYSTEM_UNICODE_UTF_SYMBOL(UTF8.Produce);



} // namespace System::Unicode::UTF


/*namespace std::ranges
{


template <>
inline constexpr bool enable_borrowed_range<System::Unicode::UTF::utf8_input_view> = true;

template <>
inline constexpr bool enable_borrowed_range<System::Unicode::UTF::utf8_nul_terminated_input_view> = true;


} // namespace std::ranges*/


#endif /* ifndef __SYSTEM_UNICODE_UTF_UTF8_HH */
