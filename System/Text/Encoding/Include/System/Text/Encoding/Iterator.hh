#pragma once
#ifndef __SYSTEM_TEXT_ENCODING_ITERATOR_H
#define __SYSTEM_TEXT_ENCODING_ITERATOR_H


#include <System/Text/Encoding/Private/Config.hh>

#include <System/Text/Encoding/EncodingID.hh>

#include <compare>
#include <cstddef>
#include <memory>
#include <optional>
#include <span>


namespace System::Text::Encoding
{


// Forward declarations.
class Codec;


class Iterator
{
public:

    // Iterator typedefs.
    using value_type        = char32_t;

    // Suitably large for all reasonable codecs.
    //
    // This value forms part of the ABI and must not be changed.
    static constexpr std::size_t StorageSize = 8;

    // Storage for iterator context. The context must be trivially copyable/movable/destructible.
    using context_t = std::aligned_storage<StorageSize, alignof(void*)>;

    
    // Iterators are constexpr constructible, trivially destructible and trivially movable/copyable.
    constexpr Iterator() noexcept = default;
    constexpr Iterator(const Iterator&) noexcept = default;
    constexpr Iterator(Iterator&&) noexcept = default;
    constexpr Iterator& operator=(const Iterator&) noexcept = default;
    constexpr Iterator& operator=(Iterator&&) noexcept = default;
    ~Iterator() = default;

    Iterator(EncodingID encid, std::span<const std::byte> bytes) :
        Iterator(iteratorCreate(encid, bytes))
    {
    }

    Iterator(const Codec* codec, std::span<const std::byte> bytes) :
        Iterator(iteratorCreate(codec, bytes))
    {
    }


    // Validates that this iterator is valid to read a single codepoint from. Reasons it could be invalid include:
    //  - the iterator is beyond the given end iterator
    //  - the encoding is variable-length and the encoded codepoint extends beyond the given end iterator
    //  - the byte sequence at the iterator is not a valid encoding
    bool canReadOne(const Iterator& end = {}) const
    {
        return iteratorCanReadOne(this, &end);
    }

    // Similar to canReadOne but validates all codepoints between this iterator and the end iterator.
    bool canReadUntil(const Iterator& end = {}) const
    {
        return iteratorCanReadUntil(this, &end);
    }

    // Attempts to read a single codepoint from the iterator.
    std::optional<char32_t> tryRead(const Iterator& end = {}) const
    {

    }

    // Attempts to read a single codepoint from the iterator, returning the given replacement codepoint if canReadOne
    // would return false.
    char32_t readOr(char32_t replacement, Iterator end = {}) const
    {
        return iteratorReadOr(this, replacement, &end);
    }

    value_type operator*() const
    {
        return iteratorRead(this);
    }

    Iterator& operator++()
    {
        iteratorIncrement(this);
        return *this;
    }

    Iterator operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    friend constexpr std::partial_ordering operator<=>(const Iterator& a, const Iterator& b)
    {
        if (a.m_vtbl != b.m_vtbl)
            return std::partial_ordering::unordered;

        return a.m_ptr <=> b.m_ptr;
    }

    constexpr bool operator==(const Iterator& other) const = default;

protected:

    const void*             m_vtbl      = nullptr;
    const std::byte*        m_ptr       = nullptr;
    context_t               m_context   = {};

private:

    __SYSTEM_TEXT_ENCODING_EXPORT static Iterator   iteratorCreate(EncodingID, std::span<const std::byte>) __SYSTEM_TEXT_ENCODING_SYMBOL(Iterator.CreateFromEncodingIdAndBytes);
    __SYSTEM_TEXT_ENCODING_EXPORT static Iterator   iteratorCreate(const Codec*, std::span<const std::byte>) __SYSTEM_TEXT_ENCODING_SYMBOL(Iterator.CreateFromCodecAndBytes);
    __SYSTEM_TEXT_ENCODING_EXPORT static bool       iteratorCanReadOne(const Iterator*, const Iterator*) __SYSTEM_TEXT_ENCODING_SYMBOL(Iterator.CanReadOne);
    __SYSTEM_TEXT_ENCODING_EXPORT static bool       iteratorCanReadUntil(const Iterator*, const Iterator*) __SYSTEM_TEXT_ENCODING_SYMBOL(Iterator.CanReadUntil);
    __SYSTEM_TEXT_ENCODING_EXPORT static char32_t   iteratorReadOr(const Iterator*, char32_t, const Iterator*) __SYSTEM_TEXT_ENCODING_SYMBOL(Iterator.ReadOr);
    __SYSTEM_TEXT_ENCODING_EXPORT static void       iteratorIncrement(Iterator*) __SYSTEM_TEXT_ENCODING_SYMBOL(Iterator.Increment);
    __SYSTEM_TEXT_ENCODING_EXPORT static char32_t   iteratorRead(const Iterator*) __SYSTEM_TEXT_ENCODING_SYMBOL(Iterator.Read);
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_ITERATOR_H */
