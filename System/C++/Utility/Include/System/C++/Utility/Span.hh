#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SPAN_H
#define __SYSTEM_CXX_UTILITY_SPAN_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Ranges.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class, size_t> struct array;
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
    using size_type             = size_t;
    using difference_type       = ptrdiff_t;
    using pointer               = element_type*;
    using const_pointer         = const element_type*;
    using reference             = element_type&;
    using const_reference       = const element_type&;
    using iterator              = pointer;
    using reverse_iterator      = __XVI_STD_NS::reverse_iterator<iterator>;
  
    static constexpr size_type extent = _Extent;

    constexpr span() noexcept
        requires (_Extent == dynamic_extent || _Extent == 0)
        : _M_data(nullptr),
          _M_size(0)
    {
    }

    template <class _It>
        requires contiguous_iterator<_It> && is_convertible_v<remove_reference_t<iter_reference_t<_It>>(*)[], element_type(*)[]>
    constexpr explicit(extent != dynamic_extent) span(_It __first, size_type __count)
        : _M_data(to_address(__first)),
          _M_size(__count)
    {
    }

    template <class _It, class _End>
        requires is_convertible_v<remove_reference_t<iter_reference_t<_It>>(*)[], element_type(*)[]>
                 && contiguous_iterator<_It>
                 && sized_sentinel_for<_End, _It>
                 && (!is_convertible_v<_End, size_t>)
    constexpr explicit(extent != dynamic_extent) span(_It __first, _End __last)
        : _M_data(to_address(__first)),
          _M_size(static_cast<std::size_t>(__last - __first))
    {
    }

    template <size_t _N>
    constexpr span(type_identity_t<element_type> (&__arr)[_N]) noexcept
        requires (extent == dynamic_extent || extent == _N)
                  && is_convertible_v<remove_pointer_t<decltype(data(__arr))>(*)[], element_type(*)[]>
        : _M_data(std::addressof(__arr)),
          _M_size(_N)
    {
    }

    template <class _T, size_t _N>
    constexpr span(std::array<_T, _N>& __arr) noexcept
        requires (extent == dynamic_extent || extent == _N)
                  && is_convertible_v<remove_pointer_t<decltype(data(__arr))>(*)[], element_type(*)[]>
        : _M_data(std::data(__arr)),
          _M_size(_N)
    {
    }

    template <class _T, size_t _N>
    constexpr span(const std::array<_T, _N>& __arr) noexcept
        requires (extent == dynamic_extent || extent == _N)
                 && is_convertible_v<remove_pointer_t<decltype(data(__arr))>(*)[], element_type(*)[]>
        : _M_data(std::data(__arr)),
          _M_size(_N)
    {
    }

    template <class _R>
        requires ranges::contiguous_range<remove_cvref_t<_R>>
                 && ranges::sized_range<remove_cvref_t<_R>>
                 && (ranges::borrowed_range<remove_cvref_t<_R>> || is_const_v<element_type>)
                 && (!__detail::__is_span_specialization<remove_cvref_t<_R>>::value)
                 && (!__detail::__is_array_specialization<remove_cvref_t<_R>>::value)
                 && (!is_array_v<remove_cvref_t<remove_cvref_t<_R>>>)
                 && is_convertible_v<remove_reference_t<ranges::range_reference_t<remove_cvref_t<_R>>>(*)[], element_type(*)[]>
    constexpr explicit(extent != dynamic_extent) span(_R&& __r)
        : _M_data(ranges::data(__r)),
          _M_size(ranges::size(__r))
    {
    }

    constexpr span(const span&) noexcept = default;

    template <class _OtherElementType, size_t _OtherExtent>
        requires (extent == dynamic_extent || _OtherExtent == dynamic_extent || extent == _OtherExtent)
                 && is_convertible_v<_OtherElementType(*)[], element_type(*)[]>
    constexpr explicit(extent != dynamic_extent && _OtherExtent == dynamic_extent)
    span(const span<_OtherElementType, _OtherExtent>& __s) noexcept
        : _M_data(__s.data()),
          _M_size(__s.size())
    {
    }

    ~span() noexcept = default;

    constexpr span& operator=(const span&) noexcept = default;

    template <size_t _Count>
    constexpr span<element_type, _Count> first() const
    {
        return span<element_type, _Count>{data(), _Count};
    }

    template <size_t _Count>
    constexpr span<element_type, _Count> last() const
    {
        return span<element_type, _Count>{data() + (size() - _Count), _Count};
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

    constexpr span<element_type, dynamic_extent> first(size_type __count) const
    {
        return {data(), __count};
    }

    constexpr span<element_type, dynamic_extent> last(size_type __count) const
    {
        return {data() + (size() - __count), __count};
    }

    constexpr span<element_type, dynamic_extent> subspan(size_type __offset, size_type __count = dynamic_extent) const
    {
        return {data() + __offset, __count == dynamic_extent ? size() - __offset : __count};
    }

    constexpr size_type size() const noexcept
    {
        return _M_size;
    }

    constexpr size_type size_bytes() const noexcept
    {
        return size() * sizeof(element_type);
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr reference operator[](size_type __idx) const
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

    constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }

private:

    pointer     _M_data;
    size_type   _M_size;
};

template <class _It, class _EndOrSize>
    requires contiguous_iterator<_It>
span(_It, _EndOrSize) -> span<remove_reference_t<iter_reference_t<_It>>>;

template <class _T, size_t _N>
span(_T (&)[_N]) -> span<_T, _N>;

template <class _T, size_t _N>
span(array<_T, _N>&) -> span<_T, _N>;

template <class _T, size_t _N>
span(const array<_T, _N>&) -> span<const _T, _N>;

template <class _R>
    requires ranges::contiguous_range<_R>
span(_R&&) -> span<remove_reference_t<ranges::range_reference_t<_R>>>;


template <class _ElementType, size_t _Extent>
span<const byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_ElementType) * _Extent>
as_bytes(span<_ElementType, _Extent> __s) noexcept
{
    using _R = span<const byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_ElementType) * _Extent>;
    return _R{reinterpret_cast<const byte*>(__s.data()), __s.size_bytes()};
}

template <class _ElementType, size_t _Extent>
span<byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_ElementType) * _Extent>
as_writable_bytes(span<_ElementType, _Extent> __s) noexcept
{
    using _R = span<byte, _Extent == dynamic_extent ? dynamic_extent : sizeof(_ElementType) * _Extent>;
    return _R{reinterpret_cast<byte*>(__s.data()), __s.size_bytes()};
}


template <class _ElementType, size_t _Extent>
inline constexpr bool ranges::enable_view<span<_ElementType, _Extent>> = _Extent == 0 || _Extent == dynamic_extent;

template <class _ElementType, size_t _Extent>
inline constexpr bool ranges::enable_borrowed_range<span<_ElementType, _Extent>> = true;


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SPAN_H */
