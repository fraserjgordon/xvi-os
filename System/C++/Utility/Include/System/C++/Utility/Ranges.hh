#pragma once
#ifndef __SYSTEM_CXX_UTILITY_RANGES_H
#define __SYSTEM_CXX_UTILITY_RANGES_H


#include <System/C++/LanguageSupport/InitializerList.hh>

#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Tuple.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_CONTAINERS_NS
{

//! @TODO: move to regex namespace.
template <class, class> class match_results;

template <class, class, class> class multiset;
template <class, class, class> class set;
template <class, class, class, class> class unordered_multiset;
template <class, class, class, class> class unordered_set;

} // namespace __XVI_STD_CONTAINERS_NS


namespace __XVI_STD_UTILITY_NS
{


namespace ranges
{


// Forward declarations.
template <InputIterator _I, Sentinel<_I> _S, class _Proj = identity,
          IndirectUnaryPredicate<projected<_I, _Proj>> _Pred>
constexpr _I find_if(_I __first, _S __last, _Pred __pred, _Proj __proj = {});


template <_ForwardingRange _R>
using safe_iterator_t = iterator_t<_R>;


namespace __detail
{

template <class _T, template <class...> class _Of>
struct __is_specialization_of : false_type {};

template <template <class...> class _Template, class... _Args>
struct __is_specialization_of<_Template<_Args...>, _Template> : true_type {};

template <class _T, template <class...> class _Of>
inline constexpr bool __is_specialization_of_v = __is_specialization_of<_T, _Of>::value;

} // namespace __detail


struct view_base {};

template <class _T>
inline constexpr bool enable_view =
    DerivedFrom<_T, view_base>
    ? true
    : (__detail::__is_specialization_of_v<_T, initializer_list>
       || __detail::__is_specialization_of_v<_T, set>
       || __detail::__is_specialization_of_v<_T, multiset>
       || __detail::__is_specialization_of_v<_T, unordered_set>
       || __detail::__is_specialization_of_v<_T, unordered_multiset>
       || __detail::__is_specialization_of_v<_T, match_results>)
    ? false
    : Range<_T> && Range<const _T> && !Same<iter_reference_t<iterator_t<_T>>, iter_reference_t<iterator_t<const _T>>>
    ? false
    : true;

template <class _T>
concept bool View = Range<_T> && Semiregular<_T> && enable_view<_T>;


template <class _R, class _T>
concept bool OutputRange = Range<_T> && OutputIterator<iterator_t<_R>, _T>;

template <class _T>
concept bool InputRange = Range<_T> && InputIterator<iterator_t<_T>>;

template <class _T>
concept bool ForwardRange = InputRange<_T> && ForwardIterator<iterator_t<_T>>;

template <class _T>
concept bool BidirectionalRange = ForwardRange<_T> && BidirectionalIterator<iterator_t<_T>>;

template <class _T>
concept bool RandomAccessRange = BidirectionalRange<_T> && RandomAccessIterator<iterator_t<_T>>;

template <class _T>
concept bool ContiguousRange = RandomAccessRange<_T> && ContiguousIterator<iterator_t<_T>>
    && requires(_T& __t)
    {
        { ranges::data(__t) } -> Same<add_pointer_t<iter_reference_t<iterator_t<_T>>>>;
    };

template <class _T>
concept bool CommonRange = Range<_T> && Same<iterator_t<_T>, sentinel_t<_T>>;

template <class _T>
concept bool ViewableRange = Range<_T> && (_ForwardingRange<_T> || View<decay_t<_T>>);


template <class _R>
concept bool _SimpleView = View<_R> && Range<const _R>
    && Same<iterator_t<_R>, iterator_t<const _R>>
    && Same<sentinel_t<_R>, sentinel_t<const _R>>;

template <InputIterator _I>
concept bool _HasArrow = is_pointer_v<_I> || requires(_I __i) { __i.operator->(); };

template <class _T, class _U>
concept bool _NotSameAs = !Same<remove_cvref_t<_T>, remove_cvref_t<_U>>;

template <class _R>
struct __range_common_iterator_impl
{
    using type = common_iterator<iterator_t<_R>, sentinel_t<_R>>;
};

template <CommonRange _R>
struct __range_common_iterator_impl<_R>
{
    using type = iterator_t<_R>;
};

template <Range _R>
using __range_common_iterator = typename __range_common_iterator_impl<_R>::type;


template <class _D>
    requires is_class_v<_D> && Same<_D, remove_cv_t<_D>>
class view_interface
    : public view_base
{
private:

    constexpr _D& __derived() noexcept
    {
        return static_cast<_D&>(*this);
    }

    constexpr const _D& __derived() const noexcept
    {
        return static_cast<const _D&>(*this);
    }

public:

    constexpr bool empty()
        requires ForwardRange<_D>
    {
        return ranges::begin(__derived()) == ranges::end(__derived());
    }

    constexpr bool empty() const
        requires ForwardRange<const _D>
    {
        return ranges::begin(__derived()) == ranges::end(__derived());
    }

    constexpr explicit operator bool()
        requires requires { ranges::empty(__derived()); }
    {
        return !ranges::empty(__derived());
    }

    constexpr explicit operator bool() const
        requires requires { ranges::empty(__derived()); }
    {
        return !ranges::empty(__derived());
    }

    constexpr auto data()
        requires ContiguousIterator<iterator_t<_D>>
    {
        return ranges::empty(__derived()) ? nullptr : addressof(*ranges::begin(__derived()));
    }

    constexpr auto data() const
        requires Range<const _D> && ContiguousIterator<iterator_t<const _D>>
    {
        return ranges::empty(__derived()) ? nullptr : addressof(*ranges::begin(__derived()));
    }

    constexpr auto size()
        requires ForwardRange<_D> && SizedSentinel<sentinel_t<_D>, iterator_t<_D>>
    {
        return ranges::end(__derived()) - ranges::begin(__derived());
    }

    constexpr auto size() const
        requires ForwardRange<const _D> && SizedSentinel<sentinel_t<const _D>, iterator_t<const _D>>
    {
        return ranges::end(__derived()) - ranges::begin(__derived());
    }

    constexpr decltype(auto) front()
        requires ForwardRange<_D>
    {
        return *ranges::begin(__derived());
    }

    constexpr decltype(auto) front() const
        requires ForwardRange<const _D>
    {
        return *ranges::begin(__derived());
    }

    constexpr decltype(auto) back()
        requires BidirectionalRange<_D> && CommonRange<_D>
    {
        return *ranges::prev(ranges::end(__derived()));
    }

    constexpr decltype(auto) back() const
        requires BidirectionalRange<const _D> && CommonRange<const _D>
    {
        return *ranges::prev(ranges::end(__derived()));
    }

    template <RandomAccessRange _R = _D>
    constexpr decltype(auto) operator[](iter_difference_t<iterator_t<_R>> __n)
    {
        return ranges::begin(__derived())[__n];
    }

    template <RandomAccessRange _R = _D>
    constexpr decltype(auto) operator[](iter_difference_t<iterator_t<_R>> __n) const
    {
        return ranges::begin(__derived())[__n];
    }
};


template <class _T>
concept bool _PairLike = !is_reference_v<_T>
    && requires(_T __t)
    {
        typename tuple_size<_T>::type;
        requires DerivedFrom<tuple_size<_T>, integral_constant<size_t, 2>>;
        typename tuple_element_t<0, remove_const_t<_T>>;
        typename tuple_element_t<1, remove_const_t<_T>>;
        { get<0>(__t) } -> const tuple_element_t<0, _T>&;
        { get<1>(__t) } -> const tuple_element_t<1, _T>&;  
    };

template <class _T, class _U, class _V>
concept bool _PairLikeConvertibleTo = !Range<_T> && _PairLike<remove_reference_t<_T>>
    && requires(_T&& __t)
    {
        { get<0>(__XVI_STD_NS::forward<_T>(__t)) } -> ConvertibleTo<_U>;
        { get<1>(__XVI_STD_NS::forward<_T>(__t)) } -> ConvertibleTo<_V>;
    };

template <class _T, class _U, class _V>
concept bool _PairLikeConvertibleFrom = !Range<_T> && _PairLike<_T> && Constructible<_T, _U, _V>;

template <class _T>
concept bool _IteratorSentinelPair = !Range<_T> && _PairLike<_T>
    && Sentinel<tuple_element_t<1, _T>, tuple_element_t<0, _T>>;

enum class subrange_kind : bool
{
    sized,
    unsized,
};

namespace __detail
{

template <class _I, bool> struct __subrange_store_size
{
    iter_difference_t<_I> __size = 0;
};

template <class _I> struct __subrange_store_size<_I, false>
{
};

} // namespace __detail

template <Iterator _I, Sentinel<_I> _S = _I, subrange_kind _K = SizedSentinel<_S, _I> ? subrange_kind::sized : subrange_kind::unsized>
    requires (_K == subrange_kind::sized || !SizedSentinel<_S, _I>)
class subrange
    : public view_interface<subrange<_I, _S, _K>>
{
private:

    static constexpr bool _StoreSize = (_K == subrange_kind::sized && !SizedSentinel<_S, _I>);
    using __size_storage_t = __detail::__subrange_store_size<_I, _StoreSize>;

    _I _M_begin = _I();
    _S _M_end   = _S();
    [[no_unique_address]] __size_storage_t _M_size;

public:

    constexpr subrange() = default;

    constexpr subrange(_I __i, _S __s)
        requires (!_StoreSize)
        : _M_begin(__i), _M_end(__s)
    {
    }

    constexpr subrange(_I __i, _S __s, iter_difference_t<_I> __n)
        requires (_K == subrange_kind::sized)
        : _M_begin(__i), _M_end(__s)
    {
        if constexpr (_StoreSize)
            _M_size.__size = __n;
    }

    template<_NotSameAs<subrange> _R>
        requires _ForwardingRange<_R>
            && ConvertibleTo<iterator_t<_R>, _I>
            && ConvertibleTo<sentinel_t<_R>, _S>
    constexpr subrange(_R&& __r)
        requires (!_StoreSize || SizedRange<_R>)
        : _M_begin(ranges::begin(__r)),
          _M_end(ranges::end(__r))
    {
        if constexpr (_StoreSize)
            _M_size.__size = ranges::size(__r);
    }

    template <_ForwardingRange _R>
        requires ConvertibleTo<iterator_t<_R>, _I> && ConvertibleTo<sentinel_t<_R>, _I>
    constexpr subrange(_R&& __r, iter_difference_t<_I> __n)
        requires (_K == subrange_kind::sized)
        : subrange(ranges::begin(__r), ranges::end(__r), __n)
    {
    }

    template <_NotSameAs<subrange> _PairLike>
        requires _PairLikeConvertibleTo<_PairLike, _I, _S>
    constexpr subrange(_PairLike&& __r)
        requires (!_StoreSize)
        : subrange{__XVI_STD_NS::get<0>(__XVI_STD_NS::forward<_PairLike>(__r)), __XVI_STD_NS::get<1>(__XVI_STD_NS::forward<_PairLike>(__r))}
    {
    }

    template <_PairLikeConvertibleTo<_I, _S> _PairLike>
    constexpr subrange(_PairLike&& __r, iter_difference_t<_I> __n)
        requires (_K == subrange_kind::sized)
        : subrange{__XVI_STD_NS::get<0>(__XVI_STD_NS::forward<_PairLike>(__r)), __XVI_STD_NS::get<1>(__XVI_STD_NS::forward<_PairLike>(__r)), __n}
    {
    }

    template <_NotSameAs<subrange> _PairLike>
        requires _PairLikeConvertibleFrom<_PairLike, const _I&, const _S&>
    constexpr operator _PairLike() const
    {
        return _PairLike(_M_begin, _M_end);
    }

    constexpr _I begin() const
    {
        return _M_begin;
    }

    constexpr _S end() const
    {
        return _M_end;
    }

    constexpr bool empty() const
    {
        return _M_begin == _M_end;
    }

    constexpr iter_difference_t<_I> size() const
        requires (_K == subrange_kind::sized)
    {
        if constexpr (_StoreSize)
            return _M_size;
        else
            return _M_end - _M_begin;
    }

    [[nodiscard]] constexpr subrange next(iter_difference_t<_I> __n = 1) const
    {
        auto __tmp = *this;
        __tmp.advance(__n);
        return __tmp;
    }

    [[nodiscard]] constexpr subrange subrange_prev(iter_difference_t<_I> __n = 1) const
        requires BidirectionalIterator<_I>
    {
        auto __tmp = *this;
        __tmp.advance(-__n);
        return __tmp;
    }
    
    constexpr subrange& advance(iter_difference_t<_I> __n)
    {
        if constexpr (_StoreSize)
            _M_size -= __n - ranges::advance(_M_begin, __n, _M_end);
        else
            ranges::advance(_M_begin, __n, _M_end);
        return *this;
    }

    friend constexpr _I begin(subrange&& __r)
    {
        return __r.begin();
    }

    friend constexpr _S end(subrange&& __r)
    {
        return __r.end();
    }
};

template <Iterator _I, Sentinel<_I> _S>
subrange(_I, _S, iter_difference_t<_I>) -> subrange<_I, _S, subrange_kind::sized>;

template <_IteratorSentinelPair _P>
subrange(_P) -> subrange<tuple_element_t<0, _P>, tuple_element_t<1, _P>>;

template <_IteratorSentinelPair _P>
subrange(_P, iter_difference_t<tuple_element_t<0, _P>>)
    -> subrange<tuple_element_t<0, _P>, tuple_element_t<1, _P>, subrange_kind::sized>;

template <_ForwardingRange _R>
subrange(_R&&) -> subrange<iterator_t<_R>, sentinel_t<_R>,
                           (SizedRange<_R> || SizedSentinel<sentinel_t<_R>, iterator_t<_R>>)
                                ? subrange_kind::sized : subrange_kind::unsized>;

template <_ForwardingRange _R>
subrange(_R&&, iter_difference_t<iterator_t<_R>>)
    -> subrange<iterator_t<_R>, sentinel_t<_R>, subrange_kind::sized>;

template <size_t _N, class _I, class _S, subrange_kind _K>
    requires (_N < 2)
constexpr auto get(const subrange<_I, _S, _K>& __r)
{
    if constexpr (_N == 0)
        return __r.begin();
    else
        return __r.end();
}


} // namespace ranges


using ranges::get;


namespace ranges
{


template <_ForwardingRange _R>
using safe_subrange_t = subrange<iterator_t<_R>>;


template <class _T>
    requires is_object_v<_T>
class empty_view
    : public view_interface<empty_view<_T>>
{
public:

    static constexpr _T* begin() noexcept
        { return nullptr; }

    static constexpr _T* end() noexcept
        { return nullptr; }

    static constexpr _T* data() noexcept
        { return nullptr; }

    static constexpr ptrdiff_t size() noexcept
        { return 0; }

    static constexpr bool empty() noexcept
        { return true; }

    friend constexpr _T* begin(empty_view) noexcept
        { return nullptr; }

    friend constexpr _T* end(empty_view) noexcept
        { return nullptr; }
};


namespace view
{

template <class _T>
inline constexpr empty_view<_T> empty {};

} // namespace view


namespace __detail
{

template <CopyConstructible _T>
    requires is_object_v<_T>
class __semiregular
    : public optional<_T>
{
public:

    using optional<_T>::optional;

    constexpr __semiregular()
        noexcept(is_nothrow_default_constructible_v<_T>)
        requires DefaultConstructible<_T>
        : __semiregular{in_place}
    {
    }

    __semiregular& operator=(const __semiregular& __that)
        noexcept(is_nothrow_copy_constructible_v<_T>)
        requires !Assignable<_T&, const _T&>
    {
        if (__that)
            optional<_T>::emplace(*__that);
        else
            optional<_T>::reset();
        return *this;
    }

    __semiregular& operator=(__semiregular&& __that)
        noexcept(is_nothrow_move_constructible_v<_T>)
        requires !Assignable<_T&, _T>
    {
        if (__that)
            optional<_T>::emplace(__XVI_STD_NS::move(*__that));
        else
            optional<_T>::reset();
        return *this;
    }
};

} // namespace __detail


template <CopyConstructible _T>
    requires is_object_v<_T>
class single_view
    : public view_interface<single_view<_T>>
{
public:

    single_view() = default;

    constexpr explicit single_view(const _T& __t)
        : _M_value(__t)
    {
    }

    constexpr explicit single_view(_T&& __t)
        : _M_value(__XVI_STD_NS::move(__t))
    {
    }

    template <class... _Args>
        requires Constructible<_T, _Args...>
    constexpr single_view(in_place_t, _Args&&... __args)
        : _M_value{in_place, __XVI_STD_NS::forward<_Args>(__args)...}
    {
    }

    constexpr _T* begin() noexcept
    {
        return data();
    }

    constexpr const _T* begin() const noexcept
    {
        return data();
    }

    constexpr _T* end() noexcept
    {
        return data() + 1;
    }

    constexpr const _T* end() const noexcept
    {
        return data() + 1;
    }

    static constexpr ptrdiff_t size() noexcept
    {
        return 1;
    }

    constexpr _T* data() noexcept
    {
        return _M_value.operator->();
    }

    constexpr const _T* data() const noexcept
    {
        return _M_value.operator->();
    }

private:

    __detail::__semiregular<_T> _M_value;
};


namespace view
{

struct __single
{
    template <class _E>
    constexpr auto operator()(_E&& __e)
    {
        return single_view{__XVI_STD_NS::forward<_E>(__e)};
    }
};

inline constexpr __single single = {};

} // namespace view


template <class _I>
concept bool _Decrementable = Incrementable<_I>
    && requires (_I __i)
    {
        { --__i } -> Same<_I&>;
        { __i-- } -> Same<_I>;
    };

template <class _I>
concept bool _Advanceable = _Decrementable<_I>
    && StrictTotallyOrdered<_I>
    && requires (_I __i, const _I __j, const iter_difference_t<_I> __n)
    {
        { __i += __n } -> Same<_I&>;
        { __i -= __n } -> Same<_I&>;
        { __j +  __n } -> Same<_I>;
        { __j -  __n } -> Same<_I>;
        { __n +  __j } -> Same<_I>;
        { __j -  __j } -> Same<iter_difference_t<_I>>;
    };

template <WeaklyIncrementable _W, Semiregular _Bound = unreachable_sentinel_t>
    requires _WeaklyEqualityComparableWith<_W, _Bound>
class iota_view
    : public view_interface<iota_view<_W, _Bound>>
{
private:

    struct __iterator
    {
    public:

        using iterator_category = conditional_t<_Advanceable<_W>,
                                                random_access_iterator_tag,
                                                conditional_t<_Decrementable<_W>,
                                                              bidirectional_iterator_tag,
                                                              conditional_t<Incrementable<_W>,
                                                                            forward_iterator_tag,
                                                                            input_iterator_tag>
                                                               >
                                                >;
        using value_type = _W;
        using difference_type = iter_difference_t<_W>;

        __iterator() = default;

        constexpr explicit __iterator(_W __value)
            : _M_value(__XVI_STD_NS::move(__value))
        {
        }

        constexpr _W operator*() const
            noexcept(is_nothrow_copy_constructible_v<_W>)
        {
            return _M_value;
        }

        constexpr __iterator& operator++()
        {
            ++_M_value;
            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        constexpr __iterator operator++(int)
            requires Incrementable<_W>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires _Decrementable<_W>
        {
            --_M_value;
            return *this;
        }

        constexpr __iterator operator--(int)
            requires _Decrementable<_W>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        constexpr __iterator& operator+=(difference_type __n)
            requires _Advanceable<_W>
        {
            _M_value += __n;
            return *this;
        }

        constexpr __iterator& operator-=(difference_type __n)
            requires _Advanceable<_W>
        {
            _M_value -= __n;
            return *this;
        }

        constexpr _W operator[](difference_type __n) const
            requires _Advanceable<_W>
        {
            return _M_value + __n;
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires EqualityComparable<_W>
        {
            return __x._M_value == __y._M_value;
        }

        friend constexpr bool operator!=(const __iterator& __x, const __iterator& __y)
            requires EqualityComparable<_W>
        {
            return !(__x == __y);
        }

        friend constexpr bool operator<(const __iterator& __x, const __iterator& __y)
            requires StrictTotallyOrdered<_W>
        {
            return __x._M_value < __y._M_value;
        }

        friend constexpr bool operator>(const __iterator& __x, const __iterator& __y)
            requires StrictTotallyOrdered<_W>
        {
            return __y < __x;
        }

        friend constexpr bool operator<=(const __iterator& __x, const __iterator& __y)
            requires StrictTotallyOrdered<_W>
        {
            return !(__y < __x);
        }

        friend constexpr bool operator>=(const __iterator& __x, const __iterator& __y)
            requires StrictTotallyOrdered<_W>
        {
            return !(__x < __y);
        }

        friend constexpr __iterator operator+(__iterator __i, difference_type __n)
            requires _Advanceable<_W>
        {
            return __iterator{__i._M_value + __n};
        }

        friend constexpr __iterator operator+(difference_type __n, __iterator __i)
            requires _Advanceable<_W>
        {
            return __i + __n;
        }

        friend constexpr __iterator operator-(__iterator __i, difference_type __n)
            requires _Advanceable<_W>
        {
            return __i + -__n;
        }

        friend constexpr difference_type operator-(const __iterator& __i, const __iterator& __j)
            requires _Advanceable<_W>
        {
            return __i._M_value - __j._M_value;
        }

    private:

        _W _M_value = _W();
    };

    struct __sentinel
    {
    public:

        __sentinel() = default;

        constexpr explicit __sentinel(_Bound __bound)
            : _M_bound(__XVI_STD_NS::move(__bound))
        {
        }

        friend constexpr bool operator==(const __iterator& __x, const __sentinel& __y)
        {
            return __x._M_value == __y._M_bound;
        }

        friend constexpr bool operator==(const __sentinel& __x, const __iterator& __y)
        {
            return __y == __x;
        }

        friend constexpr bool operator!=(const __iterator& __x, const __sentinel& __y)
        {
            return !(__x == __y);
        }

        friend constexpr bool operator!=(const __iterator& __x, const __sentinel& __y)
        {
            return !(__y == __x);
        }

    private:

        _Bound _M_bound = _Bound();
    };

public:

    iota_view() = default;

    constexpr explicit iota_view(_W __value)
        : _M_value(__XVI_STD_NS::move(__value))
    {
    }

    constexpr iota_view(type_identity_t<_W> __value, type_identity_t<_Bound> __bound)
        : _M_value(__XVI_STD_NS::move(__value)), _M_bound(__XVI_STD_NS::move(__bound))
    {
    }

    constexpr __iterator begin() const
    {
        return __iterator{_M_value};
    }

    constexpr __sentinel end() const
    {
        return __sentinel{_M_bound};
    }

    constexpr __iterator end() const
        requires Same<_W, _Bound>
    {
        return __iterator{_M_bound};
    }

    constexpr auto size() const
        requires (Same<_W, _Bound> && _Advanceable<_W>)
            || (Integral<_W> && Integral<_Bound>)
            || SizedSentinel<_Bound, _W>
    {
        return _M_bound - _M_value;
    }

private:

    _W _M_value = _W();
    _Bound _M_bound = _Bound();
};

template <class _W, class _Bound>
    requires (!Integral<_W> || !Integral<_Bound> || is_signed_v<_W> == is_signed_v<_Bound>)
iota_view(_W, _Bound) -> iota_view<_W, _Bound>;


namespace view
{

struct __iota
{
    template <class _E>
    constexpr auto operator()(_E&& __e)
    {
        return iota_view{__XVI_STD_NS::forward<_E>(__e)};
    }

    template <class _E, class _F>
    constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return iota_view{__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)};
    }
};

inline constexpr __iota iota = {};

} // namespace view


namespace __detail
{

template <Range _R>
    requires is_object_v<_R>
class __ref_view
    : public view_interface<__ref_view<_R>>
{
public:

    static void __fun(_R&);
    static void __fun(_R&&) = delete;

    constexpr __ref_view() noexcept = default;

    template <_NotSameAs<__ref_view> _T>
        requires ConvertibleTo<_T, _R&> && requires { __fun(declval<_T>()); }
    constexpr __ref_view(_T&& __t)
        : _M_r(addressof(static_cast<_R&>(__XVI_STD_NS::forward<_T>(__t))))
    {
    }

    constexpr _R& base() const
    {
        return *_M_r;
    }

    constexpr iterator_t<_R> begin() const
    {
        return ranges::begin(*_M_r);
    }

    constexpr sentinel_t<_R> end() const
    {
        return ranges::end(*_M_r);
    }

    constexpr bool empty() const
        requires requires { ranges::empty(declval<const _R&>()); }
    {
        return ranges::empty(*_M_r);
    }

    constexpr bool size() const
        requires SizedRange<_R>
    {
        return ranges::size(*_M_r);
    }

    constexpr auto data() const
        requires ContiguousRange<_R>
    {
        return ranges::data(*_M_r);
    }

    friend constexpr iterator_t<_R> begin(__ref_view __r)
    {
        return __r.begin();
    }

    friend constexpr sentinel_t<_R> end(__ref_view __r)
    {
        return __r.end();
    }

private:

    _R* _M_r = nullptr;
};

template <class _T>
concept bool _RefView = requires { __ref_view{declval<_T>()}; };

struct __range_adaptor_closure
{
    //! @TODO.
};

} // namespace __detail


namespace view
{

struct __all
    : public __detail::__range_adaptor_closure
{
    template <ViewableRange _E>
    constexpr auto operator()(_E&& __e)
    {
        if constexpr (View<decltype(__decay_copy(__XVI_STD_NS::forward<_E>(__e)))>)
            return __decay_copy(__XVI_STD_NS::forward<_E>(__e));
        else if (__detail::_RefView<_E>)
            return __detail::__ref_view{__XVI_STD_NS::forward<_E>(__e)};
        else
            return subrange{__XVI_STD_NS::forward<_E>(__e)};
    }

    template <ViewableRange _R>
    friend constexpr auto operator|(_R&& __r, const __all& __a)
    {
        return __a(__XVI_STD_NS::forward<_R>(__r));
    }
};

inline constexpr __all all = all;

} // namespace view


template <ViewableRange _R>
using all_view = decltype(view::all(declval<_R>()));


template <InputRange _V, IndirectUnaryPredicate<iterator_t<_V>> _Pred>
    requires View<_V> && is_object_v<_Pred>
class filter_view
    : public view_interface<filter_view<_V, _Pred>>
{
private:

    class __iterator
    {
    public:

        using iterator_concept  = conditional_t<BidirectionalRange<_V>,
                                                random_access_iterator_tag,
                                                conditional_t<ForwardRange<_V>,
                                                              forward_iterator_tag,
                                                              input_iterator_tag>
                                                >;
        using iterator_category = conditional_t<DerivedFrom<typename iterator_traits<iterator_t<_V>>::iterator_category, bidirectional_iterator_tag>,
                                                bidirectional_iterator_tag,
                                                conditional_t<DerivedFrom<typename iterator_traits<iterator_t<_V>>::iterator_category, forward_iterator_tag>,
                                                              forward_iterator_tag,
                                                              input_iterator_tag>
                                                >;
        using value_type        = iter_value_t<iterator_t<_V>>;
        using difference_type   = iter_difference_t<iterator_t<_V>>;

        __iterator() = default;

        constexpr __iterator(filter_view& __parent, iterator_t<_V> __current)
            : _M_current(__XVI_STD_NS::move(__current)), _M_parent(addressof(__parent))
        {
        }

        constexpr iterator_t<_V> base() const
        {
            return _M_current;
        }

        constexpr iter_reference_t<iterator_t<_V>> operator*() const
        {
            return *_M_current;
        }

        constexpr iterator_t<_V> operator->() const
            requires _HasArrow<iterator_t<_V>>
        {
            return _M_current;
        }

        constexpr __iterator& operator++()
        {
            _M_current = ranges::find_if(++_M_current, ranges::end(_M_parent->_M_base), ref(*_M_parent->_M_pred));
            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        constexpr __iterator& operator++(int)
            requires ForwardRange<_V>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires BidirectionalRange<_V>
        {
            do
                --_M_current;
            while (!invoke(*_M_parent->_M_pred, *_M_current));
            return *this;
        }

        constexpr __iterator operator--(int)
            requires BidirectionalRange<_V>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires EqualityComparable<iterator_t<_V>>
        {
            return __x._M_current == __y._M_current;
        }

        friend constexpr bool operator!=(const __iterator& __x, const __iterator& __y)
            requires EqualityComparable<iterator_t<_V>>
        {
            return !(__x == __y);
        }

        friend constexpr iter_rvalue_reference_t<iterator_t<_V>> iter_move(const __iterator& __i)
            noexcept(noexcept(ranges::iter_move(__i._M_current)))
        {
            return ranges::iter_move(__i._M_current);
        }

        friend constexpr void iter_swap(const __iterator& __x, const __iterator& __y)
            noexcept(noexcept(ranges::iter_swap(__x._M_current, __y._M_current)))
            requires IndirectlySwappable<iterator_t<_V>>
        {
            return ranges::iter_swap(__x._M_current, __y._M_current);
        }

    private:

        iterator_t<_V> _M_current = iterator_t<_V>();
        filter_view* _M_parent = nullptr;
    };

    class __sentinel
    {
    public:

        __sentinel() = default;

        constexpr explicit __sentinel(filter_view& __parent)
            : _M_end(ranges::end(__parent))
        {
        }

        constexpr sentinel_t<_V> base() const
        {
            return _M_end;
        }

        friend constexpr bool operator==(const __iterator& __x, const __sentinel& __y)
        {
            return __x._M_current == __y._M_end;
        }

        friend constexpr bool operator==(const __sentinel& __x, const __iterator& __y)
        {
            return __y == __x;
        }

        friend constexpr bool operator!=(const __iterator& __x, const __sentinel& __y)
        {
            return !(__x == __y);
        }

        friend constexpr bool operator!=(const __sentinel& __x, const __iterator& __y)
        {
            return !(__y == __x);
        }

    private:

        sentinel_t<_V> _M_end = sentinel_t<_V>();
    };

public:

    filter_view() = default;
    
    constexpr filter_view(_V __base, _Pred __pred)
        : _M_base(__XVI_STD_NS::move(__base)), _M_pred(__XVI_STD_NS::move(__pred))
    {
    }

    template <InputRange _R>
        requires ViewableRange<_R> && Constructible<_V, all_view<_R>>
    constexpr filter_view(_R&& __r, _Pred __pred)
        : _M_base(view::all(__XVI_STD_NS::forward<_R>(__r))),
          _M_pred(__XVI_STD_NS::move(__pred))
    {
    }

    constexpr _V base() const
    {
        return _M_base;
    }

    constexpr __iterator begin()
    {
        return {*this, ranges::find_if(_M_base, ref(*_M_pred))};
    }

    constexpr auto end()
    {
        if constexpr (CommonRange<_V>)
            return __iterator{*this, ranges::end(*_M_base)};
        else
            return __sentinel{*this};
    }

private:

    _V _M_base = _V();
    __detail::__semiregular<_Pred> _M_pred;
};


namespace view
{

struct __filter
{
    template <class _P>
    struct __closure
        : public __detail::__range_adaptor_closure
    {
        _P _M_p;

        __closure(_P&& __p)
            : _M_p(__XVI_STD_NS::forward<_P>(__p))
        {
        }

        template <class _E>
        constexpr auto operator()(_E&& __e)
        {
            return filter_view{__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_P>(_M_p)};
        }

        template <ViewableRange _E>
        friend auto operator|(__closure& __c, _E&& __e)
        {
            return __c(__XVI_STD_NS::forward<_E>(__e));
        }
    };
    
    template <class _P>
    constexpr __closure<_P> operator()(_P&& __p)
    {
        return __closure(__XVI_STD_NS::forward<_P>(__p));
    }

    template <ViewableRange _E, class _P>
    constexpr auto operator()(_E&& __e, _P&& __p)
    {
        return operator()(__XVI_STD_NS::forward<_P>(__p))(__XVI_STD_NS::forward<_E>(__e));
    }
};

inline constexpr __filter filter = {};

} // namespace view


template <InputRange _V, CopyConstructible _F>
    requires View<_V> && is_object_v<_F> && RegularInvocable<_F&, iter_reference_t<iterator_t<_V>>>
class transform_view;


namespace view
{

//inline constexpr ??? transform = ...;

} // namespace view


template <View> class take_view;


namespace view
{

//inline constexpr ??? take = ...;

} // namespace view


template <InputRange _V>
    requires View<_V> && InputRange<iter_reference_t<iterator_t<_V>>>
        && (is_reference_v<iter_reference_t<iterator_t<_V>>>
            || View<iter_value_t<iterator_t<_V>>>)
class join_view;


namespace view
{

//inline constexpr ??? join = ...;

} // namespace view


template <class _R>
concept bool _TinyRange = false;//...;

template <InputRange _V, ForwardRange _Pattern>
    requires View<_V> && View<_Pattern>
        && IndirectlyComparable<iterator_t<_V>, iterator_t<_Pattern>, ranges::equal_to<>>
        && (ForwardRange<_V> || _TinyRange<_Pattern>)
class split_view;


namespace view
{

//inline constexpr ??? split = ...;

} // namespace view


namespace view
{

//inline constexpr ??? counted = ...;

} // namespace view


template <View _V>
    requires (!CommonRange<_V>)
class common_view;


namespace view
{

//inline constexpr ??? common = ...;

} // namespace view


template <View _V>
    requires BidirectionalRange<_V>
class reverse_view;


namespace view
{

//inline constexpr ??? reverse = ...;

} // namespace view


} // namespace ranges


namespace view = ranges::view;


template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_size<ranges::subrange<_I, _S, _K>>
    : integral_constant<size_t, 2> {};

template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_element<0, ranges::subrange<_I, _S, _K>>
{
    using type = _I;
};

template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_element<1, ranges::subrange<_I, _S, _K>>
{
    using type = _S;
};


} // namespace __XVI_STD_UTILITY_NS


// Required for the implementation of ranges::find_if and we have a mutual dependency.
#include <System/C++/Utility/Algorithm.hh>


#endif /* ifndef __SYSTEM_CXX_UTILITY_RANGES_H */
