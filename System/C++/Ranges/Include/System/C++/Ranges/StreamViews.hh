#ifndef __SYSTEM_CXX_RANGES_STREAMVIEWS_H
#define __SYSTEM_CXX_RANGES_STREAMVIEWS_H


#include <System/C++/Ranges/Private/Config.hh>


namespace __XVI_STD_RANGES_NS_DECL
{


namespace ranges
{


namespace __detail
{

template <class _Val, class _CharT, class _Traits>
concept __stream_extractable = requires(basic_istream<_CharT, _Traits>& __is, _Val& __t)
{
    __is >> __t;
};

} // namespace __detail

template <movable _Val, class _CharT, class _Traits = char_traits<_CharT>>
    requires default_initializable<_Val>
        && __detail::__stream_extractable<_Val, _CharT, _Traits>
class basic_istream_view :
    public view_interface<basic_istream_view<_Val, _CharT, _Traits>>
{
public:

    basic_istream_view() = default;

    constexpr explicit basic_istream_view(basic_istream<_CharT, _Traits>& __stream) :
        _M_stream(addressof(__stream))
    {
    }

    constexpr auto begin()
    {
        if (_M_stream)
            *_M_stream >> _M_object;

        return iterator{*this};
    }

    constexpr default_sentinel_t end() const noexcept
    {
        return default_sentinel;
    }

private:

    basic_istream<_CharT, _Traits>* _M_stream = nullptr;
    _Val                            _M_object = _Val();

    class iterator
    {
    public:

        using iterator_category     = input_iterator_tag;
        using difference_type       = ptrdiff_t;
        using value_type            = _Val;

        iterator() = default;

        constexpr explicit iterator(basic_istream_view& __parent) noexcept :
            _M_parent(addressof(__parent))
        {
        }

        iterator(const iterator&) = delete;
        iterator(iterator&&) = default;

        iterator& operator=(const iterator&) = delete;
        iterator& operator=(iterator&&) = default;

        iterator& operator++()
        {
            *_M_parent->_M_stream >> _M_parent->_M_object;
            return *this;
        }

        void operator++(int)
        {
            ++*this;
        }

        _Val& operator*() const
        {
            return _M_parent->_M_object;
        }

        friend bool operator==(const iterator& __x, default_sentinel_t)
        {
            auto parent = __x._M_parent;
            return parent == nullptr || !(*parent->_M_stream);
        }

    private:

        basic_istream_view* _M_parent = nullptr;
    };
};

template <class _Val>
using istream_view = basic_istream_view<_Val, char>;

template <class _Val>
using wistream_view = basic_istream_view<_Val, wchar_t>;


} // namespace ranges


} // namespace __XVI_STD_RANGES_NS_DECL


#endif /* ifndef __SYSTEM_CXX_RANGES_STREAMVIEWS_H */
