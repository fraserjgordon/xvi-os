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


namespace __detail
{


struct __empty_array_element {};


} // namespace __detail


template <class _T, size_t _N>
struct array
{
    using __element_type            = std::conditional_t<_N == 0, __detail::__empty_array_element, _T>;

    using value_type                = _T;
    using pointer                   = _T*;
    using const_pointer             = const _T*;
    using reference                 = _T&;
    using const_reference           = const _T&;
    using size_type                 = std::size_t;
    using difference_type           = std::ptrdiff_t;
    using iterator                  = __element_type*;
    using const_iterator            = const __element_type*;
    using reverse_iterator          = __XVI_STD_UTILITY_NS::reverse_iterator<iterator>;
    using const_reverse_iterator    = __XVI_STD_UTILITY_NS::reverse_iterator<const_iterator>;


    // Note: need to support the degenerate _N == 0 case.
    __element_type _M_arr[_N == 0 ? 1 : _N];


    constexpr void fill(const _T& __u)
    {
        fill_n(begin(), _N, __u);
    }

    constexpr void swap(array& __y)
        noexcept(std::is_nothrow_swappable_v<__element_type>)
    {
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
            __XVI_CXX_UTILITY_THROW(out_of_range("out-of-bounds array access"));

        return _M_arr[__n];
    }

    constexpr const_reference at(size_type __n) const
    {
        if (__n >= _N)
            __XVI_CXX_UTILITY_THROW(out_of_range("out-of-bounds array access"));

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
        if constexpr (_N == 0)
            return nullptr;
        else
            return _M_arr;
    }

    constexpr const _T* data() const noexcept
    {
        if constexpr (_N == 0)
            return nullptr;
        else
            return _M_arr;
    }
};

template <class _T, class... _U>
    requires (is_same_v<_T, _U> && ...)
array(_T, _U...) -> array<_T, 1 + sizeof...(_U)>;


template <class _T, size_t _N>
constexpr bool operator==(const array<_T, _N>& __x, const array<_T, _N>& __y)
{
    return std::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _T, size_t _N>
constexpr __detail::__synth_three_way_result<_T> operator<=>(const array<_T, _N>& __x, const array<_T, _N>& __y)
{
    return std::lexicographical_compare_three_way(__x.begin(), __x.end(), __y.begin(), __y.end(), __detail::__synth_three_way);
}


template <class _T, size_t _N>
constexpr void swap(array<_T, _N>& __x, array<_T, _N>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}


namespace __detail
{

template <class _T, size_t _N, size_t... _Idx>
constexpr array<remove_cv_t<_T>, _N> __make_array(std::index_sequence<_Idx...>, _T (&__a)[_N])
{
    return {{__a[_Idx]...}};
}

template <class _T, size_t _N, size_t... _Idx>
constexpr array<remove_cv_t<_T>, _N> __make_array(std::index_sequence<_Idx...>, _T (&&__a)[_N])
{
    return {{std::move(__a[_Idx])...}};
}

} // namespace __detail


template <class _T, size_t _N>
    requires (!std::is_array_v<_T> && std::is_constructible_v<_T, _T&>)
constexpr array<std::remove_cv_t<_T>, _N> to_array(_T (&__a)[_N])
{
    return __detail::__make_array(std::make_index_sequence<_N>(), __a);
}

template <class _T, size_t _N>
constexpr array<std::remove_cv_t<_T>, _N> to_array(_T (&&__a)[_N])
{
    return __detail::__make_array(std::make_index_sequence<_N>(), std::move(__a));
}


template <class> struct tuple_size;
template <size_t, class> struct tuple_element;

template <class _T, size_t _N>
struct tuple_size<array<_T, _N>>
    : std::integral_constant<size_t, _N> {};

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
constexpr _T&& get(array<_T, _N>&& __a) noexcept
{
    static_assert(_I < _N);

    return std::move(__a[_I]);
}

template <size_t _I, class _T, size_t _N>
constexpr const _T&& get(const array<_T, _N>&& __a) noexcept
{
    static_assert(_I < _N);

    return std::move(__a[_I]);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ARRAY_H */
