#pragma once
#ifndef __SYSTEM_UNICODE_UTF_UTF8_H
#define __SYSTEM_UNICODE_UTF_UTF8_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <System/Unicode/UTF/Export.h>


#ifdef __cplusplus
extern "C" {
#endif


enum $System$Unicode$UTF$utf8_variant_t
{
    $System$Unicode$UTF$UTF8VariantNone             = 0,
    $System$Unicode$UTF$UTF8VariantOverlongNuls     = 0x0001,
    $System$Unicode$UTF$UTF8VariantSMPSurrogates    = 0x0002,
    $System$Unicode$UTF$UTF8VariantAllowSurrogates  = 0x0004,
    $System$Unicode$UTF$UTF8VariantAllowOverlong    = 0x0008,
    $System$Unicode$UTF$UTF8VariantAllowNonUnicode  = 0x0010,
};


// The "null" iterator can be created by initialising all fields to zero.
struct $System$Unicode$UTF$decode_iterator_utf8_t
{
    uintptr_t _reserved1;
    uintptr_t _reserved2;
    uintptr_t _reserved3;
    uintptr_t _reserved4;
};


__SYSTEM_UNICODE_UTF_EXPORT bool $System$Unicode$UTF$IsValidUTF8ByteSequence(const char*, size_t);

__SYSTEM_UNICODE_UTF_EXPORT char32_t $System$Unicode$UTF$DecodeOneUTF8(const char*, size_t);

__SYSTEM_UNICODE_UTF_EXPORT void $System$Unicode$UTF$DecodeIteratorUTF8CreateWithBytes(struct $System$Unicode$UTF$decode_iterator_utf8_t*, const char*, size_t, bool, $System$Unicode$UTF$utf8_variant_t);
__SYSTEM_UNICODE_UTF_EXPORT void $System$Unicode$UTF$DecodeIteratorUTF8Copy(struct $System$Unicode$UTF$decode_iterator_utf8_t*, const struct $System$Unicode$UTF$decode_iterator_utf8_t*);
__SYSTEM_UNICODE_UTF_EXPORT char32_t $System$Unicode$UTF$DecodeIteratorUTF8Get(const struct $System$Unicode$UTF$decode_iterator_utf8_t*);
__SYSTEM_UNICODE_UTF_EXPORT char32_t $System$Unicode$UTF$DecodeIteratorUTF8GetAndIncrement(struct $System$Unicode$UTF$decode_iterator_utf8_t*);
__SYSTEM_UNICODE_UTF_EXPORT void $System$Unicode$UTF$DecodeIteratorUTF8Decrement(struct $System$Unicode$UTF$decode_iterator_utf8_t*);
__SYSTEM_UNICODE_UTF_EXPORT void $System$Unicode$UTF$DecodeIteratorUTF8Destroy(struct $System$Unicode$UTF$decode_iterator_utf8_t*);
__SYSTEM_UNICODE_UTF_EXPORT int $System$Unicode$UTF$DecodeIteratorUTF8Compare(const struct $System$Unicode$UTF$decode_iterator_utf8_t*, const struct $System$Unicode$UTF$decode_iterator_utf8_t*);
__SYSTEM_UNICODE_UTF_EXPORT void $System$Unicode$UTF$DecodeIteratorUTF8SkipToEnd(struct $System$Unicode$UTF$decode_iterator_utf8_t*);


#ifdef __cplusplus
} // extern "C"
#endif


#ifdef __cplusplus


#include <iterator>


namespace System::Unicode::UTF
{


using _decode_iterator_utf8_t = $System$Unicode$UTF$decode_iterator_utf8_t;
using utf8_variant_t = $System$Unicode$UTF$utf8_variant_t;


namespace UTF8Variant
{

constexpr utf8_variant_t None            = $System$Unicode$UTF$UTF8VariantNone;

}


__SYSTEM_UNICODE_UTF_EXPORT
bool
IsValidUTF8ByteSequence(const char*, size_t)
__SYSTEM_UNICODE_UTF_SYMBOL(IsValidUTF8ByteSequence);

__SYSTEM_UNICODE_UTF_EXPORT
char32_t
DecodeOneUTF8(const char*, size_t)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeOneUTF8);

__SYSTEM_UNICODE_UTF_EXPORT
void DecodeIteratorUTF8CreateWithBytes(_decode_iterator_utf8_t*, const char*, size_t, bool, utf8_variant_t)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8CreateWithBytes);

__SYSTEM_UNICODE_UTF_EXPORT
void DecodeIteratorUTF8Copy(_decode_iterator_utf8_t*, const _decode_iterator_utf8_t*)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8Copy);

__SYSTEM_UNICODE_UTF_EXPORT
char32_t DecodeIteratorUTF8Get(const _decode_iterator_utf8_t*)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8Get);

__SYSTEM_UNICODE_UTF_EXPORT
char32_t DecodeIteratorUTF8GetAndIncrement(_decode_iterator_utf8_t*)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8GetAndIncrement);

__SYSTEM_UNICODE_UTF_EXPORT
void DecodeIteratorUTF8Decrement(_decode_iterator_utf8_t*)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8Decrement);

__SYSTEM_UNICODE_UTF_EXPORT
void DecodeIteratorUTF8Destroy(_decode_iterator_utf8_t*)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8Destroy);

__SYSTEM_UNICODE_UTF_EXPORT
int DecodeIteratorUTF8Compare(const _decode_iterator_utf8_t*, const _decode_iterator_utf8_t*)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8Compare);

__SYSTEM_UNICODE_UTF_EXPORT
int DecodeIteratorUTF8SkipToEnd(_decode_iterator_utf8_t*)
__SYSTEM_UNICODE_UTF_SYMBOL(DecodeIteratorUTF8SkipToEnd);


class decode_iterator_utf8_t
    : public _decode_iterator_utf8_t
{
public:

    using difference_type = ::std::ptrdiff_t;
    using value_type = char32_t;
    using pointer = char32_t*;
    using reference = char32_t&;
    using iterator_category = ::std::bidirectional_iterator_tag;

    constexpr decode_iterator_utf8_t()
        : _decode_iterator_utf8_t{0, 0, 0, 0}
    {
    }

    decode_iterator_utf8_t(const char* bytes, size_t len, bool external_rep = false, utf8_variant_t variant = UTF8Variant::None)
        : decode_iterator_utf8_t{}
    {
        DecodeIteratorUTF8CreateWithBytes(this, bytes, len, external_rep, variant);
    }

    decode_iterator_utf8_t(const decode_iterator_utf8_t& other)
        : decode_iterator_utf8_t{}
    {
        DecodeIteratorUTF8Copy(this, &other);
    }

    decode_iterator_utf8_t& operator=(const decode_iterator_utf8_t& other)
    {
        DecodeIteratorUTF8Copy(this, &other);
        return *this;
    }

    ~decode_iterator_utf8_t()
    {
        DecodeIteratorUTF8Destroy(this);
    }

    decode_iterator_utf8_t& operator++()
    {
        DecodeIteratorUTF8GetAndIncrement(this);
        return *this;
    }

    decode_iterator_utf8_t operator++(int)
    {
        decode_iterator_utf8_t copy{*this};
        DecodeIteratorUTF8GetAndIncrement(this);
        return copy;
    }

    decode_iterator_utf8_t& operator--()
    {
        DecodeIteratorUTF8Decrement(this);
        return *this;
    }

    decode_iterator_utf8_t operator--(int)
    {
        decode_iterator_utf8_t copy{*this};
        DecodeIteratorUTF8Decrement(this);
        return copy;
    }

    char32_t operator*() const
    {
        return DecodeIteratorUTF8Get(this);
    }

    bool operator==(const decode_iterator_utf8_t& other) const
    {
        return (DecodeIteratorUTF8Compare(this, &other) == 0);
    }

    bool operator!=(const decode_iterator_utf8_t& other) const
    {
        return !operator==(other);
    }
};


class UTF8Decoder
{
public:

    decode_iterator_utf8_t begin() const
    {
        return cbegin();
    }

    decode_iterator_utf8_t end() const
    {
        return cend();
    }

    decode_iterator_utf8_t cbegin() const
    {
        return m_iter;
    }

    decode_iterator_utf8_t cend() const
    {
        decode_iterator_utf8_t end{m_iter};
        DecodeIteratorUTF8SkipToEnd(&end);
        return end;
    }

private:

    decode_iterator_utf8_t m_iter = {};
};


}
#endif // ifdef __cplusplus



#endif /* ifndef __SYSTEM_UNICODE_UTF_UTF8_H */
