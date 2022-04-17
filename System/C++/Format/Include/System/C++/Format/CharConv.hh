#ifndef __SYSTEM_CXX_FORMAT_CHARCONV_H
#define __SYSTEM_CXX_FORMAT_CHARCONV_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/Utility/SystemError.hh>

#include <System/C++/Format/Private/Config.hh>


namespace __XVI_STD_FORMAT_NS
{


enum class chars_format
{
    scientific  = 0x01,
    fixed       = 0x02,
    hex         = 0x04,
    general     = fixed | scientific,
};

struct to_chars_result
{
    char*       ptr;
    std::errc   ec;

    friend bool operator==(const to_chars_result&, const to_chars_result&) = default;
};

struct from_chars_result
{
    const char* ptr;
    std::errc   ec;

    friend bool operator==(const from_chars_result&, const from_chars_result&) = default;
};


namespace __detail
{

template <class _I>
to_chars_result __to_chars(char* __first, char* __last, _I __value, int __base)
{
    auto __original_first = __first;
    auto __push = [&__first, __last](char __c) -> bool
    {
        if (__first >= __last)
            return false;

        *__first++ = __c;
        return true;
    };

    constexpr char _C[36] =
    {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'
    };

    if (__value < 0)
    {
        if (!__push('-'))
            return {__first, std::errc::value_too_large};

        // Recurse to the unsigned variant as the absolute value may not be representable otherwise.
        return __to_chars(__first, __last, static_cast<std::make_unsigned_t<_I>>(-__value), __base);
    }

    if (__value == 0)
    {
        if (__push('0'))
            return {__first, {}};
        else
            return {__first, std::errc::value_too_large};
    }

    while (__value > 0)
    {
        if (!__push(_C[__value % static_cast<_I>(__base)]))
            return {__first, std::errc::value_too_large};

        __value /= static_cast<_I>(__base);
    }

    std::reverse(__original_first, __first);

    return {__first, {}};
}


struct __from_chars_table
{
    signed char __table[256];
};

constexpr __from_chars_table __make_from_chars_table()
{
    __from_chars_table __fct {};
    
    for (int __i = 0; __i < 256; ++__i)
        __fct.__table[__i] = -1;

    __fct.__table[static_cast<unsigned char>('0')] = 0;
    __fct.__table[static_cast<unsigned char>('1')] = 1;
    __fct.__table[static_cast<unsigned char>('2')] = 2;
    __fct.__table[static_cast<unsigned char>('3')] = 3;
    __fct.__table[static_cast<unsigned char>('4')] = 4;
    __fct.__table[static_cast<unsigned char>('5')] = 5;
    __fct.__table[static_cast<unsigned char>('6')] = 6;
    __fct.__table[static_cast<unsigned char>('7')] = 7;
    __fct.__table[static_cast<unsigned char>('8')] = 8;
    __fct.__table[static_cast<unsigned char>('9')] = 9;
    __fct.__table[static_cast<unsigned char>('a')] = 10;
    __fct.__table[static_cast<unsigned char>('b')] = 11;
    __fct.__table[static_cast<unsigned char>('c')] = 12;
    __fct.__table[static_cast<unsigned char>('d')] = 13;
    __fct.__table[static_cast<unsigned char>('e')] = 14;
    __fct.__table[static_cast<unsigned char>('f')] = 15;
    __fct.__table[static_cast<unsigned char>('g')] = 16;
    __fct.__table[static_cast<unsigned char>('h')] = 17;
    __fct.__table[static_cast<unsigned char>('i')] = 18;
    __fct.__table[static_cast<unsigned char>('j')] = 19;
    __fct.__table[static_cast<unsigned char>('k')] = 20;
    __fct.__table[static_cast<unsigned char>('l')] = 21;
    __fct.__table[static_cast<unsigned char>('m')] = 22;
    __fct.__table[static_cast<unsigned char>('n')] = 23;
    __fct.__table[static_cast<unsigned char>('o')] = 24;
    __fct.__table[static_cast<unsigned char>('p')] = 25;
    __fct.__table[static_cast<unsigned char>('q')] = 26;
    __fct.__table[static_cast<unsigned char>('r')] = 27;
    __fct.__table[static_cast<unsigned char>('s')] = 28;
    __fct.__table[static_cast<unsigned char>('t')] = 29;
    __fct.__table[static_cast<unsigned char>('u')] = 30;
    __fct.__table[static_cast<unsigned char>('v')] = 31;
    __fct.__table[static_cast<unsigned char>('w')] = 32;
    __fct.__table[static_cast<unsigned char>('x')] = 33;
    __fct.__table[static_cast<unsigned char>('y')] = 34;
    __fct.__table[static_cast<unsigned char>('z')] = 35;

    return __fct;
}

template <class _I>
from_chars_result __from_chars(const char* __first, const char* __last, _I& __value, int __base)
{
    if (__first < __last && std::is_signed_v<_I> && *__first == '-')
    {
        // Attempt to decode the corresponding unsigned value.
        using _U = std::make_unsigned_t<_I>;
        _U __uvalue;
        auto __result = __from_chars(__first + 1, __last, __uvalue, __base);

        if (__result.ec != errc{})
        {
            // Failures due to no valid input require the returned ptr to equal first.
            if (__result.ec == std::errc::invalid_argument)
                __result.ptr = __first;

            return __result;
        }

        // Check that the resulting value is representable.
        if (_I(-__uvalue) > 0)
            return {__result.ptr, std::errc::result_out_of_range};

        __value = _I(-__uvalue);
        return {__result.ptr, {}};
    }

    const char* __original_first = __first;
    auto __read_next = [&__first, __last, __base](_I& __c) -> bool
    {
        if (__first >= __last)
            return false;

        constexpr __from_chars_table __fct = __make_from_chars_table();

        auto __v = __fct.__table[static_cast<unsigned char>(*__first)];
        if (__v < 0 || __v >= __base)
            return false;

        __c = static_cast<_I>(__v);
        ++__first;
        return true;        
    };

    _I __v = 0;
    _I __i;
    while (__read_next(__i))
    {
        // Check for overflow.
        constexpr _I __max = std::numeric_limits<_I>::max();
        if ((__max - __i) / static_cast<_I>(__base) < __v)
            return {__first, std::errc::result_out_of_range};

        __v = (__v * static_cast<_I>(__base)) + __i;
    }

    // Check that we decoded anything.
    if (__first == __original_first)
        return {__first, std::errc::invalid_argument};

    __value = __v;
    return {__first, {}};
}

} // namespace __detail


inline to_chars_result to_chars(char* __first, char* __last, char __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, signed char __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, unsigned char __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, short __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, unsigned short __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, int __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, unsigned int __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, long __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, unsigned long __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, long long __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, unsigned long long __value, int __base = 10)
{
    return __detail::__to_chars(__first, __last, __value, __base);
}

inline to_chars_result to_chars(char* __first, char* __last, bool __value, int __base = 10) = delete;

inline to_chars_result to_chars(char* __first, char* __last, float __value, chars_format __fmt);

inline to_chars_result to_chars(char* __first, char* __last, double __value, chars_format __fmt);

inline to_chars_result to_chars(char* __first, char* __last, long double __value, chars_format __fmt);

inline to_chars_result to_chars(char* __first, char* __last, float __value, chars_format __fmt, int __precision);

inline to_chars_result to_chars(char* __first, char* __last, double __value, chars_format __fmt, int __precision);

inline to_chars_result to_chars(char* __first, char* __last, long double __value, chars_format __fmt, int __precision);


inline from_chars_result from_chars(const char* __first, const char* __last, char& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, signed char& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, unsigned char& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, short& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, unsigned short& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, int& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, unsigned int& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, long& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, unsigned long& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, long long& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, unsigned long long& __value, int __base = 10)
{
    return __detail::__from_chars(__first, __last, __value, __base);
}

inline from_chars_result from_chars(const char* __first, const char* __last, float& __value, chars_format = chars_format::general);

inline from_chars_result from_chars(const char* __first, const char* __last, double& __value, chars_format = chars_format::general);

inline from_chars_result from_chars(const char* __first, const char* __last, long double& __value, chars_format = chars_format::general);


} // namespace __XVI_STD_FORMAT_NS


#endif /* ifndef __SYSTEM_CXX_FORMAT_CHARCONV_H */
