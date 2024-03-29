#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONCEPTS_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONCEPTS_H


#include <System/C++/TypeTraits/TypeTraits.hh>
#include <System/C++/TypeTraits/Private/Namespace.hh>


// The 'swappable' and 'swappable_with' concepts are not defined here as they depend on ranges::swap.

namespace __XVI_STD_TYPETRAITS_NS_DECL
{


// We need to forward-declare this for the 'swappable' and 'swappable_with' concepts.
namespace ranges::__detail
{

struct __swap;

} // namespace ranges


namespace __detail
{

template <class _T, class _U>
concept __same_as_impl = is_same_v<_T, _U>;

} // namespace __detail


template <class _T, class _U>
concept same_as = __detail::__same_as_impl<_T, _U> && __detail::__same_as_impl<_U, _T>;

template <class _Derived, class _Base>
concept derived_from = is_base_of_v<_Base, _Derived> && is_convertible_v<const volatile _Derived*, const volatile _Base*>;

template <class _From, class _To>
concept convertible_to = is_convertible_v<_From, _To>
    && requires
    {
        static_cast<_To>(declval<_From>());
    };

template <class _T, class _U>
concept common_reference_with = same_as<common_reference_t<_T, _U>, common_reference_t<_U, _T>>
    && convertible_to<_T, common_reference_t<_T, _U>>
    && convertible_to<_U, common_reference_t<_T, _U>>;

template <class _T, class _U>
concept common_with = same_as<common_type_t<_T, _U>, common_type_t<_U, _T>>
    && requires
    {
        static_cast<common_type_t<_T, _U>>(declval<_T>());
        static_cast<common_type_t<_T, _U>>(declval<_U>());
    }
    && common_reference_with<add_lvalue_reference_t<const _T>,
                             add_lvalue_reference_t<const _U>>
    && common_reference_with<add_lvalue_reference_t<common_type_t<_T, _U>>,
                             common_reference_t<add_lvalue_reference_t<const _T>,
                                                add_lvalue_reference_t<const _U>>>;

template <class _T>
concept integral = is_integral_v<_T>;

template <class _T>
concept signed_integral = integral<_T> && is_signed_v<_T>;

template <class _T>
concept unsigned_integral = integral<_T> && !signed_integral<_T>;

template <class _T>
concept floating_point = is_floating_point_v<_T>;

template <class _LHS, class _RHS>
concept assignable_from = is_lvalue_reference_v<_LHS>
    && common_reference_with<const remove_reference_t<_LHS>&, const remove_reference_t<_RHS>&>
    && requires(_LHS __lhs, _RHS&& __rhs)
    {
        { __lhs = __XVI_STD_NS::forward<_RHS>(__rhs) } -> same_as<_LHS>;
    };

template <class _T>
concept swappable = requires(_T& __a, _T& __b) { declval<ranges::__detail::__swap>()(__a, __b); };

template <class _T, class _U>
concept swappable_with = common_reference_with<const remove_reference_t<_T>&, const remove_reference_t<_U>&>
    && requires(_T&& __t, _U&& __u)
    {
        declval<ranges::__detail::__swap>()(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_T>(__t));
        declval<ranges::__detail::__swap>()(__XVI_STD_NS::forward<_U>(__u), __XVI_STD_NS::forward<_U>(__u));
        declval<ranges::__detail::__swap>()(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_U>(__u));
        declval<ranges::__detail::__swap>()(__XVI_STD_NS::forward<_U>(__u), __XVI_STD_NS::forward<_T>(__t));
    };

template <class _T>
concept destructible = is_nothrow_destructible_v<_T>;

template <class _T, class... _Args>
concept constructible_from = destructible<_T> && is_constructible_v<_T, _Args...>;

template <class _T>
concept default_initializable = constructible_from<_T>
    && requires
    {
        _T{};

        // The spec requires the constraint to be that the variable definition 'T t;' would be valid. But we can only
        // test an expression here, so use the 'new' operator to make it an expression.
        new _T;
    };

template <class _T>
concept move_constructible = constructible_from<_T, _T> && convertible_to<_T, _T>;

template <class _T>
concept copy_constructible = move_constructible<_T>
    && constructible_from<_T, _T&> && convertible_to<_T&, _T>
    && constructible_from<_T, const _T&> && convertible_to<const _T&, _T>
    && constructible_from<_T, const _T> && convertible_to<const _T, _T>;


namespace __detail
{


template <class _T>
concept __boolean_testable_impl = convertible_to<_T, bool>;

template <class _T>
concept __boolean_testable = __boolean_testable_impl<_T>
    && requires (_T&& __t)
    {
        { !__XVI_STD_NS::forward<_T>(__t) } -> __boolean_testable_impl;
    };

template <class _T, class _U, class _C = common_reference_t<const _T&, const _U&>>
concept __comparison_common_type_with_impl =
    same_as<common_reference_t<const _T&, const _U&>, common_reference_t<const _U&, const _T&>> &&
    requires
    {
        requires convertible_to<const _T&, const _C&> || convertible_to<_T, const _C&>;
        requires convertible_to<const _U&, const _C&> || convertible_to<_U, const _C&>;
    };

template <class _T, class _U>
concept __comparison_common_type_with = __comparison_common_type_with_impl<remove_cvref_t<_T>, remove_cvref_t<_U>>;

template <class _T, class _U>
concept __weakly_equality_comparable_with =
    requires(const remove_reference_t<_T>& __t, const remove_reference_t<_U>& __u)
    {
        { __t == __u } -> __boolean_testable;
        { __t != __u } -> __boolean_testable;
        { __u == __t } -> __boolean_testable;
        { __u != __t } -> __boolean_testable;
    };

template <class _T, class _U>
concept __partially_ordered_with = requires(const remove_reference_t<_T>& __t, const remove_reference_t<_U>& __u)
{
    { __t <  __u } -> __boolean_testable;
    { __t >  __u } -> __boolean_testable;
    { __t <= __u } -> __boolean_testable;
    { __t >= __u } -> __boolean_testable;
    { __u <  __t } -> __boolean_testable;
    { __u >  __t } -> __boolean_testable;
    { __u <= __t } -> __boolean_testable;
    { __u >= __t } -> __boolean_testable;
};

} // namespace __detail

template <class _T>
concept equality_comparable = __detail::__weakly_equality_comparable_with<_T, _T>;

template <class _T, class _U>
concept equality_comparable_with = equality_comparable<_T> && equality_comparable<_U>
    && __detail::__comparison_common_type_with<_T, _U>
    && equality_comparable<common_reference_t<const remove_reference_t<_T>&,
                                              const remove_reference_t<_U>&>>
    && __detail::__weakly_equality_comparable_with<_T, _U>;

template <class _T>
concept totally_ordered = equality_comparable<_T> && __detail::__partially_ordered_with<_T, _T>;

template <class _T, class _U>
concept totally_ordered_with = totally_ordered<_T> && totally_ordered<_U>
    && equality_comparable_with<_T, _U>
    && totally_ordered<common_reference_t<const remove_reference_t<_T>&,
                                          const remove_reference_t<_U>&>>
    && __detail::__partially_ordered_with<_T, _U>;

template <class _T>
concept movable = is_object_v<_T> && move_constructible<_T> && assignable_from<_T&, _T> && swappable<_T>;

template <class _T>
concept copyable = copy_constructible<_T> && movable<_T> && assignable_from<_T&, _T&> && assignable_from<_T&, const _T&> && assignable_from<_T&, const _T>;

template <class _T>
concept semiregular = copyable<_T> && default_initializable<_T>;

template <class _T>
concept regular = semiregular<_T> && equality_comparable<_T>;

template <class _F, class... _Args>
concept invocable = is_invocable_v<_F, _Args...>;

template <class _F, class... _Args>
concept regular_invocable = invocable<_F, _Args...>;

template <class _F, class... _Args>
concept predicate = regular_invocable<_F, _Args...> && __detail::__boolean_testable<invoke_result_t<_F, _Args...>>;

template <class _R, class _T, class _U>
concept relation = predicate<_R, _T, _T> && predicate<_R, _U, _U> && predicate<_R, _T, _U> && predicate<_R, _U, _T>;

template <class _R, class _T, class _U>
concept equivalence_relation = relation<_R, _T, _U>;

template <class _R, class _T, class _U>
concept strict_weak_order = relation<_R, _T, _U>;


} // namespace __XVI_STD_TYPETRAITS_NS_DECL


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONCEPTS_H */
