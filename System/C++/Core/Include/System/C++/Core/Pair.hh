#ifndef __SYSTEM_CXX_CORE_PAIR_H
#define __SYSTEM_CXX_CORE_PAIR_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/CtorTags.hh>
#include <System/C++/Core/IntegerSequence.hh>
#include <System/C++/Core/ReferenceWrapper.hh>
#include <System/C++/Core/Swap.hh>
#include <System/C++/Core/TupleTraits.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class...> class tuple;
template <class> struct tuple_size;
template <size_t, class> struct tuple_element;


namespace __detail
{

template <class> struct __is_ranges_subrange_specialisation : false_type {};

} // namespace __detail


template <class _T1, class _T2>
struct pair
{
    using first_type = _T1;
    using second_type = _T2;

    _T1 first {};
    _T2 second {};

    constexpr 
    explicit(!__detail::__is_implicit_default_constructible_v<first_type> || !__detail::__is_implicit_default_constructible_v<second_type>)
    pair()
        requires __XVI_STD_NS::is_default_constructible_v<first_type> && __XVI_STD_NS::is_default_constructible_v<second_type>
        = default;

    pair(const pair&) = default;
    pair(pair&&) = default;

    constexpr 
    explicit(!is_convertible_v<const first_type&, first_type> || !is_convertible_v<const second_type&, second_type>)
    pair(const _T1& __t1, const _T2& __t2)
        requires __XVI_STD_NS::is_copy_constructible_v<first_type> && __XVI_STD_NS::is_copy_constructible_v<second_type>
        : first(__t1), second(__t2) {}

    template <class _U1, class _U2>
        requires __XVI_STD_NS::is_constructible_v<first_type, _U1> && __XVI_STD_NS::is_constructible_v<second_type, _U2>
        && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>))
    constexpr
    explicit(!is_convertible_v<_U1, first_type> || !is_convertible_v<_U2, second_type>)
    pair(_U1&& __u1, _U2&& __u2)
        : first(__XVI_STD_NS::forward<_U1>(__u1)), second(__XVI_STD_NS::forward<_U2>(__u2)) {}

    template <class _U1, class _U2>
        requires __XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>
    explicit(!is_convertible_v<_U1, first_type> || !is_convertible_v<_U2, second_type>)
    pair(_U1&&, _U2&&) = delete;

    template <class _U1, class _U2>
    constexpr
    explicit(!is_convertible_v<const _U1&, first_type> || !is_convertible_v<const _U2&, second_type>)
    pair(pair<_U1, _U2>& __p)
        requires __XVI_STD_NS::is_constructible_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
            && __XVI_STD_NS::is_constructible_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>
            && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>))
        :first(get<0>(static_cast<decltype(__p)>(__p))), second(get<1>(static_cast<decltype(__p)>(__p))) {}

    template <class _U1, class _U2>
        requires __XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>
    constexpr
    explicit(!is_convertible_v<const _U1&, first_type> || !is_convertible_v<const _U2&, second_type>)
    pair(pair<_U1, _U2>& __p) = delete;

    template <class _U1, class _U2>
    constexpr
    explicit(!is_convertible_v<const _U1&, first_type> || !is_convertible_v<const _U2&, second_type>)
    pair(const pair<_U1, _U2>& __p)
        requires __XVI_STD_NS::is_constructible_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
            && __XVI_STD_NS::is_constructible_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>
            && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>))
        : first(get<0>(static_cast<decltype(__p)>(__p))), second(get<1>(static_cast<decltype(__p)>(__p))) {}

    template <class _U1, class _U2>
        requires __XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>
    constexpr
    explicit(!is_convertible_v<const _U1&, first_type> || !is_convertible_v<const _U2&, second_type>)
    pair(const pair<_U1, _U2>& __p) = delete;

    template <class _U1, class _U2>
    constexpr
    explicit(!is_convertible_v<_U1&&, first_type> || !is_convertible_v<_U2&&, second_type>)
    pair(pair<_U1, _U2>&& __p)
        requires __XVI_STD_NS::is_constructible_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
            && __XVI_STD_NS::is_constructible_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>
            && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>))
        : first(get<0>(static_cast<decltype(__p)>(__p))), second(get<1>(static_cast<decltype(__p)>(__p))) {}

    template <class _U1, class _U2>
        requires __XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>
    constexpr
    explicit(!is_convertible_v<_U1&&, first_type> || !is_convertible_v<_U2&&, second_type>)
    pair(pair<_U1, _U2>&& __p) = delete;

    template <class _U1, class _U2>
    constexpr
    explicit(!is_convertible_v<_U1&&, first_type> || !is_convertible_v<_U2&&, second_type>)
    pair(const pair<_U1, _U2>&& __p)
        requires __XVI_STD_NS::is_constructible_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
            && __XVI_STD_NS::is_constructible_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>
            && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>))
        : first(get<0>(static_cast<decltype(__p)>(__p))), second(get<1>(static_cast<decltype(__p)>(__p))) {}

    template <class _U1, class _U2>
        requires __XVI_STD_NS::reference_constructs_from_temporary_v<first_type, _U1> || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, _U2>
    constexpr
    explicit(!is_convertible_v<_U1&&, first_type> || !is_convertible_v<_U2&&, second_type>)
    pair(const pair<_U1, _U2>&& __p) = delete;

    template <__detail::__pair_like _P>
    constexpr
    explicit(!is_convertible_v<decltype(get<0>(declval<_P>())), first_type> || !is_convertible_v<decltype(get<1>(declval<_P>())), second_type>)
    pair(_P&& __p)
        requires (!__detail::__is_ranges_subrange_specialisation<_P>::value)
            && __XVI_STD_NS::is_constructible_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
            && __XVI_STD_NS::is_constructible_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>
            && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
                || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>))
        : first(get<0>(static_cast<decltype(__p)>(__p))), second(get<1>(static_cast<decltype(__p)>(__p))) {}

    template <__detail::__pair_like _P>
    constexpr 
    explicit(!is_convertible_v<decltype(get<0>(declval<_P>())), first_type> || !is_convertible_v<decltype(get<2>(declval<_P>())), second_type>)
    pair(_P&& __p)
        requires (!__detail::__is_ranges_subrange_specialisation<_P>::value)
            && __XVI_STD_NS::is_constructible_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
            && __XVI_STD_NS::is_constructible_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>
            && (__XVI_STD_NS::reference_constructs_from_temporary_v<first_type, decltype(get<0>(static_cast<decltype(__p)>(__p)))>
                || __XVI_STD_NS::reference_constructs_from_temporary_v<second_type, decltype(get<1>(static_cast<decltype(__p)>(__p)))>)
        = delete;

    template <class... _Args1, class... _Args2>
        requires __XVI_STD_NS::is_constructible_v<first_type, _Args1...> && __XVI_STD_NS::is_constructible_v<second_type, _Args2...>
    constexpr pair(piecewise_construct_t, tuple<_Args1...> __first_args, tuple<_Args2...> __second_args)
        : pair(piecewise_construct,
               __XVI_STD_NS::move(__first_args), make_index_sequence<sizeof...(_Args1)>(),
               __XVI_STD_NS::move(__second_args), make_index_sequence<sizeof...(_Args2)>()) {}

    template <class... _Args1, class... _Args2,
              size_t... _Idx1, size_t... _Idx2>
    constexpr pair(piecewise_construct_t, 
                   tuple<_Args1...> __first_args, index_sequence<_Idx1...>,
                   tuple<_Args2...> __second_args, index_sequence<_Idx2...>)
        : first(__XVI_STD_NS::forward<_Args1>(get<_Idx1>(__first_args))...),
          second(__XVI_STD_NS::forward<_Args2>(get<_Idx2>(__second_args))...) {}

    constexpr ~pair() = default;

    constexpr pair& operator=(const pair& __p) 
        noexcept(__XVI_STD_NS::is_nothrow_copy_assignable_v<first_type> && __XVI_STD_NS::is_nothrow_copy_assignable_v<second_type>)
    = default;

    constexpr const pair& operator=(const pair& __p) const
        requires __XVI_STD_NS::is_copy_assignable_v<const _T1> && __XVI_STD_NS::is_copy_assignable_v<const _T2>
    {
        first = __p.first;
        second = __p.second;
        return *this;
    }

    template <class _U1, class _U2>
        requires __XVI_STD_NS::is_assignable_v<first_type&, const _U1&> && __XVI_STD_NS::is_assignable_v<second_type&, const _U2&>
    constexpr pair& operator=(const pair<_U1, _U2>& __p)
    {
        first = __p.first;
        second = __p.second;
        return *this;
    }

    template <class _U1, class _U2>
        requires __XVI_STD_NS::is_assignable_v<const first_type&, const _U1&> && __XVI_STD_NS::is_assignable_v<const second_type&, const _U2&>
    constexpr const pair& operator=(const pair<_U1, _U2>& __p) const
    {
        first = __p.first;
        second = __p.second;
        return *this;
    }

    constexpr pair& operator=(pair&& __p)
        noexcept(is_nothrow_move_assignable_v<first_type> && is_nothrow_move_assignable_v<second_type>)
    = default;

    constexpr const pair& operator&=(pair&& __p) const
        requires __XVI_STD_NS::is_assignable_v<const first_type&, first_type> && is_assignable_v<const second_type&, second_type>
    {
        first = __XVI_STD_NS::forward<first_type>(__p.first);
        second = __XVI_STD_NS::forward<second_type>(__p.second);
        return *this;
    }

    template <class _U1, class _U2>
        requires __XVI_STD_NS::is_assignable_v<first_type&, _U1> && __XVI_STD_NS::is_assignable_v<second_type&, _U2>
    constexpr pair& operator=(pair<_U1, _U2>&& __p)
    {
        first = __XVI_STD_NS::forward<_U1>(__p.first);
        second = __XVI_STD_NS::forward<_U2>(__p.second);
        return *this;
    }

    template <class _U1, class _U2>
        requires __XVI_STD_NS::is_assignable_v<const first_type&, _U1> && __XVI_STD_NS::is_assignable_v<const second_type&, _U2>
    constexpr const pair& operator=(pair<_U1, _U2>&& __p) const
    {
        first = __XVI_STD_NS::forward<_U1>(__p.first);
        second = __XVI_STD_NS::forward<_U2>(__p.second);
        return *this;
    }

    template <__detail::__pair_like _P>
    constexpr pair& operator=(_P&& __p)
        requires (!same_as<remove_cvref_t<_P>, pair>)
            && (!__detail::__is_ranges_subrange_specialisation<_P>::value)
            && is_assignable_v<_T1&, decltype(get<0>(__XVI_STD_NS::forward<_P>(__p)))>
            && is_assignable_v<_T2&, decltype(get<1>(__XVI_STD_NS::forward<_P>(__p)))>
    {
        first = get<0>(__XVI_STD_NS::forward<_P>(__p));
        second = get<1>(__XVI_STD_NS::forward<_P>(__p));
        return *this;
    }

    template <__detail::__pair_like _P>
    constexpr const pair& operator=(_P&& __p) const
        requires (!same_as<remove_cvref_t<_P>, pair>)
            && (!__detail::__is_ranges_subrange_specialisation<_P>::value)
            && is_assignable_v<const _T1&, decltype(get<0>(__XVI_STD_NS::forward<_P>(__p)))>
            && is_assignable_v<const _T2&, decltype(get<1>(__XVI_STD_NS::forward<_P>(__p)))>
    {
        first = get<0>(__XVI_STD_NS::forward<_P>(__p));
        second = get<1>(__XVI_STD_NS::forward<_P>(__p));
        return *this;
    }

    constexpr void swap(pair& __p)
        noexcept(is_nothrow_swappable_v<first_type> && is_nothrow_swappable_v<second_type>)
    {
        using __XVI_STD_NS::swap;
        swap(first, __p.first);
        swap(second, __p.second);
    }

    constexpr void swap(const pair& __p) const
        noexcept(is_nothrow_swappable_v<const first_type> && is_nothrow_swappable_v<const second_type>)
    {
        using __XVI_STD_NS::swap;
        swap(first, __p.first);
        swap(second, __p.second);
    }
};

template <class _T1, class _T2>
pair(_T1, _T2) -> pair<_T1, _T2>;


template <class _T1, class _T2>
constexpr bool operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    return __x.first == __y.first && __x.second == __y.second;
}

template <class _T1, class _T2>
constexpr __XVI_STD_NS::common_comparison_category_t<__detail::__synth_three_way_result<_T1>, __detail::__synth_three_way_result<_T2>>
operator<=>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
    if (auto __c = __detail::__synth_three_way(__x.first, __y.first); __c != 0)
        return __c;
    return __detail::__synth_three_way(__x.second, __y.second);
}

template <class _T1, class _T2>
    requires __XVI_STD_NS::is_swappable_v<_T1> && __XVI_STD_NS::is_swappable_v<_T2>
constexpr void swap(pair<_T1, _T2>& __x, pair<_T1, _T2>& __y)
    noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _T1, class _T2>
    requires __XVI_STD_NS::is_swappable_v<const _T1> && __XVI_STD_NS::is_swappable_v<const _T2>
constexpr void swap(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    noexcept(noexcept(__x.swap(__y)))
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


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_PAIR_H */
