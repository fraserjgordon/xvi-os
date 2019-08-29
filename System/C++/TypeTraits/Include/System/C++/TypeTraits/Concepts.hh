#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONCEPTS_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONCEPTS_H


#include <System/C++/TypeTraits/TypeTraits.hh>
#include <System/C++/TypeTraits/Private/Namespace.hh>


namespace __XVI_STD_TYPETRAITS_NS
{


namespace ranges
{

template <class _E1, class _E2> void swap(_E1&, _E2&);

} // namespace ranges


template <class _T, class _U>
concept bool Same = is_same_v<_T, _U>;

template <class _Derived, class _Base>
concept bool DerivedFrom = is_base_of_v<_Base, _Derived> && is_convertible_v<const volatile _Derived*, const volatile _Base*>;

template <class _From, class _To>
concept bool ConvertibleTo = is_convertible_v<_From, _To>
    && requires(_From (&f)()) { static_cast<_To>(f()); };

template <class _T, class _U>
concept bool CommonReference = Same<common_reference_t<_T, _U>, common_reference_t<_U, _T>>
    && ConvertibleTo<_T, common_reference_t<_T, _U>>
    && ConvertibleTo<_U, common_reference_t<_T, _U>>;

template <class _T, class _U>
concept bool Common = Same<common_type_t<_T, _U>, common_type_t<_U, _T>>
    && requires
    {
        static_cast<common_type_t<_T, _U>>(declval<_T>());
        static_cast<common_type_t<_T, _U>>(declval<_U>());
    }
    && CommonReference<add_lvalue_reference_t<common_type<_T, _U>>,
                       common_reference_t<add_lvalue_reference_t<const _T>,
                                          add_lvalue_reference_t<const _U>>>;

template <class _T>
concept bool Integral = is_integral<_T>::value;

template <class _T>
concept bool SignedIntegral = Integral<_T> && is_signed_v<_T>;

template <class _T>
concept bool UnsignedIntegral = Integral<_T> && !SignedIntegral<_T>;

template <class _LHS, class _RHS>
concept bool Assignable = is_lvalue_reference_v<_LHS>
    && CommonReference<const remove_reference_t<_LHS>&, const remove_reference_t<_RHS>&>
    && requires(_LHS __lhs, _RHS&& __rhs)
    {
        __lhs = __XVI_STD_NS::forward<_RHS>(__rhs);
        requires Same<decltype(__lhs = __XVI_STD_NS::forward<_RHS>(__rhs)), _LHS>;
        //{ __lhs = __XVI_STD_NS::forward<_RHS>(__rhs) } -> Same<_LHS>;
    };

template <class _T>
concept bool Swappable = requires(_T& __a, _T& __b) { ranges::swap(__a, __b); };

template <class _T, class _U>
concept bool SwappableWith = CommonReference<const remove_reference_t<_T>&, const remove_reference_t<_U>&>
    && requires(_T&& __t, _U&& __u)
    {
        ranges::swap(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_T>(__t));
        ranges::swap(__XVI_STD_NS::forward<_U>(__u), __XVI_STD_NS::forward<_U>(__u));
        ranges::swap(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_U>(__u));
        ranges::swap(__XVI_STD_NS::forward<_U>(__u), __XVI_STD_NS::forward<_T>(__t));
    };

template <class _T>
concept bool Destructible = is_nothrow_destructible_v<_T>;

template <class _T, class... _Args>
concept bool Constructible = Destructible<_T> && is_constructible_v<_T, _Args...>;

template <class _T>
concept bool DefaultConstructible = Constructible<_T>;

template <class _T>
concept bool MoveConstructible = Constructible<_T, _T> && ConvertibleTo<_T, _T>;

template <class _T>
concept bool CopyConstructible = MoveConstructible<_T>
    && Constructible<_T, _T&> && ConvertibleTo<_T&, _T>
    && Constructible<_T, const _T&> && ConvertibleTo<const _T&, _T>
    && Constructible<_T, const _T> && ConvertibleTo<const _T, _T>;

template <class _T>
concept bool Movable = is_object_v<_T> && MoveConstructible<_T> && Assignable<_T&, _T> && Swappable<_T>;

template <class _T>
concept bool Copyable = CopyConstructible<_T> && Movable<_T> && Assignable<_T&, const _T&>;

template <class _B>
concept bool Boolean = Movable<remove_cvref_t<_B>>
    && requires(const remove_reference_t<_B>& __b1, const remove_reference_t<_B>& __b2, const bool __a)
    {
        requires ConvertibleTo<const remove_reference_t<_B>&, bool>;
        { !__b1 } -> ConvertibleTo<bool>;
        { __b1 &&  __a } -> Same<bool>;
        { __b1 ||  __a } -> Same<bool>;
        { __b1 && __b2 } -> Same<bool>;
        {  __a && __b2 } -> Same<bool>;
        { __b1 || __b2 } -> Same<bool>;
        {  __a || __b2 } -> Same<bool>;
        { __b1 == __b2 } -> ConvertibleTo<bool>;
        { __b1 ==  __a } -> ConvertibleTo<bool>;
        {  __a == __b2 } -> ConvertibleTo<bool>;
        { __b1 != __b2 } -> ConvertibleTo<bool>;
        { __b1 !=  __a } -> ConvertibleTo<bool>;
        {  __a != __b2 } -> ConvertibleTo<bool>;
    };

template <class _T, class _U>
concept bool _WeaklyEqualityComparableWith =
    requires(const remove_reference_t<_T>& __t, const remove_reference_t<_U>& __u)
    {
        { __t == __u } -> Boolean;
        { __t != __u } -> Boolean;
        { __u == __t } -> Boolean;
        { __u != __t } -> Boolean;
    };

template <class _T>
concept bool EqualityComparable = _WeaklyEqualityComparableWith<_T, _T>;

template <class _T, class _U>
concept bool EqualityComparableWith = EqualityComparable<_T> && EqualityComparable<_U>
    && CommonReference<const remove_reference_t<_T>&, const remove_reference_t<_U>&>
    && EqualityComparable<common_reference_t<const remove_reference_t<_T>&,
                                             const remove_reference_t<_U>&>>
    && _WeaklyEqualityComparableWith<_T, _U>;

template <class _T>
concept bool StrictTotallyOrdered = EqualityComparable<_T>
    && requires(const remove_reference_t<_T>& __a, const remove_reference_t<_T>& __b)
    {
        { __a <  __b } -> Boolean;
        { __a >  __b } -> Boolean;
        { __a <= __b } -> Boolean;
        { __a >= __b } -> Boolean;
    };

template <class _T, class _U>
concept bool StrictTotallyOrderedWith = StrictTotallyOrdered<_T> && StrictTotallyOrdered<_U>
    && CommonReference<const remove_reference_t<_T>&, const remove_reference_t<_T>&>
    && StrictTotallyOrdered<common_reference_t<const remove_reference_t<_T>&,
                                               const remove_reference_t<_U>&>>
    && EqualityComparableWith<_T, _U>
    && requires(const remove_reference_t<_T>& __t, const remove_reference_t<_U>& __u)
    {
        { __t <  __u } -> Boolean;
        { __t >  __u } -> Boolean;
        { __t <= __u } -> Boolean;
        { __t >= __u } -> Boolean;
        { __u <  __t } -> Boolean;
        { __u >  __t } -> Boolean;
        { __u <= __t } -> Boolean;
        { __u >= __t } -> Boolean;
    };

template <class _T>
concept bool Semiregular = Copyable<_T> && DefaultConstructible<_T>;

template <class _T>
concept bool Regular = Semiregular<_T> && EqualityComparable<_T>;

template <class _F, class... _Args>
concept bool Invocable =
    requires(_F&& __f, _Args&&... __args)
    {
        invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::forward<_Args>(__args)...);
    };

template <class _F, class... _Args>
concept bool RegularInvocable = Invocable<_F, _Args...>;

template <class _F, class... _Args>
concept bool Predicate = RegularInvocable<_F, _Args...> && Boolean<invoke_result_t<_F, _Args...>>;

template <class _R, class _T, class _U>
concept bool Relation = Predicate<_R, _T, _T> && Predicate<_R, _U, _U> && Predicate<_R, _T, _U> && Predicate<_R, _U, _T>;

template <class _R, class _T, class _U>
concept bool StrictWeakOrder = Predicate<_R, _T, _U>;


} // namespace __XVI_STD_TYPETRAITS_NS


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_CONCEPTS_H */
