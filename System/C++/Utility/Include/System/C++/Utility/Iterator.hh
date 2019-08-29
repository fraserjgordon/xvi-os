#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ITERATOR_H
#define __SYSTEM_CXX_UTILITY_ITERATOR_H


#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/Variant.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class> struct iterator_traits;


struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
struct contiguous_iterator_tag : public random_access_iterator_tag {};

/*
namespace __detail
{

template <class _T> struct __is_primary_iterator_traits : false_type {};

template <class _T>
    requires requires { typename iterator_traits<_T>::__is_primary_template; }
struct __is_primary_iterator_traits<_T> : true_type {};

template <class _T> inline constexpr bool __is_primary_iterator_traits_v =__is_primary_iterator_traits<_T>::value;

template <class _A>
using __operator_arrow_detector = decltype(declval<_A&>().operator->());

} // namespace __detail


template <class _T> using __with_reference = _T&;

template <class _T>
concept bool _CanReference = requires { typename __with_reference<_T>; };

template <class _T>
concept bool _Dereferenceable =
    requires(_T& __t)
    {
        { *__t } -> _CanReference;
    };


template <class> struct incrementable_traits {};

template <class _T>
    requires is_object_v<_T>
struct incrementable_traits<_T*>
{
    using difference_type = ptrdiff_t;
};

template <class _I>
struct incrementable_traits<const _I>
    : incrementable_traits<_I> {};

template <class _T>
    requires requires { typename _T::difference_type; }
struct incrementable_traits<_T>
{
    using difference_type = typename _T::difference_type;
};

template <class _T>
    requires (!requires { typename _T::difference_type; }
        && requires(const _T& __a, const _T& __b) { { __a - __b } -> Integral; })
struct incrementable_traits<_T>
{
    using difference_type = make_signed_t<decltype(declval<_T>() - declval<_T>())>;
};

template <class _T>
using iter_difference_t = typename conditional_t
<
    __detail::__is_primary_iterator_traits_v<_T>,
    incrementable_traits<_T>,
    iterator_traits<_T>
>::difference_type;


template <class> struct __cond_value_type {};
template <class _T>
    requires is_object_v<_T>
struct __cond_value_type<_T>
{
    using value_type = remove_cv_t<_T>;
};

template <class> struct readable_traits {};

template <class _T>
struct readable_traits<_T*> : __cond_value_type<_T> {};

template <class _I>
    requires is_array_v<_I>
struct readable_traits<_I>
{
    using value_type = remove_cv_t<remove_extent_t<_I>>;
};

template <class _I>
struct readable_traits<const _I> : readable_traits<_I> {};

template <class _T>
    requires requires { typename _T::value_type; }
struct readable_traits<_T> : __cond_value_type<typename _T::value_type> {};

template <class _T>
    requires requires { typename _T::element_type; }
struct readable_traits<_T> : __cond_value_type<typename _T::element_type> {};

template <class _T> using iter_value_t = typename conditional_t
<
    __detail::__is_primary_iterator_traits_v<_T>,
    readable_traits<_T>,
    iterator_traits<_T>
>::value_type;


template <_Dereferenceable _T> using iter_reference_t = decltype(*declval<_T&>());


template <class _I>
concept bool _Cpp17Iterator = Copyable<_I>
    && requires(_I __i)
    {
        {   *__i } -> _CanReference;
        {  ++__i } -> _CanReference;
        { *__i++ } -> _CanReference;
    };

template <class _I>
concept bool _Cpp17InputIterator = _Cpp17Iterator<_I> && EqualityComparable<_I>
    && requires(_I __i)
    {
        typename incrementable_traits<_I>::difference_type;
        typename readable_traits<_I>::value_type;
        typename common_reference_t<iter_reference_t<_I>&&, typename readable_traits<_I>::value_type&>;
        *__i++;
        typename common_reference_t<decltype(*__i++), typename readable_traits<_I>::value_type&>;
        requires SignedIntegral<typename incrementable_traits<_I>::difference_type>;
    };

template <class _I>
concept bool _Cpp17ForwardIterator = _Cpp17InputIterator<_I>
    && Constructible<_I>
    && is_lvalue_reference_v<iter_reference_t<_I>>
    && Same<remove_cvref_t<iter_reference_t<_I>>, typename readable_traits<_I>::value_type>
    && requires(_I __i)
    {
        {  __i++ } -> const _I&;
        { *__i++ } -> Same<iter_reference_t<_I>>;
    };

template <class _I>
concept bool _Cpp17BidirectionalIterator = _Cpp17ForwardIterator<_I>
    && requires(_I __i)
    {
        {  --__i } -> Same<_I&>;
        {  __i-- } -> const _I&;
        { *__i-- } -> Same<iter_reference_t<_I>>;
    };

template <class _I>
concept bool _Cpp17RandomAccessIterator = _Cpp17BidirectionalIterator<_I>
    && StrictTotallyOrdered<_I>
    && requires(_I __i, typename incrementable_traits<_I>::difference_type __n)
    {
        { __i += __n } -> Same<_I&>;
        { __i -= __n } -> Same<_I&>;
        { __i +  __n } -> Same<_I>;
        { __n +  __i } -> Same<_I>;
        { __i -  __n } -> Same<_I>;
        { __i -  __i } -> Same<decltype(__n)>;
        {  __i[__n]  } -> iter_reference_t<_I>;
    };


template <class> struct iterator_traits
{
    using __is_primary_iterator_traits = void;
};

template <class _I>
    requires requires
    {
        typename _I::difference_type;
        typename _I::value_type;
        typename _I::reference;
        typename _I::iterator_category;
    }
struct iterator_traits<_I>
{
    using iterator_category     = typename _I::iterator_category;
    using value_type            = typename _I::value_type;
    using difference_type       = typename _I::difference_type;
    using reference             = typename _I::reference;

    template <class _J> struct __pointer
        { using pointer = void; };
    template <class _J> requires requires { typename _J::pointer; }
        struct __pointer<_J>
            { using pointer = typename _J::pointer; };

    using pointer               = typename __pointer<_I>::pointer;

    using __is_primary_iterator_traits = void;
};

template <class _I>
    requires _Cpp17InputIterator<_I>
struct iterator_traits<_I>
{
    using value_type            = typename readable_traits<_I>::value_type;
    using difference_type       = typename incrementable_traits<_I>::value_type;

    template <class _J> struct __pointer
        { using pointer = void; };
    template <class _J> requires requires { typename _J::pointer; }
        struct __pointer<_J>
            { using pointer = typename _J::pointer; };
    template <class _J> requires requires { { declval<_J&>().operator->() }; }
        struct __pointer<_J>
            { using pointer = decltype(declval<_J&>().operator->()); };

    using pointer               = typename __pointer<_I>::pointer;

    template <class _J> struct __reference
        { using reference = iter_reference_t<_J>; };
    template <class _J> requires requires { typename _J::reference; }
        struct __reference<_J>
            { using reference = typename _J::reference; };

    using reference             = typename __reference<_I>::reference;

    template <class _J> struct __iterator_category
        { using iterator_category = input_iterator_tag; };
    template <class _J> requires _Cpp17RandomAccessIterator<_J>
        struct __iterator_category<_J>
            { using iterator_category = random_access_iterator_tag; };
    template <class _J> requires _Cpp17BidirectionalIterator<_J>
        struct __iterator_category<_J>
            { using iterator_category = bidirectional_iterator_tag; };
    template <class _J> requires _Cpp17ForwardIterator<_J>
        struct __iterator_category<_J>
            { using iterator_category = forward_iterator_tag; };
    
    using iterator_category     = typename __iterator_category<_I>::iterator_category;

    using __is_primary_iterator_traits = void;
};

template <class _I>
    requires _Cpp17Iterator<_I>
struct iterator_traits<_I>
{
    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using pointer               = void;
    using reference             = void;

    template <class _J> struct __difference_type
        { using difference_type = void; };
    template <class _J> requires requires { typename _J::difference_type; }
        struct __difference_type<_J>
            { using difference_type = typename _J::difference_type; };

    using difference_type       = typename __difference_type<_I>::difference_type;

    using __is_primary_iterator_traits = void;
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

inline namespace __iter
{

template <class _E>
concept bool _HasIterMove = requires(_E&& __e)
{
    { iter_move(declval<_E>()) };
};

template <class _E>
concept bool _HasIterMoveFallback = !_HasIterMove<_E>
    && requires(_E&& __e)
    {
        { __XVI_STD_NS::move(*__XVI_STD_NS::forward<_E>(__e)) };
    };

struct __iter_move
{
    template <class _E> requires _HasIterMove<_E>
    constexpr decltype(auto) operator()(_E&& __e) const
        noexcept(noexcept(iter_move(__XVI_STD_NS::forward<_E>(__e))))
    {
        return iter_move(__XVI_STD_NS::forward<_E>(__e));
    }

    template <class _E> requires _HasIterMoveFallback<_E>
    constexpr decltype(auto) operator()(_E&& __e) const
        noexcept(noexcept(__XVI_STD_NS::move(__e)))
    {
        return __XVI_STD_NS::move(*__XVI_STD_NS::forward<_E>(__e));
    }
};

inline constexpr __iter_move iter_move = {};

} // namespace __iter

} // namespace ranges


namespace __detail
{

template <class _I>
using _ITER_TRAITS = conditional_t<__is_primary_iterator_traits_v<_I>, _I, iterator_traits<_I>>;

template <class _I> struct __iterator_concept {};

template <class _I>
concept bool _HasIteratorConcept = requires { typename _ITER_TRAITS<_I>::iterator_concept; };

template <class _I>
concept bool _HasIteratorCategory = requires { typename _ITER_TRAITS<_I>::iterator_category; };

template <class _I> requires _HasIteratorConcept<_I>
    struct __iterator_concept<_I> { using iterator_concept = typename _ITER_TRAITS<_I>::iterator_concept; };
template <class _I> requires !_HasIteratorConcept<_I> && _HasIteratorCategory<_I>
    struct __iterator_concept<_I> { using iterator_concept = typename _ITER_TRAITS<_I>::iterator_category; };
template <class _I> requires !_HasIteratorConcept<_I> && !_HasIteratorCategory<_I> && __is_primary_iterator_traits_v<_I>
    struct __iterator_concept<_I> { using iterator_concept = random_access_iterator_tag; };

template <class _I>
using _ITER_CONCEPT = typename __iterator_concept<_I>::iterator_concept;

} // namespace __detail


template <_Dereferenceable _T>
    requires requires(_T& __t)
    {
        { ranges::iter_move(__t) } -> _CanReference;
    }
using iter_rvalue_reference_t = decltype(ranges::iter_move(declval<_T&>()));


template <class _In>
concept bool Readable = requires
{
    typename iter_value_t<_In>;
    typename iter_reference_t<_In>;
    typename iter_rvalue_reference_t<_In>;
}
    && CommonReference<iter_reference_t<_In>&&, iter_value_t<_In>&>
    && CommonReference<iter_reference_t<_In>&&, iter_rvalue_reference_t<_In>&&>
    && CommonReference<iter_rvalue_reference_t<_In>&&, const iter_value_t<_In>&>;

template <Readable _T>
using iter_common_reference_t = common_reference_t<iter_reference_t<_T>, iter_value_t<_T>&>;

template <class _Out, class _T>
concept bool Writable = requires(_Out&& __o, _T&& __t)
{
    *__o = __XVI_STD_NS::forward<_T>(__t);
    *__XVI_STD_NS::forward<_Out>(__o) = __XVI_STD_NS::forward<_T>(__t);
    const_cast<const iter_reference_t<_Out>&&>(*__o) = __XVI_STD_NS::forward<_T>(__t);
    const_cast<const iter_reference_t<_Out>&&>(*__XVI_STD_NS::forward<_Out>(__o)) = __XVI_STD_NS::forward<_T>(__t);
};

template <class _I>
concept bool WeaklyIncrementable = Semiregular<_I>
    && requires(_I __i)
    {
        typename iter_difference_t<_I>;
        requires SignedIntegral<iter_difference_t<_I>>;
        { ++__i } -> Same<_I&>;
        __i++;
    };

template <class _I>
concept bool Incrementable = Regular<_I> && WeaklyIncrementable<_I>
    && requires(_I __i)
    {
        { __i++ } -> Same<_I>;
    };

template <class _I>
concept bool Iterator = requires(_I __i)
{
    { *__i } -> _CanReference;
}
    && WeaklyIncrementable<_I>;

template <class _S, class _I>
concept bool Sentinel = Semiregular<_S> && Iterator<_I> && _WeaklyEqualityComparableWith<_S, _I>;

template <class _S, class _I>
inline constexpr bool disable_sized_sentinel = false;

template <class _S, class _I>
concept bool SizedSentinel = Sentinel<_S, _I>
    && !disable_sized_sentinel<remove_cv_t<_S>, remove_cv_t<_I>>
    && requires(const _I& __i, const _S& __s)
    {
        { __s - __i } -> Same<iter_difference_t<_I>>;
        { __i - __s } -> Same<iter_difference_t<_I>>;
    };

template <class _I>
concept bool InputIterator = Iterator<_I> && Readable<_I>
    && requires { typename __detail::_ITER_CONCEPT<_I>; }
    && DerivedFrom<__detail::_ITER_CONCEPT<_I>, input_iterator_tag>;

template <class _I, class _T>
concept bool OutputIterator = Iterator<_I> && Writable<_I>
    && requires(_I __i, _T&& __t)
    {
        *__i++ = __XVI_STD_NS::forward<_T>(__t);
    };

template <class _I>
concept bool ForwardIterator = InputIterator<_I>
    && DerivedFrom<__detail::_ITER_CONCEPT<_I>, forward_iterator_tag>
    && Incrementable<_I>
    && Sentinel<_I, _I>;

template <class _I>
concept bool BidirectionalIterator = ForwardIterator<_I>
    && DerivedFrom<__detail::_ITER_CONCEPT<_I>, bidirectional_iterator_tag>
    && requires(_I __i)
    {
        { --__i } -> Same<_I&>;
        { __i-- } -> Same<_I>;
    };

template <class _I>
concept bool RandomAccessIterator = BidirectionalIterator<_I>
    && DerivedFrom<__detail::_ITER_CONCEPT<_I>, random_access_iterator_tag>
    && StrictTotallyOrdered<_I>
    && SizedSentinel<_I, _I>
    && requires(_I __i, const _I __j, const iter_difference_t<_I> __n)
    {
        { __i += __n } -> Same<_I&>;
        { __j +  __n } -> Same<_I>;
        { __n +  __j } -> Same<_I>;
        { __i -= __n } -> Same<_I&>;
        { __j -  __n } -> Same<_I>;
        {  __j[__n]  } -> Same<iter_reference_t<_I>>;
    };

template <class _I>
concept bool ContiguousIterator = RandomAccessIterator<_I>
    && DerivedFrom<__detail::_ITER_CONCEPT<_I>, contiguous_iterator_tag>
    && is_lvalue_reference_v<iter_reference_t<_I>>
    && Same<iter_value_t<_I>, remove_cvref_t<iter_reference_t<_I>>>;


template <class _F, class _I>
concept bool IndirectUnaryInvocable = Readable<_I>
    && CopyConstructible<_F>
    && Invocable<_F, iter_value_t<_I>&>
    && Invocable<_F, iter_reference_t<_I>>
    && Invocable<_F, iter_common_reference_t<_I>>
    && CommonReference<invoke_result_t<_F&, iter_value_t<_I>&>,
                       invoke_result_t<_F&, iter_reference_t<_I>>>;

template <class _F, class _I>
concept bool IndirectRegularUnaryInvocable = Readable<_I>
    && CopyConstructible<_F>
    && RegularInvocable<_F, iter_value_t<_I>&>
    && RegularInvocable<_F, iter_reference_t<_I>>
    && RegularInvocable<_F, iter_common_reference_t<_I>>
    && CommonReference<invoke_result_t<_F&, iter_value_t<_I>&>,
                       invoke_result_t<_F&, iter_reference_t<_I>>>;

template <class _F, class _I>
concept bool IndirectUnaryPredicate = Readable<_I>
    && CopyConstructible<_F>
    && Predicate<_F&, iter_value_t<_I>&>
    && Predicate<_F&, iter_reference_t<_I>>
    && Predicate<_F&, iter_common_reference_t<_I>>;

template <class _F, class _I1, class _I2 = _I1>
concept bool IndirectRelation = Readable<_I1> && Readable<_I2>
    && CopyConstructible<_F>
    && Relation<_F&, iter_value_t<_I1>&, iter_value_t<_I2>&>
    && Relation<_F&, iter_value_t<_I2>&, iter_value_t<_I1>&>
    && Relation<_F&, iter_reference_t<_I1>, iter_reference_t<_I2>>
    && Relation<_F&, iter_reference_t<_I2>, iter_reference_t<_I1>>
    && Relation<_F&, iter_common_reference_t<_I1>, iter_common_reference_t<_I2>>;

template <class _F, class _I1, class _I2 = _I1>
concept bool IndirectStrictWeakOrder = Readable<_I1> && Readable<_I2>
    && CopyConstructible<_F>
    && StrictWeakOrder<_F&, iter_value_t<_I1>&, iter_value_t<_I2>&>
    && StrictWeakOrder<_F&, iter_value_t<_I2>&, iter_value_t<_I1>&>
    && StrictWeakOrder<_F&, iter_reference_t<_I1>, iter_reference_t<_I2>>
    && StrictWeakOrder<_F&, iter_reference_t<_I2>, iter_reference_t<_I1>>
    && StrictWeakOrder<_F&, iter_common_reference_t<_I1>, iter_common_reference_t<_I2>>;

template <class _F, class... _Is>
    requires (Readable<_Is> && ...) && Invocable<_F, iter_reference_t<_Is>...>
using indirect_result_t = invoke_result_t<_F, iter_reference_t<_Is>...>;

template <Readable _I, IndirectRegularUnaryInvocable<_I> _Proj>
struct projected
{
    using value_type = remove_cvref_t<indirect_result_t<_Proj&, _I>>;
    indirect_result_t<_Proj&, _I> operator*() const;
};

template <WeaklyIncrementable _I, class _Proj>
struct incrementable_traits<projected<_I, _Proj>>
{
    using difference_type = iter_difference_t<_I>;
};

template <class _In, class _Out>
concept bool IndirectlyMovable = Readable<_In> && Writable<_Out, iter_rvalue_reference_t<_In>>;

template <class _In, class _Out>
concept bool IndirectlyMovableStorable = IndirectlyMovable<_In, _Out>
    && Writable<_Out, iter_value_t<_In>>
    && Movable<iter_value_t<_In>>
    && Constructible<iter_value_t<_In>, iter_rvalue_reference_t<_In>>
    && Assignable<iter_value_t<_In>&, iter_rvalue_reference_t<_In>>;

template <class _In, class _Out>
concept bool IndirectlyCopyable = Readable<_In>
    && Writable<_Out, iter_reference_t<_In>>;

template <class _In, class _Out>
concept bool IndirectlyCopyableStorable = IndirectlyCopyable<_In, _Out>
    && Writable<_Out, const iter_value_t<_In>&>
    && Copyable<iter_value_t<_In>>
    && Constructible<iter_value_t<_In>, iter_reference_t<_In>>
    && Assignable<iter_value_t<_In>&, iter_reference_t<_In>>;


namespace ranges
{

inline namespace __iter
{

template <class _E1, class _E2>
concept bool _HasIterSwap = requires(_E1&& __e1, _E2&& __e2)
{
    { (void)iter_swap(__e1, __e2) };
};

template <class _E1, class _E2>
concept bool _HasIterSwapFallback1 = !_HasIterSwap<_E1, _E2>
    && Readable<_E1>
    && Readable<_E2>
    && SwappableWith<_E1, _E2>;

template <class _E1, class _E2>
concept bool _HasIterSwapFallback2 = !_HasIterSwap<_E1, _E2> && !_HasIterSwapFallback1<_E1, _E2>
    && IndirectlyMovableStorable<_E1, _E2>
    && IndirectlyMovableStorable<_E1, _E2>;

struct __iter_swap
{
    template <class _I1, class _I2>
    void iter_swap(_I1, _I2) = delete;

    template <class _X, class _Y>
    constexpr iter_value_t<remove_reference_t<_X>> __iter_exchange_move(_X&& __x, _Y&& __y)
        noexcept(noexcept(iter_value_t<remove_reference_t<_X>>(iter_move(__x)))
                 && noexcept(*__x = iter_move(__y)))
    {
        iter_value_t<remove_reference_t<_X>> __old_value(iter_move(__x));
        *__x = iter_move(__y);
        return __old_value;
    }

    template <class _E1, class _E2> requires _HasIterSwap<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const
        noexcept(noexcept(iter_swap(__e1, __e2)))
    {
        (void)iter_swap(__XVI_STD_NS::forward<_E1>(__e1), __XVI_STD_NS::forward<_E2>(__e2));
    } 

    template <class _E1, class _E2> requires _HasIterSwapFallback1<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const
        noexcept(noexcept(ranges::swap(*__e1, *__e2)))
    {
        (void)ranges::swap(*__e1, *__e2);
    }

    template <class _E1, class _E2> requires _HasIterSwapFallback2<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const
        noexcept(noexcept((void)(*__e1 = __iter_exchange_move(__e2, __e1))))
    {
        (void)(*__e1 = __iter_exchange_move(__e2, __e1));
    }
};

inline constexpr __iter_swap iter_swap = {};

} // inline namespace __iter

} // namespace ranges


template <class _I1, class _I2 = _I1>
concept bool IndirectlySwappable = Readable<_I1> && Readable<_I2>
    && requires(_I1& __i1, _I2& __i2)
    {
        ranges::iter_swap(__i1, __i1);
        ranges::iter_swap(__i2, __i2);
        ranges::iter_swap(__i1, __i2);
        ranges::iter_swap(__i2, __i1);
    };

template <class _I1, class _I2, class _R, class _P1 = identity, class _P2 = identity>
concept bool IndirectlyComparable = IndirectRelation<_R, projected<_I1, _P1>, projected<_I2, _P2>>;

template <class _I>
concept bool Permutable = ForwardIterator<_I>
    && IndirectlyMovableStorable<_I, _I>
    && IndirectlySwappable<_I, _I>;

template <class _I1, class _I2, class _Out, class _R = ranges::less<>, class _P1 = identity, class _P2 = identity>
concept bool Mergeable = InputIterator<_I1> && InputIterator<_I2>
    && WeaklyIncrementable<_Out>
    && IndirectlyCopyable<_I1, _Out>
    && IndirectlyCopyable<_I2, _Out>
    && IndirectStrictWeakOrder<_R, projected<_I1, _P1>, projected<_I2, _P2>>;

template <class _I, class _R = ranges::less<>, class _P = identity>
concept bool Sortable = Permutable<_I>
    && IndirectStrictWeakOrder<_R, projected<_I, _P>>;


template <class> inline constexpr bool disable_sized_range = false;


namespace ranges
{

inline namespace __iter
{

template <class _T>
constexpr decay_t<_T> __decay_copy(_T&& __v)
     noexcept(is_nothrow_convertible_v<_T, decay_t<_T>>)
{
    return __XVI_STD_NS::forward<_T>(__v);
}


struct __begin
{
    template <class _T> void begin(_T&&) = delete;
    template <class _T> void begin(initializer_list<_T>&&) = delete;
    
    template <class _E>
        requires is_lvalue_reference_v<_E> && is_array_v<remove_cvref_t<_E>>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __e + 0;
    }

    template <class _E>
        requires is_lvalue_reference_v<_E> && !is_array_v<remove_cvref_t<_E>>
        && requires(_E&& __e) { { __decay_copy(__XVI_STD_NS::forward<_E>(__e).begin()) } -> Iterator; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(__XVI_STD_NS::forward<_E>(__e).begin());
    }

    template <class _E>
        requires requires(_E&& __e) { { __decay_copy(begin(__XVI_STD_NS::forward<_E>(__e))) } -> Iterator; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(begin(__XVI_STD_NS::forward<_E>(__e)));
    }
};

inline constexpr __begin begin = {};

struct __end
{
    template <class _T> void end(_T&&) = delete;
    template <class _T> void end(initializer_list<_T>&&) = delete;

    template <class _E>
        requires is_lvalue_reference_v<_E> && is_array_v<remove_cvref_t<_E>>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __e + extent_v<remove_cvref_t<_E>>;
    }

    template <class _E>
        requires is_lvalue_reference_v<_E> && !is_array_v<remove_cvref_t<_E>>
        && requires(_E&& __e) { { __decay_copy(__XVI_STD_NS::forward<_E>(__e).end()) } -> Sentinel<decltype(ranges::begin(__XVI_STD_NS::forward<_E>(__e)))>; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(__XVI_STD_NS::forward<_E>(__e).end());
    }

    template <class _E>
        requires requires(_E&& __e) { { __decay_copy(end(__XVI_STD_NS::forward<_E>(__e))) } -> Sentinel<decltype(ranges::begin(__XVI_STD_NS::forward<_E>(__e)))>; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(__XVI_STD_NS::forward<_E>(__e));
    }
};

inline constexpr __end end = {};

struct __cbegin
{
    template <class _E>
        requires is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::begin(static_cast<const _E&>(__e));
    }

    template <class _E>
        requires !is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::begin(static_cast<const _E&&>(__e));
    }
};

inline constexpr __cbegin cbegin = {};

struct __cend
{
    template <class _E>
        requires is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::end(static_cast<const _E&>(__e));
    }

    template <class _E>
        requires !is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::end(static_cast<const _E&&>(__e));
    }
};

inline constexpr __cend cend = {};

struct __rbegin
{
    template <class _T> void rbegin(_T&&) = delete;

    template <class _E>
        requires is_lvalue_reference_v<_E> && requires(_E&& __e) { { __decay_copy(__XVI_STD_NS::forward<_E>(__e).rbegin()) } -> Iterator; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(__XVI_STD_NS::forward<_E>(__e).rbegin());
    }

    template <class _E>
        requires requires(_E&& __e) { { __decay_copy(rbegin(__XVI_STD_NS::forward<_E>(__e))) } -> Iterator; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(rbegin(__XVI_STD_NS::forward<_E>(__e)));
    }

    template <class _E>
        requires requires(_E&& __e)
        {
            { ranges::begin(__XVI_STD_NS::forward<_E>(__e)) } -> BidirectionalIterator;
            { ranges::end(__XVI_STD_NS::forward<_E>(__e))   } -> BidirectionalIterator;
        }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return make_reverse_iterator(ranges::end(__XVI_STD_NS::forward<_E>(__e)));
    }
};

inline constexpr __rbegin rbegin = {};

struct __rend
{
    template <class _T> void rend(_T&&) = delete;

    template <class _E>
        requires is_lvalue_reference_v<_E> && requires(_E&& __e) { { __decay_copy(__XVI_STD_NS::forward<_E>(__e).rend()) } -> Iterator; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(__XVI_STD_NS::forward<_E>(__e).rend());
    }

    template <class _E>
        requires requires(_E&& __e) { { __decay_copy(rend(__XVI_STD_NS::forward<_E>(__e))) } -> Sentinel<decltype(ranges::rbegin(__XVI_STD_NS::forward<_E>(__e)))>; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(rend(__XVI_STD_NS::forward<_E>(__e)));
    }

    template <class _E>
        requires requires(_E&& __e)
        {
            { ranges::begin(__XVI_STD_NS::forward<_E>(__e)) } -> BidirectionalIterator;
            { ranges::end(__XVI_STD_NS::forward<_E>(__e))   } -> BidirectionalIterator;
        }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return make_reverse_iterator(ranges::begin(__XVI_STD_NS::forward<_E>(__e)));
    }
};

inline constexpr __rend rend = {};

struct __crbegin
{
    template <class _E>
        requires is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::rbegin(static_cast<const _E&>(__e));
    }

    template <class _E>
        requires !is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::rbegin(static_cast<const _E&&>(__e));
    }
};

inline constexpr __crbegin crbegin = {};

struct __crend
{
    template <class _E>
        requires is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::rend(static_cast<const _E&>(__e));
    }

    template <class _E>
        requires !is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::rend(static_cast<const _E&&>(__e));
    }
};

inline constexpr __crend crend = {};

struct __size
{
    template <class _T> void size(_T&&) = delete;

    template <class _E>
        requires is_array_v<remove_cvref_t<_E>>
    constexpr decltype(auto) operator()(_E&&) const noexcept
    {
        return __decay_copy(extent_v<remove_cvref_t<_E>>);
    }

    template <class _E>
        requires !is_array_v<remove_cvref_t<_E>>
        && !disable_sized_range<remove_cv_t<_E>>
        && requires(_E&& __e) { { __decay_copy(__XVI_STD_NS::forward<_E>(__e).size()) } -> Integral; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(__XVI_STD_NS::forward<_E>(__e).size());
    }

    template <class _E>
        requires !is_array_v<remove_cvref_t<_E>>
        && !disable_sized_range<remove_cv_t<_E>>
        && requires(_E&& __e) { { __decay_copy(size(__XVI_STD_NS::forward<_E>(__e))) } -> Integral; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(size(__XVI_STD_NS::forward<_E>(__e)));
    }

    template <class _E>
        requires !is_array_v<remove_cvref_t<_E>>
        && requires(_E&& __e)
        {
            { ranges::end(__e) - ranges::begin(__e) };
            { ranges::begin(__e) } -> ForwardIterator;
            { ranges::end(__e)   } -> ForwardIterator;
        }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return (ranges::end(__e) - ranges::begin(__e));
    }
};

inline constexpr __size size = {};

struct __empty
{
    template <class _E>
        requires requires(_E&& __e) { bool(__XVI_STD_NS::forward<_E>(__e).empty()); }
    constexpr bool operator()(_E&& __e) const noexcept
    {
        return bool(__XVI_STD_NS::forward<_E>(__e).empty());
    }

    template <class _E>
        requires requires(_E&& __e) { ranges::size(__XVI_STD_NS::forward<_E>(__e)) == 0; }
    constexpr bool operator()(_E&& __e) const noexcept
    {
        return ranges::size(__XVI_STD_NS::forward<_E>(__e)) == 0;
    }

    template <class _E>
        requires requires(_E&& __e)
        {
            { ranges::begin(__XVI_STD_NS::forward<_E>(__e)) } -> ForwardIterator;
            { bool(ranges::begin(__e) == ranges::end(__e)) };
        }
    constexpr bool operator()(_E&& __e) const noexcept
    {
        return bool(ranges::begin(__XVI_STD_NS::forward<_E>(__e)) == ranges::end(__XVI_STD_NS::forward<_E>(__e)));
    }
};

inline constexpr __empty empty = {};

template <class _T>
concept bool _Pointer = is_pointer_v<_T>;

struct __data
{
    template <class _E>
        requires is_lvalue_reference_v<_E>
        && requires(_E&& __e)
        {
            { __decay_copy(__XVI_STD_NS::forward<_E>(__e).data()) } -> _Pointer;
        }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return __decay_copy(__XVI_STD_NS::forward<_E>(__e).data());
    }

    template <class _E>
        requires requires(_E&& __e) { { ranges::begin(__XVI_STD_NS::forward<_E>(__e)) } -> ContiguousIterator; }
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::begin(__XVI_STD_NS::forward<_E>(__e)) == ranges::end(__XVI_STD_NS::forward<_E>(__e))
            ? nullptr
            : addressof(*ranges::begin(__XVI_STD_NS::forward<_E>(__e)));
    }
};

inline constexpr __data data = {};

struct __cdata
{
    template <class _E>
        requires is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::data(static_cast<const _E&>(__e));
    }

    template <class _E>
        requires !is_lvalue_reference_v<_E>
    constexpr decltype(auto) operator()(_E&& __e) const noexcept
    {
        return ranges::data(static_cast<const _E&&>(__e));
    }
};

inline constexpr __cdata cdata = {};

} // inline namespace __iter

template <class _T>
using iterator_t = decltype(ranges::begin(declval<_T&>()));

template <class _T>
using sentinel_t = decltype(ranges::end(declval<_T&>()));

template <class _T>
concept bool _RangeImpl =
    requires(_T&& __t)
    {
        ranges::begin(__XVI_STD_NS::forward<_T>(__t));
        ranges::end(__XVI_STD_NS::forward<_T>(__t));
    };

template <class _T>
concept bool Range = _RangeImpl<_T&>;

template <class _T>
concept bool _ForwardingRange = Range<_T> && _RangeImpl<_T>;

template <class _T>
concept bool SizedRange = Range<_T>
    && disable_sized_range<remove_cvref_t<_T>>
    && requires(_T& __t)
    {
        ranges::size(__t);
    };

} // namespace ranges
*/

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
    // if constexpr (_Cpp17RandomAccessIterator<_InputIterator>)
    //     return __last - __first;
    
    size_t __i = 0;
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

/*
namespace ranges
{

template <Iterator _I>
constexpr void advance(_I& __i, iter_difference_t<_I> __n)
{
    if constexpr (RandomAccessIterator<_I>)
        __i += __n;
    else if constexpr (BidirectionalIterator<_I>)
    {
        if (__n > 0)
            for (size_t __j = 0; __j < __n; ++__j)
                ++__i;
        else if (__n < 0)
            for (size_t __j = 0; __j < -__n; ++__j)
                --__i;
    }
    else
        for (size_t __j = 0; __j < __n; ++__j)
            ++__i;
}

template <Iterator _I, Sentinel<_I> _S>
constexpr void advance(_I& __i, _S __bound)
{
    if constexpr (Assignable<_I&, _S>)
        __i = __XVI_STD_NS::move(__bound);
    else if constexpr (SizedSentinel<_S, _I>)
        ranges::advance(__i, __bound - __i);
    else
        while (bool(__i != __bound))
            ++__i;
}

template <Iterator _I, Sentinel<_I> _S>
constexpr iter_difference_t<_I> advance(_I& __i, iter_difference_t<_I> __n, _S __bound)
{
    auto __abs = [](auto __a) { (__a < 0) ? -__a : __a; };
    
    if constexpr (SizedSentinel<_S, _I>)
    {
        if (__abs(__n) >= __abs(__bound - __i))
            ranges::advance(__i, __bound);
        else
            ranges::advance(__i, __n);
    }
    else
    {
        if (__n >= 0)
            while (bool(__i != __bound) && __n > 0)
                ++__i;
        else
            while (bool(__i != __bound) && __n < 0)
                --__i;
    }
}

template <Iterator _I, Sentinel<_I> _S>
constexpr iter_difference_t<_I> distance(_I __first, _I __last)
{
    if constexpr (SizedSentinel<_S, _I>)
        return (__last - __first);
    else
    {
        size_t __i = 0;
        while (__first != __last)
            ++__i, ++__first;
        return __i;
    }
}

template <Range _R>
constexpr iter_difference_t<iterator_t<_R>> distance(_R&& __r)
{
    if constexpr (SizedRange<_R>)
        return ranges::size(__r);
    else
        return ranges::distance(ranges::begin(__r), ranges::end(__r));
}

template <Iterator _I>
constexpr _I next(_I __x)
{
    ++__x;
    return __x;
}

template <Iterator _I>
constexpr _I next(_I __x, iter_difference_t<_I> __n)
{
    ranges::advance(__x, __n);
    return __x;
}

template <Iterator _I, Sentinel<_I> _S>
constexpr _I next(_I __x, _S __bound)
{
    ranges::advance(__x, __bound);
    return __x;
}

template <Iterator _I, Sentinel<_I> _S>
constexpr _I next(_I __x, iter_difference_t<_I> __n, _S __bound)
{
    ranges::advance(__x, __n, __bound);
    return __x;
}

template <BidirectionalIterator _I>
constexpr _I prev(_I __x)
{
    --__x;
    return __x;
}

template <BidirectionalIterator _I>
constexpr _I prev(_I __x, iter_difference_t<_I> __n)
{
    ranges::advance(__x, -__n);
    return __x;
}

template <BidirectionalIterator _I, Sentinel<_I> _S>
constexpr _I prev(_I __x, iter_difference_t<_I> __n, _S __bound)
{
    ranges::advance(__x, -__n, __bound);
    return __x;
}

} // namespace ranges
*/

template <class _Iterator>
class reverse_iterator
{
public:

    using iterator_type     = _Iterator;
    // using iterator_concept  = conditional_t<RandomAccessIterator<_Iterator>,
    //                                         random_access_iterator_tag,
    //                                         bidirectional_iterator_tag>;
    // using iterator_category = conditional_t<DerivedFrom<typename iterator_traits<_Iterator>::iterator_category, random_access_iterator_tag>,
    //                                         random_access_iterator_tag,
    //                                         typename iterator_traits<_Iterator>::iterator_category>;
    using value_type        = remove_reference_t<decltype(*declval<_Iterator>())>;//iter_value_t<_Iterator>;
    using difference_type   = ptrdiff_t;//iter_difference_t<_Iterator>;
    using pointer           = value_type*;//typename iterator_traits<_Iterator>::pointer;
    using reference         = value_type&;//iter_reference_t<_Iterator>;

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

    // friend constexpr iter_rvalue_reference_t<_Iterator> iter_move(const reverse_iterator& __i)
    //     noexcept(is_nothrow_copy_constructible_v<_Iterator> && noexcept(ranges::iter_move(--declval<_Iterator&>())))
    // {
    //     auto __tmp = __i.base();
    //     return ranges::iter_move(--__tmp);
    // }

    // template <IndirectlySwappable<_Iterator> _Iterator2>
    // friend constexpr void iter_swap(const reverse_iterator& __x, const reverse_iterator<_Iterator2>& __y)
    //     noexcept(is_nothrow_copy_constructible_v<_Iterator>
    //              && is_nothrow_copy_constructible_v<_Iterator2>
    //              && noexcept(ranges::iter_swap(--declval<_Iterator&>(), --declval<_Iterator2&>())))
    // {
    //     auto __xtmp = __x.base();
    //     auto __ytmp = __y.base();
    //     ranges::iter_swap(--__xtmp, --__ytmp);
    // }

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

/*
template <class _Iterator1, class _Iterator2>
    requires (!SizedSentinel<_Iterator1, _Iterator2>)
inline constexpr bool disable_sized_sentinel<reverse_iterator<_Iterator1>, reverse_iterator<_Iterator2>> = true;


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

    constexpr explicit back_insert_iterator(_Container& __x)
        : container(addressof(__x))
    {
    }

    constexpr back_insert_iterator& operator=(const typename _Container::value_type& __value)
    {
        container->push_back(__value);
        return *this;
    }

    constexpr back_insert_iterator& operator=(typename _Container::value_type&& __value)
    {
        container->push_back(__XVI_STD_NS::move(__value));
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

protected:

    _Container* container       = nullptr;
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

    constexpr front_insert_iterator() = default;

    constexpr explicit front_insert_iterator(_Container& __x)
        : container(addressof(__x))
    {
    }

    constexpr front_insert_iterator& operator=(const typename _Container::value_type& __value)
    {
        container->push_front(__value);
        return *this;
    }

    constexpr front_insert_iterator& operator=(typename _Container::value_type& __value)
    {
        container->push_front(__XVI_STD_NS::move(__value));
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

protected:

    _Container container = nullptr;
};

template <class _Container>
constexpr front_insert_iterator<_Container> front_inserter(_Container& __x)
{
    return front_insert_iterator<_Container>(__x);
}


template <class _Container>
class insert_iterator
{
public:

    using iterator_category     = output_iterator_tag;
    using value_type            = void;
    using difference_type       = ptrdiff_t;
    using pointer               = void;
    using reference             = void;
    using container_type        = _Container;

    constexpr insert_iterator() = default;

    constexpr insert_iterator(_Container& __x, ranges::iterator_t<_Container> __i)
        : container(addressof(__x)),
          iter(__i)
    {
    }

    constexpr insert_iterator& operator=(const typename _Container::value_type& __value)
    {
        iter = container->insert(iter, __value);
        ++iter;
        return *this;
    }

    constexpr insert_iterator& operator=(typename _Container::value_type&& __value)
    {
        iter = container->insert(iter, __XVI_STD_NS::move(__value));
        ++iter;
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

protected:

    _Container*                     container = nullptr;
    ranges::iterator_t<_Container>  iter = ranges::iterator_t<_Container>();
};

template <class _Container>
constexpr insert_iterator<_Container> inserter(_Container& __x, ranges::iterator_t<_Container> __i)
{
    return insert_iterator<_Container>(__x, __i);
}


// Forward declaration - defined below.
template <Semiregular _S>
class move_sentinel;

template <class _Iterator>
class move_iterator
{
public:

    using iterator_type         = _Iterator;
    using iterator_concept      = input_iterator_tag;
    using iterator_category     = conditional_t<DerivedFrom<typename iterator_traits<_Iterator>::iterator_category, random_access_iterator_tag>,
                                                random_access_iterator_tag,
                                                typename iterator_traits<_Iterator>::iterator_category>;
    using value_type            = iter_value_t<_Iterator>;
    using difference_type       = iter_difference_t<_Iterator>;
    using pointer               = _Iterator;
    using reference             = iter_rvalue_reference_t<_Iterator>;

    constexpr move_iterator()
        : _M_current{}
    {
    }

    constexpr explicit move_iterator(_Iterator __i)
        : _M_current(__i)
    {
    }

    template <class _U>
    constexpr move_iterator(const move_iterator<_U>& __u)
        : _M_current(__u.base())
    {
    }

    template <class _U>
    constexpr move_iterator& operator=(const move_iterator<_U>& __u)
    {
        _M_current = __u.base();
        return *this;
    }

    constexpr iterator_type base() const
    {
        return _M_current;
    }

    constexpr reference operator*() const
    {
        return ranges::iter_move(_M_current);
    }

    constexpr pointer operator->() const
    {
        return _M_current;
    }

    constexpr move_iterator& operator++()
    {
        ++_M_current;
        return *this;
    }

    constexpr auto operator++(int)
    {
        if constexpr (ForwardIterator<_Iterator>)
        {
            move_iterator __tmp = *this;
            ++_M_current;
            return __tmp;
        }
        else
        {
            ++_M_current;
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

    template <Sentinel<_Iterator> _S>
    friend constexpr bool operator==(const move_iterator& __x, const move_sentinel<_S>& __y)
    {
        return __x.base() == __y.base();
    }

    template <Sentinel<_Iterator> _S>
    friend constexpr bool operator==(const move_sentinel<_S>& __x, const move_iterator& __y)
    {
        return __x.base() == __y.base();
    }

    template <Sentinel<_Iterator> _S>
    friend constexpr bool operator!=(const move_iterator& __x, const move_sentinel<_S>& __y)
    {
        return !(__x == __y);
    }

    template <Sentinel<_Iterator> _S>
    friend constexpr bool operator!=(const move_sentinel<_S>& __x, const move_iterator& __y)
    {
        return !(__x == __y);
    }

    template <SizedSentinel<_Iterator> _S>
    friend constexpr iter_difference_t<_Iterator> operator-(const move_iterator& __x, const move_sentinel<_S>& __y)
    {
        return __x.base() - __y.base();
    }

    template <SizedSentinel<_Iterator> _S>
    friend constexpr iter_difference_t<_Iterator> operator-(const move_sentinel<_S>& __x, const move_iterator& __y)
    {
        return __x.base() - __y.base();
    }

    friend constexpr iter_rvalue_reference_t<_Iterator> iter_move(const move_iterator& __x)
        noexcept(noexcept(ranges::iter_move(__x._M_current)))
    {
        return ranges::iter_move(__x._M_current);
    }

    template <IndirectlySwappable<_Iterator> _Iterator2>
    friend constexpr void iter_swap(const move_iterator& __x, const move_iterator<_Iterator2>& __y)
        noexcept(noexcept(ranges::iter_swap(__x._M_current, __y._M_current)))
    {
        return ranges::iter_swap(__x._M_current, __y._M_current);
    }

private:

    _Iterator _M_current;
};


template <class _Iterator1, class _Iterator2>
constexpr bool operator==(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
{
    return __x.base() == __y.base();
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator!=(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
{
    return !(__x == __y);
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator< (const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
{
    return __x.base() < __y.base();
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator> (const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
{
    return __y < __x;
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator<=(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
{
    return !(__y < __x);
}

template <class _Iterator1, class _Iterator2>
constexpr bool operator>=(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
{
    return !(__x < __y);
}


template <class _Iterator1, class _Iterator2>
constexpr auto operator-(const move_iterator<_Iterator1>& __x, const move_iterator<_Iterator2>& __y)
    -> decltype(__x.base() - __y.base())
{
    return __x.base() - __y.base();
}

template <class _Iterator>
constexpr move_iterator<_Iterator> operator+(typename move_iterator<_Iterator>::difference_type __n, const move_iterator<_Iterator>& __x)
{
    return __x + __n;
}


template <class _Iterator>
constexpr move_iterator<_Iterator> make_move_iterator(_Iterator __i)
{
    return move_iterator<_Iterator>(__i);
}
*/
/*
template <Semiregular _S>
class move_sentinel
{
public:

    constexpr move_sentinel()
        : _M_last{}
    {
    }

    constexpr explicit move_sentinel(_S __s)
        : _M_last{__XVI_STD_NS::move(__s)}
    {
    }

    template <class _S2>
        requires ConvertibleTo<const _S2&, _S>
    constexpr move_sentinel(const move_sentinel<_S2>& __s)
        : _M_last{__s._M_last}
    {
    }

    template <class _S2>
        requires Assignable<_S&, const _S2&>
    constexpr move_sentinel& operator=(const move_sentinel<_S2>& __s)
    {
        _M_last = __s._M_last;
        return *this;
    }

    constexpr _S base() const
    {
        return _M_last;
    }

private:

    template <class> friend class move_sentinel;

    _S _M_last;
};


template <Iterator _I, Sentinel<_I> _S>
    requires (!Same<_I, _S>)
class common_iterator
{
    constexpr common_iterator() = default;

    constexpr common_iterator(_I __i)
        : _M_var{in_place_type<_I>, __XVI_STD_NS::move(__i)}
    {
    }

    constexpr common_iterator(_S __s)
        : _M_var{in_place_type<_S>, __XVI_STD_NS::move(__s)}
    {
    }

    template <class _I2, class _S2>
        requires ConvertibleTo<const _I2&, _I> && ConvertibleTo<const _S2&, _S>
    constexpr common_iterator(const common_iterator<_I2, _S2>& __x)
        : _M_var{__x.index() == 0 ? variant<_I, _S>{in_place_index<0>, get<0>(__x._M_var)}
                                  : variant<_I, _S>{in_place_index<1>, get<1>(__x._M_var)}}
    {
    }

    template <class _I2, class _S2>
        requires ConvertibleTo<const _I2&, _I> && ConvertibleTo<const _S2&, _S>
            && Assignable<_I&, const _I2&> && Assignable<_S&, const _S2&>
    common_iterator& operator=(const common_iterator<_I2, _S2>& __x)
    {
        if (_M_var.index() == __x._M_var.index())
        {
            if (_M_var.index() == 0)
                get<0>(_M_var) = get<0>(__x._M_var);
            else
                get<1>(_M_var) = get<1>(__x._M_var);
        }
        else
        {
            if (__x._M_var.index() == 0)
                _M_var.emplace<0>(get<0>(__x._M_var));
            else
                _M_var.emplace<1>(get<1>(__x._M_var));
        }

        return *this;
    }

    decltype(auto) operator*()
    {
        return *get<_I>(_M_var);
    }

    decltype(auto) operator*() const
        requires _Dereferenceable<const _I>
    {
        return *get<_I>(_M_var);
    }

    decltype(auto) operator->() const
        requires Readable<const _I>
            && (requires(const _I& __i) { __i.operator->(); }
                || is_reference_v<iter_reference_t<_I>>
                || Constructible<iter_value_t<_I>, iter_reference_t<_I>>)
    {
        if constexpr (is_pointer_v<_I> || __detail::is_detected_v<decltype(get<_I>(_M_var))>)
            return get<_I>(_M_var);
        else if (is_reference_v<iter_reference_t<_I>>)
        {
            auto&& __tmp = *get<_I>(_M_var);
            return addressof(__tmp);
        }
        else
        {
            class __proxy
            {
                iter_value_t<_I> _M_keep;

                __proxy(iter_reference_t<_I>&& __x)
                    : _M_keep(__XVI_STD_NS::move(__x))
                {
                }

            public:

                const iter_value_t<_I>* operator->() const
                {
                    return addressof(_M_keep);
                }
            };

            return __proxy(*get<_I>(_M_var));
        }
    }

    common_iterator& operator++()
    {
        ++get<_I>(_M_var);
        return *this;
    }

    decltype(auto) operator++(int)
    {
        if constexpr (ForwardIterator<_I>)
        {
            common_iterator __tmp = *this;
            ++*this;
            return __tmp;
        }
        else
        {
            return get<_I>(_M_var)++;
        }
    }

    template <class _I2, Sentinel<_I> _S2>
        requires Sentinel<_S, _I2>
    friend bool operator==(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
    {
        auto __i = __x._M_var.index();
        auto __j = __y._M_var.index();

        if (__i == __j)
            return true;
        else if (__i == 0)
            return get<0>(__x._M_var) == get<1>(__y._M_var);
        else
            return get<1>(__x._M_var) == get<0>(__y._M_var);
    }

    template <class _I2, Sentinel<_I> _S2>
        requires Sentinel<_S, _I2> && EqualityComparableWith<_I, _I2>
    friend bool operator==(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
    {
        auto __i = __x._M_var.index();
        auto __j = __y._M_var.index();

        if (__i == 1 && __j == 1)
            return true;
        else if (__i == 0 && __j == 0)
            return get<0>(__x._M_var) == get<0>(__y._M_var);
        else if (__i == 1)
            return get<1>(__x._M_var) == get<0>(__y._M_var);
        else
            return get<0>(__x._M_var) == get<1>(__y._M_var);
    }

    template <class _I2, Sentinel<_I> _S2>
        requires Sentinel<_S, _I2>
    friend bool operator!=(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
    {
        return !(__x == __y);
    }

    template <SizedSentinel<_I> _I2, SizedSentinel<_I> _S2>
        requires SizedSentinel<_S, _I2>
    friend iter_difference_t<_I2> operator-(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
    {
        auto __i = __x._M_var.index();
        auto __j = __y._M_var.index();

        if (__i == 1 && __j == 1)
            return 0;
        else if (__i == 0 && __j == 0)
            return get<0>(__x._M_var) - get<0>(__y._M_var);
        else if (__i == 1)
            return get<1>(__x._M_var) - get<0>(__y._M_var);
        else
            return get<0>(__x._M_var) - get<1>(__y._M_var);
    }

    friend iter_rvalue_reference_t<_I> iter_move(const common_iterator& __i)
        noexcept(noexcept(ranges::iter_move(declval<const _I&>())))
        requires InputIterator<_I>
    {
        return ranges::iter_move(get<_I>(__i._M_var));
    }

    template <IndirectlySwappable<_I> _I2, class _S2>
    friend void iter_swap(const common_iterator& __x, const common_iterator<_I2, _S2>& __y)
        noexcept(noexcept(ranges::iter_swap(declval<_I&>(), declval<_I2&>())))
    {
        ranges::iter_swap(get<_I>(__x._M_var), get<_I2>(__y._M_var));
    }

private:

    template <class, class> friend class common_iterator;

    variant<_I, _S> _M_var;
};


template <class _I, class _S>
struct incrementable_traits<common_iterator<_I, _S>>
{
    using difference_type = iter_difference_t<_I>;
};


template <class _I, class _S>
struct iterator_traits<common_iterator<_I, _S>>
{
    using iterator_concept      = conditional_t<ForwardIterator<_I>, forward_iterator_tag, input_iterator_tag>;
    using iterator_category     = conditional_t<DerivedFrom<typename iterator_traits<_I>::iterator_category, forward_iterator_tag>,
                                                forward_iterator_tag,
                                                input_iterator_tag>;
    using value_type            = iter_value_t<_I>;
    using difference_type       = iter_difference_t<_I>;
    using pointer               = __detail::detected_or<void, __detail::__operator_arrow_detector, common_iterator<_I, _S>>;
    using reference             = iter_reference_t<_I>;
};


struct default_sentinel_t {};

inline constexpr default_sentinel_t default_sentinel {};


template <Iterator _I>
class counted_iterator
{
public:

    using iterator_type = _I;

    constexpr counted_iterator() = default;

    constexpr counted_iterator(_I __x, iter_difference_t<_I> __n)
        : _M_current(__x), _M_length(__n)
    {
    }

    template <class _I2>
        requires ConvertibleTo<const _I2&, _I>
    constexpr counted_iterator(const counted_iterator<_I2>& __x)
        : _M_current(__x._M_current), _M_length(__x._M_length)
    {
    }

    template <class _I2>
        requires Assignable<_I&, const _I2&>
    constexpr counted_iterator& operator=(const counted_iterator<_I2>& __x)
    {
        _M_current = __x._M_current;
        _M_length = __x._M_length;
        return *this;
    }

    constexpr _I base() const
    {
        return _M_current;
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
        requires _Dereferenceable<const _I>
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
        requires ForwardIterator<_I>
    {
        counted_iterator __tmp = *this;
        --*this;
        return __tmp;
    }

    constexpr counted_iterator& operator--()
        requires BidirectionalIterator<_I>
    {
        --_M_current;
        ++_M_length;
        return *this;
    }

    constexpr counted_iterator operator--(int)
        requires BidirectionalIterator<_I>
    {
        counted_iterator __tmp = *this;
        --*this;
        return __tmp;
    }

    constexpr counted_iterator operator+(iter_difference_t<_I> __n) const
        requires RandomAccessIterator<_I>
    {
        return counted_iterator(_M_current + __n, _M_length - __n);
    }

    friend constexpr counted_iterator operator+(iter_difference_t<_I> __n, const counted_iterator& __x)
        requires RandomAccessIterator<_I>
    {
        return __x + __n;
    }

    constexpr counted_iterator& operator+=(iter_difference_t<_I> __n)
        requires RandomAccessIterator<_I>
    {
        _M_current += __n;
        _M_length -= __n;
        return *this;
    }

    constexpr counted_iterator operator-(iter_difference_t<_I> __n) const
        requires RandomAccessIterator<_I>
    {
        return counted_iterator(_M_current - __n, _M_length + __n);
    }

    template <Common<_I> _I2>
    friend constexpr iter_difference_t<_I2> operator-(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __x._M_length - __y._M_length;
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
        requires RandomAccessIterator<_I>
    {
        _M_current -= __n;
        _M_length += __n;
        return *this;
    }

    constexpr decltype(auto) operator[](iter_difference_t<_I> __n) const
        requires RandomAccessIterator<_I>
    {
        return _M_current[__n];
    }

    template <Common<_I> _I2>
    friend constexpr bool operator==(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __x._M_length == __y._M_length;
    }

    friend constexpr bool operator==(const counted_iterator& __x, default_sentinel_t)
    {
        return __x._M_length == 0;
    }

    friend constexpr bool operator==(default_sentinel_t, const counted_iterator& __y)
    {
        return __y._M_length == 0;
    }

    template <Common<_I> _I2>
    friend constexpr bool operator!=(const counted_iterator& __x, const counted_iterator& __y)
    {
        return !(__x == __y);
    }

    friend constexpr bool operator!=(const counted_iterator& __x, default_sentinel_t __y)
    {
        return !(__x == __y);
    }

    friend constexpr bool operator!=(default_sentinel_t __x, const counted_iterator& __y)
    {
        return !(__x == __y);
    }

    template <Common<_I> _I2>
    friend constexpr bool operator<(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __y._M_length < __x._M_length;
    }

    template <Common<_I> _I2>
    friend constexpr bool operator>(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __y < __x;
    }

    template <Common<_I> _I2>
    friend constexpr bool operator<=(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return !(__y < __x);
    }

    template <Common<_I> _I2>
    friend constexpr bool operator>=(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return !(__x < __y);
    }

    friend constexpr iter_rvalue_reference_t<_I> iter_move(const counted_iterator& __i)
        noexcept(noexcept(ranges::iter_move(__i._M_current)))
        requires InputIterator<_I>
    {
        return ranges::iter_move(__i._M_current);
    }

    template <IndirectlySwappable<_I> _I2>
    friend constexpr void iter_swap(const counted_iterator& __x, const counted_iterator<_I2>& __y)
        noexcept(noexcept(ranges::iter_swap(__x._M_current, __y._M_current)))
    {
        ranges::iter_swap(__x._M_current, __y._M_current);
    }

private:

    template <class> friend class counted_iterator;

    _I                      _M_current = _I();
    iter_difference_t<_I>   _M_length = 0;
};


template <class _I>
struct incrementable_traits<counted_iterator<_I>>
{
    using difference_type = iter_difference_t<_I>;
};


template <InputIterator _I>
struct iterator_traits<counted_iterator<_I>>
    : iterator_traits<_I>
{
    using pointer = void;
};


struct unreachable_sentinel_t
{
    template <WeaklyIncrementable _I>
    friend constexpr bool operator==(unreachable_sentinel_t, const _I&) noexcept
    {
        return false;
    }

    template <WeaklyIncrementable _I>
    friend constexpr bool operator==(const _I&, unreachable_sentinel_t) noexcept
    {
        return false;
    }

    template <WeaklyIncrementable _I>
    friend constexpr bool operator!=(unreachable_sentinel_t, const _I&) noexcept
    {
        return true;
    }

    template <WeaklyIncrementable _I>
    friend constexpr bool operator!=(const _I&, unreachable_sentinel_t) noexcept
    {
        return true;
    }
};

inline constexpr unreachable_sentinel_t unreachable_sentinel {};
*/
/*
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

    constexpr istream_iterator()
        : _M_in_stream(nullptr), _M_value()
    {
    }

    constexpr istream_iterator(default_sentinel_t)
        : istream_iterator()
    {
    }

    istream_iterator(istream_type& __s)
        : _M_in_stream(addressof(__s))
    {
    }

    istream_iterator(const istream_iterator& __x) = default;
    ~istream_iterator() = default;
    istream_iterator& operator=(const istream_iterator&) = default;

    const _T& operator*() const
    {
        return _M_value;
    }

    const _T* operator->() const
    {
        return addressof(operator*());
    }

    istream_iterator& operator++()
    {
        *_M_in_stream >> _M_value;
        return *this;
    }

    istream_iterator operator++(int)
    {
        istream_iterator __tmp = *this;
        *_M_in_stream >> _M_value;
        return __tmp;
    }

    friend bool operator==(const istream_iterator& __i, default_sentinel_t)
    {
        return !(__i._M_in_stream);
    }

    friend bool operator==(default_sentinel_t, const istream_iterator& __i)
    {
        return !(__i._M_in_stream);
    }

    friend bool operator!=(const istream_iterator& __x, default_sentinel_t __y)
    {
        return !(__x == __y);
    }

    friend bool operator!=(default_sentinel_t __x, const istream_iterator& __y)
    {
        return !(__x == __y);
    }

private:

    basic_istream<_CharT, _Traits>* _M_in_stream;
    _T                              _M_value;
};

template <class _T, class _CharT, class _Traits, class _Distance>
bool operator==(const istream_iterator<_T, _CharT, _Traits, _Distance>& __x, const istream_iterator<_T, _CharT, _Traits, _Distance>& __y)
{
    return __x._M_in_stream == __y._M_in_stream;
}

template <class _T, class _CharT, class _Traits, class _Distance>
bool operator!=(const istream_iterator<_T, _CharT, _Traits, _Distance>& __x, const istream_iterator<_T, _CharT, _Traits, _Distance>& __y)
{
    return !(__x == __y);
}


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

    ostream_iterator(ostream_type& __s)
        : _M_out_stream(addressof(__s))
    {
    }

    ostream_iterator(ostream_type& __s, const _CharT* __delim)
        : _M_out_stream(addressof(__s)), _M_delim(__delim)
    {
    }

    ostream_iterator(const ostream_iterator& __x)
        : _M_out_stream(__x._M_out_stream), _M_delim(__x._M_delim)
    {
    }

    ~ostream_iterator()
    {
    }

    ostream_iterator& operator=(const ostream_iterator& __x) = default;

    ostream_iterator& operator=(const _T& __value)
    {
        *_M_out_stream << __value;
        if (_M_delim)
            *_M_out_stream << _M_delim;
        return *this;
    }

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
    const _CharT*                   _M_delim = nullptr;
};


template <class _CharT, class _Traits>
class istreambuf_iterator
{
public:

    class __proxy;

    using iterator_category     = input_iterator_tag;
    using value_type            = _CharT;
    using difference_type       = typename _Traits::off_type;
    using pointer               = __proxy;
    using reference             = _CharT;
    using char_type             = _CharT;
    using traits_type           = _Traits;
    using int_type              = typename _Traits::int_type;
    using streambuf_type        = basic_streambuf<_CharT, _Traits>;
    using istream_type          = basic_istream<_CharT, _Traits>;

    class __proxy
    {
        _CharT _M_keep;
        basic_streambuf<_CharT, _Traits>* _M_sbuf;

        __proxy(_CharT __c, basic_streambuf<_CharT, _Traits>* __sbuf)
            : _M_keep(__c), _M_sbuf(__sbuf)
        {
        }

    public:

        _CharT operator*()
        {
            return _M_keep;
        }
    };

    constexpr istreambuf_iterator() noexcept
        : _M_sbuf(nullptr)
    {
    }

    constexpr istreambuf_iterator(default_sentinel_t) noexcept
        : istreambuf_iterator()
    {
    }

    istreambuf_iterator(const istreambuf_iterator&) noexcept = default;
    ~istreambuf_iterator() = default;

    istreambuf_iterator(istream_type& __s) noexcept
        : _M_sbuf(__s.rdbuf())
    {
    }

    istreambuf_iterator(streambuf_type* __s) noexcept
        : _M_sbuf(__s)
    {
    }

    istreambuf_iterator(const __proxy& __p) noexcept
        : _M_sbuf(__p._M_sbuf)
    {
    }

    istreambuf_iterator& operator=(const istreambuf_iterator&) noexcept = default;

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
        return _M_sbuf->eof() != __b.eof();
    }

    friend bool operator==(default_sentinel_t __s, const istreambuf_iterator& __i)
    {
        return __i.equal(__s);
    }

    friend bool operator==(const istreambuf_iterator& __i, default_sentinel_t __s)
    {
        return __i.equal(__s);
    }

    friend bool operator!=(default_sentinel_t __a, const istreambuf_iterator& __b)
    {
        return !__b.equal(__a);
    }

    friend bool operator!=(const istreambuf_iterator& __a, default_sentinel_t __b)
    {
        return !__a.equal(__b);
    }

private:

    streambuf_type* _M_sbuf;
};

template <class _CharT, class _Traits>
bool operator==(const istreambuf_iterator<_CharT, _Traits>& __x, const istreambuf_iterator<_CharT, _Traits>& __y)
{
    return __x.equal(__y);
}

template <class _CharT, class _Traits>
bool operator!=(const istreambuf_iterator<_CharT, _Traits>& __x, const istreambuf_iterator<_CharT, _Traits>& __y)
{
    return !(__x.equal(__y));
}


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

    ostreambuf_iterator(ostream_type& __s) noexcept
        : _M_sbuf(__s.rdbuf())
    {
    }

    ostreambuf_iterator(streambuf_type* __s) noexcept
        : _M_sbuf(__s)
    {
    }

    ostreambuf_iterator& operator=(_CharT __c)
    {
        if (!failed())
            _M_failed = (_M_sbuf->sputc(__c) != _Traits::eof());

        return *this;
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
        return _M_failed;
    }

private:

    streambuf_type* _M_sbuf = nullptr;
    bool _M_failed = false;
};
*/

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
template <class _T, size_t _N> constexpr size_t size(const _T (&__array)[_N]) noexcept
    { return _N; }
template <class _C> [[nodiscard]] constexpr auto empty(const _C& __c) -> decltype(__c.empty())
    { return __c.empty();}
template <class _T, size_t _N> [[nodiscard]] constexpr bool empty(const _T (&__array)[_N]) noexcept
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
