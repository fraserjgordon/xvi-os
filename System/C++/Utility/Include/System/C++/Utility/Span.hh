#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SPAN_H
#define __SYSTEM_CXX_UTILITY_SPAN_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class, size_t> class array;
template <class, size_t> class span;


namespace __detail
{

template <class> struct __is_span_specialization : false_type {};
template <class _Elem, size_t _Ext> struct __is_span_specialization<span<_Elem, _Ext>> : true_type {};

template <class> struct __is_array_specialization : false_type {};
template <class _Elem, size_t _Ext> struct __is_array_specialization<array<_Elem, _Ext>> : true_type {};

} // namespace __detail


inline constexpr size_t dynamic_extent = numeric_limits<size_t>::max();

template <class _ElementType, size_t _Extent = dynamic_extent>
class span
{
public:

    using element_type          = _ElementType;
    using value_type            = remove_cv_t<_ElementType>;
    using index_type            = size_t;
    using difference_type       = ptrdiff_t;
    using pointer               = element_type*;
    using const_pointer         = const element_type*;
    using reference             = element_type&;
    using const_reference       = const element_type&;
    using iterator              = pointer;
    using const_iterator        = const_pointer;
    using reverse_iterator      = __XVI_STD_NS::reverse_iterator<iterator>;
    using const_reverse_iterator= __XVI_STD_NS::reverse_iterator<const_iterator>;

    static constexpr index_type extent = _Extent;

    constexpr span() noexcept
        : _M_data(nullptr),
          _M_size(0)
    {
    }

    constexpr span(pointer __ptr, index_type __count)
        : _M_data(__ptr),
          _M_size(__count)
    {
    }

    constexpr span(pointer __first, pointer __last)
        : span(__first, __last - __first)
    {
    }

    template <size_t _N,
              class = enable_if_t<(extent == dynamic_extent || extent == _N)
                                  && is_convertible_v<remove_pointer_t<decltype(data(declval<element_type(&)[_N]>()))>(*)[], _ElementType(*)[]>, void>>
    constexpr span(element_type (&__arr)[_N]) noexcept
        : _M_data(data(__XVI_STD_NS::forward<element_type(&)[_N]>(__arr))),
          _M_size(_N)
    {
    }

    template <size_t _N,
              class = enable_if_t<(extent == dynamic_extent || extent == _N)
                                  && is_convertible_v<remove_pointer_t<decltype(data(declval<array<value_type, _N>&>()))>(*)[], _ElementType(*)[]>, void>>
    constexpr span(array<value_type, _N>& __arr) noexcept
        : _M_data(data(__arr)),
          _M_size(_N)
    {
    }

    template <size_t _N,
              class = enable_if_t<(extent == dynamic_extent || extent == _N)
                                  && is_convertible_v<_ElementType(*)[], remove_pointer_t<decltype(data(declval<const array<value_type, _N>&>()))>(*)[]>, void>>
    constexpr span(const array<value_type, _N>& __arr) noexcept
        : _M_data(data(__arr)),
          _M_size(_N)
    {
    }

    template <class _Container,
              class = void_t<decltype(data(declval<_Container&>())), decltype(size(declval<_Container&>()))>,
              class = enable_if_t<extent == dynamic_extent
                                  && !__detail::__is_span_specialization<_Container>::value
                                  && !__detail::__is_array_specialization<_Container>::value
                                  && !is_array_v<_Container>
                                  && is_convertible_v<remove_pointer_t<decltype(data(declval<_Container&>()))>, _ElementType(*)[]>, void>>
    constexpr span(_Container& __cont)
        : _M_data(data(__cont)),
          _M_size(size(__cont))
    {
    }

    template <class _Container,
              class = void_t<decltype(data(declval<const _Container&>())), decltype(size(declval<const _Container&>()))>,
              class = enable_if_t<extent == dynamic_extent
                                  && !__detail::__is_span_specialization<_Container>::value
                                  && !__detail::__is_array_specialization<_Container>::value
                                  && !is_array_v<_Container>
                                  && is_convertible_v<remove_pointer_t<decltype(data(declval<const _Container&>()))>, _ElementType(*)[]>, void>>
    constexpr span(const _Container& __cont)
        : _M_data(data(__cont)),
          _M_size(size(__cont))
    {
    }

    constexpr span(const span&) noexcept = default;

    template <class _OtherElementType, size_t _OtherExtent,
              class = enable_if_t<(extent == dynamic_extent || _OtherExtent == _Extent)
                                  && is_convertible_v<_OtherElementType(*)[], _ElementType(*)[]>, void>>
    constexpr span(const span<_OtherElementType, _OtherExtent>& __s) noexcept
        : _M_data(__s.data()),
          _M_size(__s.size())
    {
    }

    ~span() noexcept = default;

    constexpr span& operator=(const span&) noexcept = default;

    template <size_t _Count>
    constexpr span<element_type, _Count> first() const
    {
        return {data(), _Count};
    }

    template <size_t _Count>
    constexpr span<element_type, _Count> last() const
    {
        return {data() + (size() - _Count), _Count};
    }

    template <size_t _Offset, size_t _Count = dynamic_extent>
    constexpr auto subspan() const
    {
        constexpr size_t _E = _Count != dynamic_extent
                              ? _Count
                              : (_Extent != dynamic_extent
                                 ? _Extent - _Offset
                                 : dynamic_extent);

        return span<_ElementType, _E>(data() + _Offset,
                                     _Count != dynamic_extent ? _Count : size() - _Offset);
    }

    constexpr span<element_type, dynamic_extent> first(index_type __count) const
    {
        return {data(), __count};
    }

    constexpr span<element_type, dynamic_extent> last(index_type __count) const
    {
        return {data() + (size() - __count), __count};
    }

    constexpr span<element_type, dynamic_extent> subspan(index_type __offset, index_type __count = dynamic_extent) const
    {
        return {data() + __offset, __count == dynamic_extent ? size() - __offset : __count};
    }

    constexpr index_type size() const noexcept
    {
        return _M_size;
    }

    constexpr index_type size_bytes() const noexcept
    {
        return size() * sizeof(element_type);
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr reference operator[](index_type __idx) const
    {
        return *(data() + __idx);
    }

    constexpr reference front() const
    {
        return *(data());
    }

    constexpr reference back() const
    {
        return (*data() + (size() - 1));
    }

    constexpr pointer data() const noexcept
    {
        return _M_data;
    }

    constexpr iterator begin() const noexcept
    {
        return _M_data;
    }

    constexpr iterator end() const noexcept
    {
        return _M_data + _M_size;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    friend constexpr iterator begin(span __s) noexcept
    {
        return __s.begin();
    }

    friend constexpr iterator end(span __s) noexcept
    {
        return __s.end();
    }

private:

    pointer     _M_data;
    index_type  _M_size;
};

template <class _T, size_t _N>
    span(_T (&)[_N]) -> span<_T, _N>;

template <class _T, size_t _N>
    span(array<_T, _N>&) -> span<_T, _N>;

template <class _T, size_t _N>
    span(const array<_T, _N>&) -> span<const _T, _N>;

template <class _Container>
    span(_Container&) -> span<typename _Container::value_type>;

template <class _Container>
    span(const _Container&) -> span<const typename _Container::value_type>;


template <class _ElementType, size_t _Extent>
span<const byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_ElementType) * _Extent>
as_bytes(span<_ElementType, _Extent> __s) noexcept
{
    return {reinterpret_cast<const byte*>(__s.data()), __s.size_bytes()};
}

template <class _ElementType, size_t _Extent>
span<byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_ElementType) * _Extent>
as_writable_bytes(span<_ElementType, _Extent> __s) noexcept
{
    return {reinterpret_cast<byte*>(__s.data()), __s.size_bytes()};
}


template <class> class tuple_size;
template <size_t, class> class tuple_element;

template <class _ElementType, size_t _Extent>
struct tuple_size<span<_ElementType, _Extent>>
    : integral_constant<size_t, _Extent> {};

// Not defined.
template <class _ElementType>
struct tuple_size<span<_ElementType, dynamic_extent>>;

template <size_t _I, class _ElementType, size_t _Extent>
struct tuple_element<_I, span<_ElementType, _Extent>>
{
    static_assert(_Extent != dynamic_extent);
    static_assert(_I < _Extent);

    using type = _ElementType;
};

template <size_t _I, class _ElementType, size_t _Extent>
constexpr _ElementType& get(span<_ElementType, _Extent> __s) noexcept
{
    static_assert(_Extent != dynamic_extent);
    static_assert(_I < _Extent);

    return __s[_I];
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SPAN_H */
