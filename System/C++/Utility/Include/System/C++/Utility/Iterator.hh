#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ITERATOR_H
#define __SYSTEM_CXX_UTILITY_ITERATOR_H


#include <System/C++/Core/IteratorAdaptors.hh>
#include <System/C++/Core/IteratorTraits.hh>
#include <System/C++/Core/IteratorUtils.hh>
#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/Private/Config.hh>



namespace __XVI_STD_UTILITY_NS
{


template <class _T, class _CharT = char, class _Traits = char_traits<_CharT>, class _Distance = ptrdiff_t>
class istream_iterator
{
public:

    using iterator_category     = input_iterator_tag;
    using value_type            = _T;
    using difference_type       = _Distance;
    using pointer               = const _T*;
    using reference             = const _T&;
    using char_type             = _CharT;
    using traits_type           = _Traits;
    using istream_type          = basic_istream<_CharT, _Traits>;

    constexpr istream_iterator() = default;

    constexpr istream_iterator(default_sentinel_t) :
        istream_iterator()
    {
    }

    istream_iterator(istream_type& __s) :
        _M_in_stream(addressof(__s)),
        _M_value()
    {
    }

    istream_iterator(const istream_iterator&) = default;
    istream_iterator(istream_iterator&&) = default;

    istream_iterator& operator=(const istream_iterator&) = default;
    istream_iterator& operator=(istream_iterator&&) = default;

    ~istream_iterator() = default;

    const _T& operator*() const
    {
        return _M_value;
    }

    const _T* operator->() const
    {
        return addressof(_M_value);
    }

    istream_iterator& operator++()
    {
        if (!(*_M_in_stream >> _M_value))
            _M_in_stream = nullptr;

        return *this;
    }

    istream_iterator operator++(int)
    {
        istream_iterator __tmp = *this;
        ++*this;
        return __tmp;
    }

    friend bool operator==(const istream_iterator& __i, default_sentinel_t)
    {
        return !__i._M_in_stream;
    }

    friend bool operator==(const istream_iterator& __x, const istream_iterator& __y)
    {
        return __x._M_in_stream == __y._M_stream;
    }

private:

    basic_istream<_CharT, _Traits>* _M_in_stream = nullptr;
    _T                              _M_value = {};
};

template <class _T, class _CharT = char, class _Traits = char_traits<_CharT>>
class ostream_iterator
{
public:

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = ptrdiff_t;
    using pointer               = void;
    using reference             = void;
    using char_type             = _CharT;
    using traits_type           = _Traits;
    using ostream_type          = basic_ostream<_CharT, _Traits>;

    constexpr ostream_iterator() noexcept = default;
    ostream_iterator(const ostream_iterator&) = default;
    ostream_iterator(ostream_iterator&&) = default;

    ostream_iterator(ostream_type& __s) :
        _M_out_stream(addressof(__s)),
        _M_delim(nullptr)
    {
    }

    ostream_iterator(ostream_type& __s, const _CharT* __delimiter) :
        _M_out_stream(addressof(__s)),
        _M_delim(__delimiter)
    {
    }

    ostream_iterator& operator=(const ostream_iterator&) = default;

    ostream_iterator& operator=(const _T& __value)
    {
        *_M_out_stream << __value;

        if (_M_delim)
            *_M_out_stream << _M_delim;

        return *this;
    }

    ~ostream_iterator() = default;

    ostream_iterator& operator*()
    {
        return *this;
    }

    ostream_iterator& operator++()
    {
        return *this;
    }

    ostream_iterator& operator++(int)
    {
        return *this;
    }

private:

    basic_ostream<_CharT, _Traits>* _M_out_stream = nullptr;
    const _CharT* _M_delim = nullptr;
};

template <class _CharT, class _Traits>
class istreambuf_iterator
{
public:

    class __proxy
    {
    public:

        __proxy(_CharT __c, basic_streambuf<_CharT, _Traits>& __sbuf) :
            _M_keep(__c),
            _M_sbuf(__sbuf)
        {
        }

        _CharT operator*()
        {
            return _M_keep;
        }

    private:

        _CharT                              _M_keep;
        basic_streambuf<_CharT, _Traits>*   _M_sbuf;
    };

    using iterator_category     = input_iterator_tag;
    using value_type            = _CharT;
    using difference_type       = typename _Traits::off_type;
    using pointer               = _CharT*;
    using reference             = _CharT;
    using char_type             = _CharT;
    using traits_type           = _Traits;
    using int_type              = typename _Traits::int_type;
    using streambuf_type        = basic_streambuf<_CharT, _Traits>;
    using istream_type          = basic_istream<_CharT, _Traits>;

    constexpr istreambuf_iterator() noexcept = default;

    constexpr istreambuf_iterator(default_sentinel_t) noexcept :
        istreambuf_iterator()
    {
    }

    istreambuf_iterator(const istreambuf_iterator&) noexcept = default;
    istreambuf_iterator(istreambuf_iterator&&) noexcept = default;

    istreambuf_iterator(istream_type& __s) noexcept :
        _M_sbuf(__s.rdbuf())
    {
    }

    istreambuf_iterator(streambuf_type* __s) noexcept :
        _M_sbuf(__s)
    {
    }

    istreambuf_iterator(const __proxy& __p) noexcept :
        _M_sbuf(__p._M_sbuf)
    {
    }

    istreambuf_iterator& operator=(const istreambuf_iterator&) noexcept = default;
    istreambuf_iterator& operator=(istreambuf_iterator&&) noexcept = default;

    ~istreambuf_iterator() = default;

    _CharT operator*() const
    {
        return _M_sbuf->sgetc();
    }

    istreambuf_iterator& operator++()
    {
        _M_sbuf->sbumpc();
        return *this;
    }

    __proxy operator++(int)
    {
        return __proxy(_M_sbuf->sbumpc(), _M_sbuf);
    }

    bool equal(const istreambuf_iterator& __b) const
    {
        if (_M_sbuf == nullptr)
            return __b._M_sbuf == nullptr;        

        return _M_sbuf->eof() == __b._M_sbuf->eof();
    }

    friend bool operator==(const istreambuf_iterator& __i, default_sentinel_t __s)
    {
        return __i.equal(__s);
    }

    friend bool operator==(const istreambuf_iterator& __a, const istreambuf_iterator& __b)
    {
        return __a.equal(__b);
    }

private:

    streambuf_type* _M_sbuf = nullptr;
};

template <class _CharT, class _Traits>
class ostreambuf_iterator
{
public:

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = ptrdiff_t;
    using pointer               = void;
    using reference             = void;
    using char_type             = _CharT;
    using traits_type           = _Traits;
    using streambuf_type        = basic_streambuf<_CharT, _Traits>;
    using ostream_type          = basic_ostream<_CharT, _Traits>;

    constexpr ostreambuf_iterator() noexcept = default;
    constexpr ostreambuf_iterator(const ostreambuf_iterator&) noexcept = default;
    constexpr ostreambuf_iterator(ostreambuf_iterator&&) noexcept = default;

    ostreambuf_iterator(ostream_type& __s) :
        _M_sbuf(__s.rdbuf())
    {
    }

    ostreambuf_iterator(streambuf_type* __sbuf) :
        _M_sbuf(__sbuf)
    {
    }

    ostreambuf_iterator& operator=(const ostreambuf_iterator&) = default;
    ostreambuf_iterator& operator=(ostreambuf_iterator&&) = default;

    ostreambuf_iterator& operator=(_CharT __c)
    {
        if (!failed())
            _M_eof = (_M_sbuf->sputc(__c) == _Traits::eof());
    }

    ostreambuf_iterator& operator*()
    {
        return *this;
    }

    ostreambuf_iterator& operator++()
    {
        return *this;
    }

    ostreambuf_iterator& operator++(int)
    {
        return *this;
    }

    bool failed() const noexcept
    {
        return _M_eof;
    }

private:

    streambuf_type* _M_sbuf = nullptr;
    bool            _M_eof = false;
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ITERATOR_H */
