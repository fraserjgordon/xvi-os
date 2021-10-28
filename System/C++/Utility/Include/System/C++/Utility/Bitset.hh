#pragma once
#ifndef __SYSTEM_CXX_UTILITY_BITSET_H
#define __SYSTEM_CXX_UTILITY_BITSET_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/StdExcept.hh>
#include <System/C++/Utility/StringFwd.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <size_t _N>
class bitset
{
public:

    class reference
    {
    public:

        reference(const reference&) = default;
        ~reference() = default;

        reference& operator=(bool __val) noexcept
        {
            _M_bitset.set(_M_offset, __val);
            return *this;
        }

        reference& operator=(const reference& __x) noexcept
        {
            return operator=(bool(__x));
        }

        bool operator~() const noexcept
        {
            return !_M_bitset.test(_M_offset);
        }

        operator bool() const noexcept
        {
            return _M_bitset.test(_M_offset);
        }

        reference& flip() noexcept
        {
            _M_bitset.flip(_M_offset);
            return *this;
        }

    private:

        friend class bitset;

        reference(bitset& __b, size_t __n) noexcept
            : _M_bitset(__b),
              _M_offset(__n)
        {
        }

        bitset& _M_bitset;
        size_t _M_offset;
    };

    constexpr bitset() noexcept = default;

    constexpr bitset(unsigned long long __u) noexcept
        : bitset()
    {
        _M_words[0] = static_cast<__word_t>(__u);
    }

    //! @TODO: implement.
    template <class _CharT, class _Traits, class _Allocator>
    explicit bitset
        (
            const basic_string<_CharT, _Traits, _Allocator>&,
            typename basic_string<_CharT, _Traits, _Allocator>::size_type = 0,
            typename basic_string<_CharT, _Traits, _Allocator>::size_type n = basic_string<_CharT, _Traits, _Allocator>::npos,
            _CharT = _CharT('0'),
            _CharT = _CharT('1')
        );

    //! @TODO: implement.
    template <class _CharT>
    explicit bitset
        (
            const _CharT*,
            typename basic_string<_CharT>::size_type = basic_string<_CharT>::npos,
            _CharT = _CharT('0'),
            _CharT = _CharT('1')
        );

    bitset& operator&=(const bitset& __rhs) noexcept
    {
        for (size_t __i = 0; __i < _W; ++__i)
            _M_words[__i] &= __rhs._M_words[__i];
        return *this;
    }

    bitset& operator|=(const bitset& __rhs) noexcept
    {
        for (size_t __i = 0; __i < _W; ++__i)
            _M_words[__i] |= __rhs._M_words[__i];
        return *this;
    }

    bitset& operator^=(const bitset& __rhs) noexcept
    {
        for (size_t __i = 0; __i < _W; ++__i)
            _M_words[__i] ^= __rhs._M_words[__i];
        return *this;
    }

    //! @TODO: implement.
    bitset& operator<<=(size_t __n) noexcept;

    //! @TODO: implement.
    bitset& operator>>=(size_t) noexcept;

    bitset& set() noexcept
    {
        for (size_t __i = 0; __i < (_W - 1); ++__i)
            _M_words[__i] = ~__word_t(0);
        _M_words[_W - 1] = __top_mask();

        return *this;
    }

    bitset& set(size_t __n, bool __val = true)
    {
        if (__n >= _N)
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid bitset index"));

        if (__val)
            _M_words[__n / _WB] |= __word_t(1) << (__n % _WB);
        else
            _M_words[__n / _WB] &= ~(__word_t(1) << (__n % _WB));

        return *this;
    }

    bitset& reset() noexcept
    {
        for (size_t __i = 0; __i < _W; ++__i)
            _M_words[__i] = 0;

        return *this;
    }

    bitset& reset(size_t __n)
    {
        return set(__n, false);
    }

    bitset operator~() const noexcept
    {
        bitset __x = *this;
        __x.flip();
        return __x;
    }

    bitset& flip() noexcept
    {
        for (size_t __i = 0; __i < (_W - 1); ++__i)
            _M_words[__i] ^= ~__word_t(0);
        _M_words[_W - 1] ^= __top_mask();

        return *this;
    }

    bitset& flip(size_t __n)
    {
        if (__n >= _N)
            __XVI_CXX_UTILITY_THROW(out_of_range("out-of-bounds bitset access"));

        _M_words[__n / _WB] ^= (__word_t(1) << (__n % _WB));

        return *this;
    }

    constexpr bool operator[](size_t __n) const
    {
        return (_M_words[__n / _WB] & (__word_t(1) << (__n % _WB)) != 0);
    }

    reference operator[](size_t __n)
    {
        return reference(*this, __n);
    }

    unsigned long to_ulong() const
    {
        if (_N > numeric_limits<unsigned long>::digits)
        {
            bitset __valid = bitset(static_cast<unsigned long long>(~0UL));
            bitset __test = *this;
            __test &= ~__valid;

            if (__test.any())
                __XVI_CXX_UTILITY_THROW(overflow_error());
        }

        return static_cast<unsigned long>(_M_words[0]);
    }

    unsigned long long to_ullong() const
    {
        if (_N > numeric_limits<unsigned long long>::digits)
        {
            bitset __valid = bitset(~0ULL);
            bitset __test = *this;
            __test &= ~__valid;

            if (__test.any())
                __XVI_CXX_UTILITY_THROW(overflow_error());
        }

        return static_cast<unsigned long long>(_M_words[0]);
    }

    //! @TODO: implement.
    template <class _CharT = char, class _Traits = char_traits<_CharT>, class _Allocator = allocator<_CharT>>
    basic_string<_CharT, _Traits, _Allocator> to_string(_CharT = _CharT('0'), _CharT = _CharT('1')) const;

    size_t count() const noexcept
    {
        size_t __count = 0;
        for (size_t __i = 0; __i < _W; ++__i)
            __count += __builtin_popcount(_M_words[__i]);

        return __count;
    }

    constexpr size_t size() const noexcept
    {
        return _N;
    }

    bool operator==(const bitset& __x) const noexcept
    {
        for (size_t __i = 0; __i < _W; ++__i)
        {
            if (_M_words[__i] != __x._M_words[__i])
                return false;
        }

        return true;
    }

    bool operator!=(const bitset& __x) const noexcept
    {
        return !operator==(__x);
    }

    bool test(size_t __n) const
    {
        if (__n >= _N)
            __XVI_CXX_UTILITY_THROW(out_of_range("invalid bitset index"));

        return (_M_words[__n / _WB] & ((__word_t(1) << (__n % _WB)) != 0));
    }

    bool all() const noexcept
    {
        for (size_t __i = 0; __i < (_W - 1); ++__i)
        {
            if (_M_words[__i] != ~__word_t(0))
                return false;
        }

        return _M_words[_W - 1] == __top_mask();
    }


    bool any() const noexcept
    {
        for (size_t __i = 0; __i < (_W - 1); ++__i)
        {
            if (_M_words[__i] != 0)
                return true;
        }

        return (_M_words[_W - 1] & __top_mask()) != 0;
    }

    bool none() const noexcept
    {
        return !any();
    }

    bitset operator<<(size_t __n) const noexcept
    {
        bitset __x = *this;
        __x <<= __n;
        return __x;
    }

    bitset operator>>(size_t __n) const noexcept
    {
        bitset __x = *this;
        __x >>= __n;
        return __x;
    }

private:

    // Underlying representation type.
    using __word_t = conditional_t<(_N > 32),
                                   uint64_t,
                                   conditional_t<(_N > 16),
                                                 uint32_t,
                                                 conditional_t<(_N > 8),
                                                              uint16_t,
                                                              uint8_t>>>;

    // Size of each word.
    static constexpr size_t _WB = numeric_limits<__word_t>::digits;

    // Number of words required for the bitset.
    static constexpr size_t _W = (_N + (_WB - 1)) / _WB;

    __word_t _M_words[_W] = {};


    static constexpr size_t __top_bits()
    {
        auto __remainder = _N % _WB;
        return (__remainder == 0) ? _WB : __remainder;
    }

    static constexpr __word_t __top_mask()
    {
        auto __bits = __top_bits();
        return (__bits == _WB) ? ~__word_t(0) : ((__word_t(1) << __bits) - 1);
    }
};


template <size_t _N>
bitset<_N> operator&(const bitset<_N>& __x, const bitset<_N>& __y) noexcept
{
    bitset<_N> __b = __x;
    __b &= __y;
    return __b;
}

template <size_t _N>
bitset<_N> operator|(const bitset<_N>& __x, const bitset<_N>& __y) noexcept
{
    bitset<_N> __b = __x;
    __b |= __y;
    return __b;
}

template <size_t _N>
bitset<_N> operator^(const bitset<_N>& __x, const bitset<_N>& __y) noexcept
{
    bitset<_N> __b = __x;
    __b ^= __y;
    return __b;
}

//! @TODO: implement.
template <class _CharT, class _Traits, size_t _N>
__XVI_STD_IOS_NS::basic_istream<_CharT, _Traits>& operator>>(__XVI_STD_IOS_NS::basic_istream<_CharT, _Traits>&, bitset<_N>&);

//! @TODO: implement.
template <class _CharT, class _Traits, size_t _N>
__XVI_STD_IOS_NS::basic_ostream<_CharT, _Traits>& operator<<(__XVI_STD_IOS_NS::basic_ostream<_CharT, _Traits>&, const bitset<_N>&);


//! @TODO: hash support.
template <class> struct hash;
template <size_t _N> struct hash<bitset<_N>>;


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_BITSET_H */
