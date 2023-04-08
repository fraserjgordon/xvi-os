#pragma once
#ifndef __SYSTEM_ABI_STRING_STRINGIMPL_H
#define __SYSTEM_ABI_STRING_STRINGIMPL_H


#include <System/ABI/String/String.hh>


// As this implements a number of built-in functions, prohibit auto-generated calls to them.
#if defined(__GNUC__) && !defined(__clang__)
#  pragma GCC optimize "no-tree-loop-distribute-patterns"
#endif


namespace System::ABI::String
{


using byte_t = std::byte;


template <class T>
static constexpr void memory_copy(T* dest, const T* source, std::size_t count)
{
    // Check for overlaps.
    if (dest >= source && dest < (source + count))
    {
        // The beginning of the destination buffer lies within the source buffer. Perform the copy backwards.
        for (std::size_t i = 0; i < count; ++i)
            dest[count - i - 1] = source[count - i - 1];
        return;
    }
    
    // Perform a normal forwards copy.
    for (std::size_t i = 0; i < count; ++i)
        dest[i] = source[i];
}

template <class T, class TV>
static constexpr T* memory_copy_until(T* __restrict dest, const T* __restrict source, TV terminator, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        auto c = source[i];
        dest[i] = c;

        if (TV(c) == terminator)
            return &dest[i] + 1;
    }

    return nullptr;
}

template <class T, class TV>
static constexpr T* memory_find(T* array, TV value, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        if (TV(array[i]) == value)
            return &array[i];
    }

    return nullptr;
}

template <class T, class TV>
static constexpr T* memory_find_last(T* array, TV value, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        if (TV(array[count - i -1]) == value)
            return &array[count - i - 1];
    }

    return nullptr;
}

template <class T, class I = int>
static constexpr auto memory_compare(const T* one, const T* two, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        I diff = I(one[i]) - I(two[i]);
        if (diff != 0)
            return diff;
    }

    return 0;
}

template <class T, class TV>
static constexpr void memory_set(T* array, TV value, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
        array[i] = T(value);
}


template <class T, class U>
static constexpr T* memory_search_naive(T* string, std::size_t string_len, const U* pattern, std::size_t pattern_len)
{
    if (pattern_len == 0)
        return string;

    while (string_len-- >= pattern_len)
    {
        if (memory_compare(string, pattern, pattern_len) == 0)
            return string;
        ++string;
    }

    return nullptr;
}

// Bayer-Moore-Horspool string search algorithm.
template <class T, class U>
static constexpr T* memory_search_horspool(T* string, std::size_t string_len, const U* pattern, std::size_t pattern_len)
{
    // Preprocess the pattern string.
    constexpr std::size_t TableSize = std::size_t(1) << ((sizeof(U) * __CHAR_BIT__) - 1);
    std::size_t table[TableSize];
    for (std::size_t i = 0; i < TableSize; ++i)
        table[i] = pattern_len;
    for (std::size_t i = 0; i < (pattern_len-1); ++i)
        table[std::size_t(pattern[i])] = pattern_len - i - 1;

    // Perform the search.
    std::size_t offset = 0;
    while ((string_len - offset) >= pattern_len)
    {
        if (memory_compare(string + offset, pattern, pattern_len) == 0)
            return string + offset;

        offset += table[std::size_t(string[offset + pattern_len - 1])];
    }

    return nullptr;
}

template <class T, class U>
static constexpr T* memory_search(T* string, std::size_t string_len, const U* pattern, std::size_t pattern_len)
{
    // Arbitrary threshold for selecting algorithms.
    if (pattern_len == 0)
        return string;
    else if (pattern_len < 8)
        return memory_search_naive(string, string_len, pattern, pattern_len);
    else
        return memory_search_horspool(string, string_len, pattern, pattern_len);
}


template <class C, C Nul = C(0)>
static constexpr C* string_copy(C* __restrict dest, const C* __restrict source)
{
    C current;
    do
    {
        current = *source++;
        *dest++ = current;
    }
    while (current != Nul);
    return dest;
}

template <class C, C Nul = C(0)>
static constexpr C* string_copy_n(C* __restrict dest, const C* __restrict source, std::size_t n)
{
    // Same semantics as strncpy: nul-fill and leave unterminated on overflow.
    while (n > 0 && *source != Nul)
    {
        *dest++ = *source++;
        --n;
    }

    memory_set(dest, Nul, n);
    return dest;
}

template <class C, class CV, CV Nul = C(0)>
static constexpr C* string_find(C* string, CV value)
{
    while (true)
    {
        if (*string == value)
            return string;

        if (*string == Nul)
            return nullptr;

        ++string;
    }

    return nullptr;
}

template <class C, class CV, CV Nul = C(0)>
static constexpr C* string_find_last(C* string, CV value)
{
    C* last = nullptr;
    while (true)
    {
        if (*string == value)
            last = string;

        if (*string == Nul)
            break;

        string++;
    }

    return last;
}

template <class C, class I = int, C Nul = C(0)>
static constexpr auto string_compare(const C* one, const C* two)
{
    C one_current;
    C two_current;
    I diff;
    do
    {
        one_current = *one++;
        two_current = *two++;
        diff = I(one_current) - I(two_current);
        if (diff != 0)
            return diff;
    }
    while (one_current != Nul && two_current != Nul);

    return diff;
}

template <class C, class I = int, C Nul = C(0)>
static constexpr int string_compare(const C* one, const C* two, std::size_t n)
{
    while (n-- > 0)
    {
        C one_current = *one++;
        C two_current = *two++;
        I diff = I(one_current) - I(two_current);
        
        if (diff != 0)
            return diff;
        
        if (one_current == Nul)
            return 0;
    }

    return 0;
}

template <class C, C Nul = C(0)>
static constexpr std::size_t string_length(const C* string)
{
    std::size_t len = 0;
    while (*string++ != Nul)
        ++len;
    return len;
}

template <class C, C Nul = C(0)>
static constexpr std::size_t string_length(const C* string, std::size_t n)
{
    std::size_t len = 0;
    while (n-- > 0 && *string++ != Nul)
        ++len;
    return len;
}

template <class C, C Nul = C(0)>
static constexpr std::size_t string_copy_l(C* __restrict dest, const C* __restrict source, std::size_t n)
{
    // Same semantics as strlcpy; nul-terminate and truncate on overflow.
    std::size_t copied = 0;
    while (n-- > 1 && *source != Nul)
    {
        *dest++ = *source++;
        ++copied;
    }
    *dest = Nul;

    return (n >= 1) ? copied : (copied + string_length(source));
}

template <class C, C Nul = C(0)>
static constexpr void string_concatenate(C* __restrict dest, const C* __restrict source)
{
    auto dest_len = string_length(dest);
    string_copy(dest + dest_len, source);
}

template <class C, C Nul = C(0)>
static constexpr void string_concatenate_n(C* __restrict dest, const C* __restrict source, std::size_t n)
{
    auto dest_len = string_length(dest, n);
    string_copy_n(dest + dest_len, source, n - dest_len);
}

template <class C, C Nul = C(0)>
static constexpr std::size_t string_concatenate_l(C* __restrict dest, const char* __restrict source, std::size_t n)
{
    auto dest_len = string_length(dest, n);

    if (dest_len >= n)
        return n;

    return dest_len + string_copy_l(dest + dest_len, source, n - dest_len);
}

template <class C, class C2, C Nul = C(0)>
static constexpr C* string_search_naive(C* string, const C2* pattern, std::size_t pattern_len)
{
    while (*string != Nul)
    {
        if (string_compare(string, pattern, pattern_len) == 0)
            return string;
        ++string;
    }

    return nullptr;
}

// Bayer-Moore-Horspool string search algorithm.
template <class C, class C2, C Nul = C(0)>
static constexpr C* string_search_horspool(C* string, const C2* pattern, std::size_t pattern_len)
{
    // Preprocess the pattern string.
    constexpr std::size_t TableSize = std::size_t(1) << ((sizeof(C) * __CHAR_BIT__) - 1);
    std::size_t table[TableSize];
    for (std::size_t i = 0; i < TableSize; ++i)
        table[i] = pattern_len;
    for (std::size_t i = 0; i < (pattern_len-1); ++i)
        table[std::make_unsigned_t<C>(pattern[i])] = pattern_len - i - 1;

    // Perform the search.
    std::size_t string_len = string_length(string);
    std::size_t offset = 0;
    while ((string_len - offset) >= pattern_len)
    {
        if (memory_compare(string + offset, pattern, pattern_len) == 0)
            return string + offset;

        offset += table[std::make_unsigned_t<C>(string[offset + pattern_len - 1])];
    }

    return nullptr;
}

template <class C, class C2, C Nul = C(0)>
static constexpr C* string_search(C* string, const C2* pattern)
{
    // Arbitrary threshold for selecting algorithms.
    std::size_t pattern_len = string_length(pattern);
    if (pattern_len == 0)
        return string;
    else if (pattern_len < 8)
        return string_search_naive(string, pattern, pattern_len);
    else
        return string_search_horspool(string, pattern, pattern_len);
}


} // namespace System::ABI::String


#endif /* ifndef __SYSTEM_ABI_STRING_STRINGIMPL_H */
