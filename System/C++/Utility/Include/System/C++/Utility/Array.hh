#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ARRAY_H
#define __SYSTEM_CXX_UTILITY_ARRAY_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/StdExcept.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _T, size_t _N>
struct array
{
    using value_type                = _T;
    using pointer                   = _T*;
    using const_pointer             = const _T*;
    using reference                 = _T&;
    using const_reference           = const _T&;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;
    using iterator                  = pointer;
    using const_iterator            = const_pointer;
    using reverse_iterator          = __XVI_STD_NS::reverse_iterator<iterator>;
    using const_reverse_iterator    = __XVI_STD_NS::reverse_iterator<const_iterator>;

    constexpr void fill(const _T& __u)
    {
        fill_n(begin(), _N, __u);
    }

    constexpr void swap(array& __y)
        noexcept(_N == 0 || is_nothrow_swappable_v<_T>)
    {
        if constexpr (_N > 0)
            swap_ranges(begin(), end(), __y.begin());
    }

    constexpr iterator begin() noexcept
    {
        return _M_arr;
    }

    constexpr iterator end() noexcept
    {
        return _M_arr + _N;
    }

    constexpr const_iterator begin() const noexcept
    {
        return _M_arr;
    }

    constexpr const_iterator end() const noexcept
    {
        return _M_arr + _N;
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    constexpr size_type size() const noexcept
    {
        return _N;
    }

    constexpr size_type max_size() const noexcept
    {
        return _N;
    }

    constexpr reference operator[](size_type __n)
    {
        return _M_arr[__n];
    }

    constexpr const_reference operator[](size_type __n) const
    {
        return _M_arr[__n];
    }

    constexpr reference at(size_type __n)
    {
        if (__n >= _N)
            __XVI_CXX_UTILITY_THROW(out_of_range());

        return _M_arr[__n];
    }

    constexpr const_reference at(size_type __n) const
    {
        if (__n >= _N)
            __XVI_CXX_UTILITY_THROW(out_of_range());

        return _M_arr[__n];
    }

    constexpr reference front()
    {
        return _M_arr[0];
    }

    constexpr const_reference front() const
    {
        return _M_arr[0];
    }

    constexpr reference back()
    {
        return _M_arr[_N - 1];
    }

    constexpr const_reference back() const
    {
        return _M_arr[_N - 1];
    }

    constexpr _T* data() noexcept
    {
        return _M_arr;
    }

    constexpr const _T* data() const noexcept
    {
        return _M_arr;
    }


    // Note: need to support the degenerate _N == 0 case.
    _T _M_arr[_N];
};

//! @TODO: mandates (is_same_v<_T, _U> && ...).
template <class _T, class... _U>
    array(_T, _U...) -> array<_T, 1 + sizeof...(_U)>;


template <class _T, size_t _N>
constexpr void swap(array<_T, _N>& __x, array<_T, _N>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}


template <class> struct tuple_size;
template <size_t, class> struct tuple_element;

template <class _T, size_t _N>
struct tuple_size<array<_T, _N>>
    : integral_constant<size_t, _N> {};

template <size_t _I, class _T, size_t _N>
struct tuple_element<_I, array<_T, _N>>
{
    static_assert(_I < _N);

    using type = _T;
};

template <size_t _I, class _T, size_t _N>
constexpr _T& get(array<_T, _N>& __a) noexcept
{
    static_assert(_I < _N);
    return __a[_I];
}

template <size_t _I, class _T, size_t _N>
constexpr const _T& get(const array<_T, _N>& __a) noexcept
{
    static_assert(_I < _N);
    return __a[_I];
}

template <size_t _I, class _T, size_t _N>
constexpr _T&& get(array<_T, _N>& __a) noexcept
{
    static_assert(_I < _N);
    return __XVI_STD_NS::move(__a[_I]);
}

template <size_t _I, class _T, size_t _N>
constexpr const _T&& get(const array<_T, _N>& __a) noexcept
{
    static_assert(_I < _N);
    return __XVI_STD_NS::move(__a[_I]);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ARRAY_H */
