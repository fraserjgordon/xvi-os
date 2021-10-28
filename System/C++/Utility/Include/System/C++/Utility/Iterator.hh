#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ITERATOR_H
#define __SYSTEM_CXX_UTILITY_ITERATOR_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Utility/Abs.hh>
#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/PointerTraits.hh>
#include <System/C++/Utility/Variant.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class> struct iterator_traits;
namespace ranges::__detail
{
    struct __less;
}


namespace __detail
{

template <class _T> using __with_reference = _T&;

template <class _T>
concept __can_reference = requires { typename __with_reference<_T>; };

template <class _T>
concept __dereferencable = requires(_T& __t)
{
        { *__t } -> __can_reference;
};

} // namespace __detail


template <__detail::__dereferencable _T>
using iter_reference_t = decltype((*declval<_T&>()));


template <class> struct incrementable_traits {};

template <class _T>
    requires is_object_v<_T>
struct incrementable_traits<_T*>
{
    using difference_type = ptrdiff_t;
};

template <class _I>
struct incrementable_traits<const _I> : incrementable_traits<_I> {};

template <class _T>
    requires requires { typename _T::difference_type; }
struct incrementable_traits<_T>
{
    using difference_type = typename _T::difference_type;
};

template <class _T>
    requires (!requires { typename _T::difference_type; }
        && requires(const _T& __a, const _T& __b) { { __a - __b } -> integral; })
struct incrementable_traits<_T>
{
    using difference_type = make_signed_t<decltype(declval<_T>() - declval<_T>())>;
};


namespace __detail
{

// Not considered to be a primary iterator traits template if the special tag type is inherited from a base class.
template <class _T>
concept __is_primary_iterator_traits =
    requires { typename iterator_traits<_T>::__primary_iterator_traits_for; }
    && same_as<typename iterator_traits<_T>::__primary_iterator_traits_for, _T>;

template <class _T>
struct __iter_difference_t { using __type = typename iterator_traits<_T>::difference_type; };

template <class _T>
    requires __is_primary_iterator_traits<_T>
struct __iter_difference_t<_T> { using __type = typename incrementable_traits<_T>::difference_type; };

} // namespace __detail


template <class _T>
using iter_difference_t = typename __detail::__iter_difference_t<_T>::__type;


namespace __detail
{

template <class> struct __cond_value_type {};

template <class _T>
    requires is_object_v<_T>
struct __cond_value_type<_T>
{
    using value_type = remove_cv_t<_T>;
};

} // namespace __detail


template <class> struct indirectly_readable_traits {};

template <class _T>
struct indirectly_readable_traits<_T*> :
    __detail::__cond_value_type<_T> {};

template <class _I>
    requires is_array_v<_I>
struct indirectly_readable_traits<_I>
{
    using value_type = remove_cv_t<remove_extent_t<_I>>;
};

template <class _I>
struct indirectly_readable_traits<const _I> :
    indirectly_readable_traits<_I> {};

template <class _T>
    requires requires { typename _T::value_type; }
struct indirectly_readable_traits<_T> :
    __detail::__cond_value_type<typename _T::value_type> {};

template <class _T>
    requires requires { typename _T::element_type; }
struct indirectly_readable_traits<_T> :
    __detail::__cond_value_type<typename _T::element_type> {};


namespace __detail
{

template <class _T>
struct __iter_value_t { using __type = typename iterator_traits<_T>::value_type; };

template <class _T>
    requires __is_primary_iterator_traits<_T>
struct __iter_value_t<_T> { using __type = typename indirectly_readable_traits<_T>::value_type; };

} // namespace __detail


template <class _T>
using iter_value_t = typename __detail::__iter_value_t<_T>::__type;


namespace __detail
{

template <class _I>
concept __cpp17_iterator = copyable<_I>
    && requires(_I __i)
    {
        {   *__i } -> __can_reference;
        {  ++__i } -> same_as<_I&>;
        { *__i++ } -> __can_reference;
    };

template <class _I>
concept __cpp17_input_iterator = __cpp17_iterator<_I>
    && equality_comparable<_I>
    && requires(_I __i)
    {
        typename incrementable_traits<_I>::difference_type;
        typename indirectly_readable_traits<_I>::value_type;
        typename common_reference_t<iter_reference_t<_I>&&, typename indirectly_readable_traits<_I>::value_type&>;
        typename common_reference_t<decltype(*__i++)&&, typename indirectly_readable_traits<_I>::value_type&>;
        requires signed_integral<typename incrementable_traits<_I>::difference_type>;
    };

template <class _I>
concept __cpp17_forward_iterator = __cpp17_input_iterator<_I>
    && constructible_from<_I>
    //&& is_lvalue_reference_v<iter_reference_t<_I>> // does the wrong thing for iota_view
    /* TODO: remove me */ && !is_rvalue_reference_v<iter_reference_t<_I>>
    && same_as<remove_cvref_t<iter_reference_t<_I>>, typename indirectly_readable_traits<_I>::value_type>
    && requires(_I __i)
    {
        {  __i++ } -> convertible_to<const _I&>;
        { *__i++ } -> same_as<iter_reference_t<_I>>;
    };

template <class _I>
concept __cpp17_bidirectional_iterator = __cpp17_forward_iterator<_I>
    && requires(_I __i)
    {
        {  --__i } -> same_as<_I&>;
        {  __i-- } -> convertible_to<const _I&>;
        { *__i-- } -> same_as<iter_reference_t<_I>>;
    };

template <class _I>
concept __cpp17_random_access_iterator = __cpp17_bidirectional_iterator<_I>
    && totally_ordered<_I>
    && requires(_I __i, typename incrementable_traits<_I>::difference_type __n)
    {
        { __i += __n } -> same_as<_I&>;
        { __i -= __n } -> same_as<_I&>;
        { __i  + __n } -> same_as<_I>;
        { __n  + __i } -> same_as<_I>;
        { __i  - __n } -> same_as<_I>;
        { __i  - __i } -> same_as<decltype(__n)>;
        {  __i[__n]  } -> convertible_to<iter_reference_t<_I>>;
    };

} // namespace __detail


struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag : public random_access_iterator_tag {};


namespace __detail
{

template <class _I>
concept __has_iterator_types = requires
{
    typename _I::difference_type;
    typename _I::value_type;
    typename _I::reference;
    typename _I::iterator_category;
};

template <class _T>
concept __has_pointer_type = requires { typename _T::pointer; };

template <class _T>
concept __has_operator_arrow = requires(_T& __t) { __t.operator->(); };

template <class _T>
struct __iterator_traits_pointer { using __type = void; };

template <class _T>
    requires __has_pointer_type<_T>
struct __iterator_traits_pointer<_T> { using __type = typename _T::pointer; };

template <class _T>
    requires (!__has_pointer_type<_T> && !__has_iterator_types<_T> && __has_operator_arrow<_T>)
struct __iterator_traits_pointer<_T> { using __type = decltype(declval<_T&>().operator->()); };

template <class _T>
struct __iterator_traits_reference { using __type = iter_reference_t<_T>; };

template <class _T>
    requires requires { typename _T::reference_type; }
struct __iterator_traits_reference<_T> { using __type = typename _T::reference_type; };

template <class _I>
struct __iterator_traits_category;

template <class _I>
    requires __cpp17_random_access_iterator<_I>
struct __iterator_traits_category<_I> { using __type = random_access_iterator_tag; };

template <class _I>
    requires __cpp17_bidirectional_iterator<_I>
struct __iterator_traits_category<_I> { using __type = bidirectional_iterator_tag; };

template <class _I>
    requires __cpp17_forward_iterator<_I>
struct __iterator_traits_category<_I> { using __type = forward_iterator_tag; };

template <class _I>
    requires __cpp17_input_iterator<_I>
struct __iterator_traits_category<_I> { using __type = input_iterator_tag; };

template <class _I>
struct __iterator_traits_difference_type { using __type = void; };

template <class _I>
    requires requires { typename _I::difference_type; }
struct __iterator_traits_difference_type<_I> { using __type = typename _I::difference_type; };

} // namespace __detail


template <class _I>
struct iterator_traits { using __primary_iterator_traits_for = _I; };

template <class _I>
    requires __detail::__has_iterator_types<_I>
struct iterator_traits<_I>
{
    using __primary_iterator_traits_for = _I;

    using iterator_category     = typename _I::iterator_category;
    using value_type            = typename _I::value_type;
    using difference_type       = typename _I::difference_type;
    using pointer               = typename __detail::__iterator_traits_pointer<_I>::__type;
    using reference             = typename _I::reference;
};

template <class _I>
    requires (!__detail::__has_iterator_types<_I> && __detail::__cpp17_input_iterator<_I>)
struct iterator_traits<_I>
{
    using __primary_iterator_traits_for = _I;

    using iterator_category     = typename __detail::__iterator_traits_category<_I>::__type;
    using value_type            = typename indirectly_readable_traits<_I>::value_type;
    using difference_type       = typename incrementable_traits<_I>::difference_type;
    using pointer               = typename __detail::__iterator_traits_pointer<_I>::__type;
    using reference             = typename __detail::__iterator_traits_reference<_I>::__type;
};

template <class _I>
    requires (!__detail::__has_iterator_types<_I>
        && !__detail::__cpp17_input_iterator<_I>
        && __detail::__cpp17_iterator<_I>)
struct iterator_traits<_I>
{
    using __primary_iterator_traits_for = _I;

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = typename __detail::__iterator_traits_difference_type<_I>::__type;
    using pointer               = void;
    using reference             = void;
};

template <class _T>
    requires is_object_v<_T>
struct iterator_traits<_T*>
{
    using iterator_concept      = contiguous_iterator_tag;
    using iterator_category     = random_access_iterator_tag;
    using value_type            = remove_cv_t<_T>;
    using difference_type       = ptrdiff_t;
    using pointer               = _T*;
    using reference             = _T&;
};


namespace ranges
{

namespace __detail
{

struct __iter_move
{
    template <class _T>
        requires requires(_T&& __t) { iter_move(std::forward<_T>(__t)); }
    auto operator()(_T&& __t) const
        noexcept(noexcept(iter_move(std::forward<_T>(__t))))
    {
        return iter_move(std::forward<_T>(__t));
    }

    template <class _T>
        requires (!requires(_T&& __t) { iter_move(std::forward<_T>(__t)); }
            && requires(_T&& __t) { *std::forward<_T>(__t); })
    auto operator()(_T&& __t) const
        noexcept(noexcept(*std::forward<_T>(__t)))
    {
        if constexpr (is_rvalue_reference_v<decltype(*std::forward<_T>(__t))>)
            return *std::forward<_T>(__t);
        else
            return std::move(*std::forward<_T>(__t));
    }
};

} // namespace __detail

inline namespace __iter_move
{

inline constexpr __detail::__iter_move iter_move = {};

} // namespace __iter_move

} // namespace ranges


template <__detail::__dereferencable _T>
    requires requires(_T& __t) { { ranges::iter_move(__t) } -> __detail::__can_reference; }
using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<_T&>()));


namespace __detail
{

template <class _In>
concept __indirectly_readable_impl = requires(const _In __in)
    {
        typename iter_value_t<_In>;
        typename iter_reference_t<_In>;
        typename iter_rvalue_reference_t<_In>;
        { *__in } -> same_as<iter_reference_t<_In>>;
        { ranges::iter_move(__in) } -> same_as<iter_rvalue_reference_t<_In>>;
    }
    && common_reference_with<iter_reference_t<_In>&&, iter_value_t<_In>&>
    && common_reference_with<iter_reference_t<_In>&&, iter_rvalue_reference_t<_In>&&>
    && common_reference_with<iter_rvalue_reference_t<_In>&&, const iter_value_t<_In>&>;

}


template <class _In>
concept indirectly_readable = __detail::__indirectly_readable_impl<remove_cvref_t<_In>>;

template <class _Out, class _T>
concept indirectly_writable = requires(_Out&& __o, _T&& __t)
    {
        *__o = std::forward<_T>(__t);
        *std::forward<_Out>(__o) = std::forward<_T>(__t);
        const_cast<const iter_reference_t<_Out>&&>(*__o) = std::forward<_T>(__t);
        const_cast<const iter_reference_t<_Out>&&>(*std::forward<_Out>(__o)) = std::forward<_T>(__t);
    };

template <class _In, class _Out>
concept indirectly_movable = indirectly_readable<_In>
    && indirectly_writable<_Out, iter_rvalue_reference_t<_In>>;

template <class _In, class _Out>
concept indirectly_movable_storable = indirectly_movable<_In, _Out>
    && indirectly_writable<_Out, iter_value_t<_In>>
    && movable<iter_value_t<_In>>
    && constructible_from<iter_value_t<_In>, iter_rvalue_reference_t<_In>>
    && assignable_from<iter_value_t<_In>&, iter_rvalue_reference_t<_In>>;


namespace ranges
{

namespace __detail
{

template <class _I1, class _I2>
void iter_swap(_I1, _I2) = delete;

template <class _X, class _Y>
constexpr iter_value_t<remove_reference_t<_X>> __iter_exchange_move(_X&& __x, _Y&& __y)
    noexcept(noexcept(iter_value_t<remove_reference_t<_X>>(iter_move(__x))) && noexcept(*__x = iter_move(__y)))
{
    iter_value_t<remove_reference_t<_X>> __old_value(iter_move(__x));
    *__x = iter_move(__y);
    return __old_value;
}

struct __iter_swap
{
    template <class _T, class _U>
        requires requires(_T __t, _U __u) { iter_swap(__t, __u); }
    void operator()(_T&& __t, _U&& __u) const noexcept(noexcept(iter_swap(declval<_T>(), declval<_U>())))
    {
        iter_swap(std::forward<_T>(__t), std::forward<_U>(__u));
    }

    template <class _T, class _U, class _E1 = remove_reference_t<_T>, class _E2 = remove_reference_t<_U>>
        requires (!requires(_T __t, _U __u) { iter_swap(__t, __u); }
            && indirectly_readable<_E1> && indirectly_readable<_E2> && swappable_with<_T, _U>)
    void operator()(_T&& __t, _U&& __u) const
        noexcept(noexcept(ranges::swap(*declval<_T>(), *declval<_U>())))
    {
        ranges::swap(*std::forward<_T>(__t), std::forward<_U>(__u));
    }

    template <class _T, class _U, class _E1 = remove_reference_t<_T>, class _E2 = remove_reference_t<_U>>
        requires (!requires(_T __t, _U __u) { iter_swap(__t, __u); }
            && !(indirectly_readable<_E1> && indirectly_readable<_E2> && swappable_with<_T, _U>)
            && indirectly_movable_storable<_T, _U>
            && indirectly_movable_storable<_U, _T>)
    void operator()(_T&& __t, _U&& __u) const
        noexcept(noexcept(__iter_exchange_move(std::forward<_T>(__t), std::forward<_U>(__u))))
    {
        *std::forward<_T>(__t) = __iter_exchange_move(std::forward<_U>(__u), std::forward<_T>(__t));
    }
};

} // namespace __detail

inline namespace __iter_swap
{

inline constexpr __detail::__iter_swap iter_swap = {};

} // namespace __iter_swap

} // namespace ranges


namespace __detail
{

template <class _I>
struct __iter_traits_t { using __type = iterator_traits<_I>; };

template <class _I>
    requires __is_primary_iterator_traits<_I>
struct __iter_traits_t<_I> { using __type = _I; };

template <class _I>
using _ITER_TRAITS = typename __iter_traits_t<_I>::__type;

template <class _I>
struct __iter_concept_t {};

template <class _I>
    requires requires { typename _ITER_TRAITS<_I>::iterator_concept; }
struct __iter_concept_t<_I> { using __type = typename _ITER_TRAITS<_I>::iterator_concept; };

template <class _I>
    requires (!requires { typename _ITER_TRAITS<_I>::iterator_concept; }
        && requires { typename _ITER_TRAITS<_I>::iterator_category; })
struct __iter_concept_t<_I> { using __type = typename _ITER_TRAITS<_I>::iterator_category; };

template <class _I>
    requires (!requires { typename _ITER_TRAITS<_I>::iterator_concept; }
        && !requires { typename _ITER_TRAITS<_I>::iterator_category; }
        && __is_primary_iterator_traits<_I>)
struct __iter_concept_t<_I> { using __type = random_access_iterator_tag; };

template <class _I>
using _ITER_CONCEPT = typename __iter_concept_t<_I>::__type;

template <class _T>
inline constexpr bool __is_integer_like = integral<_T>;

template <class _T>
inline constexpr bool __is_signed_integer_like = signed_integral<_T>;

} // namespace __detail


template <indirectly_readable _T>
using iter_common_reference_t = common_reference_t<iter_reference_t<_T>, iter_value_t<_T>&>;

template <class _I>
concept weakly_incrementable = default_constructible<_I>
    && movable<_I>
    && requires(_I __i)
    {
        typename iter_difference_t<_I>;
        requires __detail::__is_signed_integer_like<iter_difference_t<_I>>;
        { ++__i } -> same_as<_I&>;
        __i++;
    };

template <class _I>
concept incrementable = regular<_I>
    && weakly_incrementable<_I>
    && requires(_I __i)
    {
        { __i++ } -> same_as<_I>;
    };

template <class _I>
concept input_or_output_iterator = requires(_I __i)
    {
        { *__i } -> __detail::__can_reference;
    }
    && weakly_incrementable<_I>;

template <class _S, class _I>
concept sentinel_for = semiregular<_S>
    && input_or_output_iterator<_I>
    && __detail::__weakly_equality_comparable_with<_S, _I>;

template <class _S, class _I>
inline constexpr bool disable_sized_sentinel = false;

template <class _S, class _I>
concept sized_sentinel_for = sentinel_for<_S, _I>
    && !disable_sized_sentinel<remove_cv_t<_S>, remove_cv_t<_I>>
    && requires(const _I& __i, const _S& __s)
    {
        { __s - __i } -> same_as<iter_difference_t<_I>>;
        { __i - __s } -> same_as<iter_difference_t<_I>>;
    };

template <class _I>
concept input_iterator = input_or_output_iterator<_I>
    && indirectly_readable<_I>
    && requires { typename __detail::_ITER_CONCEPT<_I>; }
    && derived_from<__detail::_ITER_CONCEPT<_I>, input_iterator_tag>;

template <class _I, class _T>
concept output_iterator = input_or_output_iterator<_I>
    && indirectly_writable<_I, _T>
    && requires(_I __i, _T&& __t)
    {
        *__i++ = std::forward<_T>(__t);
    };

template <class _I>
concept forward_iterator = input_iterator<_I>
    && derived_from<__detail::_ITER_CONCEPT<_I>, forward_iterator_tag>
    && incrementable<_I>
    && sentinel_for<_I, _I>;

template <class _I>
concept bidirectional_iterator = forward_iterator<_I>
    && derived_from<__detail::_ITER_CONCEPT<_I>, bidirectional_iterator_tag>
    && requires(_I __i)
    {
        { --__i } -> same_as<_I&>;
        { __i-- } -> same_as<_I>;
    };

template <class _I>
concept random_access_iterator = bidirectional_iterator<_I>
    && derived_from<__detail::_ITER_CONCEPT<_I>, random_access_iterator_tag>
    && totally_ordered<_I>
    && sized_sentinel_for<_I, _I>
    && requires(_I __i, const _I __j, const iter_difference_t<_I> __n)
    {
        { __i += __n } -> same_as<_I&>;
        { __j +  __n } -> same_as<_I>;
        { __n +  __j } -> same_as<_I>;
        { __i -= __n } -> same_as<_I&>;
        { __j -  __n } -> same_as<_I>;
        {  __j[__n]  } -> same_as<iter_reference_t<_I>>;
    };

template <class _I>
concept contiguous_iterator = random_access_iterator<_I>
    && derived_from<__detail::_ITER_CONCEPT<_I>, contiguous_iterator_tag>
    && is_lvalue_reference_v<iter_reference_t<_I>>
    && same_as<iter_value_t<_I>, remove_cvref_t<iter_reference_t<_I>>>
    && requires(const _I& __i)
    {
        { to_address(__i) } -> same_as<add_pointer_t<iter_reference_t<_I>>>;
    };


template <class _F, class _I>
concept indirectly_unary_invocable = indirectly_readable<_I>
    && copy_constructible<_F>
    && invocable<_F&, iter_value_t<_I>&>
    && invocable<_F&, iter_reference_t<_I>>
    && invocable<_F&, iter_common_reference_t<_I>>
    && common_reference_with<invoke_result_t<_F&, iter_value_t<_I>&>, invoke_result_t<_F&, iter_reference_t<_I>>>;

template <class _F, class _I>
concept indirect_regular_unary_invocable = indirectly_readable<_I>
    && copy_constructible<_F>
    && regular_invocable<_F, iter_value_t<_I>&>
    && regular_invocable<_F, iter_reference_t<_I>>
    && regular_invocable<_F, iter_common_reference_t<_I>>
    && common_reference_with<invoke_result_t<_F&, iter_value_t<_I>&>, invoke_result_t<_F&, iter_reference_t<_I>>>;

template <class _F, class _I>
concept indirect_unary_predicate = indirectly_readable<_I>
    && copy_constructible<_F>
    && predicate<_F&, iter_value_t<_I>&>
    && predicate<_F&, iter_reference_t<_I>>
    && predicate<_F&, iter_common_reference_t<_I>>;

template <class _F, class _I1, class _I2>
concept indirect_binary_predicate = indirectly_readable<_I1>
    && indirectly_readable<_I2>
    && copy_constructible<_F>
    && predicate<_F&, iter_value_t<_I1>&, iter_value_t<_I2>&>
    && predicate<_F&, iter_value_t<_I1>&, iter_reference_t<_I2>>
    && predicate<_F&, iter_reference_t<_I1>, iter_value_t<_I2>&>
    && predicate<_F&, iter_reference_t<_I1>, iter_reference_t<_I2>>
    && predicate<_F&, iter_common_reference_t<_I1>, iter_common_reference_t<_I2>>;

template <class _F, class _I1, class _I2 = _I1>
concept indirect_equivalence_relation = indirectly_readable<_I1> && indirectly_readable<_I2>
    && copy_constructible<_F>
    && equivalence_relation<_F&, iter_value_t<_I1>&, iter_value_t<_I2>&>
    && equivalence_relation<_F&, iter_value_t<_I1>&, iter_reference_t<_I2>>
    && equivalence_relation<_F&, iter_reference_t<_I1>, iter_value_t<_I2>&>
    && equivalence_relation<_F&, iter_reference_t<_I1>, iter_reference_t<_I2>>
    && equivalence_relation<_F&, iter_common_reference_t<_I1>, iter_common_reference_t<_I2>>;

template <class _F, class _I1, class _I2 = _I1>
concept indirect_strict_weak_order = indirectly_readable<_I1> && indirectly_readable<_I2>
    && copy_constructible<_F>
    && strict_weak_order<_F&, iter_value_t<_I1>&, iter_value_t<_I2>&>
    && strict_weak_order<_F&, iter_value_t<_I1>&, iter_reference_t<_I2>>
    && strict_weak_order<_F&, iter_reference_t<_I1>, iter_value_t<_I2>&>
    && strict_weak_order<_F&, iter_reference_t<_I1>, iter_reference_t<_I2>>
    && strict_weak_order<_F&, iter_common_reference_t<_I1>, iter_common_reference_t<_I2>>;


template <class _F, class... _Is>
    requires (indirectly_readable<_Is> && ...)
        && invocable<_F, iter_reference_t<_Is>...>
using indirect_result_t = invoke_result_t<_F, iter_reference_t<_Is>...>;


template <indirectly_readable _I, indirect_regular_unary_invocable<_I> _Proj>
struct projected
{
    using value_type = remove_cvref_t<indirect_result_t<_Proj&, _I>>;

    // Not defined.
    indirect_result_t<_Proj&, _I> operator*() const;
};

template <weakly_incrementable _I, class _Proj>
struct incrementable_traits<projected<_I, _Proj>>
{
    using difference_type = iter_difference_t<_I>;
};


template <class _In, class _Out>
concept indirectly_copyable = indirectly_readable<_In>
    && indirectly_writable<_Out, iter_reference_t<_In>>;

template <class _In, class _Out>
concept indirectly_copyable_storable = indirectly_copyable<_In, _Out>
    && indirectly_writable<_Out, const iter_value_t<_In>&>
    && copyable<iter_value_t<_In>>
    && constructible_from<iter_value_t<_In>, iter_reference_t<_In>>
    && assignable_from<iter_value_t<_In>&, iter_reference_t<_In>>;

template <class _I1, class _I2 = _I1>
concept indirectly_swappable = indirectly_readable<_I1>
    && indirectly_readable<_I2>
    && requires(_I1& __i1, _I2& __i2)
    {
        ranges::iter_swap(__i1, __i1);
        ranges::iter_swap(__i1, __i2);
        ranges::iter_swap(__i2, __i1);
        ranges::iter_swap(__i2, __i2);
    };

template <class _I1, class _I2, class _R, class _P1 = identity, class _P2 = identity>
concept indirectly_comparable = indirect_binary_predicate<_R, projected<_I1, _P1>, projected<_I2, _P2>>;

template <class _I>
concept permutable = forward_iterator<_I>
    && indirectly_movable_storable<_I, _I>
    && indirectly_swappable<_I, _I>;

template <class _I1, class _I2, class _Out, class _R = ranges::__detail::__less, class _P1 = identity, class _P2 = identity>
concept mergeable = input_iterator<_I1>
    && input_iterator<_I2>
    && weakly_incrementable<_Out>
    && indirectly_copyable<_I1, _Out>
    && indirectly_copyable<_I2, _Out>
    && indirect_strict_weak_order<_R, projected<_I1, _P1>, projected<_I2, _P2>>;

template <class _I, class _R = ranges::__detail::__less, class _P = identity>
concept sortable = permutable<_I>
    && indirect_strict_weak_order<_R, projected<_I, _P>>;


template <class _InputIterator, class _Distance>
constexpr void advance(_InputIterator& __i, _Distance __n)
{
    //if constexpr (_Cpp17RandomAccessIterator<_InputIterator>)
    if constexpr (is_base_of_v<random_access_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
    {
        if (__n > 0)
            __i += __n;
        else if (__n < 0)
            __i -= -__n;
    }
    //else if constexpr (_Cpp17BidirectionalIterator<_InputIterator>)
    else if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
    {
        if (__n > 0)
            for (_Distance __j = 0; __j < __n; ++__j)
                ++__i;
        else if (__n < 0)
            for (_Distance __j = 0; __j < -__n; ++__j)
                --__i;
    }
    else
    {
        for (_Distance __j = 0; __j < __n; ++__j)
            ++__i;
    }
}

template <class _InputIterator>
constexpr typename iterator_traits<_InputIterator>::difference_type
distance(_InputIterator __first, _InputIterator __last)
{
    //if constexpr (_Cpp17RandomAccessIterator<_InputIterator>)
    if constexpr (is_base_of_v<random_access_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
        return __last - __first;
    
    typename iterator_traits<_InputIterator>::difference_type __i = 0;
    while (__first != __last)
        ++__i, ++__first;

    return __i;
}

template <class _InputIterator>
constexpr _InputIterator next(_InputIterator __x, typename iterator_traits<_InputIterator>::difference_type __n = 1)
{
    advance(__x, __n);
    return __x;
}

template <class _BidirectionalIterator>
constexpr _BidirectionalIterator prev(_BidirectionalIterator __x, typename iterator_traits<_BidirectionalIterator>::difference_type __n = 1)
{
    advance(__x, -__n);
    return __x;
}


namespace ranges
{

template <class _T>
inline constexpr bool disable_sized_range = false;

template <class _T>
inline constexpr bool enable_borrowed_range = false;

namespace __detail
{

template <class _T>
constexpr decay_t<_T> __decay_copy(_T&& __v)
     noexcept(is_nothrow_convertible_v<_T, decay_t<_T>>)
{
    return __XVI_STD_NS::forward<_T>(__v);
}

template <class _T>
concept __lvalue_or_borrowed_range = !is_rvalue_reference_v<_T> || enable_borrowed_range<remove_cvref_t<_T>>;

template <class _T> void begin(_T&&) = delete;
template <class _T> void begin(initializer_list<_T>&&) = delete;    

struct __begin
{
    template <class _T>
        requires (__lvalue_or_borrowed_range<_T> && is_array_v<remove_cvref_t<_T>>)
    constexpr decltype(auto) operator()(_T&& __t) const noexcept
    {
        return __t + 0;
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !is_array_v<remove_cvref_t<_T>>
            && requires(_T& __t) { { __decay_copy(__t.begin()) } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(__t.begin())))
    {
        return __decay_copy(static_cast<_T&>(__t).begin());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T> 
            && !is_array_v<remove_cvref_t<_T>>
            && !requires(_T& __t) { { __decay_copy(__t.begin())  } -> input_or_output_iterator; }
            && requires(_T& __t) { { __decay_copy(begin(__t)) } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(begin(static_cast<_T&>(__t)))))
    {
        return __decay_copy(begin(static_cast<_T&>(__t)));
    }
};

template <class _T> void end(_T&&) = delete;
template <class _T> void end(initializer_list<_T>&&) = delete;

struct __end
{
    template <class _T>
        requires (__lvalue_or_borrowed_range<_T> && is_array_v<remove_cvref_t<_T>>)
    constexpr decltype(auto) operator()(_T&& __t) const noexcept
    {
        return __t + extent_v<remove_cvref_t<_T>>;
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !is_array_v<remove_cvref_t<_T>>
            && requires(_T& __t) { { __decay_copy(__t.end()) } -> sentinel_for<decltype(__begin()(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(__t.end())))
    {
        return __decay_copy(__t.end());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T> 
            && !is_array_v<remove_cvref_t<_T>>
            && !requires(_T& __t) { { __decay_copy(__t.end()) } -> sentinel_for<decltype(__begin()(__t))>; }
            && requires(_T& __t) { { __decay_copy(end(__t)) } -> sentinel_for<decltype(__begin()(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(end(static_cast<_T&>(__t)))))
    {
        return __decay_copy(end(statit_cast<_T&>(__t)));
    }
};

struct __cbegin
{
    template <class _T>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__begin()(static_cast<const _T&&>(__t))))
    {
        return __begin()(static_cast<const _T&&>(__t));
    }

    template <class _T>
        requires is_lvalue_reference_v<_T>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__begin()(static_cast<const _T&>(__t))))
    {
        return __begin()(static_cast<const _T&>(__t));
    }
};

struct __cend
{
    template <class _T>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__end()(static_cast<const _T&&>(__t))))
    {
        return __end()(static_cast<const _T&&>(__t));
    }

    template <class _T>
        requires is_lvalue_reference_v<_T>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__end()(static_cast<const _T&>(__t))))
    {
        return __end()(static_cast<const _T&>(__t));
    }
};

template <class _T> void size(_T&&) = delete;

struct __size
{
    template <class _T>
        requires is_array_v<remove_cvref_t<_T>>
    constexpr decltype(auto) operator()(_T&&) const noexcept
    {
        return __decay_copy(extent_v<remove_cvref_t<_T>>);
    }

    template <class _T>
        requires (!is_array_v<remove_cvref_t<_T>>
            && !disable_sized_range<remove_cvref_t<_T>>
            && requires(_T&& __t) { __decay_copy(std::forward<_T>(__t).size()); }
            && __XVI_STD_UTILITY_NS::__detail::__is_integer_like<decltype(__decay_copy(declval<_T&&>().size()))>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(std::forward<_T>(__t).size())))
    {
        return __decay_copy(std::forward<_T>(__t).size());
    }

    template <class _T>
    requires (!is_array_v<remove_cvref_t<_T>>
        && !disable_sized_range<remove_cvref_t<_T>>
        && (!requires(_T&& __t) { __decay_copy(std::forward<_T>(__t).size()); }
                || !__XVI_STD_UTILITY_NS::__detail::__is_integer_like<decltype(__decay_copy(declval<_T&&>().size()))>)
        && requires(_T&& __t) { __decay_copy(size(std::forward<_T>(__t))); }
        && __XVI_STD_UTILITY_NS::__detail::__is_integer_like<decltype(__decay_copy(size(declval<_T&&>())))>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(size(std::forward<_T>(__t)))))
    {
        return __decay_copy(size(std::forward<_T>(__t)));
    }

    template <class _T>
        requires (!is_array_v<remove_cvref_t<_T>>
            && (!requires(_T&& __t) { __decay_copy(std::forward<_T>(__t).size()); }
                 || !__XVI_STD_UTILITY_NS::__detail::__is_integer_like<decltype(__decay_copy(declval<_T&&>().size()))>)
            && (!requires(_T&& __t) { __decay_copy(size(std::forward<_T>(__t))); }
                || !__XVI_STD_UTILITY_NS::__detail::__is_integer_like<decltype(__decay_copy(size(declval<_T&&>())))>)
            && requires(_T&& __t)
                {
                    __end()(std::forward<_T>(__t)) - __begin()(std::forward<_T>(__t));
                }
            && sized_sentinel_for<decltype(__end()(declval<_T&&>())), decltype(__begin()(declval<_T&&>()))>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__end()(std::forward<_T>(__t)) - __begin()(std::forward<_T>(__t))))
    {
        return __end()(std::forward<_T>(__t)) - __begin()(std::forward<_T>(__t));
    }
};

struct __empty
{
    template <class _T>
        requires requires(_T&& __t) { bool(std::forward<_T>(__t).empty()); }
    constexpr bool operator()(_T&& __t) const
        noexcept(noexcept(bool(std::forward<_T>(__t).empty())))
    {
        return bool(std::forward<_T>(__t).empty());
    }

    template <class _T>
        requires (!requires(_T&& __t) { bool(std::forward<_T>(__t).empty()); }
            && requires(_T&& __t) { __size()(std::forward<_T>(__t)) == 0; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__size()(std::forward<_T>(__t)) == 0))
    {
        return __size()(std::forward<_T>(__t)) == 0;
    }

    template <class _T>
        requires (!requires(_T&& __t) { bool(std::forward<_T>(__t).empty()); }
            && !requires(_T&& __t) { __size()(std::forward<_T>(__t)) == 0; }
            && requires(_T&& __t)
            {
                { __begin()(std::forward<_T>(__t)) } -> forward_iterator;
                bool(__begin()(std::forward<_T>(__t)) == __end()(std::forward<_T>(__t)));
            })
    constexpr bool operator()(_T&& __t) const
        noexcept(noexcept(bool(__begin()(std::forward<_T>(__t)) == __end()(std::forward<_T>(__t)))))
    {
        return bool(__begin()(std::forward<_T>(__t)) == __end()(std::forward<_T>(__t)));
    }
};

struct __data
{
    template <class _T>
        requires (requires(_T&& __t) { __decay_copy(std::forward<_T>(__t).data()); }
            && is_pointer_v<decltype(__decay_copy(declval<_T&&>().data()))>
            && is_object_v<remove_pointer_t<decltype(__decay_copy(declval<_T&&>().data()))>>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(std::forward<_T>(__t).data())))
    {
        return __decay_copy(std::forward<_T>(__t).data());
    }

    template <class _T>
        requires ((!requires(_T&& __t) { __decay_copy(std::forward<_T>(__t).data()); }
                   || !is_pointer_v<decltype(__decay_copy(declval<_T&&>().data()))>
                   || !is_object_v<remove_pointer_t<decltype(__decay_copy(declval<_T&&>().data()))>>)
            && requires(_T&& __t) { { __begin()(std::forward<_T>(__t)) } -> contiguous_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(to_address(__begin()(std::forward<_T>(__t)))))
    {
        return to_address(__begin()(std::forward<_T>(__t)));
    }
};

struct __cdata
{
    template <class _T>
        requires is_lvalue_reference_v<_T>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__data()(static_cast<const _T&>(__t))))
    {
        return __data()(static_cast<const _T&>(__t));
    }

    template <class _T>
        requires (!is_lvalue_reference_v<_T>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__data()(static_cast<const _T&&>(__t))))
    {
        return __data()(static_cast<const _T&&>(__t));
    }
};

} // namespace __detail

inline namespace __begin { inline constexpr __detail::__begin begin = {}; }
inline namespace __end { inline constexpr __detail::__end end = {}; }
inline namespace __cbegin { inline constexpr __detail::__cbegin cbegin = {}; }
inline namespace __cend { inline constexpr __detail::__cend cend = {}; }
inline namespace __size { inline constexpr __detail::__size size = {}; }
inline namespace __empty { inline constexpr __detail::__empty empty = {}; }
inline namespace __data { inline constexpr __detail::__data data = {}; }
inline namespace __cdata { inline constexpr __detail::__cdata cdata = {}; }


template <class _T>
concept range = requires(_T& __t)
{
    ranges::begin(__t);
    ranges::end(__t);
};

template <class _T>
concept borrowed_range = range<_T>
    && (is_lvalue_reference_v<_T> || enable_borrowed_range<remove_cvref_t<_T>>);

template <class _T>
concept sized_range = range<_T>
    && !disable_sized_range<remove_cvref_t<_T>>
    && requires(_T& __t)
    {
        ranges::size(__t);
    };

template <range _R>
using iterator_t = decltype(ranges::begin(declval<_R&>()));

template <range _R>
using sentinel_t = decltype(ranges::end(declval<_R&>()));

template <range _R>
using range_difference_t = iter_difference_t<iterator_t<_R>>;

template <range _R>
using range_value_t = iter_value_t<iterator_t<_R>>;

template <range _R>
using range_reference_t = iter_reference_t<iterator_t<_R>>;

template <range _R>
using range_rvalue_reference_t = iter_rvalue_reference_t<iterator_t<_R>>;


namespace __detail
{

struct __advance
{
    template <input_or_output_iterator _I>
    constexpr void operator()(_I& __i, iter_difference_t<_I> __n) const
    {
        for (iter_difference_t<_I> __j = 0; __j < __n; ++__j)
            ++__i;
    }

    template <bidirectional_iterator _I>
    constexpr void operator()(_I& __i, iter_difference_t<_I> __n) const
    {
        if (__n < 0)
        {
            for (iter_difference_t<_I> __j = 0; __j < -__n; ++__j)
                --__i;
        }
        else
        {
            for (iter_difference_t<_I> __j = 0; __j < __n; ++__j)
                ++__i;
        }
    }

    template <random_access_iterator _I>
    constexpr void operator()(_I& __i, iter_difference_t<_I> __n) const
    {
        __i += __n;
    }


    template <input_or_output_iterator _I, sentinel_for<_I> _S>
    constexpr void operator()(_I& __i, _S __bound) const
    {
        while (bool(__i != __bound))
            ++__i;
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
        requires assignable_from<_I&, _S>
    constexpr void operator()(_I& __i, _S __bound) const
    {
        __i = std::move(__bound);
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
        requires (!assignable_from<_I&, _S>
            && sized_sentinel_for<_S, _I>)
    constexpr void operator()(_I& __i, _S __bound) const
    {
        operator()(__i, __bound - __i);
    }


    template <input_or_output_iterator _I, sentinel_for<_I> _S>
    constexpr iter_difference_t<_I> operator()(_I& __i, iter_difference_t<_I> __n, _S __bound) const
    {
        while (bool(__i != __bound) && __n-- > 0)
            ++__i;
        return __n;
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
        requires sized_sentinel_for<_S, _I>
    constexpr iter_difference_t<_I> operator()(_I& __i, iter_difference_t<_I> __n, _S __bound) const
    {
        auto __length = __bound - __i;
        if (__XVI_STD_UTILITY_NS::abs(__n) >= __XVI_STD_UTILITY_NS::abs(__length))
        {
            operator()(__i, __bound);
            return __n - __length;
        }
        else
        {
            operator()(__i, __n);
            return __n - __n;
        }
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
        requires (!sized_sentinel_for<_S, _I>
            &&bidirectional_iterator<_S> && same_as<_I, _S>)
    constexpr iter_difference_t<_I> operator()(_I& __i, iter_difference_t<_I> __n, _S __bound) const
    {
        if (__n >= 0)
        {
            while (bool(__i != __bound) && __n-- > 0)
                ++__i;
            return __n;
        }
        else
        {
            while (bool(__i != __bound) && __n++ < 0)
                --__i;
            return __n;
        }
    }
};

struct __distance
{
    template <input_or_output_iterator _I, sentinel_for<_I> _S>
    constexpr iter_difference_t<_I> operator()(_I __first, _S __last) const
    {
        iter_difference_t<_I> __n = 0;
        while (!bool(__first == __last))
            ++__n;
        return __n;
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
        requires sized_sentinel_for<_S, _I>
    constexpr iter_difference_t<_I> operator()(_I __first, _S __last) const
    {
        return (__last - __first);
    }

    template <range _R>
    constexpr range_difference_t<_R> operator()(_R&& __r) const
    {
        return __distance()(__begin()(__r), __end()(__r));
    }

    template <range _R>
        requires sized_range<_R>
    constexpr range_difference_t<_R> operator()(_R&& __r) const
    {
        return static_cast<range_difference_t<_R>>(__size()(__r));
    }
};

} // namespace __detail

inline namespace __advance { inline constexpr __detail::__advance advance = {}; }
inline namespace __distance { inline constexpr __detail::__distance distance = {}; }

namespace __detail
{

struct __next
{
    template <input_or_output_iterator _I>
    constexpr _I operator()(_I __x) const
        noexcept(noexcept(++__x))
    {
        ++__x;
        return __x;
    }

    template <input_or_output_iterator _I>
    constexpr _I operator()(_I __x, iter_difference_t<_I> __n) const
        noexcept(noexcept(ranges::advance(__x, __n)))
    {
        ranges::advance(__x, __n);
        return __x;
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
    constexpr _I operator()(_I __x, _S __bound) const
        noexcept(noexcept(ranges::advance(__x, __bound)))
    {
        ranges::advance(__x, __bound);
        return __x;
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
    constexpr _I operator()(_I __x, iter_difference_t<_I> __n, _S __bound) const
        noexcept(noexcept(ranges::advance(__x, __n, __bound)))
    {
        ranges::advance(__x, __n, __bound);
        return __x;
    }
};

struct __prev
{
    template <bidirectional_iterator _I>
    constexpr _I operator()(_I __x) const
        noexcept(noexcept(--__x))
    {
        --__x;
        return __x;
    }

    template <bidirectional_iterator _I>
    constexpr _I operator()(_I __x, iter_difference_t<_I> __n) const
        noexcept(noexcept(ranges::advance(__x, -__n)))
    {
        ranges::advance(__x, -__n);
        return __x;
    }

    template <bidirectional_iterator _I, sentinel_for<_I> _S>
    constexpr _I operator()(_I __x, iter_difference_t<_I> __n, _S __bound) const
        noexcept(noexcept(ranges::advance(__x, -__n, __bound)))
    {
        ranges::advance(__x, -__n, __bound);
        return __x;
    }
};

} // namespace __detail

inline namespace __next { inline constexpr __detail::__next next = {}; }
inline namespace __prev { inline constexpr __detail::__prev prev = {}; }

} // namespace ranges



template <class _Iterator>
class reverse_iterator
{
public:

    using iterator_type     = _Iterator;
    using iterator_concept  = conditional_t<random_access_iterator<_Iterator>,
                                            random_access_iterator_tag,
                                            bidirectional_iterator_tag>;
    using iterator_category = conditional_t<derived_from<typename iterator_traits<_Iterator>::iterator_category, random_access_iterator_tag>,
                                            random_access_iterator_tag,
                                            typename iterator_traits<_Iterator>::iterator_category>;
    using value_type        = iter_value_t<_Iterator>;
    using difference_type   = iter_difference_t<_Iterator>;
    using pointer           = typename iterator_traits<_Iterator>::pointer;
    using reference         = iter_reference_t<_Iterator>;

    constexpr reverse_iterator()
        : _M_current{}
    {
    }

    constexpr explicit reverse_iterator(_Iterator __x)
        : _M_current(__x)
    {
    }

    template <class _U>
    constexpr reverse_iterator(const reverse_iterator<_U>& __u)
        : _M_current(__u._M_current)
    {
    }

    template <class _U>
    constexpr reverse_iterator& operator=(const reverse_iterator<_U>& __u)
    {
        _M_current = __u.base();
        return *this;
    }

    constexpr _Iterator base() const
    {
        return _M_current;
    }

    constexpr reference operator*() const
    {
        auto __tmp = _M_current;
        return *--__tmp;
    }

    constexpr pointer operator->() const
        requires (is_pointer_v<_Iterator> || requires(const _Iterator __i) { __i.operator->(); })
    {
        if constexpr (is_pointer_v<_Iterator>)
            return prev(_M_current);
        else
            return prev(_M_current).operator->();
    }

    constexpr reverse_iterator& operator++()
    {
        --_M_current;
        return *this;
    }

    constexpr reverse_iterator operator++(int)
    {
        reverse_iterator __tmp = *this;
        --_M_current;
        return __tmp;
    }

    constexpr reverse_iterator& operator--()
    {
        ++_M_current;
        return *this;
    }

    constexpr reverse_iterator operator--(int)
    {
        reverse_iterator __tmp = *this;
        ++_M_current;
        return __tmp;
    }

    constexpr reverse_iterator operator+(difference_type __n) const
    {
        return reverse_iterator(_M_current - __n);
    }

    constexpr reverse_iterator& operator+=(difference_type __n)
    {
        _M_current -= __n;
        return *this;
    }

    constexpr reverse_iterator operator-(difference_type __n) const
    {
        return reverse_iterator(_M_current + __n);
    }

    constexpr reverse_iterator& operator-=(difference_type __n)
    {
        _M_current += __n;
        return *this;
    }

    constexpr decltype(auto) operator[](difference_type __n) const
    {
        return _M_current[__n - 1];
    }

    friend constexpr iter_rvalue_reference_t<_Iterator> iter_move(const reverse_iterator& __i)
        noexcept(is_nothrow_copy_constructible_v<_Iterator> && noexcept(ranges::iter_move(--declval<_Iterator&>())))
    {
        auto __tmp = __i.base();
        return ranges::iter_move(--__tmp);
    }

    template <indirectly_swappable<_Iterator> _Iterator2>
    friend constexpr void iter_swap(const reverse_iterator& __x, const reverse_iterator<_Iterator2>& __y)
        noexcept(is_nothrow_copy_constructible_v<_Iterator>
                 && is_nothrow_copy_constructible_v<_Iterator2>
                 && noexcept(ranges::iter_swap(--declval<_Iterator&>(), --declval<_Iterator2&>())))
    {
        auto __xtmp = __x.base();
        auto __ytmp = __y.base();
        ranges::iter_swap(--__xtmp, --__ytmp);
    }

protected:

    template <class> friend class reverse_iterator;

    _Iterator _M_current;
};

template <class _Iter1, class _Iter2>
constexpr bool operator==(const reverse_iterator<_Iter1>& __x, const reverse_iterator<_Iter2>& __y)
{
    return __x.base() == __y.base();
}

template <class _Iter1, class _Iter2>
constexpr bool operator!=(const reverse_iterator<_Iter1>& __x, const reverse_iterator<_Iter2>& __y)
{
    return __x.base() != __y.base();
}

template <class _Iter1, class _Iter2>
constexpr bool operator< (const reverse_iterator<_Iter1>& __x, const reverse_iterator<_Iter2>& __y)
{
    return __x.base() > __y.base();
}

template <class _Iter1, class _Iter2>
constexpr bool operator> (const reverse_iterator<_Iter1>& __x, const reverse_iterator<_Iter2>& __y)
{
    return __x.base() < __y.base();
}

template <class _Iter1, class _Iter2>
constexpr bool operator<=(const reverse_iterator<_Iter1>& __x, const reverse_iterator<_Iter2>& __y)
{
    return __x.base() >= __y.base();
}

template <class _Iter1, class _Iter2>
constexpr bool operator>=(const reverse_iterator<_Iter1>& __x, const reverse_iterator<_Iter2>& __y)
{
    return __x.base() <= __y.base();
}

template <class _Iterator1, three_way_comparable_with<_Iterator1, weak_equality> _Iterator2>
constexpr compare_three_way_result_t<_Iterator1, _Iterator2> operator<=>(const reverse_iterator<_Iterator1>& __x, const reverse_iterator<_Iterator2>& __y)
{
    return __y.base() <=> __x.base();
}


template <class _Iter1, class _Iter2>
constexpr auto operator-(const reverse_iterator<_Iter1>& __x, const reverse_iterator<_Iter2>& __y)
    -> decltype(__y.base() - __x.base())
{
    return __y.base() - __x.base();
}

template <class _Iterator>
constexpr reverse_iterator<_Iterator> operator+(typename reverse_iterator<_Iterator>::difference_type __n, const reverse_iterator<_Iterator>& __x)
{
    return reverse_iterator<_Iterator>(__x.base() - __n);
}

template <class _Iterator>
constexpr reverse_iterator<_Iterator> make_reverse_iterator(_Iterator __i)
{
    return reverse_iterator<_Iterator>(__i);
}

template <class _Iterator1, class _Iterator2>
    requires (!sized_sentinel_for<_Iterator1, _Iterator2>)
inline constexpr bool disable_sized_sentinel<reverse_iterator<_Iterator1>, reverse_iterator<_Iterator2>> = true;


namespace ranges
{

namespace __detail
{

template <class _T> void rbegin(_T&&) = delete;

struct __rbegin
{
    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && requires(_T& __t) { { __decay_copy(__t).rbegin() } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(static_cast<_T&>(__t).begin())))
    {
        return __decay_copy(static_cast<_T&>(__t).rbegin());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __decay_copy(__t.rbegin()) } -> input_or_output_iterator; }
            && requires(_T& __t) { { __decay_copy(rbegin(__t)) } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(rbegin(static_cast<_T&>(__t)))))
    {
        return __decay_copy(rbegin(static_cast<_T&>(__t)));
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __decay_copy(__t.rbegin()) } -> input_or_output_iterator; }
            && !requires(_T& __t) { { __decay_copy(rbegin(__t)) } -> input_or_output_iterator; }
            && requires(_T& __t)
            {
                { ranges::begin(__t) } -> bidirectional_iterator;
                { ranges::end(__t)   } -> bidirectional_iterator;
            }
            && same_as<decltype(ranges::begin(declval<_T&>())), decltype(ranges::end(declval<_T&>()))>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(make_reverse_iterator(ranges::end(static_cast<_T&>(__t)))))
    {
        return make_reverse_iterator(ranges::end(static_cast<_T&>(__t)));
    }
};

} // namespace __detail

inline namespace __rbegin { inline constexpr __detail::__rbegin rbegin = {}; }

namespace __detail
{

template <class _T> void rend(_T&&) = delete;

struct __rend
{
    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && requires(_T& __t) { { __decay_copy(__t.rend()) } -> sentinel_for<decltype(ranges::rbegin(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(static_cast<_T&>(__t).begin())))
    {
        return __decay_copy(static_cast<_T&>(__t).begin());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __decay_copy(__t.rend()) } -> sentinel_for<decltype(ranges::rbegin(__t))>; }
            && requires(_T& __t) { { __decay_copy(rend(__t)) } -> sentinel_for<decltype(ranges::rbegin(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__decay_copy(rbegin(static_cast<_T&>(__t)))))
    {
        return __decay_copy(rbegin(static_cast<_T&>(__t)));
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __decay_copy(__t.rend()) } -> sentinel_for<decltype(ranges::rbegin(__t))>; }
            && !requires(_T& __t) { { __decay_copy(rend(__t)) } -> sentinel_for<decltype(ranges::rbegin(__t))>; }
            && requires(_T& __t)
            {
                { ranges::begin(__t) } -> bidirectional_iterator;
                { ranges::end(__t)   } -> bidirectional_iterator;
            }
            && same_as<decltype(ranges::begin(declval<_T&>())), decltype(ranges::end(declval<_T&>()))>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(make_reverse_iterator(ranges::begin(static_cast<_T&>(__t)))))
    {
        return make_reverse_iterator(ranges::begin(static_cast<_T&>(__t)));
    }
};

} // namespace __detail

inline namespace __rend { inline constexpr __detail::__rend rend = {}; }

namespace __detail
{

struct __crbegin
{
    template <class _T>
        requires (is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rbegin(static_cast<const _T&>(std::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rbegin(static_cast<const _T&>(std::forward<_T>(__t)))))
    {
        return ranges::rbegin(static_cast<const _T&>(std::forward<_T>(__t)));
    }

    template <class _T>
        requires (!is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rbegin(static_cast<const _T&&>(std::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rbegin(static_cast<const _T&&>(std::forward<_T>(__t)))))
    {
        return ranges::rbegin(static_cast<const _T&&>(std::forward<_T>(__t)));
    }
};

struct __crend
{
    template <class _T>
        requires (is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rend(static_cast<const _T&>(std::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rend(static_cast<const _T&>(std::forward<_T>(__t)))))
    {
        return ranges::rend(static_cast<const _T&>(std::forward<_T>(__t)));
    }

    template <class _T>
        requires (!is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rend(static_cast<const _T&&>(std::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rend(static_cast<const _T&&>(std::forward<_T>(__t)))))
    {
        return ranges::rend(static_cast<const _T&&>(std::forward<_T>(__t)));
    }
};

} // namespace __detail

inline namespace __crbegin { inline constexpr __detail::__crbegin crbegin = {}; }
inline namespace __crend { inline constexpr __detail::__crend crend = {}; }

} // namespace ranges


template <class _Container>
class back_insert_iterator
{
public:

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = ptrdiff_t;
    using pointer               = void;
    using reference             = void;
    using container_type        = _Container;

    constexpr back_insert_iterator() noexcept = default;

    constexpr explicit back_insert_iterator(_Container& __x) :
        _M_container(addressof(__x))
    {
    }

    constexpr back_insert_iterator& operator=(const typename _Container::value_type& __value)
    {
        _M_container->push_back(__value);
        return *this;
    }

    constexpr back_insert_iterator operator=(typename _Container::value_type&& __value)
    {
        _M_container->push_back(std::move(__value));
        return *this;
    }

    constexpr back_insert_iterator& operator*()
    {
        return *this;
    }

    constexpr back_insert_iterator& operator++()
    {
        return *this;
    }

    constexpr back_insert_iterator operator++(int)
    {
        return *this;
    }

private:

    _Container* _M_container = nullptr;
};

template <class _Container>
constexpr back_insert_iterator<_Container> back_inserter(_Container& __x)
{
    return back_insert_iterator<_Container>(__x);
}


template <class _Container>
class front_insert_iterator
{
public:

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = ptrdiff_t;
    using pointer               = void;
    using reference             = void;
    using container_type        = _Container;

    constexpr front_insert_iterator() noexcept = default;

    constexpr explicit front_insert_iterator(const _Container& __x) :
        _M_container(addressof(__x))
    {
    }

    constexpr front_insert_iterator& operator=(const typename _Container::value_type& __value)
    {
        _M_container->push_front(__value);
        return *this;
    }

    constexpr front_insert_iterator& operator=(typename _Container::value_type&& __value)
    {
        _M_container->push_front(std::move(__value));
        return *this;
    }

    constexpr front_insert_iterator& operator*()
    {
        return *this;
    }

    constexpr front_insert_iterator& operator++()
    {
        return *this;
    }

    constexpr front_insert_iterator operator++(int)
    {
        return *this;
    }

private:

    _Container* _M_container = nullptr;
};

template <class _Container>
constexpr front_insert_iterator<_Container> front_inserter(_Container& __x)
{
    return front_insert_iterator<_Container>(__x);
}


template <class _Container>
class insert_iterator
{
protected:

    _Container*                     container = nullptr;
    ranges::iterator_t<_Container>  iter = ranges::iterator_t<_Container>();

public:

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = ptrdiff_t;
    using pointer               = void;
    using reference             = void;
    using container_type        = _Container;

    insert_iterator() = default;

    constexpr insert_iterator(_Container& __x, ranges::iterator_t<_Container> __i) :
        container(addressof(__x)),
        iter(__i)
    {
    }

    constexpr insert_iterator& operator=(const typename _Container::value_type& __value)
    {
        iter = container->insert(iter, __value);
        return *this;
    }

    constexpr insert_iterator& operator=(typename _Container::value_type&& __value)
    {
        iter = container->insert(iter, std::move(__value));
        return *this;
    }

    constexpr insert_iterator& operator*()
    {
        return *this;
    }

    constexpr insert_iterator& operator++()
    {
        return *this;
    }

    constexpr insert_iterator operator++(int)
    {
        return *this;
    }
};

template <class _Container>
constexpr insert_iterator<_Container> inserter(_Container& __x, ranges::iterator_t<_Container> __i)
{
    return insert_iterator<_Container>(__x, __i);
}


template <semiregular _S>
class move_sentinel
{
public:

    constexpr move_sentinel() = default;
    constexpr move_sentinel(const move_sentinel&) = default;
    constexpr move_sentinel(move_sentinel&&) = default;

    constexpr explicit move_sentinel(_S __s) :
        _M_last(std::move(__s))
    {
    }

    template <class _S2>
        requires convertible_to<const _S2, _S>
    constexpr move_sentinel(const move_sentinel<_S2>& __s) :
        _M_last(__s._M_last)
    {
    }

    constexpr move_sentinel& operator=(const move_sentinel&) = default;
    constexpr move_sentinel& operator=(move_sentinel&&) = default;

    template <class _S2>
        requires assignable_from<_S&, const _S2&>
    constexpr move_sentinel& operator=(const move_sentinel<_S2>& __s)
    {
        _M_last = __s._M_last;
        return *this;
    }

    ~move_sentinel() = default;

    constexpr _S base() const
    {
        return _M_last;
    }

private:

    _S _M_last = {};
};

template <class _Iterator>
class move_iterator
{
public:

    using iterator_type         = _Iterator;
    using iterator_concept      = input_iterator_tag;
    using iterator_category     = conditional_t<derived_from<typename iterator_traits<_Iterator>::iterator_category, random_access_iterator_tag>,
                                                random_access_iterator_tag,
                                                typename iterator_traits<_Iterator>::iterator_category>;
    using value_type            = iter_value_t<_Iterator>;
    using difference_type       = iter_difference_t<_Iterator>;
    using pointer               = _Iterator;
    using reference             = iter_rvalue_reference_t<_Iterator>;

    constexpr move_iterator() = default;
    constexpr move_iterator(const move_iterator&) = default;
    constexpr move_iterator(move_iterator&&) = default;

    constexpr explicit move_iterator(_Iterator __i) :
        _M_current(std::move(__i))
    {
    }

    template <class _U>
    constexpr move_iterator(const move_iterator<_U>& __u) :
        _M_current(__u.base())
    {
    }

    constexpr move_iterator& operator=(const move_iterator&) = default;
    constexpr move_iterator& operator=(move_iterator&&) = default;

    template <class _U>
    constexpr move_iterator& operator=(const move_iterator<_U>& __u)
    {
        _M_current = __u.base();
        return *this;
    }

    ~move_iterator() = default;

    constexpr iterator_type base() const &
        requires copy_constructible<_Iterator>
    {
        return _M_current;
    }

    constexpr iterator_type base() &&
    {
        return std::move(_M_current);
    }

    constexpr reference operator*() const
    {
        return ranges::iter_move(_M_current);
    }

    constexpr move_iterator& operator++()
    {
        ++_M_current;
        return *this;
    }

    constexpr auto operator++(int)
    {
        if constexpr (forward_iterator<_Iterator>)
        {
            move_iterator __tmp = *this;
            ++_M_current;
            return __tmp;
        }
        else
        {
            return ++_M_current;
        }
    }

    constexpr move_iterator& operator--()
    {
        --_M_current;
        return *this;
    }

    constexpr move_iterator operator--(int)
    {
        move_iterator __tmp = *this;
        --_M_current;
        return __tmp;
    }

    constexpr move_iterator operator+(difference_type __n) const
    {
        return move_iterator(_M_current + __n);
    }

    constexpr move_iterator& operator+=(difference_type __n)
    {
        _M_current += __n;
        return *this;
    }

    constexpr move_iterator operator-(difference_type __n) const
    {
        return move_iterator(_M_current - __n);
    }

    constexpr move_iterator& operator-=(difference_type __n)
    {
        _M_current -= __n;
        return *this;
    }

    constexpr reference operator[](difference_type __n) const
    {
        return ranges::iter_move(_M_current + __n);
    }

    template <sentinel_for<_Iterator> _S>
        requires requires(const move_iterator& __x, const move_sentinel<_S>& __y)
            { { __x.base() == __y.base() } -> convertible_to<bool>; }
    friend constexpr bool operator==(const move_iterator& __x, const move_sentinel<_S>& __y)
    {
        return __x.base() == __y.base();
    }

    template <sized_sentinel_for<_Iterator> _S>
    friend constexpr iter_difference_t<_Iterator> operator-(const move_sentinel<_S>& __x, const move_iterator& __y)
    {
        return __x.base() - __y.base();
    }

    template <sized_sentinel_for<_Iterator> _S>
    friend constexpr iter_difference_t<_Iterator> operator-(const move_iterator& __x, const move_sentinel<_S>& __y)
    {
        return __x.base() - __y.base();
    }

    friend constexpr iter_rvalue_reference_t<_Iterator> iter_move(const move_iterator& __i)
        noexcept(noexcept(ranges::iter_move(__i._M_current)))
    {
        return ranges::iter_move(__i._M_current);
    }

    template <indirectly_swappable<_Iterator> _Iterator2>
    friend constexpr void iter_swap(const move_iterator& __x, const move_iterator& __y)
        noexcept(noexcept(ranges::iter_swap(__x._M_current, __y._M_current)))
    {
        ranges::iter_swap(__x._M_current, __y._M_current);
    }

private:

    _Iterator   _M_current = {};
};

template <class _Iterator1, class _Iterator2>
constexpr bool operator==(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
    requires requires { { __x.base() == __y.base() } -> convertible_to<bool>; }
{
    return __x.base() == __y.base();
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator<(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
    requires requires { { __x.base() < __y.base() } -> convertible_to<bool>; }
{
    return __x.base() < __y.base();
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator>(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
    requires requires { { __y.base() < __x.base() } -> convertible_to<bool>; }
{
    return __y < __x;
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator<=(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
    requires requires { { __y.base() < __x.base() } -> convertible_to<bool>; }
{
    return !(__y < __x);
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator>=(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
    requires requires { { __x.base() < __y.base() } -> convertible_to<bool>; }
{
    return !(__x < __y);
}

template <class _Iterator1, three_way_comparable_with<_Iterator1, weak_equality> _Iterator2>
constexpr compare_three_way_result_t<_Iterator1, _Iterator2> operator<=>(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
{
    return __x.base() <=> __y.base();
}

template <class _Iterator1, class _Iterator2>
constexpr auto operator-(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
    -> decltype(__x.base() - __y.base())
{
    return __x.base() - __y.base();
}

template <class _Iterator>
constexpr move_iterator<_Iterator> operator+(iter_difference_t<_Iterator> __n, const move_iterator<_Iterator>& __x)
    requires requires { { __x + __n } -> same_as<_Iterator>; }
{
    return __x + __n;
}

template <class _Iterator>
constexpr move_iterator<_Iterator> make_move_iterator(_Iterator __i)
{
    return move_iterator<_Iterator>(__i);
}


template <input_or_output_iterator _I, sentinel_for<_I> _S>
    requires (!same_as<_I, _S>)
class common_iterator
{
public:

    constexpr common_iterator() = default;
    constexpr common_iterator(const common_iterator&) = default;
    constexpr common_iterator(common_iterator&&) = default;

    constexpr common_iterator(_I __i) :
        _M_variant{in_place_type<_I>, std::move(__i)}
    {
    }

    constexpr common_iterator(_S __s) :
        _M_variant{in_place_type<_S>, std::move(__s)}
    {
    }

    template <class _I2, class _S2>
        requires convertible_to<const _I2&, _I> && convertible_to<const _S2&, _S>
    constexpr common_iterator(const common_iterator<_I2, _S2>& __x) :
        _M_variant{__x._M_variant.index() == 0 ? variant<_I, _S>{in_place_index<0>, get<0>(__x._M_variant)}
                                               : variant<_I, _S>{in_place_index<1>, get<1>(__x._M_variant)}}
    {
    }

    constexpr common_iterator& operator=(const common_iterator&) = default;
    constexpr common_iterator& operator=(common_iterator&&) = default;

    template <class _I2, class _S2>
        requires assignable_from<_I&, const _I2&> && assignable_from<_S&, const _S2&>
    constexpr common_iterator& operator=(const common_iterator<_I2, _S2>& __x)
    {
        if (_M_variant.index() == __x._M_variant.index())
        {
            if (_M_variant.index() == 0)
                get<0>(_M_variant) = get<0>(__x._M_variant);
            else
                get<1>(_M_variant) = get<1>(__x._M_variant);
        }
        else
        {
            if (__x._M_variant.index() == 0)
                _M_variant.emplace<0>(get<0>(__x._M_variant));
            else
                _M_variant.emplace<1>(get<1>(__x._M_variant));
        }

        return *this;
    }

    ~common_iterator() = default;

    decltype(auto) operator*()
    {
        return get<_I>(_M_variant);
    }

    decltype(auto) operator*() const
        requires __detail::__dereferencable<const _I>
    {
        return get<_I>(_M_variant);
    }

    decltype(auto) operator->() const
        requires indirectly_readable<const _I>
            && (requires(const _I& __i) { __i.operator->(); }
                || is_reference_v<iter_reference_t<_I>>
                || constructible_from<iter_value_t<_I>, iter_reference_t<_I>>)
    {
        if constexpr (is_pointer_v<_I> || requires { get<_I>(_M_variant).operator->(); })
        {
            return get<_I>(_M_variant);
        }
        else if constexpr (is_reference_v<iter_reference_t<_I>>)
        {
            auto&& __tmp = *get<_I>(_M_variant);
            return addressof(__tmp);
        }
        else
        {
            class __proxy
            {
            public:

                constexpr explicit __proxy(iter_reference_t<_I>&& __x) :
                    _M_keep(std::move(__x))
                {
                }

                constexpr const iter_value_t<_I>* operator->() const
                {
                    return addressof(_M_keep);
                }

            private:

                iter_value_t<_I> _M_keep;
            };

            return __proxy(*get<_I>(_M_variant));
        }
    }

    common_iterator& operator++()
    {
        ++get<_I>(_M_variant);
        return *this;
    }

    decltype(auto) operator++(int)
    {
        if constexpr (forward_iterator<_I>)
        {
            common_iterator __tmp = *this;
            ++*this;
            return __tmp;
        }
        else
        {
            return get<_I>(_M_variant)++;
        }
    }

    template <class _I2, sentinel_for<_I> _S2>
        requires sentinel_for<_S, _I2>
    friend bool operator==(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
    {
        if (__x._M_variant.index() == __y._M_variant.index())
            return true;

        if (__x._M_variant.index() == 0)
            return get<0>(__x._M_variant) == get<1>(__y._M_variant);
        else
            return get<1>(__x._M_variant) == get<0>(__y._M_variant);
    }

    template <class _I2, sentinel_for<_I> _S2>
        requires sentinel_for<_S, _I2> && equality_comparable_with<_I, _I2>
    friend bool operator==(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
    {
        if (__x._M_variant.index() == 1 && __y._M_variant.index() == 1)
            return true;

        if (__x._M_variant.index() == 0)
        {
            if (__y._M_variant.index() == 0)
                return get<0>(__x._M_variant) == get<0>(__y._M_variant);
            else
                return get<0>(__x._M_variant) == get<1>(__y._M_variant);
        }
        else
            return get<1>(__x._M_variant) == get<0>(__y._M_variant);
    }

    template <sized_sentinel_for<_I> _I2, sized_sentinel_for<_I> _S2>
        requires sized_sentinel_for<_S, _I2>
    friend iter_difference_t<_I2> operator-(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
    {
        if (__x._M_variant.index() == 1 && __y._M_variant.index() == 1)
            return 0;

        if (__x._M_variant.index() == 0)
        {
            if (__y._M_variant.index() == 0)
                return get<0>(__x._M_variant) - get<0>(__y._M_variant);
            else
                return get<0>(__x._M_variant) - get<1>(__y._M_variant);
        }
        else
            return get<1>(__x._M_variant) - get<0>(__y._M_variant);
    }

    friend iter_rvalue_reference_t<_I> iter_move(const common_iterator& __i)
        noexcept(noexcept(ranges::iter_move(declval<const _I&>())))
        requires input_iterator<_I>
    {
        return ranges::iter_move(get<_I>(__i._M_variant));
    }

    template <indirectly_swappable<_I> _I2, class _S2>
    friend void iter_swap(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
        noexcept(noexcept(ranges::iter_swap(declval<const _I&>(), declval<const _I2&>())))
    {
        ranges::iter_swap(get<_I>(__x._M_variant), get<_I2>(__y._M_variant));
    }

private:

    variant<_I, _S> _M_variant = {};
};

template <class _I, class _S>
struct incrementable_traits<common_iterator<_I, _S>> { using difference_type = iter_difference_t<_I>; };

template <input_iterator _I, class _S>
struct iterator_traits<common_iterator<_I, _S>>
{
    template <class _I2, class _S2>
    struct __pointer_helper { using __type = void; };

    template <class _I2, class _S2>
        requires requires(const common_iterator<_I2, _S2>& __a) { __a.operator->(); }
    struct __pointer_helper<_I2, _S2> { using __type = decltype(declval<const common_iterator<_I2, _S2>&>().operator->()); };
    
    using iterator_concept      = conditional_t<forward_iterator<_I>, forward_iterator_tag, input_iterator_tag>;
    using iterator_category     = conditional_t<derived_from<typename iterator_traits<_I>::iterator_category, forward_iterator_tag>,
                                                forward_iterator_tag,
                                                input_iterator_tag>;
    using value_type            = iter_value_t<_I>;
    using difference_type       = iter_difference_t<_I>;
    using pointer               = typename __pointer_helper<_I, _S>::__type;
    using reference             = iter_reference_t<_I>;
};


struct default_sentinel_t {};

inline constexpr default_sentinel_t default_sentinel = {};


template <input_or_output_iterator _I>
class counted_iterator
{
public:

    using iterator_type = _I;

    constexpr counted_iterator() = default;
    constexpr counted_iterator(const counted_iterator&) = default;
    constexpr counted_iterator(counted_iterator&&) = default;

    constexpr counted_iterator(_I __x, iter_difference_t<_I> __n) :
        _M_current(__x),
        _M_length(__n)
    {
    }

    template <class _I2>
        requires convertible_to<const _I2&, _I>
    constexpr counted_iterator(const counted_iterator<_I2>& __x) :
        _M_current(__x._M_current),
        _M_length(__x._M_length)
    {
    }

    constexpr counted_iterator& operator=(const counted_iterator&) = default;
    constexpr counted_iterator& operator=(counted_iterator&&) = default;

    template <class _I2>
        requires assignable_from<_I&, const _I2&>
    constexpr counted_iterator& operator=(const counted_iterator<_I2>& __x)
    {
        _M_current = __x._M_current;
        _M_length = __x._M_length;
    }

    constexpr _I base() const &
        requires copy_constructible<_I>
    {
        return _M_current;
    }

    constexpr _I base() &&
    {
        return std::move(_M_current);
    }

    constexpr iter_difference_t<_I> count() const noexcept
    {
        return _M_length;
    }

    constexpr decltype(auto) operator*()
    {
        return *_M_current;
    }

    constexpr decltype(auto) operator*() const
        requires __detail::__dereferencable<const _I>
    {
        return *_M_current;
    }

    constexpr counted_iterator& operator++()
    {
        ++_M_current;
        --_M_length;
        return *this;
    }

    decltype(auto) operator++(int)
    {
        --_M_length;
        __XVI_CXX_UTILITY_TRY
        {
            return _M_current++;
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            ++_M_length;
            __XVI_CXX_UTILITY_RETHROW;
        }
    }

    constexpr counted_iterator operator++(int)
        requires forward_iterator<_I>
    {
        counted_iterator __tmp = *this;
        ++*this;
        return __tmp;
    }

    constexpr counted_iterator& operator--()
        requires bidirectional_iterator<_I>
    {
        --_M_current;
        ++_M_length;
        return *this;
    }

    constexpr counted_iterator operator--(int)
        requires bidirectional_iterator<_I>
    {
        counted_iterator __tmp = *this;
        --*this;
        return __tmp;
    }

    constexpr counted_iterator operator+(iter_difference_t<_I> __n) const
        requires random_access_iterator<_I>
    {
        return counted_iterator(_M_current + __n, _M_length - __n);
    }

    friend constexpr counted_iterator operator+(iter_difference_t<_I> __n, const counted_iterator& __x)
        requires random_access_iterator<_I>
    {
        return __x + __n;
    }

    constexpr counted_iterator& operator+=(iter_difference_t<_I> __n)
        requires random_access_iterator<_I>
    {
        _M_current += __n;
        _M_length -= __n;
        return *this;
    }

    constexpr counted_iterator operator-(iter_difference_t<_I> __n) const
        requires random_access_iterator<_I>
    {
        return counted_iterator(_M_current - __n, _M_length + __n);
    }

    template <common_with<_I> _I2>
    friend constexpr iter_difference_t<_I2> operator-(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __y._M_length - __x._M_length;
    }

    friend constexpr iter_difference_t<_I> operator-(const counted_iterator& __x, default_sentinel_t)
    {
        return -__x._M_length;
    }

    friend constexpr iter_difference_t<_I> operator-(default_sentinel_t, const counted_iterator& __y)
    {
        return __y._M_length;
    }

    constexpr counted_iterator& operator-=(iter_difference_t<_I> __n)
        requires random_access_iterator<_I>
    {
        _M_current -= __n;
        _M_length += __n;
        return *this;
    }

    constexpr decltype(auto) operator[](iter_difference_t<_I> __n) const
        requires random_access_iterator<_I>
    {
        return _M_current[__n];
    }

    template <common_with<_I> _I2>
    friend constexpr bool operator==(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __x._M_length == __y._M_length;
    }

    friend constexpr bool operator==(const counted_iterator& __x, default_sentinel_t)
    {
        return __x._M_length == 0;
    }

    template <common_with<_I> _I2>
    friend constexpr strong_ordering operator<=>(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __y._M_length <=> __x._M_length;
    }

    friend constexpr iter_rvalue_reference_t<_I> iter_move(const counted_iterator& __i)
        noexcept(noexcept(ranges::iter_move(__i._M_current)))
    {
        return ranges::iter_move(__i._M_current);
    }

    template <indirectly_swappable<_I> _I2>
    friend constexpr void iter_swap(const counted_iterator& __x, const counted_iterator<_I2>& __y)
        noexcept(noexcept(ranges::iter_swap(__x._M_current, __y._M_current)))
    {
        ranges::iter_swap(__x._M_current, __y._M_current);
    }

private:

    _I                      _M_current = {};
    iter_difference_t<_I>   _M_length = 0;
};

template <class _I>
struct incrementable_traits<counted_iterator<_I>>
{
    using difference_type = iter_difference_t<_I>;
};

template <input_iterator _I>
struct iterator_traits<counted_iterator<_I>> : iterator_traits<_I>
{
    using pointer = void;
};


struct unreachable_sentinel_t
{
    template <weakly_incrementable _I>
    friend constexpr bool operator==(unreachable_sentinel_t, const _I&) noexcept
    {
        return false;
    }
};

inline constexpr unreachable_sentinel_t unreachable_sentinel = {};


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


template <class _C> constexpr auto begin(_C& __c) -> decltype(__c.begin())
    { return __c.begin(); }
template <class _C> constexpr auto begin(const _C& __c) -> decltype(__c.begin())
    { return __c.begin(); }
template <class _C> constexpr auto end(_C& __c) -> decltype(__c.end())
    { return __c.end(); }
template <class _C> constexpr auto end(const _C& __c) -> decltype(__c.end())
    { return __c.end(); }
template <class _T, size_t _N> constexpr _T* begin(_T (&__array)[_N]) noexcept
    { return __array; }
template <class _T, size_t _N> constexpr _T* end(_T (&__array)[_N]) noexcept
    { return __array + _N; }
template <class _C> constexpr auto cbegin(const _C& __c) noexcept(noexcept(__XVI_STD_NS::begin(__c))) -> decltype(__XVI_STD_NS::begin(__c))
    { return __XVI_STD_NS::begin(__c); }
template <class _C> constexpr auto cend(const _C& __c) noexcept(noexcept(__XVI_STD_NS::end(__c))) -> decltype(__XVI_STD_NS::end(__c))
    { return __XVI_STD_NS::end(__c); }
template <class _C> constexpr auto rbegin(_C& __c) -> decltype(__c.rbegin())
    { return __c.rbegin(); }
template <class _C> constexpr auto rbegin(const _C& __c) -> decltype(__c.rbegin())
    { return __c.rbegin(); }
template <class _C> constexpr auto rend(_C& __c) -> decltype(__c.rend())
    { return __c.rend(); }
template <class _C> constexpr auto rend(const _C& __c) -> decltype(__c.rend())
    { return __c.rend(); }
template <class _T, size_t _N> constexpr reverse_iterator<_T*> rbegin(_T (&__array)[_N])
    { return reverse_iterator<_T*>(__array + _N); }
template <class _T, size_t _N> constexpr reverse_iterator<_T*> rend(_T (&__array)[_N])
    { return reverse_iterator<_T*>(__array); }
template <class _E> constexpr reverse_iterator<const _E*> rbegin(initializer_list<_E> __il)
    { return reverse_iterator<const _E*>(__il.end()); }
template <class _E> constexpr reverse_iterator<const _E*> rend(initializer_list<_E> __il)
    { return reverse_iterator<const _E*>(__il.begin()); }
template <class _C> constexpr auto crbegin(const _C& __c) -> decltype(__XVI_STD_NS::rbegin(__c))
    { return __XVI_STD_NS::rbegin(__c); }
template <class _C> constexpr auto crend(const _C& __c) -> decltype(__XVI_STD_NS::rend(__c))
    { return __XVI_STD_NS::rend(__c); }


template <class _C> constexpr auto size(const _C& __c) -> decltype(__c.size())
    { return __c.size(); }
template <class _T, size_t _N> constexpr size_t size(const _T (&)[_N]) noexcept
    { return _N; }
template <class _C> [[nodiscard]] constexpr auto empty(const _C& __c) -> decltype(__c.empty())
    { return __c.empty();}
template <class _T, size_t _N> [[nodiscard]] constexpr bool empty(const _T (&)[_N]) noexcept
    { return false; }
template <class _E> [[nodiscard]] constexpr bool empty(initializer_list<_E> __il) noexcept
    { return __il.size() == 0; }
template <class _C> constexpr auto data(_C& __c) -> decltype(__c.data())
    { return __c.data(); }
template <class _C> constexpr auto data(const _C& __c) -> decltype(__c.data())
    { return __c.data(); }
template <class _T, size_t _N> constexpr _T* data(_T (&__array)[_N]) noexcept
    { return __array; }
template <class _E> constexpr const _E* data(initializer_list<_E> __il) noexcept
    { return __il.begin(); }

} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ITERATOR_H */
