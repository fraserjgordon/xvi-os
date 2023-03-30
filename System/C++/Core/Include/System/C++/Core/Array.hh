#ifndef __SYSTEM_CXX_CORE_ARRAY_H
#define __SYSTEM_CXX_CORE_ARRAY_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/IteratorAdaptors.hh>
#include <System/C++/Core/IteratorUtils.hh>


namespace __XVI_STD_CORE_NS_DECL
{


namespace __detail
{


struct __empty_array_element {};


// Defined elsewhere (needs quite a lot of <stdexcept> to work).
[[noreturn]] void __array_out_of_bounds_access();


} // namespace __detail


template <class _T, size_t _N>
struct array
{
    using __element_type            = conditional_t<_N == 0, __detail::__empty_array_element, _T>;

    using value_type                = _T;
    using pointer                   = _T*;
    using const_pointer             = const _T*;
    using reference                 = _T&;
    using const_reference           = const _T&;
    using size_type                 = size_t;
    using difference_type           = ptrdiff_t;
    using iterator                  = __element_type*;
    using const_iterator            = const __element_type*;
    using reverse_iterator          = __XVI_STD_NS::reverse_iterator<iterator>;
    using const_reverse_iterator    = __XVI_STD_NS::reverse_iterator<const_iterator>;


    // Note: need to support the degenerate _N == 0 case.
    __element_type _M_arr[_N == 0 ? 1 : _N];


    constexpr void fill(const _T& __u)
    {
        for (size_t __i = 0; __i < _N; ++__i)
            _M_arr[__i] = __u;
    }

    constexpr void swap(array& __y)
        noexcept(is_nothrow_swappable_v<__element_type>)
    {
        ranges::swap_ranges(begin(), end(), __y.begin(), __y.end());
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
            __out_of_bounds();

        return _M_arr[__n];
    }

    constexpr const_reference at(size_type __n) const
    {
        if (__n >= _N)
            __out_of_bounds();

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

    [[noreturn]] constexpr void __out_of_bounds() const
    {
        __detail::__array_out_of_bounds_access();
    }
};

template <class _T, class... _U>
    requires (is_same_v<_T, _U> && ...)
array(_T, _U...) -> array<_T, 1 + sizeof...(_U)>;


template <class _T, size_t _N>
constexpr bool operator==(const array<_T, _N>& __x, const array<_T, _N>& __y)
{
    for (size_t __i = 0; __i < _N; ++__i)
        if (__x[__i] != __y[__i])
            return false;

    return true;
}

template <class _T, size_t _N>
constexpr __detail::__synth_three_way_result<_T> operator<=>(const array<_T, _N>& __x, const array<_T, _N>& __y)
{
    for (size_t __i = 0; __i < _N; ++__i)
    {
        const auto& __a = __x[__i];
        const auto& __b = __y[__i];

        auto __res = __synth_three_way(__a, __b);
        if (__res != 0)
            return __res;
    }

    return strong_ordering::equal;
}


template <class _T, size_t _N>
constexpr void swap(array<_T, _N>& __x, array<_T, _N>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}


namespace __detail
{

template <class _T, size_t _N, size_t... _Idx>
constexpr array<remove_cv_t<_T>, _N> __make_array(index_sequence<_Idx...>, _T (&__a)[_N])
{
    return {{__a[_Idx]...}};
}

template <class _T, size_t _N, size_t... _Idx>
constexpr array<remove_cv_t<_T>, _N> __make_array(index_sequence<_Idx...>, _T (&&__a)[_N])
{
    return {{__XVI_STD_NS::move(__a[_Idx])...}};
}

} // namespace __detail


template <class _T, size_t _N>
    requires (!is_array_v<_T> && is_constructible_v<_T, _T&>)
constexpr array<remove_cv_t<_T>, _N> to_array(_T (&__a)[_N])
{
    return __detail::__make_array(make_index_sequence<_N>(), __a);
}

template <class _T, size_t _N>
constexpr array<remove_cv_t<_T>, _N> to_array(_T (&&__a)[_N])
{
    return __detail::__make_array(make_index_sequence<_N>(), __XVI_STD_NS::move(__a));
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
constexpr _T&& get(array<_T, _N>&& __a) noexcept
{
    static_assert(_I < _N);

    return __XVI_STD_NS::move(__a[_I]);
}

template <size_t _I, class _T, size_t _N>
constexpr const _T&& get(const array<_T, _N>&& __a) noexcept
{
    static_assert(_I < _N);

    return __XVI_STD_NS::move(__a[_I]);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ARRAY_H */
