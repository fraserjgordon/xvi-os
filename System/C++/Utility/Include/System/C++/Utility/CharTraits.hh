#pragma once
#ifndef __SYSTEM_CXX_UTILITY_CHARTRAITS_H
#define __SYSTEM_CXX_UTILITY_CHARTRAITS_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class> struct char_traits;


namespace __detail
{

template <class _T>
constexpr int __char_traits_compare(const _T* __s1, const _T* __s2, size_t __n)
{
    for (size_t __i = 0; __i < __n; ++__i)
    {
        int __diff = static_cast<int>(__s1[__n] - __s2[__n]);
        if (__diff != 0)
            return __diff;
    }

    return 0;
}

template <class _T>
constexpr size_t __char_traits_length(const _T* __s)
{
    size_t __n = 0;
    while (__s[__n] != _T())
        ++__n;
    return __n;
}

template <class _T>
constexpr const _T* __char_traits_find(const _T* __s, size_t __n, const _T& __a)
{
    for (size_t __i = 0; __i < __n; ++__i)
        if (char_traits<_T>::eq(__s[__i], __a))
            return &__s[__i];

    return nullptr;
}

template <class _T>
constexpr _T* __char_traits_move(_T* __dest, const _T* __src, size_t __n)
{
    // True iff the beginning of dest overlaps the end of src.
    if (!std::is_constant_evaluated() && __dest >= __src && __dest < (__src + __n)) 
    {
        for (size_t __i = 0; __i < __n; ++__i)
            __dest[__n - 1 - __i] = __src[__n - 1 - __i];
        return __dest;
    }

    for (size_t __i = 0; __i < __n; ++__i)
        __dest[__i] = __src[__i];
    return __dest;
}

template <class _T>
constexpr _T* __char_traits_copy(_T* __dest, const _T* __src, size_t __n)
{
    return __char_traits_move(__dest, __src, __n);
}

template <class _T>
constexpr _T* __char_traits_assign(_T* __dest, size_t __n, _T __a)
{
    for (size_t __i = 0; __i < __n; ++__i)
        char_traits<_T>::assign(__dest[__i], __a);
    return __dest;
}

} // namespace __detail


template <class> struct char_traits;

template <> struct char_traits<char>
{
    using char_type         = char;
    using int_type          = int;
    using off_type          = streamoff;
    using pos_type          = streampos;
    using state_type        = mbstate_t;
    using comparison_cateogory = std::strong_ordering;

    static constexpr void assign(char_type& __c1, const char_type& __c2) noexcept
    {
        __c1 = __c2;
    }

    static constexpr bool eq(char_type __c1, char_type __c2) noexcept
    {
        return static_cast<unsigned char>(__c1) == static_cast<unsigned char>(__c2);
    }

    static constexpr bool lt(char_type __c1, char_type __c2) noexcept
    {
        return static_cast<unsigned char>(__c1) < static_cast<unsigned char>(__c2);
    }

    static constexpr int compare(const char_type* __s1, const char_type* __s2, size_t __n)
    {
        for (size_t __i = 0; __i < __n; ++__i)
        {
            int __diff = static_cast<unsigned char>(__s1[__i]) - static_cast<unsigned char>(__s2[__i]);
            if (__diff != 0)
                return __diff;
        }

        return 0;
    }

    static constexpr size_t length(const char_type* __s)
    {
        return __detail::__char_traits_length(__s);
    }

    static constexpr const char_type* find(const char_type* __s, size_t __n, const char_type& __a)
    {
        return __detail::__char_traits_find(__s, __n, __a);
    }

    static constexpr char_type* move(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_move(__s1, __s2, __n);
    }

    static constexpr char_type* copy(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_copy(__s1, __s2, __n);
    }

    static constexpr char_type* assign(char_type* __s, size_t __n, char_type __a)
    {
        return __detail::__char_traits_assign(__s, __n, __a);
    }

    static constexpr int_type not_eof(int_type __c) noexcept
    {
        return __c == eof() ? 0 : __c;
    }

    static constexpr char_type to_char_type(int_type __c) noexcept
    {
        return static_cast<char_type>(__c);
    }

    static constexpr int_type to_int_type(char_type __c) noexcept
    {
        return static_cast<int_type>(__c);
    }

    static constexpr bool eq_int_type(int_type __c1, int_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr int_type eof() noexcept
    {
        return -1;
    }
};

template <> struct char_traits<char8_t>
{
    using char_type         = char8_t;
    using int_type          = unsigned int;
    using off_type          = streamoff;
    using pos_type          = u8streampos;
    using state_type        = mbstate_t;
    using comparison_category = std::strong_ordering;

    static constexpr void assign(char_type& __c1, const char_type& __c2) noexcept
    {
        __c1 = __c2;
    }

    static constexpr bool eq(char_type __c1, char_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr bool lt(char_type __c1, char_type __c2) noexcept
    {
        return __c1 < __c2;
    }

    static constexpr int compare(const char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_compare(__s1, __s2, __n);
    }

    static constexpr size_t length(const char_type* __s)
    {
        return __detail::__char_traits_length(__s);
    }

    static constexpr const char_type* find(const char_type* __s, size_t __n, const char_type& __a)
    {
        return __detail::__char_traits_find(__s, __n, __a);
    }

    static constexpr char_type* move(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_move(__s1, __s2, __n);
    }

    static constexpr char_type* copy(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_copy(__s1, __s2, __n);
    }

    static constexpr char_type* assign(char_type* __s, size_t __n, char_type __a)
    {
        return __detail::__char_traits_assign(__s, __n, __a);
    }

    static constexpr int_type not_eof(int_type __c) noexcept
    {
        return __c == eof() ? 0 : __c;
    }

    static constexpr char_type to_char_type(int_type __c) noexcept
    {
        return static_cast<char_type>(__c);
    }

    static constexpr int_type to_int_type(char_type __c) noexcept
    {
        return static_cast<int_type>(__c);
    }

    static constexpr bool eq_int_type(int_type __c1, int_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr int_type eof() noexcept
    {
        return static_cast<int_type>(~0);
    }
};

template <> struct char_traits<char16_t>
{
    using char_type         = char16_t;
    using int_type          = uint_least16_t;
    using off_type          = streamoff;
    using pos_type          = u16streampos;
    using state_type        = mbstate_t;
    using comparison_category = std::strong_ordering;

    static constexpr void assign(char_type& __c1, const char_type& __c2) noexcept
    {
        __c1 = __c2;
    }

    static constexpr bool eq(char_type __c1, char_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr bool lt(char_type __c1, char_type __c2) noexcept
    {
        return __c1 < __c2;
    }

    static constexpr int compare(const char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_compare(__s1, __s2, __n);
    }

    static constexpr size_t length(const char_type* __s)
    {
        return __detail::__char_traits_length(__s);
    }

    static constexpr const char_type* find(const char_type* __s, size_t __n, const char_type& __a)
    {
        return __detail::__char_traits_find(__s, __n, __a);
    }

    static constexpr char_type* move(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_move(__s1, __s2, __n);
    }

    static constexpr char_type* copy(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_copy(__s1, __s2, __n);
    }

    static constexpr char_type* assign(char_type* __s, size_t __n, char_type __a)
    {
        return __detail::__char_traits_assign(__s, __n, __a);
    }

    static constexpr int_type not_eof(int_type __c) noexcept
    {
        return __c == eof() ? 0 : __c;
    }

    static constexpr char_type to_char_type(int_type __c) noexcept
    {
        return static_cast<char_type>(__c);
    }

    static constexpr int_type to_int_type(char_type __c) noexcept
    {
        return static_cast<int_type>(__c);
    }

    static constexpr bool eq_int_type(int_type __c1, int_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr int_type eof() noexcept
    {
        return static_cast<int_type>(~0);
    }
};

template <> struct char_traits<char32_t>
{
    using char_type         = char32_t;
    using int_type          = uint_least32_t;
    using off_type          = streamoff;
    using pos_type          = u32streampos;
    using state_type        = mbstate_t;
    using comparison_category = std::strong_ordering;

    static constexpr void assign(char_type& __c1, const char_type& __c2) noexcept
    {
        __c1 = __c2;
    }

    static constexpr bool eq(char_type __c1, char_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr bool lt(char_type __c1, char_type __c2) noexcept
    {
        return __c1 < __c2;
    }

    static constexpr int compare(const char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_compare(__s1, __s2, __n);
    }

    static constexpr size_t length(const char_type* __s)
    {
        return __detail::__char_traits_length(__s);
    }

    static constexpr const char_type* find(const char_type* __s, size_t __n, const char_type& __a)
    {
        return __detail::__char_traits_find(__s, __n, __a);
    }

    static constexpr char_type* move(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_move(__s1, __s2, __n);
    }

    static constexpr char_type* copy(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_copy(__s1, __s2, __n);
    }

    static constexpr char_type* assign(char_type* __s, size_t __n, char_type __a)
    {
        return __detail::__char_traits_assign(__s, __n, __a);
    }

    static constexpr int_type not_eof(int_type __c) noexcept
    {
        return __c == eof() ? 0 : __c;
    }

    static constexpr char_type to_char_type(int_type __c) noexcept
    {
        return static_cast<char_type>(__c);
    }

    static constexpr int_type to_int_type(char_type __c) noexcept
    {
        return static_cast<int_type>(__c);
    }

    static constexpr bool eq_int_type(int_type __c1, int_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr int_type eof() noexcept
    {
        return static_cast<int_type>(~0);
    }
};

template <> struct char_traits<wchar_t>
{
    using char_type         = wchar_t;
    using int_type          = __wint_t;
    using off_type          = streamoff;
    using pos_type          = wstreampos;
    using state_type        = mbstate_t;
    using comparison_category = std::strong_ordering;

    static constexpr void assign(char_type& __c1, const char_type& __c2) noexcept
    {
        __c1 = __c2;
    }

    static constexpr bool eq(char_type __c1, char_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr bool lt(char_type __c1, char_type __c2) noexcept
    {
        return __c1 < __c2;
    }

    static constexpr int compare(const char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_compare(__s1, __s2, __n);
    }

    static constexpr size_t length(const char_type* __s)
    {
        return __detail::__char_traits_length(__s);
    }

    static constexpr const char_type* find(const char_type* __s, size_t __n, const char_type& __a)
    {
        return __detail::__char_traits_find(__s, __n, __a);
    }

    static constexpr char_type* move(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_move(__s1, __s2, __n);
    }

    static constexpr char_type* copy(char_type* __s1, const char_type* __s2, size_t __n)
    {
        return __detail::__char_traits_copy(__s1, __s2, __n);
    }

    static constexpr char_type* assign(char_type* __s, size_t __n, char_type __a)
    {
        return __detail::__char_traits_assign(__s, __n, __a);
    }

    static constexpr int_type not_eof(int_type __c) noexcept
    {
        return __c == eof() ? 0 : __c;
    }

    static constexpr char_type to_char_type(int_type __c) noexcept
    {
        return static_cast<char_type>(__c);
    }

    static constexpr int_type to_int_type(char_type __c) noexcept
    {
        return static_cast<int_type>(__c);
    }

    static constexpr bool eq_int_type(int_type __c1, int_type __c2) noexcept
    {
        return __c1 == __c2;
    }

    static constexpr int_type eof() noexcept
    {
        return static_cast<int_type>(-1);
    }
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_CHARTRAITS_H */
