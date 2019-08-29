#pragma once
#ifndef __SYSTEM_CXX_UTILITY_PAIR_H
#define __SYSTEM_CXX_UTILITY_PAIR_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/IntegerSequence.hh>
#include <System/C++/Utility/ReferenceWrapper.hh>
#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/InPlace.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class...> class tuple;
template <class> struct tuple_size;
template <size_t, class> struct tuple_element;


namespace __detail
{

template <class _T> void __implicit_default_construct_detect_fn(const _T&);
template <class _T> using __implicit_default_construct_detect = decltype(__implicit_default_construct_detect_fn<_T>({}));
template <class _T> struct __is_implicit_default_constructible : bool_constant<is_detected_v<__implicit_default_construct_detect, _T>> {};
template <class _T> inline constexpr bool __is_implicit_default_constructible_v = __is_implicit_default_constructible<_T>::value;

template <class _T1, class _T2, bool _DeleteCopyAssign = !is_copy_assignable_v<_T1> || !is_copy_assignable_v<_T2>>
    struct __pair_base {};
template <class _T1, class _T2> struct __pair_base<_T1, _T2, true>
    { constexpr __pair_base& operator=(const __pair_base&) = delete; };

}


template <class _T1, class _T2>
struct pair
    : public __detail::__pair_base<_T1, _T2>
{
    using first_type = _T1;
    using second_type = _T2;

    _T1 first;
    _T2 second;

    pair(const pair&) = default;
    pair(pair&&) = default;

    template <class = enable_if_t<is_default_constructible_v<first_type> && is_default_constructible_v<second_type>, void>>
    constexpr
    explicit(!__detail::__is_implicit_default_constructible_v<first_type> || !__detail::__is_implicit_default_constructible_v<second_type>)
    pair()
        : first(), second() {}

    template <class = enable_if_t<is_copy_constructible_v<first_type> && is_copy_constructible_v<second_type>, void>>
    constexpr 
    explicit(!is_convertible_v<const first_type&, first_type> || !is_convertible_v<const second_type&, second_type>)
    pair(const _T1& __t1, const _T2& __t2)
        : first(__t1), second(__t2) {}

    template <class _U1, class _U2, 
              class = enable_if_t<is_constructible_v<first_type, _U1&&> && is_constructible_v<second_type, _U2&&>, void>>
    constexpr
    explicit(!is_convertible_v<_U1, first_type> || !is_convertible_v<_U2, second_type>)
    pair(_U1&& __u1, _U2&& __u2)
        : first(__XVI_STD_NS::forward<_U1>(__u1)), second(__XVI_STD_NS::forward<_U2>(__u2)) {}

    template <class _U1, class _U2,
              class = enable_if_t<is_constructible_v<first_type, const _U1&> && is_constructible_v<second_type, const _U2&>, void>>
    constexpr
    explicit(!is_convertible_v<const _U1&, first_type> || !is_convertible_v<const _U2&, second_type>)
    pair(const pair<_U1, _U2>& __p)
        : first(__p.first), second(__p.second) {}

    template <class _U1, class _U2,
              class = enable_if_t<is_constructible_v<first_type, _U1&&> && is_constructible_v<second_type, _U2&&>, void>>
    constexpr
    explicit(!is_convertible_v<_U1&&, first_type> || !is_convertible_v<_U2&&, second_type>)
    pair(pair<_U1, _U2>&& __p)
        : first(__XVI_STD_NS::forward<_U1>(__p.first)), second(__XVI_STD_NS::forward<_U2>(__p.second)) {}

    template <class... _Args1, class... _Args2>
    constexpr pair(piecewise_construct_t, tuple<_Args1...> __first_args, tuple<_Args2...> __second_args)
        : pair(piecewise_construct,
               __first_args, make_index_sequence<sizeof...(_Args1)>(),
               __second_args, make_index_sequence<sizeof...(_Args2)>()) {}

    template <class... _Args1, class... _Args2,
              size_t... _Idx1, size_t... _Idx2>
    constexpr pair(piecewise_construct_t, 
                   tuple<_Args1...> __first_args, index_sequence<_Idx1...>,
                   tuple<_Args2...> __second_args, index_sequence<_Idx2...>)
        : first(__XVI_STD_NS::forward<_Args1>(get<_Idx1>(__first_args))...),
          second(__XVI_STD_NS::forward<_Args2>(get<_Idx2>(__second_args))...) {}

    // Will be implicitly deleted (via __pair_base) if requirements not met.
    constexpr pair& operator=(const pair&) = default;

    template <class _U1, class _U2,
              class = enable_if_t<is_assignable_v<first_type, const _U1&> && is_assignable_v<second_type, const _U2&>, void>>
    constexpr pair& operator=(const pair<_U1, _U2>& __p)
    {
        first = __p.first;
        second = __p.second;
        return *this;
    }

    template <class = enable_if_t<is_move_assignable_v<first_type> && is_move_assignable_v<second_type>, void>>
    constexpr pair& operator&=(pair&& __p)
        noexcept(is_nothrow_move_assignable_v<first_type> && is_nothrow_move_assignable_v<second_type>)
    {
        first = __XVI_STD_NS::forward<first_type>(__p.first);
        second = __XVI_STD_NS::forward<second_type>(__p.second);
        return *this;
    }

    template <class _U1, class _U2,
              class = enable_if_t<is_assignable_v<first_type, _U1&&> && is_assignable_v<second_type, _U2&&>, void>>
    constexpr pair& operator=(pair<_U1, _U2>&& __p)
    {
        first = __XVI_STD_NS::forward<_U1>(__p.first);
        second = __XVI_STD_NS::forward<_U2>(__p.second);
        return *this;
    }

    constexpr void swap(pair& __p)
        noexcept(is_nothrow_swappable_v<first_type> && is_nothrow_swappable_v<second_type>)
    {
        swap(first, __p.first);
        swap(second, __p.second);
    }
};


template <class _T1, class _T2>
constexpr bool operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    return __x.first == __y.first && __x.second == __y.second;
}

template <class _T1, class _T2>
constexpr bool operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    return !(__x == __y);
}

template <class _T1, class _T2>
constexpr bool operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    return __x.first < __y.first || (!(__y.first < __x.first) && __x.second < __y.second);
}

template <class _T1, class _T2>
constexpr bool operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    return __y < __x;
}

template <class _T1, class _T2>
constexpr bool operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    return !(__y < __x);
}

template <class _T1, class _T2>
constexpr bool operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    return !(__x > __y);
}

template <class _T1, class _T2, class = enable_if_t<is_swappable_v<_T1> && is_swappable_v<_T2>, void>>
constexpr void swap(pair<_T1, _T2>& __x, pair<_T1, _T2>& __y)
{
    __x.swap(__y);
}


template <class _T1, class _T2>
constexpr pair<unwrap_ref_decay_t<_T1>, unwrap_ref_decay_t<_T2>> make_pair(_T1&& __x, _T2&& __y)
{
    using _U1 = unwrap_ref_decay_t<_T1>;
    using _U2 = unwrap_ref_decay_t<_T2>;
    return pair<_U1, _U2>(__XVI_STD_NS::forward<_T1>(__x), __XVI_STD_NS::forward<_T2>(__y));
}


template <class _T1, class _T2>
struct tuple_size<pair<_T1, _T2>> : integral_constant<size_t, 2> {};

template <size_t _I, class _T1, class _T2>
struct tuple_element<_I, pair<_T1, _T2>>
{
    static_assert(_I < 2);
    using type = conditional_t<_I == 0, _T1, _T2>;
};

template <size_t _I, class _T1, class _T2>
constexpr typename tuple_element<_I, pair<_T1, _T2>>::type& get(pair<_T1, _T2>& __p) noexcept
{
    static_assert(_I < 2);
    if constexpr (_I == 0)
        return __p.first;
    else
        return __p.second;
}

template <size_t _I, class _T1, class _T2>
constexpr const typename tuple_element<_I, pair<_T1, _T2>>::type& get(const pair<_T1, _T2>& __p) noexcept
{
    static_assert(_I < 2);
    if constexpr (_I == 0)
        return __p.first;
    else
        return __p.second;
}

template <size_t _I, class _T1, class _T2>
constexpr typename tuple_element<_I, pair<_T1, _T2>>::type&& get(pair<_T1, _T2>&& __p) noexcept
{
    static_assert(_I < 2);
    if constexpr (_I == 0)
        return static_cast<_T1&&>(__p.first);
    else
        return static_cast<_T2&&>(__p.second);
}

template <size_t _I, class _T1, class _T2>
constexpr const typename tuple_element<_I, pair<_T1, _T2>>::type&& get(const pair<_T1, _T2>&& __p) noexcept
{
    static_assert(_I < 2);
    if constexpr (_I == 0)
        return static_cast<const _T1&&>(__p.first);
    else
        return static_cast<const _T2&&>(__p.second);
}

template <class _T1, class _T2>
constexpr _T1& get(pair<_T1, _T2>& __p) noexcept
{
    return __p.first;
}

template <class _T1, class _T2>
constexpr const _T1& get(const pair<_T1, _T2>& __p) noexcept
{
    return __p.first;
}

template <class _T1, class _T2>
constexpr _T1&& get(pair<_T1, _T2>&& __p) noexcept
{
    return static_cast<_T1&&>(__p.first);
}

template <class _T1, class _T2>
constexpr const _T1&& get(const pair<_T1, _T2>&& __p) noexcept
{
    return static_cast<const _T1&&>(__p.first);
}

template <class _T2, class _T1>
constexpr _T2& get(pair<_T1, _T2>& __p) noexcept
{
    return __p.second;
}

template <class _T2, class _T1>
constexpr const _T2& get(const pair<_T1, _T2>& __p) noexcept
{
    return __p.second;
}

template <class _T2, class _T1>
constexpr _T2&& get(pair<_T1, _T2>&& __p) noexcept
{
    return static_cast<_T2&&>(__p.second);
}

template <class _T2, class _T1>
constexpr const _T2&& get(const pair<_T1, _T2>&& __p) noexcept
{
    return static_cast<const _T2&&>(__p.second);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_PAIR_H */
