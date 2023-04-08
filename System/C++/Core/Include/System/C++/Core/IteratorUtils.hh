#ifndef __SYSTEM_CXX_CORE_ITERATORUTILS_H
#define __SYSTEM_CXX_CORE_ITERATORUTILS_H


#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/FunctionalUtils.hh>
#include <System/C++/Core/IteratorTraits.hh>


namespace __XVI_STD_CORE_NS_DECL
{


namespace ranges
{

namespace __detail
{


// Undefined function.
void iter_move();

template <class _T>
concept __iter_move_alt1 = (is_class_v<_T> || is_enum_v<_T>)
    && requires (_T&& __t) { iter_move(__XVI_STD_NS::forward<_T>(__t)); };

template <class _T>
concept __iter_move_alt2 = requires (_T __t) { *__XVI_STD_NS::forward<_T>(__t); };

struct __iter_move
{
    template <class _T>
        requires __iter_move_alt1<_T>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(iter_move(__XVI_STD_NS::forward<_T>(__t))))
    {
        return iter_move(__XVI_STD_NS::forward<_T>(__t));
    }

    template <class _T>
        requires (!__iter_move_alt1<_T> && __iter_move_alt2<_T>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(*__XVI_STD_NS::forward<_T>(__t)))
    {
        if constexpr (is_rvalue_reference_v<decltype(*__XVI_STD_NS::forward<_T>(__t))>)
            return *__XVI_STD_NS::forward<_T>(__t);
        else
            return __XVI_STD_NS::move(*__XVI_STD_NS::forward<_T>(__t));
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
        *__o = __XVI_STD_NS::forward<_T>(__t);
        *__XVI_STD_NS::forward<_Out>(__o) = __XVI_STD_NS::forward<_T>(__t);
        const_cast<const iter_reference_t<_Out>&&>(*__o) = __XVI_STD_NS::forward<_T>(__t);
        const_cast<const iter_reference_t<_Out>&&>(*__XVI_STD_NS::forward<_Out>(__o)) = __XVI_STD_NS::forward<_T>(__t);
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
concept weakly_incrementable = movable<_I>
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
        *__i++ = __XVI_STD_NS::forward<_T>(__t);
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
        { __XVI_STD_NS::to_address(__i) } -> same_as<add_pointer_t<iter_reference_t<_I>>>;
    };


namespace __detail
{

template <class _T>
struct __indirect_value { using type = iter_value_t<_T>&; };

template <class _T>
using __indirect_value_t = __indirect_value<_T>::type;

} // namespace __detail


template <class _F, class _I>
concept indirectly_unary_invocable = indirectly_readable<_I>
    && copy_constructible<_F>
    && invocable<_F&, __detail::__indirect_value_t<_I>>
    && invocable<_F&, iter_reference_t<_I>>
    && invocable<_F&, iter_common_reference_t<_I>>
    && common_reference_with<invoke_result_t<_F&, iter_value_t<_I>&>, invoke_result_t<_F&, iter_reference_t<_I>>>;

template <class _F, class _I>
concept indirect_regular_unary_invocable = indirectly_readable<_I>
    && copy_constructible<_F>
    && regular_invocable<_F, __detail::__indirect_value_t<_I>>
    && regular_invocable<_F, iter_reference_t<_I>>
    && regular_invocable<_F, iter_common_reference_t<_I>>
    && common_reference_with<invoke_result_t<_F&, iter_value_t<_I>&>, invoke_result_t<_F&, iter_reference_t<_I>>>;

template <class _F, class _I>
concept indirect_unary_predicate = indirectly_readable<_I>
    && copy_constructible<_F>
    && predicate<_F&, __detail::__indirect_value_t<_I>>
    && predicate<_F&, iter_reference_t<_I>>
    && predicate<_F&, iter_common_reference_t<_I>>;

template <class _F, class _I1, class _I2>
concept indirect_binary_predicate = indirectly_readable<_I1>
    && indirectly_readable<_I2>
    && copy_constructible<_F>
    && predicate<_F&, __detail::__indirect_value_t<_I1>, __detail::__indirect_value_t<_I2>>
    && predicate<_F&, __detail::__indirect_value_t<_I1>, iter_reference_t<_I2>>
    && predicate<_F&, iter_reference_t<_I1>, __detail::__indirect_value_t<_I2>>
    && predicate<_F&, iter_reference_t<_I1>, iter_reference_t<_I2>>
    && predicate<_F&, iter_common_reference_t<_I1>, iter_common_reference_t<_I2>>;

template <class _F, class _I1, class _I2 = _I1>
concept indirect_equivalence_relation = indirectly_readable<_I1> && indirectly_readable<_I2>
    && copy_constructible<_F>
    && equivalence_relation<_F&, __detail::__indirect_value_t<_I1>, __detail::__indirect_value_t<_I2>>
    && equivalence_relation<_F&, __detail::__indirect_value_t<_I1>, iter_reference_t<_I2>>
    && equivalence_relation<_F&, iter_reference_t<_I1>, __detail::__indirect_value_t<_I2>>
    && equivalence_relation<_F&, iter_reference_t<_I1>, iter_reference_t<_I2>>
    && equivalence_relation<_F&, iter_common_reference_t<_I1>, iter_common_reference_t<_I2>>;

template <class _F, class _I1, class _I2 = _I1>
concept indirect_strict_weak_order = indirectly_readable<_I1> && indirectly_readable<_I2>
    && copy_constructible<_F>
    && strict_weak_order<_F&, __detail::__indirect_value_t<_I1>, __detail::__indirect_value_t<_I2>>
    && strict_weak_order<_F&, __detail::__indirect_value_t<_I1>, iter_reference_t<_I2>>
    && strict_weak_order<_F&, iter_reference_t<_I1>, __detail::__indirect_value_t<_I2>>
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

namespace __detail
{

template <class _I, class _Proj>
struct __indirect_value<projected<_I, _Proj>> { using type = invoke_result_t<_Proj&, typename __indirect_value<_I>::type>; };

} // namespace __detail


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
constexpr auto __to_unsigned_like(const _T& __t)
{
    return make_unsigned_t<_T>(__t);
}

template <class _T>
concept __lvalue_or_borrowed_range = !is_rvalue_reference_v<_T> || enable_borrowed_range<remove_cvref_t<_T>>;

void begin(auto&) = delete;
void begin(const auto&) = delete;

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
            && requires(_T& __t) { { __DECAY(__t.begin()) } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(__t.begin())))
    {
        return __DECAY(static_cast<_T&>(__t).begin());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T> 
            && !is_array_v<remove_cvref_t<_T>>
            && !requires(_T& __t) { { __DECAY(__t.begin())  } -> input_or_output_iterator; }
            && (is_class_v<remove_cvref_t<_T>> || is_enum_v<remove_cvref_t<_T>>)
            && requires(_T& __t) { { __DECAY(begin(__t)) } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(begin(static_cast<_T&>(__t)))))
    {
        return __DECAY(begin(static_cast<_T&>(__t)));
    }
};

void end(auto&) = delete;
void end(const auto&) = delete;

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
            && requires(_T& __t) { { __DECAY(__t.end()) } -> sentinel_for<decltype(__begin()(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(__t.end())))
    {
        return __DECAY(__t.end());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T> 
            && !is_array_v<remove_cvref_t<_T>>
            && !requires(_T& __t) { { __DECAY(__t.end()) } -> sentinel_for<decltype(__begin()(__t))>; }
            && (is_class_v<remove_cvref_t<_T>> || is_enum_v<remove_cvref_t<_T>>)
            && requires(_T& __t) { { __DECAY(end(__t)) } -> sentinel_for<decltype(__begin()(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(end(static_cast<_T&>(__t)))))
    {
        return __DECAY(end(static_cast<_T&>(__t)));
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

void size(auto&) = delete;
void size(const auto&) = delete;

struct __size
{
    template <class _T>
        requires is_array_v<remove_cvref_t<_T>>
    constexpr decltype(auto) operator()(_T&&) const noexcept
    {
        static_assert(extent_v<remove_cvref_t<_T>> > 0);
        return __DECAY(extent_v<remove_cvref_t<_T>>);
    }

    template <class _T>
        requires (!is_array_v<remove_cvref_t<_T>>
            && !disable_sized_range<remove_cvref_t<_T>>
            && requires(_T&& __t) { __DECAY(__XVI_STD_NS::forward<_T>(__t).size()); }
            && __XVI_STD_CORE_NS::__detail::__is_integer_like<decltype(__DECAY(declval<_T>().size()))>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(__XVI_STD_NS::forward<_T>(__t).size())))
    {
        return __DECAY(__XVI_STD_NS::forward<_T>(__t).size());
    }

    template <class _T>
    requires (!is_array_v<remove_cvref_t<_T>>
        && !disable_sized_range<remove_cvref_t<_T>>
        && (!requires(_T&& __t) { __DECAY(__XVI_STD_NS::forward<_T>(__t).size()); }
                || !__XVI_STD_CORE_NS::__detail::__is_integer_like<decltype(__DECAY(declval<_T>().size()))>)
        && requires(_T&& __t) { __DECAY(size(__XVI_STD_NS::forward<_T>(__t))); }
        && __XVI_STD_CORE_NS::__detail::__is_integer_like<decltype(__DECAY(size(declval<_T>())))>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(size(__XVI_STD_NS::forward<_T>(__t)))))
    {
        return __DECAY(size(__XVI_STD_NS::forward<_T>(__t)));
    }

    template <class _T>
        requires (!is_array_v<remove_cvref_t<_T>>
            && (!requires(_T&& __t) { __DECAY(__XVI_STD_NS::forward<_T>(__t).size()); }
                 || !__XVI_STD_CORE_NS::__detail::__is_integer_like<decltype(__DECAY(declval<_T>().size()))>)
            && (!requires(_T&& __t) { __DECAY(size(__XVI_STD_NS::forward<_T>(__t))); }
                || !__XVI_STD_CORE_NS::__detail::__is_integer_like<decltype(__DECAY(size(declval<_T>())))>)
            && requires(_T&& __t)
                {
                    __to_unsigned_like(__end()(__XVI_STD_NS::forward<_T>(__t)) - __begin()(__XVI_STD_NS::forward<_T>(__t)));
                }
            && sized_sentinel_for<decltype(__end()(declval<_T>())), decltype(__begin()(declval<_T>()))>)
            && forward_iterator<decltype(__begin()(declval<_T>()))>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__end()(__XVI_STD_NS::forward<_T>(__t)) - __begin()(__XVI_STD_NS::forward<_T>(__t))))
    {
        return __end()(__XVI_STD_NS::forward<_T>(__t)) - __begin()(__XVI_STD_NS::forward<_T>(__t));
    }
};

struct __ssize
{
    template <class _T>
        requires requires (_T&& __t) { __size()(__XVI_STD_NS::forward<_T>(__t)); }
    constexpr auto operator()(_T&& __t) const noexcept(noexcept(__size()(__XVI_STD_NS::forward<_T>(__t))))
    {
        auto __s = __size()(__XVI_STD_NS::forward<_T>(__t));

        if constexpr (sizeof(__s) < sizeof(ptrdiff_t))
            return static_cast<ptrdiff_t>(__s);
        else
            return static_cast<make_signed_t<decltype(__s)>>(__s);
    }
};

struct __empty
{
    template <class _T>
        requires requires(_T&& __t) { bool(__XVI_STD_NS::forward<_T>(__t).empty()); }
    constexpr bool operator()(_T&& __t) const
        noexcept(noexcept(bool(__XVI_STD_NS::forward<_T>(__t).empty())))
    {
        return bool(__XVI_STD_NS::forward<_T>(__t).empty());
    }

    template <class _T>
        requires (!requires(_T&& __t) { bool(__XVI_STD_NS::forward<_T>(__t).empty()); }
            && requires(_T&& __t) { __size()(__XVI_STD_NS::forward<_T>(__t)) == 0; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__size()(__XVI_STD_NS::forward<_T>(__t)) == 0))
    {
        return __size()(__XVI_STD_NS::forward<_T>(__t)) == 0;
    }

    template <class _T>
        requires (!requires(_T&& __t) { bool(__XVI_STD_NS::forward<_T>(__t).empty()); }
            && !requires(_T&& __t) { __size()(__XVI_STD_NS::forward<_T>(__t)) == 0; }
            && requires(_T&& __t)
            {
                { __begin()(__XVI_STD_NS::forward<_T>(__t)) } -> forward_iterator;
                bool(__begin()(__XVI_STD_NS::forward<_T>(__t)) == __end()(__XVI_STD_NS::forward<_T>(__t)));
            })
    constexpr bool operator()(_T&& __t) const
        noexcept(noexcept(bool(__begin()(__XVI_STD_NS::forward<_T>(__t)) == __end()(__XVI_STD_NS::forward<_T>(__t)))))
    {
        return bool(__begin()(__XVI_STD_NS::forward<_T>(__t)) == __end()(__XVI_STD_NS::forward<_T>(__t)));
    }
};

struct __data
{
    template <class _T>
        requires (__lvalue_or_borrowed_range<remove_cvref_t<_T>>
            && requires(_T&& __t) { __DECAY(__XVI_STD_NS::forward<_T>(__t).data()); }
            && is_pointer_v<decltype(__DECAY(declval<_T&&>().data()))>
            && is_object_v<remove_pointer_t<decltype(__DECAY(declval<_T&&>().data()))>>)
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(__XVI_STD_NS::forward<_T>(__t).data())))
    {
        return __DECAY(__XVI_STD_NS::forward<_T>(__t).data());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<remove_cvref_t<_T>>
            && (!requires(_T&& __t) { __DECAY(__XVI_STD_NS::forward<_T>(__t).data()); }
                   || !is_pointer_v<decltype(__DECAY(declval<_T&&>().data()))>
                   || !is_object_v<remove_pointer_t<decltype(autoF(declval<_T&&>().data()))>>)
            && requires(_T&& __t) { { __begin()(__XVI_STD_NS::forward<_T>(__t)) } -> contiguous_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(to_address(__begin()(__XVI_STD_NS::forward<_T>(__t)))))
    {
        return to_address(__begin()(__XVI_STD_NS::forward<_T>(__t)));
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
inline namespace __ssize { inline constexpr __detail::__ssize ssize = {}; }
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

template <sized_range _R>
using range_size_t = decltype(ranges::size(declval<_R&>()));

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
        if constexpr (assignable_from<_I&, _S>)
            __i = __XVI_STD_NS::move(__bound);
        else if constexpr (sized_sentinel_for<_S, _I>)
            operator()(__i, __bound - __i);
        else
            while (bool(__i != __bound))
                ++__i;
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
        auto __abs = [](iter_difference_t<_I> __x) { return (__x < 0) ? -__x : __x; };
        
        auto __length = __bound - __i;
        if (__abs(__n) >= __abs(__length))
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
            && bidirectional_iterator<_S>)
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
        requires (!sized_sentinel_for<_S, _I>)
    constexpr iter_difference_t<_I> operator()(_I __first, _S __last) const
    {
        iter_difference_t<_I> __n = 0;
        while (!bool(__first == __last))
            ++__n;
        return __n;
    }

    template <input_or_output_iterator _I, sentinel_for<_I> _S>
    constexpr iter_difference_t<_I> operator()(_I __first, _S __last) const
    {
        return (__last - static_cast<const decay_t<_I>&>(__first));
    }

    template <range _R>
    constexpr range_difference_t<_R> operator()(_R&& __r) const
    {
        if constexpr (sized_range<_R>)
            return static_cast<range_difference_t<_R>>(__size()(__r));
        else
            return __distance()(__begin()(__r), __end()(__r));
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

namespace __detail
{

template <class _I1, class _I2>
void iter_swap(_I1, _I2) = delete;

template <class _X, class _Y>
constexpr iter_value_t<_X> __iter_exchange_move(_X&& __x, _Y&& __y)
    noexcept(noexcept(iter_value_t<_X>(ranges::iter_move(__x))) && noexcept(*__x = ranges::iter_move(__y)))
{
    iter_value_t<_X> __old_value(ranges::iter_move(__x));
    *__x = ranges::iter_move(__y);
    return __old_value;
}

template <class _E1, class _E2>
concept __iter_swap_alt1 = (__enumeration_or_class<_E1> || __enumeration_or_class<_E2>)
    && requires (_E1&& __e1, _E2&& __e2) { iter_swap(__XVI_STD_NS::forward<_E1>(__e1), __XVI_STD_NS::forward<_E2>(__e2)); };

template <class _E1, class _E2>
concept __iter_swap_alt2 = indirectly_readable<_E1>
    && indirectly_readable<_E2>
    && swappable_with<iter_reference_t<_E1>, iter_reference_t<_E2>>;

template <class _E1, class _E2>
concept __iter_swap_alt3 = indirectly_movable_storable<_E1, _E2>
    && indirectly_movable_storable<_E2, _E1>;

struct __iter_swap
{
    template <class _T, class _U>
        requires __iter_swap_alt1<_T, _U>
    constexpr void operator()(_T&& __t, _U&& __u) const noexcept(noexcept(iter_swap(declval<_T>(), declval<_U>())))
    {
        iter_swap(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_U>(__u));
    }

    template <class _T, class _U>
        requires (!__iter_swap_alt1<_T, _U> && __iter_swap_alt2<_T, _U>)
    constexpr void operator()(_T&& __t, _U&& __u) const
        noexcept(noexcept(ranges::swap(*declval<_T>(), *declval<_U>())))
    {
        ranges::swap(*__XVI_STD_NS::forward<_T>(__t), *__XVI_STD_NS::forward<_U>(__u));
    }

    template <class _T, class _U>
        requires (!__iter_swap_alt1<_T, _U> && !__iter_swap_alt2<_T, _U> && __iter_swap_alt3<_T, _U>)
    constexpr void operator()(_T&& __t, _U&& __u) const
        noexcept(noexcept(__iter_exchange_move(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_U>(__u))))
    {
        *__XVI_STD_NS::forward<_T>(__t) = __iter_exchange_move(__XVI_STD_NS::forward<_U>(__u), __XVI_STD_NS::forward<_T>(__t));
    }
};

} // namespace __detail

inline namespace __iter_swap { inline constexpr __detail::__iter_swap iter_swap = {}; }

} // namespace ranges


template <class _In, class _Out>
concept indirectly_copyable = indirectly_readable<_In>
    && indirectly_writable<_Out, iter_reference_t<_In>>;

template <class _In, class _Out>
concept indirectly_copyable_storable = indirectly_copyable<_In, _Out>
    && indirectly_writable<_Out, iter_value_t<_In>&>
    && indirectly_writable<_Out, const iter_value_t<_In>&>
    && indirectly_writable<_Out, iter_value_t<_In>&&>
    && indirectly_writable<_Out, const iter_value_t<_In>&&>
    && copyable<iter_value_t<_In>>
    && constructible_from<iter_value_t<_In>, iter_reference_t<_In>>
    && assignable_from<iter_value_t<_In>&, iter_reference_t<_In>>;

template <class _I1, class _I2 = _I1>
concept indirectly_swappable = indirectly_readable<_I1>
    && indirectly_readable<_I2>
    && requires(const _I1 __i1, const _I2 __i2)
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

template <class _I1, class _I2, class _Out, class _R = ranges::less, class _P1 = identity, class _P2 = identity>
concept mergeable = input_iterator<_I1>
    && input_iterator<_I2>
    && weakly_incrementable<_Out>
    && indirectly_copyable<_I1, _Out>
    && indirectly_copyable<_I2, _Out>
    && indirect_strict_weak_order<_R, projected<_I1, _P1>, projected<_I2, _P2>>;

template <class _I, class _R = ranges::less, class _P = identity>
concept sortable = permutable<_I>
    && indirect_strict_weak_order<_R, projected<_I, _P>>;


namespace ranges
{


struct dangling
{
    constexpr dangling() noexcept = default;

    constexpr dangling(auto&&...) noexcept {}
};

template <range _R>
using borrowed_iterator_t = conditional_t<borrowed_range<_R>, iterator_t<_R>, dangling>;


namespace __detail
{

struct __view_interface_base {};

template <class _T>
struct __is_initializer_list : false_type {};

template <class _T>
struct __is_initializer_list<std::initializer_list<_T>> : true_type {};

template <class _T>
inline constexpr bool __is_initializer_list_v = __is_initializer_list<_T>::value;

} // namespace __detail


struct view_base {};

template <class _T>
inline constexpr bool enable_view =
    derived_from<_T, view_base> || derived_from<_T, __detail::__view_interface_base>;

template <class _T>
concept view = range<_T> && movable<_T> && enable_view<_T>;

template <class _R, class _T>
concept output_range = range<_R> && output_iterator<iterator_t<_R>, _T>;

template <class _T>
concept input_range = range<_T> && input_iterator<iterator_t<_T>>;

template <class _T>
concept forward_range = input_range<_T> && forward_iterator<iterator_t<_T>>;

template <class _T>
concept bidirectional_range = forward_range<_T> && bidirectional_iterator<iterator_t<_T>>;

template <class _T>
concept random_access_range = bidirectional_range<_T> && random_access_iterator<iterator_t<_T>>;

template <class _T>
concept contiguous_range = random_access_range<_T> && contiguous_iterator<iterator_t<_T>>
    && requires(_T& __t)
    {
        { ranges::data(__t) } -> same_as<add_pointer_t<range_reference_t<_T>>>;
    };

template <class _T>
concept common_range = range<_T> && same_as<iterator_t<_T>, sentinel_t<_T>>;

template <class _T>
concept viewable_range = range<_T>
    && ((view<remove_cvref_t<_T>>
            && constructible_from<remove_cvref_t<_T>, _T>)
        || (!view<remove_cvref_t<_T>>
            && (is_lvalue_reference_v<_T>
                || (movable<remove_reference_t<_T>>
                    && !__detail::__is_initializer_list_v<remove_cvref_t<_T>>))));



} // namespace ranges


namespace ranges
{


template <class _I, class _F>
struct in_fun_result
{
    [[no_unique_address]] _I in;
    [[no_unique_address]] _F fun;

    template <class _I2, class _F2>
        requires convertible_to<const _I&, _I2> && convertible_to<const _F&, _F2>
    constexpr operator in_fun_result<_I2, _F2>() const &
    {
        return {in, fun};
    }

    template <class _I2, class _F2>
        requires convertible_to<_I, _I2> && convertible_to<_F, _F2>
    constexpr operator in_fun_result<_I2, _F2>() &&
    {
        return {__XVI_STD_NS::move(in), __XVI_STD_NS::move(fun)};
    }
};

template <class _I1, class _I2>
struct in_in_result
{
    [[no_unique_address]] _I1 in1;
    [[no_unique_address]] _I2 in2;

    template <class _II1, class _II2>
        requires convertible_to<const _I1, _II1> && convertible_to<const _I2&, _II2>
    constexpr operator in_in_result<_II1, _II2>() const &
    {
        return {in1, in2};
    }

    template <class _II1, class _II2>
        requires convertible_to<_I1, _II1> && convertible_to<_I2, _II2>
    constexpr operator in_in_result<_II1, _II2>() &&
    {
        return {__XVI_STD_NS::move(in1), __XVI_STD_NS::move(in2)};
    }
};

template <class _I, class _O>
struct in_out_result
{
    [[no_unique_address]] _I in;
    [[no_unique_address]] _O out;

    template <class _I2, class _O2>
        requires convertible_to<const _I&, _I2> && convertible_to<const _O&, _O2>
    constexpr operator in_out_result<_I2, _O2>() const &
    {
        return {in, out};
    }

    template <class _I2, class _O2>
        requires convertible_to<_I, _I2> && convertible_to<_O, _O2>
    constexpr operator in_out_result<_I2, _O2>() &&
    {
        return {__XVI_STD_NS::move(in), __XVI_STD_NS::move(out)};
    }
};

template <class _I1, class _I2, class _O>
struct in_in_out_result
{
    [[no_unique_address]] _I1 in1;
    [[no_unique_address]] _I2 in2;
    [[no_unique_address]] _O  out;

    template <class _II1, class _II2, class _OO>
        requires convertible_to<const _I1&, _II1> && convertible_to<const _I2&, _II2> && convertible_to<const _O&, _OO>
    constexpr operator in_in_out_result<_II1, _II2, _OO>() const &
    {
        return {in1, in2, out};
    }

    template <class _II1, class _II2, class _OO>
        requires convertible_to<_I1, _II1> && convertible_to<_I2, _II2> && convertible_to<_O, _OO>
    constexpr operator in_in_out_result<_II1, _II2, _OO>() &&
    {
        return {__XVI_STD_NS::move(in1), __XVI_STD_NS::move(in2), __XVI_STD_NS::move(out)};
    }
};

template <class _I, class _O1, class _O2>
struct in_out_out_result
{
    [[no_unique_address]] _I    in;
    [[no_unique_address]] _O1   out1;
    [[no_unique_address]] _O2   out2;

    template <class _II, class _OO1, class _OO2>
        requires convertible_to<const _I&, _II> && convertible_to<const _O1&, _OO1> && convertible_to<const _O2&, _OO2>
    constexpr operator in_out_out_result<_II, _OO1, _OO2>() const &
    {
        return {in, out1, out2};
    }

    template <class _II, class _OO1, class _OO2>
        requires convertible_to<_I, _II> && convertible_to<_O1, _OO1> && convertible_to<_O2, _OO2>
    constexpr operator in_out_out_result<_II, _OO1, _OO2>() &&
    {
        return {__XVI_STD_NS::move(in), __XVI_STD_NS::move(out1), __XVI_STD_NS::move(out2)};
    }
};


template <class _I1, class _I2>
using swap_ranges_result = in_in_result<_I1, _I2>;


namespace __detail
{

struct __swap_ranges
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2>
        requires indirectly_swappable<_I1, _I2>
    constexpr swap_ranges_result<_I1, _I2> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2) const
    {
        for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
            ranges::iter_swap(__first1, __first2);

        return {__first1, __first2};
    }

    template <input_range _R1, input_range _R2>
        requires indirectly_swappable<iterator_t<_R1>, iterator_t<_R2>>
    constexpr swap_ranges_result<borrowed_iterator_t<_R1>, borrowed_iterator_t<_R2>> operator()(_R1&& __r1, _R2&& __r2) const
    {
        return operator()(ranges::begin(__XVI_STD_NS::forward<_R1>(__r1)), ranges::end(__XVI_STD_NS::forward<_R1>(__r1)), ranges::begin(__XVI_STD_NS::forward<_R2>(__r2)), ranges::end(__XVI_STD_NS::forward<_R2>(__r2)));
    }
};

} // namespace __detail

inline namespace __swap_ranges { inline constexpr __detail::__swap_ranges swap_ranges = {}; }


namespace __detail
{

// Now that ranges::swap_ranges is available, the last method of ranges::swap can be defined.
__XVI_STD_CORE_SWAP_RANGES_IMPL;

} // namespace __detail


} // namespace ranges


template <class _T, size_t _N>
    requires std::is_swappable_v<_T>
constexpr void swap(_T (&__a)[_N], _T (&__b)[_N])
    noexcept(std::is_nothrow_swappable_v<_T>)
{
    ranges::swap_ranges(__a, __b);
}


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


template <class _C> constexpr auto size(const _C& __c) -> decltype(__c.size())
    { return __c.size(); }
template <class _T, size_t _N> constexpr size_t size(const _T (&)[_N]) noexcept
    { return _N; }
template <class _C> constexpr auto ssize(const _C& __c) -> common_type_t<ptrdiff_t, make_signed_t<decltype(__c.size())>>
    { return static_cast<common_type_t<ptrdiff_t, make_signed_t<decltype(__c.size())>>>(__c.size()); }
template <class _T, size_t _N> constexpr ptrdiff_t ssize(const _T (&)[_N]) noexcept
    { static_assert(_N <= __XVI_STD_NS::numeric_limits<ptrdiff_t>::max()); return _N; }
template <class _C> [[nodiscard]] constexpr auto empty(const _C& __c) -> decltype(__c.empty())
    { return __c.empty();}
template <class _T, size_t _N> [[nodiscard]] constexpr bool empty(const _T (&)[_N]) noexcept
    { return false; }
template <class _E> [[nodiscard]] constexpr bool empty(std::initializer_list<_E> __il) noexcept
    { return __il.size() == 0; }
template <class _C> constexpr auto data(_C& __c) -> decltype(__c.data())
    { return __c.data(); }
template <class _C> constexpr auto data(const _C& __c) -> decltype(__c.data())
    { return __c.data(); }
template <class _T, size_t _N> constexpr _T* data(_T (&__array)[_N]) noexcept
    { return __array; }
template <class _E> constexpr const _E* data(std::initializer_list<_E> __il) noexcept
    { return __il.begin(); }


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_ITERATORUTILS_H */
