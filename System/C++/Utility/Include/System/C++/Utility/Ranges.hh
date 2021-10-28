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


#if __cpp_concepts
namespace ranges
{

// Forward declarations.
template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
constexpr _I find_if(_I __first, _S __last, _Pred __pred, _Proj __proj = {});

template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
constexpr _I find_if_not(_I __first, _S __last, _Pred __pred, _Proj __proj = {});


namespace __detail
{

// Forward declarations of algorithm CPOs.
struct __equal_to;
struct __mismatch;

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
    derived_from<_T, view_base>
    ? true
    : (__detail::__is_specialization_of_v<_T, initializer_list>
       || __detail::__is_specialization_of_v<_T, set>
       || __detail::__is_specialization_of_v<_T, multiset>
       || __detail::__is_specialization_of_v<_T, unordered_set>
       || __detail::__is_specialization_of_v<_T, unordered_multiset>
       || __detail::__is_specialization_of_v<_T, match_results>)
    ? false
    : range<_T> && range<const _T> && !same_as<range_reference_t<_T>, range_reference_t<_T>>
    ? false
    : true;

template <class _T>
concept view = range<_T> && movable<_T> && default_constructible<_T> && enable_view<_T>;


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
concept viewable_range = range<_T> && (borrowed_range<_T> || view<decay_t<_T>>);


namespace __detail
{

template <class _R>
concept __simple_view = view<_R> && range<const _R>
    && same_as<iterator_t<_R>, iterator_t<const _R>>
    && same_as<sentinel_t<_R>, sentinel_t<const _R>>;

template <class _I>
concept __has_arrow = input_iterator<_I> && (is_pointer_v<_I> || requires(_I __i) { __i.operator->(); });

template <class _T, class _U>
concept __not_same_as = !same_as<remove_cvref_t<_T>, remove_cvref_t<_U>>;

} // namespace __detail


template <class _D>
    requires is_class_v<_D> && same_as<_D, remove_cv_t<_D>>
class view_interface :
    public view_base
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
        requires forward_range<_D>
    {
        return ranges::begin(__derived()) == ranges::end(__derived());
    }

    constexpr bool empty() const
        requires forward_range<const _D>
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
        requires contiguous_iterator<iterator_t<_D>>
    {
        return to_address(ranges::begin(__derived()));
    }

    constexpr auto data() const
        requires range<const _D> && contiguous_iterator<iterator_t<const _D>>
    {
        return to_address(ranges::begin(__derived()));
    }

    constexpr auto size()
        requires forward_range<_D> && sized_sentinel_for<sentinel_t<_D>, iterator_t<_D>>
    {
        return ranges::end(__derived()) - ranges::begin(__derived());
    }

    constexpr auto size() const
        requires forward_range<const _D> && sized_sentinel_for<sentinel_t<const _D>, iterator_t<const _D>>
    {
        return ranges::end(__derived()) - ranges::begin(__derived());
    }

    constexpr decltype(auto) front()
        requires forward_range<_D>
    {
        return *ranges::begin(__derived());
    }

    constexpr decltype(auto) front() const
        requires forward_range<const _D>
    {
        return *ranges::begin(__derived());
    }

    constexpr decltype(auto) back()
        requires bidirectional_range<_D> && common_range<_D>
    {
        return *ranges::prev(ranges::end(__derived()));
    }

    constexpr decltype(auto) back() const
        requires bidirectional_range<const _D> && common_range<const _D>
    {
        return *ranges::prev(ranges::end(__derived()));
    }

    template <random_access_range _R = _D>
    constexpr decltype(auto) operator[](iter_difference_t<iterator_t<_R>> __n)
    {
        return ranges::begin(__derived())[__n];
    }

    template <random_access_range _R = const _D>
    constexpr decltype(auto) operator[](iter_difference_t<iterator_t<_R>> __n) const
    {
        return ranges::begin(__derived())[__n];
    }
};


namespace __detail
{

template <class _T>
concept __pair_like = !is_reference_v<_T>
    && requires(_T __t)
    {
        typename tuple_size<_T>::type;
        requires derived_from<tuple_size<_T>, integral_constant<size_t, 2>>;
        typename tuple_element_t<0, remove_const_t<_T>>;
        typename tuple_element_t<1, remove_const_t<_T>>;
        { get<0>(__t) } -> convertible_to<const tuple_element_t<0, _T>&>;
        { get<1>(__t) } -> convertible_to<const tuple_element_t<1, _T>&>;  
    };

template <class _T, class _U, class _V>
concept __pair_like_convertible_to = !range<_T> && __pair_like<remove_reference_t<_T>>
    && requires(_T&& __t)
    {
        { get<0>(__XVI_STD_NS::forward<_T>(__t)) } -> convertible_to<_U>;
        { get<1>(__XVI_STD_NS::forward<_T>(__t)) } -> convertible_to<_V>;
    };

template <class _T, class _U, class _V>
concept __pair_like_convertible_from = !range<_T> && __pair_like<_T> && constructible_from<_T, _U, _V>;

template <class _T>
concept __iterator_sentinel_pair = !range<_T> && __pair_like<_T>
    && sentinel_for<tuple_element_t<1, _T>, tuple_element_t<0, _T>>;

} // namespace __detail

enum class subrange_kind : bool
{
    sized,
    unsized,
};

namespace __detail
{

template <class _I, bool> struct __subrange_store_size
{
    using __type = make_unsigned_t<iter_difference_t<_I>>;
    
    __type __size = 0;
};

template <class _I> struct __subrange_store_size<_I, false>
{
    using __type = make_unsigned_t<iter_difference_t<_I>>;
};

} // namespace __detail

template <input_or_output_iterator _I, sentinel_for<_I> _S = _I, subrange_kind _K = sized_sentinel_for<_S, _I> ? subrange_kind::sized : subrange_kind::unsized>
    requires (_K == subrange_kind::sized || !sized_sentinel_for<_S, _I>)
class subrange
    : public view_interface<subrange<_I, _S, _K>>
{
private:

    static constexpr bool _StoreSize = (_K == subrange_kind::sized && !sized_sentinel_for<_S, _I>);
    using __size_storage_t = __detail::__subrange_store_size<_I, _StoreSize>;
    using __size_type = typename __size_storage_t::__type;

    _I _M_begin = _I();
    _S _M_end   = _S();
    [[no_unique_address]] __size_storage_t _M_size;


public:

    subrange() = default;

    constexpr subrange(_I __i, _S __s)
        requires (!_StoreSize)
        : _M_begin(__i), _M_end(__s)
    {
    }

    constexpr subrange(_I __i, _S __s, __size_type __n)
        requires (_K == subrange_kind::sized)
        : _M_begin(__i), _M_end(__s)
    {
        if constexpr (_StoreSize)
            _M_size.__size = __n;
    }

    template<__detail::__not_same_as<subrange> _R>
        requires borrowed_range<_R>
            && convertible_to<iterator_t<_R>, _I>
            && convertible_to<sentinel_t<_R>, _S>
    constexpr subrange(_R&& __r)
        requires (!_StoreSize || sized_range<_R>)
        : _M_begin(ranges::begin(__r)),
          _M_end(ranges::end(__r))
    {
        if constexpr (_StoreSize)
            _M_size.__size = ranges::size(__r);
    }

    template <borrowed_range _R>
        requires convertible_to<iterator_t<_R>, _I> && convertible_to<sentinel_t<_R>, _I>
    constexpr subrange(_R&& __r, __size_type __n)
        requires (_K == subrange_kind::sized)
        : subrange{ranges::begin(__r), ranges::end(__r), __n}
    {
    }

    template <__detail::__not_same_as<subrange> _PairLike>
        requires __detail::__pair_like_convertible_to<_PairLike, _I, _S>
    constexpr subrange(_PairLike&& __r)
        requires (!_StoreSize)
        : subrange{__XVI_STD_NS::get<0>(__XVI_STD_NS::forward<_PairLike>(__r)), __XVI_STD_NS::get<1>(__XVI_STD_NS::forward<_PairLike>(__r))}
    {
    }

    template <__detail::__pair_like_convertible_to<_I, _S> _PairLike>
    constexpr subrange(_PairLike&& __r, __size_type __n)
        requires (_K == subrange_kind::sized)
        : subrange{__XVI_STD_NS::get<0>(__XVI_STD_NS::forward<_PairLike>(__r)), __XVI_STD_NS::get<1>(__XVI_STD_NS::forward<_PairLike>(__r)), __n}
    {
    }

    template <__detail::__not_same_as<subrange> _PairLike>
        requires __detail::__pair_like_convertible_from<_PairLike, const _I&, const _S&>
    constexpr operator _PairLike() const
    {
        return _PairLike(_M_begin, _M_end);
    }

    constexpr _I begin() const
        requires copyable<_I>
    {
        return _M_begin;
    }

    [[nodiscard]] constexpr _I begin()
        requires (!copyable<_I>)
    {
        return std::move(_M_begin);
    }

    constexpr _S end() const
    {
        return _M_end;
    }

    constexpr bool empty() const
    {
        return _M_begin == _M_end;
    }

    constexpr __size_type size() const
        requires (_K == subrange_kind::sized)
    {
        if constexpr (_StoreSize)
            return _M_size;
        else
            return static_cast<__size_type>(_M_end - _M_begin);
    }

    [[nodiscard]] constexpr subrange next(iter_difference_t<_I> __n = 1) const &
        requires forward_iterator<_I>
    {
        auto __tmp = *this;
        __tmp.advance(__n);
        return __tmp;
    }

    [[nodiscard]] constexpr subrange next(iter_difference_t<_I> __n = 1) &&
    {
        advance(__n);
        return std::move(*this);
    }

    [[nodiscard]] constexpr subrange subrange_prev(iter_difference_t<_I> __n = 1) const
        requires bidirectional_iterator<_I>
    {
        auto __tmp = *this;
        __tmp.advance(-__n);
        return __tmp;
    }
    
    constexpr subrange& advance(iter_difference_t<_I> __n)
    {
        if constexpr (_StoreSize)
        {
            auto __d = __n - ranges::advance(_M_begin, __n, _M_end);
            if (__d >= 0)
                _M_size -= static_cast<__size_type>(__d);
            else
                _M_size += static_cast<__size_type>(-__d);
        }
        else
            ranges::advance(_M_begin, __n, _M_end);

        return *this;
    }
};

template <input_or_output_iterator _I, sentinel_for<_I> _S>
subrange(_I, _S, make_unsigned_t<iter_difference_t<_I>>) -> subrange<_I, _S, subrange_kind::sized>;

template <__detail::__iterator_sentinel_pair _P>
subrange(_P) -> subrange<tuple_element_t<0, _P>, tuple_element_t<1, _P>>;

template <__detail::__iterator_sentinel_pair _P>
subrange(_P, make_unsigned_t<iter_difference_t<tuple_element_t<0, _P>>>)
    -> subrange<tuple_element_t<0, _P>, tuple_element_t<1, _P>, subrange_kind::sized>;

template <borrowed_range _R>
subrange(_R&&) -> subrange<iterator_t<_R>, sentinel_t<_R>,
                           (sized_range<_R> || sized_sentinel_for<sentinel_t<_R>, iterator_t<_R>>)
                                ? subrange_kind::sized : subrange_kind::unsized>;

template <borrowed_range _R>
subrange(_R&&, make_unsigned_t<iter_difference_t<iterator_t<_R>>>)
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

template <size_t _N, class _I, class _S, subrange_kind _K>
    requires (_N < 2)
constexpr auto get(subrange<_I, _S, _K>&& __r)
{
    if constexpr (_N == 0)
        return __r.begin();
    else
        return __r.end();
}


template <input_or_output_iterator _I, sentinel_for<_I> _S, subrange_kind _K>
inline constexpr bool enable_borrowed_range<subrange<_I, _S, _K>> = true;


} // namespace ranges


using ranges::get;


namespace ranges
{

struct dangling
{
    constexpr dangling() noexcept = default;

    template <class... _Args>
    constexpr dangling(_Args&&...) noexcept
    {
    }
};

template <range _R>
using safe_iterator_t = conditional_t<borrowed_range<_R>, iterator_t<_R>, dangling>;

template <range _R>
using safe_subrange_t = conditional_t<borrowed_range<_R>, subrange<iterator_t<_R>>, dangling>;


} // namespace ranges
#endif // if __cpp_ranges


} // namespace __XVI_STD_UTILITY_NS


// Algorithm depends on things defined above here; the views below depend on Algorithm.
#include <System/C++/Utility/Algorithm.hh>


namespace __XVI_STD_UTILITY_NS
{


#if __cpp_concepts
namespace ranges
{


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
};


template <class _T>
inline constexpr bool enable_borrowed_range<empty_view<_T>> = true;


namespace views
{

template <class _T>
inline constexpr empty_view<_T> empty {};

} // namespace views


namespace __detail
{

template <copy_constructible _T>
    requires is_object_v<_T>
class __semiregular_box
    : public optional<_T>
{
public:

    using optional<_T>::optional;

    constexpr __semiregular_box()
        noexcept(is_nothrow_default_constructible_v<_T>)
        requires default_constructible<_T>
        : __semiregular_box{in_place}
    {
    }

    __semiregular_box& operator=(const __semiregular_box& __that)
        noexcept(is_nothrow_copy_constructible_v<_T>)
        requires (!assignable_from<_T&, const _T&>)
    {
        if (__that)
            optional<_T>::emplace(*__that);
        else
            optional<_T>::reset();
        return *this;
    }

    __semiregular_box& operator=(__semiregular_box&& __that)
        noexcept(is_nothrow_move_constructible_v<_T>)
        requires (!assignable_from<_T&, _T>)
    {
        if (__that)
            optional<_T>::emplace(__XVI_STD_NS::move(*__that));
        else
            optional<_T>::reset();
        return *this;
    }
};

} // namespace __detail


template <copy_constructible _T>
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
        requires constructible_from<_T, _Args...>
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

    __detail::__semiregular_box<_T> _M_value;
};


namespace views
{

struct __single_t
{
    template <class _E>
    constexpr auto operator()(_E&& __e)
    {
        return single_view{__XVI_STD_NS::forward<_E>(__e)};
    }
};

inline namespace __single { inline constexpr __single_t single = {}; }

} // namespace view


namespace __detail
{

template <class _W>
using __iota_diff_t = conditional_t<(!is_integral_v<_W> || sizeof(iter_difference_t<_W>) > sizeof(_W)),
                                     iter_difference_t<_W>,
                                     make_signed_t<_W>>;

template <class _I>
concept __decrementable = incrementable<_I>
    && requires (_I __i)
    {
        { --__i } -> same_as<_I&>;
        { __i-- } -> same_as<_I>;
    };

template <class _I>
concept __advanceable = __decrementable<_I>
    && totally_ordered<_I>
    && requires (_I __i, const _I __j, const __iota_diff_t<_I> __n)
    {
        { __i += __n } -> same_as<_I&>;
        { __i -= __n } -> same_as<_I&>;
        _I(__j + __n);
        _I(__n + __j);
        _I(__j - __n);
        { __j -  __j } -> convertible_to<__iota_diff_t<_I>>;
    };

} // namespace __detail

template <weakly_incrementable _W, semiregular _Bound = unreachable_sentinel_t>
    requires __XVI_STD_TYPETRAITS_NS::__detail::__weakly_equality_comparable_with<_W, _Bound>
class iota_view
    : public view_interface<iota_view<_W, _Bound>>
{
private:

    struct __iterator
    {
    public:

        using iterator_category = conditional_t<__detail::__advanceable<_W>,
                                                random_access_iterator_tag,
                                                conditional_t<__detail::__decrementable<_W>,
                                                              bidirectional_iterator_tag,
                                                              conditional_t<incrementable<_W>,
                                                                            forward_iterator_tag,
                                                                            input_iterator_tag>
                                                               >
                                                >;
        using value_type = _W;
        using difference_type = __detail::__iota_diff_t<_W>;

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
            requires incrementable<_W>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires __detail::__decrementable<_W>
        {
            --_M_value;
            return *this;
        }

        constexpr __iterator operator--(int)
            requires __detail::__decrementable<_W>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        constexpr __iterator& operator+=(difference_type __n)
            requires __detail::__advanceable<_W>
        {
            if constexpr (__XVI_STD_UTILITY_NS::__detail::__is_integer_like<_W> 
                          && !__XVI_STD_UTILITY_NS::__detail::__is_signed_integer_like<_W>)
            {
                if (__n >= difference_type(0))
                    _M_value += static_cast<_W>(__n);
                else
                    _M_value -= static_cast<_W>(__n);
            }
            else
            {
                _M_value += __n;
            }

            return *this;
        }

        constexpr __iterator& operator-=(difference_type __n)
            requires __detail::__advanceable<_W>
        {
            if constexpr (__XVI_STD_UTILITY_NS::__detail::__is_integer_like<_W>
                          && !__XVI_STD_UTILITY_NS::__detail::__is_signed_integer_like<_W>)
            {
                if (__n >= difference_type(0))
                    _M_value -= static_cast<_W>(__n);
                else
                    _M_value += static_cast<_W>(__n);
            }
            else
            {
                _M_value -= __n;
            }

            return *this;
        }

        constexpr _W operator[](difference_type __n) const
            requires __detail::__advanceable<_W>
        {
            return _W(_M_value + __n);
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires equality_comparable<_W>
        {
            return __x._M_value == __y._M_value;
        }

        friend constexpr bool operator<(const __iterator& __x, const __iterator& __y)
            requires totally_ordered<_W>
        {
            return __x._M_value < __y._M_value;
        }

        friend constexpr bool operator>(const __iterator& __x, const __iterator& __y)
            requires totally_ordered<_W>
        {
            return __y < __x;
        }

        friend constexpr bool operator<=(const __iterator& __x, const __iterator& __y)
            requires totally_ordered<_W>
        {
            return !(__y < __x);
        }

        friend constexpr bool operator>=(const __iterator& __x, const __iterator& __y)
            requires totally_ordered<_W>
        {
            return !(__x < __y);
        }

        friend constexpr compare_three_way_result_t<_W> operator<=>(const __iterator& __x, const __iterator& __y)
            requires totally_ordered<_W> && three_way_comparable<_W>
        {
            return __x._M_value <=> __y._M_value;
        }

        friend constexpr __iterator operator+(__iterator __i, difference_type __n)
            requires __detail::__advanceable<_W>
        {
            return __i += __n;
        }

        friend constexpr __iterator operator+(difference_type __n, __iterator __i)
            requires __detail::__advanceable<_W>
        {
            return __i + __n;
        }

        friend constexpr __iterator operator-(__iterator __i, difference_type __n)
            requires __detail::__advanceable<_W>
        {
            return __i -= __n;
        }

        friend constexpr difference_type operator-(const __iterator& __x, const __iterator& __y)
            requires __detail::__advanceable<_W>
        {
            using _D = difference_type;
            if constexpr (__XVI_STD_UTILITY_NS::__detail::__is_integer_like<_W>)
            {
                if constexpr (__XVI_STD_UTILITY_NS::__detail::__is_signed_integer_like<_W>)
                    return _D(_D(__x._M_value) - _D(__y._M_value));
                else
                    return (__y._M_value > __x._M_value)
                        ? _D(-_D(__y._M_value - __x._M_value))
                        : _D(__x._M_value - __y._M_value);
            }
            else
            {
                return __x._M_value - __y._M_value;
            }
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

        friend constexpr iter_difference_t<_W> operator-(const __iterator& __x, const __sentinel& __y)
            requires sized_sentinel_for<_Bound, _W>
        {
            return __x._M_value - __y._M_bound;
        }

        friend constexpr iter_difference_t<_W> operator-(const __sentinel& __x, const __iterator& __y)
            requires sized_sentinel_for<_Bound, _W>
        {
            return -(__y - __x);
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

    constexpr auto end() const
    {
        if constexpr (same_as<_Bound, unreachable_sentinel_t>)
            return unreachable_sentinel;
        else
            return __sentinel{_M_bound};
    }

    constexpr __iterator end() const
        requires same_as<_W, _Bound>
    {
        return __iterator{_M_bound};
    }

    constexpr auto size() const
        requires (same_as<_W, _Bound> && __detail::__advanceable<_W>)
            || (integral<_W> && integral<_Bound>)
            || sized_sentinel_for<_Bound, _W>
    {
        using _MUT = make_unsigned_t<_W>;

        if constexpr (__XVI_STD_UTILITY_NS::__detail::__is_integer_like<_W>
                      && __XVI_STD_UTILITY_NS::__detail::__is_integer_like<_Bound>)
        {
            return (_M_value < 0) ? ((_M_bound < 0) ? _MUT(-_M_value) - _MUT(-_M_bound)
                                                    : _MUT(_M_bound) + _MUT(-_M_value))
                                  : _MUT(_M_bound) - _MUT(_M_value);
        }
        else
            return _MUT(_M_bound - _M_value);
    }

private:

    _W _M_value = _W();
    _Bound _M_bound = _Bound();
};

template <class _W, class _Bound>
    requires (!__XVI_STD_UTILITY_NS::__detail::__is_integer_like<_W>
              || !__XVI_STD_UTILITY_NS::__detail::__is_integer_like<_Bound>
              || (__XVI_STD_UTILITY_NS::__detail::__is_signed_integer_like<_W> == __XVI_STD_UTILITY_NS::__detail::__is_signed_integer_like<_Bound>))
iota_view(_W, _Bound) -> iota_view<_W, _Bound>;


template <weakly_incrementable _W, semiregular _Bound>
inline constexpr bool enable_borrowed_range<iota_view<_W, _Bound>> = true;


namespace views
{

struct __iota_t
{
    template <class _E>
    constexpr auto operator()(_E&& __e) const
    {
        return iota_view{__XVI_STD_NS::forward<_E>(__e)};
    }

    template <class _E, class _F>
    constexpr auto operator()(_E&& __e, _F&& __f) const
    {
        return iota_view{__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)};
    }
};

inline namespace __iota { inline constexpr __iota_t iota = {}; }

} // namespace views


namespace __detail
{

template <class _Val, class _CharT, class _Traits>
concept __stream_extractable = requires(basic_istream<_CharT, _Traits>& __is, _Val& __t)
{
    __is >> __t;
};

} // namespace __detail

template <movable _Val, class _CharT, class _Traits>
    requires default_constructible<_Val>
    //    && __detail::__stream_extractable<_Val, _CharT, _Traits>
class basic_istream_view :
    public view_interface<basic_istream_view<_Val, _CharT, _Traits>>
{
public:

    basic_istream_view() = default;

    constexpr explicit basic_istream_view(basic_istream<_CharT, _Traits>& __stream) :
        _M_stream(addressof(__stream))
    {
    }

    constexpr auto begin()
    {
        if (_M_stream)
            *_M_stream >> _M_object;

        return iterator{*this};
    }

    constexpr default_sentinel_t end() const noexcept
    {
        return default_sentinel;
    }

private:

    basic_istream<_CharT, _Traits>* _M_stream = nullptr;
    _Val                            _M_object = _Val();

    class iterator
    {
    public:

        using iterator_category     = input_iterator_tag;
        using difference_type       = ptrdiff_t;
        using value_type            = _Val;

        iterator() = default;

        constexpr explicit iterator(basic_istream_view& __parent) noexcept :
            _M_parent(addressof(__parent))
        {
        }

        iterator(const iterator&) = delete;
        iterator(iterator&&) = default;

        iterator& operator=(const iterator&) = delete;
        iterator& operator=(iterator&&) = default;

        iterator& operator++()
        {
            *_M_parent->_M_stream >> _M_parent->_M_object;
            return *this;
        }

        void operator++(int)
        {
            ++*this;
        }

        _Val& operator*() const
        {
            return _M_parent->_M_object;
        }

        friend bool operator==(const iterator& __x, default_sentinel_t)
        {
            auto parent = __x._M_parent;
            return parent == nullptr || !(*parent->_M_stream);
        }

    private:

        basic_istream_view* _M_parent = nullptr;
    };
};


template <range _R>
    requires is_object_v<_R>
class ref_view :
    public view_interface<ref_view<_R>>
{
public:

    constexpr ref_view() noexcept = default;
    constexpr ref_view(const ref_view&) = default;
    constexpr ref_view(ref_view&&) = default;

    // Test functions for the _T&& constructor.
    static void __fun(_R&);
    static void __fun(_R&&) = delete;

    template <__detail::__not_same_as<ref_view> _T>
        requires (convertible_to<_T, _R&>
            && requires { __fun(declval<_T>()); })
    constexpr ref_view(_T&& __t) :
        _M_r(addressof(static_cast<_R&>(std::forward<_T>(__t))))
    {
    }

    constexpr ref_view& operator=(const ref_view&) noexcept = default;
    constexpr ref_view& operator=(ref_view&&) noexcept = default;

    ~ref_view() = default;

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
        requires requires(_R* __r) { ranges::empty(*__r); }
    {
        return ranges::empty(*_M_r);
    }

    constexpr auto size() const
        requires sized_range<_R>
    {
        return ranges::size(*_M_r);
    }

    constexpr auto data() const
        requires contiguous_range<_R>
    {
        return ranges::data(*_M_r);
    }

    friend constexpr iterator_t<_R> begin(ref_view __r)
    {
        return __r.begin();
    }

    friend constexpr sentinel_t<_R> end(ref_view __r)
    {
        return __r.end();
    }

private:

    _R* _M_r = nullptr;
};


template <class _T>
inline constexpr bool enable_borrowed_range<ref_view<_T>> = true;


namespace __detail
{

template <class _First, class _Next>
class __combined_range_closure_object;

template <class _Derived>
class __range_closure_object
{
private:

    template <class _Next>
    static constexpr auto __combine(_Derived __x, _Next __y)
    {
        return __combined_range_closure_object{std::move(__x), std::move(__y)};
    }

public:

    template <class _R>
        requires viewable_range<decltype((declval<_R&&>()))>
    friend constexpr decltype(auto) operator|(_R&& __r, _Derived __y)
    {
        return __y(std::forward<_R>(__r));
    }

    template <class _Next>
    friend constexpr auto operator|(_Derived __x, _Next __y)
    {
        return __combine(std::move(__x), std::move(__y));
    }
};

template <class _First, class _Next>
class __combined_range_closure_object :
    public __range_closure_object<__combined_range_closure_object<_First, _Next>>
{
public:

    [[no_unique_address]] _First __first;
    [[no_unique_address]] _Next __second;

    constexpr __combined_range_closure_object(_First __f, _Next __s) :
        __first(std::move(__f)),
        __second(std::move(__s))
    {
    }

    template <class _R>
        requires viewable_range<decltype((declval<_R&&>()))>
    constexpr decltype(auto) operator()(_R&& __r)
        noexcept(noexcept(__second(__first(std::forward<_R>(__r)))))
    {
        return __second(__first(std::forward<_R>(__r)));
    }
};

template <class _Lambda>
struct __add_range_closure_object :
    public __range_closure_object<__add_range_closure_object<_Lambda>>
{
    [[no_unique_address]] _Lambda _M_l;

    constexpr explicit __add_range_closure_object(_Lambda&& __l) :
        _M_l(std::forward<_Lambda>(__l))
    {
    }

    template <class _R>
        requires viewable_range<decltype((declval<_R&&>()))>
    constexpr decltype(auto) operator()(_R&& __r)
        noexcept(noexcept(_M_l(std::forward<_R>(__r))))
    {
        return _M_l(std::forward<_R>(__r));
    }
};

template <class _Lambda>
constexpr auto __make_range_closure_object(_Lambda&& __l)
{
    return __add_range_closure_object{std::forward<_Lambda>(__l)};
}


struct __all :
    public __range_closure_object<__all>
{
    template <class _T>
        requires view<decay_t<_T&&>>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__detail::__decay_copy(std::forward<_T>(__t))))
    {
        return __detail::__decay_copy(std::forward<_T>(__t));
    }

    template <class _T>
        requires (!view<decay_t<_T&&>>
            && requires(_T&& __t) { ref_view{std::forward<_T>(__t)}; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ref_view{std::forward<_T>(__t)}))
    {
        return ref_view{std::forward<_T>(__t)};
    }

    template <class _T>
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(subrange{std::forward<_T>(__t)}))
    {
        return subrange{std::forward<_T>(__t)};
    }
};

} // namespace __detail

namespace views
{

inline namespace __all { inline constexpr __detail::__all all = {}; }

template <viewable_range _R>
using all_t = decltype(views::all(declval<_R>()));

} // namespace views


template <input_range _V, indirect_unary_predicate<iterator_t<_V>> _Pred>
    requires view<_V> && is_object_v<_Pred>
class filter_view :
    public view_interface<filter_view<_V, _Pred>>
{
private:

    _V                                  _M_base = _V();
    __detail::__semiregular_box<_Pred>  _M_pred = {};

    class __iterator
    {
    private:

        iterator_t<_V>  _M_current  = iterator_t<_V>();
        filter_view*    _M_parent   = nullptr;

    public:

        using _C = typename iterator_traits<iterator_t<_V>>::iterator_category;

        using iterator_concept      = conditional_t<bidirectional_range<_V>,
                                                    bidirectional_iterator_tag,
                                                    conditional_t<forward_range<_V>,
                                                                  forward_iterator_tag,
                                                                  input_iterator_tag>>;
        using iterator_category     = conditional_t<derived_from<_C, bidirectional_iterator_tag>,
                                                    bidirectional_iterator_tag,
                                                    conditional_t<derived_from<_C, forward_iterator_tag>,
                                                                  forward_iterator_tag,
                                                                  input_iterator_tag>>;
        using value_type            = range_value_t<_V>;
        using difference_type       = range_difference_t<_V>;

        __iterator() = default;

        constexpr __iterator(filter_view& __parent, iterator_t<_V> __current) :
            _M_current(__current),
            _M_parent(addressof(__parent))
        {
        }

        constexpr iterator_t<_V> base()
        {
            return _M_current;
        }

        constexpr range_reference_t<_V> operator*() const
        {
            return *_M_current;
        }

        constexpr iterator_t<_V> operator->() const
            requires __detail::__has_arrow<iterator_t<_V>>
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

        constexpr __iterator operator++(int)
            requires forward_range<_V>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires bidirectional_range<_V>
        {
            do
                --_M_current;
            while (!invoke(*_M_parent->_M_pred, *_M_current));
            return *this;
        }

        constexpr __iterator operator--(int)
            requires bidirectional_range<_V>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires equality_comparable<iterator_t<_V>>
        {
            return __x._M_current == __y._M_current;
        }

        friend constexpr range_rvalue_reference_t<_V> iter_move(const __iterator& __i)
            noexcept(noexcept(ranges::iter_move(__i._M_current)))
        {
            return ranges::iter_move(__i._M_current);
        }

        friend constexpr void iter_swap(const __iterator& __x, const __iterator& __y)
            noexcept(noexcept(ranges::iter_swap(__x._M_current, __y._M_current)))
            requires indirectly_swappable<iterator_t<_V>>
        {
            ranges::iter_swap(__x._M_current, __y._M_current);
        }
    };

    class __sentinel
    {
    private:

        sentinel_t<_V> _M_end = sentinel_t<_V>{};

    public:

        __sentinel() = default;

        constexpr explicit __sentinel(filter_view& __parent) :
            _M_end(ranges::end(__parent._M_base))
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
    };

public:

    filter_view() = default;
    filter_view(const filter_view&) = default;
    filter_view(filter_view&&) = default;

    constexpr filter_view(_V __base, _Pred __pred) :
        _M_base(std::move(__base)),
        _M_pred(std::move(__pred))
    {
    }

    template <input_range _R>
        requires viewable_range<_R> && constructible_from<_V, views::all_t<_R>>
    constexpr filter_view(_R&& __r, _Pred __pred) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_pred(std::move(__pred))
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
        if constexpr (common_range<_V>)
            return __iterator{*this, ranges::end(_M_base)};
        else
            return __sentinel{*this};
    }
};

template <class _R, class _Pred>
filter_view(_R&&, _Pred) -> filter_view<views::all_t<_R>, _Pred>;


namespace __detail
{

struct __filter
{
    template <class _Pred>
    constexpr decltype(auto) operator()(_Pred&& __pred) const
    {
        return __make_range_closure_object([__pred = std::forward<_Pred>(__pred)]<class _E>(_E&& __e) mutable
            {
                return filter_view{std::forward<_E>(__e), std::forward<_Pred>(__pred)};
            });
    }

    template <class _E, class _P>
    constexpr decltype(auto) operator()(_E&& __e, _P&& __p) const
    {
        return operator()(std::forward<_P>(__p))(std::forward<_E>(__e));
    }
};

} // namespace __detail


namespace views
{

inline namespace __filter { inline constexpr __detail::__filter filter = {}; }

} // namespace views


template <input_range _V, copy_constructible _F>
    requires view<_V> && is_object_v<_F> && regular_invocable<_F&, range_reference_t<_V>>
class transform_view :
    public view_interface<transform_view<_V, _F>>
{
private:

    template <bool _Const>
    class __iterator
    {
    private:

        using _Parent       = conditional_t<_Const, const transform_view, transform_view>;
        using _Base         = conditional_t<_Const, const _V, _V>;

        iterator_t<_Base>   _M_current = iterator_t<_Base>();
        _Parent*            _M_parent = nullptr;

    public:

        using iterator_concept      = conditional_t<random_access_range<_V>,
                                                    random_access_iterator_tag,
                                                    conditional_t<bidirectional_range<_V>,
                                                                  bidirectional_iterator_tag,
                                                                  conditional_t<forward_range<_V>,
                                                                                forward_iterator_tag,
                                                                                input_iterator_tag>>>;
        using iterator_category     = conditional_t<derived_from<typename iterator_traits<iterator_t<_Base>>::iterator_category, contiguous_iterator_tag>,
                                                    random_access_iterator_tag,
                                                    typename iterator_traits<iterator_t<_Base>>::iterator_category>;
        using value_type            = remove_cvref_t<invoke_result_t<_F&, range_reference_t<_Base>>>;
        using difference_type       = range_difference_t<_Base>;

        __iterator() = default;

        constexpr __iterator(_Parent& __parent, iterator_t<_Base> __current) :
            _M_current(std::move(__current)),
            _M_parent(addressof(__parent))
        {
        }

        constexpr __iterator(__iterator<!_Const> __i)
            requires _Const && convertible_to<iterator_t<_V>, iterator_t<_Base>> :
            _M_current(std::move(__i._M_current)),
            _M_parent(__i._M_parent)
        {
        }

        constexpr iterator_t<_Base> base() const &
            requires copyable<iterator_t<_Base>>
        {
            return _M_current;
        }

        constexpr iterator_t<_Base> base() &&
        {
            return std::move(_M_current);
        }

        constexpr decltype(auto) operator*() const
        {
            return invoke(*_M_parent->_M_fun, *_M_current);
        }

        constexpr __iterator& operator++()
        {
            ++_M_current;
            return *this;
        }

        constexpr void operator++(int)
        {
            ++_M_current;
        }

        constexpr __iterator operator++(int)
            requires forward_range<_Base>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires bidirectional_range<_Base>
        {
            --_M_current;
            return *this;
        }

        constexpr __iterator operator--(int)
            requires bidirectional_range<_Base>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        constexpr __iterator& operator+=(difference_type __n)
            requires random_access_range<_Base>
        {
            _M_current += __n;
            return *this;
        }

        constexpr __iterator& operator-=(difference_type __n)
            requires random_access_range<_Base>
        {
            _M_current -= __n;
            return *this;
        }

        constexpr decltype(auto) operator[](difference_type __n)
            requires random_access_range<_Base>
        {
            return invoke(*_M_parent->_M_fun, _M_current[__n]);
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires equality_comparable<iterator_t<_Base>>
        {
            return __x._M_current == __y._M_current;
        }

        friend constexpr bool operator<(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return __x._M_current < __y._M_current;
        }

        friend constexpr bool operator>(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return __y < __x;
        }

        friend constexpr bool operator<=(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return !(__y < __x);
        }

        friend constexpr bool operator>=(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return !(__x < __y);
        }

        friend constexpr compare_three_way_result_t<iterator_t<_Base>> operator<=>(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base> && three_way_comparable<iterator_t<_Base>>
        {
            return __x._M_current <=> __y._M_current;
        }

        friend constexpr __iterator operator+(__iterator __i, difference_type __n)
            requires random_access_range<_Base>
        {
            return __iterator{*__i._M_parent, __i._M_current + __n};
        }

        friend constexpr __iterator operator+(difference_type __n, __iterator __i)
            requires random_access_range<_Base>
        {
            return __iterator{*__i._M_parent, __i._M_current + __n};
        }

        friend constexpr __iterator operator-(__iterator __i, difference_type __n)
            requires random_access_range<_Base>
        {
            return __iterator{*__i._M_parent, __i._M_current - __n};
        }

        friend constexpr difference_type operator-(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return __x._M_current - __y._M_current;
        }

        friend constexpr decltype(auto) iter_move(const __iterator& __i)
            noexcept(noexcept(invoke(declval<_F>(), *__i._M_current)))
        {
            if constexpr (is_lvalue_reference_v<decltype(*__i)>)
                return std::move(*__i);
            else
                return *__i;
        }

        friend constexpr void iter_swap(const __iterator& __x, const __iterator& __y)
            noexcept(noexcept(ranges::iter_swap(__x._M_current, __y._M_current)))
            requires indirectly_swappable<iterator_t<_Base>>
        {
            ranges::iter_swap(__x._M_current, __y._M_current);
        }
    };

    template <bool _Const> class __sentinel
    {
    private:

        using _Parent       = conditional_t<_Const, const transform_view, transform_view>;
        using _Base         = conditional_t<_Const, const _V, _V>;

        sentinel_t<_Base>   _M_end = sentinel_t<_Base>();

    public:

        __sentinel() = default;

        constexpr explicit __sentinel(sentinel_t<_Base> __end) :
            _M_end(__end)
        {
        }

        constexpr __sentinel(__sentinel<!_Const> __s)
            requires _Const && convertible_to<sentinel_t<_V>, sentinel_t<_Base>> :
            _M_end(std::move(__s._M_end))
        {
        }

        constexpr sentinel_t<_Base> base() const
        {
            return _M_end;
        }

        friend constexpr bool operator==(const __iterator<_Const>& __x, const __sentinel& __y)
        {
            return __x._M_current == __y._M_end;
        }

        friend constexpr range_difference_t<_Base> operator-(const __iterator<_Const>& __x, const __sentinel& __y)
            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<_Base>>
        {
            return __x._M_current - __y._M_end;
        }

        friend constexpr range_difference_t<_Base> operator-(const __sentinel& __x, const __iterator<_Const>& __y)
            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<_Base>>
        {
            return __x._M_end - __y._M_current;
        }
    };

public:

    transform_view() = default;
    transform_view(const transform_view&) = default;
    transform_view(transform_view&&) = default;

    constexpr transform_view(_V __base, _F __fun) :
        _M_base(std::move(__base)),
        _M_fun(std::move(__fun))
    {
    }

    template <input_range _R>
        requires viewable_range<_R> && constructible_from<_V, views::all_t<_R>>
    constexpr transform_view(_R&& __r, _F __fun) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_fun(std::move(__fun))
    {
    }

    transform_view& operator=(const transform_view&) = default;
    transform_view& operator=(transform_view&&) = default;

    ~transform_view() = default;

    constexpr _V base() const
    {
        return _M_base;
    }

    constexpr __iterator<false> begin()
    {
        return __iterator<false>{*this, ranges::begin(_M_base)};
    }

    constexpr __iterator<true> begin() const
        requires range<const _V> && regular_invocable<const _F&, range_reference_t<const _V>>
    {
        return __iterator<true>{*this, ranges::begin(_M_base)};
    }

    constexpr __sentinel<false> end()
    {
        return __sentinel<false>{ranges::end(_M_base)};
    }

    constexpr __iterator<false> end()
        requires common_range<_V>
    {
        return __iterator<false>{*this, ranges::end(_M_base)};
    }

    constexpr __sentinel<true> end() const
        requires range<const _V> && regular_invocable<const _F&, range_reference_t<const _V>>
    {
        return __sentinel<true>{ranges::end(_M_base)};
    }

    constexpr __iterator<true> end() const
        requires common_range<const _V> && regular_invocable<const _F&, range_reference_t<const _V>>
    {
        return __iterator<true>{*this, ranges::end(_M_base)};
    }

    constexpr auto size()
        requires sized_range<_V>
    {
        return ranges::size(_M_base);
    }

    constexpr auto size() const
        requires sized_range<const _V>
    {
        return ranges::size(_M_base);
    }

private:

    _V                              _M_base = _V();
    __detail::__semiregular_box<_F> _M_fun;
};

template <class _R, class _F>
transform_view(_R&&, _F) -> transform_view<views::all_t<_R>, _F>;

namespace __detail
{

struct __view_transform
{
    template <class _Pred>
    constexpr decltype(auto) operator()(_Pred&& __pred) const
    {
        return __make_range_closure_object([__pred = std::forward<_Pred>(__pred)]<class _E>(_E&& __e) mutable
            {
                return transform_view{std::forward<_E>(__e), std::forward<_Pred>(__pred)};
            });
    }

    template <class _E, class _P>
    constexpr decltype(auto) operator()(_E&& __e, _P&& __p) const
    {
        return operator()(std::forward<_P>(__p))(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __transform { inline constexpr __detail::__view_transform transform = {}; }

} // namespace views


template <view _V>
class take_view :
    view_interface<take_view<_V>>
{
private:

    template <bool _Const>
    class __sentinel
    {
    private:

        using _Base     = conditional_t<_Const, const _V, _V>;
        using _CI       = counted_iterator<iterator_t<_Base>>;

        sentinel_t<_Base> _M_end = sentinel_t<_Base>();

    public:

        __sentinel() = default;

        constexpr explicit __sentinel(sentinel_t<_Base> __end) :
            _M_end(__end)
        {
        }

        constexpr __sentinel(__sentinel<!_Const> __s)
            requires _Const && convertible_to<sentinel_t<_V>, sentinel_t<_Base>> :
            _M_end(std::move(__s._M_end))
        {
        }

        constexpr sentinel_t<_Base> base() const
        {
            return _M_end;
        }

        friend constexpr bool operator==(const _CI& __y, const __sentinel& __x)
        {
            return __y.count() == 0 || __y.base() == __x._M_end;
        }
    };

public:

    take_view() = default;

    constexpr take_view(_V __base, range_difference_t<_V> __count) :
        _M_base(std::move(__base)),
        _M_count(__count)
    {
    }

    template <viewable_range _R>
        requires constructible_from<_V, views::all_t<_R>>
    constexpr take_view(_R&& __r, range_difference_t<_V> __count) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_count(__count)
    {
    }
    
    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr auto begin()
        requires (!__detail::__simple_view<_V>)
    {
        if constexpr (sized_range<_V>)
        {
            if constexpr (random_access_range<_V>)
                return ranges::begin(_M_base);
            else
                return counted_iterator{ranges::begin(_M_base), size()};
        }
        else
            return counted_iterator{ranges::begin(_M_base), _M_count};
    }

    constexpr auto begin() const
        requires range<const _V>
    {
        if constexpr (sized_range<_V>)
        {
            if constexpr (random_access_range<_V>)
                return ranges::begin(_M_base);
            else
                return counted_iterator{ranges::begin(_M_base), size()};
        }
        else
            return counted_iterator{ranges::begin(_M_base), _M_count};
    }

    constexpr auto end()
        requires (!__detail::__simple_view<_V>)
    {
        if constexpr (sized_range<_V>)
        {
            if constexpr (random_access_range<_V>)
                return ranges::begin(_M_base) + size();
            else
                return default_sentinel;
        }
        else
            return __sentinel<false>{ranges::end(_M_base)};
    }

    constexpr auto end() const
        requires range<const _V>
    {
        if constexpr (sized_range<_V>)
        {
            if constexpr (random_access_range<_V>)
                return ranges::begin(_M_base) + size();
            else
                return default_sentinel;
        }
        else
            return __sentinel<true>{ranges::end(_M_base)};
    }

    constexpr auto size()
        requires sized_range<_V>
    {
        auto __n = ranges::size(_M_base);
        return std::min(__n, static_cast<decltype(__n)>(_M_count));
    }

    constexpr auto size() const
        requires sized_range<const _V>
    {
        auto __n = ranges::size(_M_base);
        return std::min(__n, static_cast<decltype(__n)>(_M_count));
    }

private:

    _V                      _M_base = _V();
    range_difference_t<_V>  _M_count = 0;
};

template <range _R>
take_view(_R&&, range_difference_t<_R>) -> take_view<views::all_t<_R>>;


namespace __detail
{

struct __take
{
    template <class _Count>
    constexpr decltype(auto) operator()(_Count&& __count) const
    {
        return __make_range_closure_object([__count = std::forward<_Count>(__count)]<class _E>(_E&& __e) mutable
            {
                return take_view{std::forward<_E>(__e), std::forward<_Count>(__count)};
            });
    }

    template <class _E, class _C>
    constexpr decltype(auto) operator()(_E&& __e, _C&& __c) const
    {
        return operator()(std::forward<_C>(__c))(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __take { inline constexpr __detail::__take take = {}; }

} // namespace views


template <view _V, class _Pred>
    requires input_range<_V> && is_object_v<_Pred> && indirect_unary_predicate<const _Pred, iterator_t<_V>>
class take_while_view :
    public view_interface<take_while_view<_V, _Pred>>
{
private:

    template <bool _Const> class __sentinel
    {
    private:

        using __base_t = conditional_t<_Const, const _V, _V>;

        sentinel_t<__base_t>    _M_end = sentinel_t<__base_t>();
        const _Pred*            _M_pred = nullptr;

    public:

        __sentinel() = default;

        constexpr explicit __sentinel(sentinel_t<__base_t> __end, const _Pred* __pred) :
            _M_end(__end),
            _M_pred(__pred)
        {
        }

        constexpr __sentinel(__sentinel<!_Const> __s)
            requires _Const && convertible_to<sentinel_t<_V>, sentinel_t<__base_t>> :
            _M_end(__s._M_end),
            _M_pred(__s._M_pred)
        {
        }

        constexpr sentinel_t<__base_t> base() const
        {
            return _M_end;
        }

        friend constexpr bool operator==(const iterator_t<__base_t>& __x, const __sentinel& __y)
        {
            return __y._M_end == __x || !invoke(*__y._M_pred, *__x);
        }
    };

public:

    take_while_view() = default;

    constexpr take_while_view(_V __base, _Pred __pred);

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr const _Pred& pred() const;

    constexpr auto begin()
        requires (!__detail::__simple_view<_V>)
    {
        return ranges::begin(_M_base);
    }

    constexpr auto begin() const
        requires range<const _V>
    {
        return ranges::begin(_M_base);
    }

    constexpr auto end()
        requires (!__detail::__simple_view<_V>)
    {
        return __sentinel<false>(ranges::end(_M_base), addressof(*_M_pred));
    }

    constexpr auto end() const
        requires range<const _V>
    {
        return __sentinel<true>(ranges::end(_M_base), addressof(*_M_pred));
    }

private:

    _V                                  _M_base;
    __detail::__semiregular_box<_Pred>  _M_pred;
};

template <class _R, class _Pred>
take_while_view(_R&&, _Pred) -> take_while_view<views::all_t<_R>, _Pred>;

namespace __detail
{

struct __take_while
{
    template <class _Pred>
    constexpr decltype(auto) operator()(_Pred&& __pred) const
    {
        return __make_range_closure_object([__pred = std::forward<_Pred>(__pred)]<class _E>(_E&& __e) mutable
            {
                return take_while_view{std::forward<_E>(__e), std::forward<_Pred>(__pred)};
            });
    }

    template <class _E, class _P>
    constexpr decltype(auto) operator()(_E&& __e, _P&& __p) const
    {
        return operator()(std::forward<_P>(__p))(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __take_while { inline constexpr __detail::__take_while take_while = {}; }

} // namespace views


template <view _V>
class drop_view :
    public view_interface<drop_view<_V>>
{
public:

    drop_view() = default;

    constexpr drop_view(_V __base, range_difference_t<_V> __count) :
        _M_base(std::move(__base)),
        _M_count(__count)
    {
    }

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr auto begin()
        requires (!(__detail::__simple_view<_V> && random_access_range<_V>))
    {
        // The range concept requires amortised constant-time complexity so cache the result.
        if (!_M_begin)
            _M_begin = ranges::next(ranges::begin(_M_base), _M_count, ranges::end(_M_base));

        return _M_begin;
    }

    constexpr auto begin() const
        requires random_access_range<const _V>
    {
        return ranges::next(ranges::begin(_M_base), _M_count, ranges::end(_M_base));
    }

    constexpr auto end()
        requires (!__detail::__simple_view<_V>)
    {
        return ranges::end(_M_base);
    }

    constexpr auto end() const
        requires range<const _V>
    {
        return ranges::end(_M_base);
    }

    constexpr auto size()
        requires sized_range<_V>
    {
        const auto __s = ranges::size(_M_base);
        const auto __c = static_cast<decltype(__s)>(_M_count);
        return (__s < __c) ? 0 : __s - __c;
    }

    constexpr auto size() const
        requires sized_range<const _V>
    {
        const auto __s = ranges::size(_M_base);
        const auto __c = static_cast<decltype(__s)>(_M_count);
        return (__s < __c) ? 0 : __s - __c;
    }

private:

    _V                          _M_base;
    range_difference_t<_V>      _M_count;
    optional<iterator_t<_V>>    _M_begin = nullopt;
};

template <class _R>
drop_view(_R&&, range_difference_t<_R>) -> drop_view<views::all_t<_R>>;


namespace __detail
{

struct __drop
{
    template <class _Count>
    constexpr decltype(auto) operator()(_Count&& __count) const
    {
        return __make_range_closure_object([__count = std::forward<_Count>(__count)]<class _E>(_E&& __e) mutable
            {
                return drop_view{std::forward<_E>(__e), std::forward<_Count>(__count)};
            });
    }

    template <class _E, class _C>
    constexpr decltype(auto) operator()(_E&& __e, _C&& __c) const
    {
        return operator()(std::forward<_C>(__c))(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __drop { inline constexpr __detail::__drop drop = {}; }

} // namespace views


template <view _V, class _Pred>
    requires input_range<_V> && is_object_v<_Pred> && indirect_unary_predicate<const _Pred, iterator_t<_V>>
class drop_while_view :
    public view_interface<drop_while_view<_V, _Pred>>
{
public:

    drop_while_view() = default;

    constexpr drop_while_view(_V __base, _Pred __pred) :
        _M_base(std::move(__base)),
        _M_pred(std::move(__pred))
    {
    }

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr const _Pred& pred() const
    {
        return *_M_pred;
    }

    constexpr auto begin()
    {
        // Cache the result to get amortised constant time.
        if (!_M_begin)
            _M_begin = ranges::find_if_not(ranges::begin(_M_base), cref(*_M_pred));

        return _M_begin;
    }

    constexpr auto end()
    {
        return ranges::end(_M_base);
    }

private:

    _V                                  _M_base;
    __detail::__semiregular_box<_Pred>  _M_pred;
    optional<iterator_t<_V>>            _M_begin = nullopt;
};

template <class _R, class _Pred>
drop_while_view(_R&&, _Pred) -> drop_while_view<views::all_t<_R>, _Pred>;

namespace __detail
{

struct __drop_while
{
    template <class _Count>
    constexpr decltype(auto) operator()(_Count&& __count) const
    {
        return __make_range_closure_object([__count = std::forward<_Count>(__count)]<class _E>(_E&& __e) mutable
            {
                return drop_while_view{std::forward<_E>(__e), std::forward<_Count>(__count)};
            });
    }

    template <class _E, class _C>
    constexpr decltype(auto) operator()(_E&& __e, _C&& __c) const
    {
        return operator()(std::forward<_C>(__c))(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __drop_while { inline constexpr __detail::__drop_while drop_while = {}; }

} // namespace views


template <input_range _V>
    requires view<_V>
        && input_range<range_reference_t<_V>>
        && (is_reference_v<range_reference_t<_V>> || view<range_value_t<_V>>)
class join_view :
    public view_interface<join_view<_V>>
{
private:

    using _InnerRange       = range_reference_t<_V>;

    template <bool _Const>
    struct __iterator
    {
    private:

        using _Parent       = conditional_t<_Const, const join_view, join_view>;
        using _Base         = conditional_t<_Const, const _V, _V>;

        using _InnerC       = typename iterator_traits<iterator_t<_Base>>::iterator_category;
        using _OuterC       = typename iterator_traits<iterator_t<range_reference_t<_Base>>>::iterator_category;

        static constexpr bool __ref_is_glvalue = is_reference_v<range_reference_t<_Base>>;

        iterator_t<_Base>                       _M_outer = iterator_t<_Base>();
        iterator_t<range_reference_t<_Base>>    _M_inner = iterator_t<range_reference_t<_Base>>();
        _Parent*                                _M_parent = nullptr;

        constexpr void __satisfy()
        {
            auto __update_inner = [this](range_reference_t<_Base> __x) -> auto&
            {
                if constexpr (__ref_is_glvalue)
                    return __x;
                else
                    return (_M_parent->_M_inner = views::all(std::move(__x)));
            };

            for (; _M_outer != ranges::end(_M_parent->_M_base); ++_M_outer)
            {
                auto& __inner = update_inner(*_M_outer);
                _M_inner = ranges::begin(__inner);

                if (_M_inner != ranges::end(__inner))
                    return;
            }

            if constexpr (__ref_is_glvalue)
                _M_inner = iterator_t<range_reference_t<_Base>>();
        }

    public:

        using iterator_concept      = conditional_t<__ref_is_glvalue && bidirectional_range<_Base> && bidirectional_range<range_reference_t<_Base>>,
                                                    bidirectional_iterator_tag,
                                                    conditional_t<__ref_is_glvalue && forward_range<_Base> && forward_range<range_reference_t<_Base>>,
                                                                  forward_iterator_tag,
                                                                  input_iterator_tag>>;
        using iterator_category     = conditional_t<__ref_is_glvalue && derived_from<_OuterC, bidirectional_iterator_tag> && derived_from<_InnerC, bidirectional_iterator_tag>,
                                                    bidirectional_iterator_tag,
                                                    conditional_t<__ref_is_glvalue && derived_from<_OuterC, forward_iterator_tag> && derived_from<_InnerC, forward_iterator_tag>,
                                                                  forward_iterator_tag,
                                                                  conditional_t<derived_from<_OuterC, input_iterator_tag> && derived_from<_InnerC, input_iterator_tag>,
                                                                                input_iterator_tag,
                                                                                output_iterator_tag>>>;
        using value_type            = range_value_t<range_reference_t<_Base>>;
        using difference_type       = common_type_t<range_difference_t<_Base>, range_difference_t<range_reference_t<_Base>>>;

        __iterator() = default;

        constexpr __iterator(_Parent& __parent, iterator_t<_V> __outer) :
            _M_outer(std::move(__outer)),
            _M_parent(addressof(__parent))
        {
            __satisfy();
        }

        constexpr __iterator(__iterator<!_Const> __i)
            requires _Const
                && convertible_to<iterator_t<_V>, iterator_t<_Base>>
                && convertible_to<iterator_t<_InnerRange>, iterator_t<range_reference_t<_Base>>> :
            _M_outer(std::move(__i._M_outer)),
            _M_inner(std::move(__i._M_inner)),
            _M_parent(__i._M_parent)
        {
        }

        constexpr decltype(auto) operator*() const
        {
            return *_M_inner;
        }

        constexpr iterator_t<_Base> operator->() const
            requires __detail::__has_arrow<iterator_t<_Base>> && copyable<iterator_t<_Base>>
        {
            return _M_inner;
        }

        constexpr __iterator& operator++()
        {
            auto __get_inner_range = [this]()
            {
                if constexpr (__ref_is_glvalue)
                    return *_M_outer;
                else
                    return _M_parent->_M_inner;
            };

            auto&& __inner_range = __get_inner_range();
            if (++_M_inner == ranges::end(__inner_range))
            {
                ++_M_outer;
                __satisfy();
            }

            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        constexpr __iterator operator++(int)
            requires __ref_is_glvalue && forward_range<_Base> && forward_range<range_reference_t<_Base>>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires __ref_is_glvalue && bidirectional_range<_Base> && bidirectional_range<range_reference_t<_Base>>
        {
            if (_M_outer == ranges::end(_M_parent->_M_base))
                _M_inner = ranges::end(*--_M_outer);

            while (_M_inner == ranges::begin(*_M_outer))
                _M_inner = ranges::end(*--_M_outer);

            --_M_inner;
            return *this;
        }

        constexpr __iterator operator--(int)
            requires __ref_is_glvalue && bidirectional_range<_Base> && bidirectional_range<range_reference_t<_Base>>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires __ref_is_glvalue && equality_comparable<iterator_t<_Base>> && equality_comparable<iterator_t<range_reference_t<_Base>>>
        {
            return __x._M_outer == __y._M_outer && __x._M_inner == __y._M_inner;
        }

        friend constexpr decltype(auto) iter_move(const __iterator& __i)
            noexcept(noexcept(ranges::iter_move(__i._M_inner)))
        {
            return ranges::iter_move(__i._M_inner);
        }

        friend constexpr void iter_swap(const __iterator& __x, const __iterator& __y)
            noexcept(noexcept(ranges::swap(__x._M_inner, __y._M_inner)))
        {
            ranges::iter_swap(__x._M_inner, __y._M_inner);
        }
    };

    template <bool _Const>
    struct __sentinel
    {
    private:

        using _Parent       = conditional_t<_Const, const join_view, join_view>;
        using _Base         = conditional_t<_Const, const _V, _V>;

        sentinel_t<_Base>   _M_end = sentinel_t<_Base>();

    public:

        __sentinel() = default;

        constexpr explicit __sentinel(_Parent& __parent) :
            _M_end(ranges::end(__parent._M_base))
        {
        }

        constexpr explicit __sentinel(__sentinel<!_Const> __s)
            requires _Const && convertible_to<sentinel_t<_V>, sentinel_t<_Base>> :
            _M_end(std::move(__s._M_end))
        {
        }

        friend constexpr bool operator==(const __iterator<_Const>& __x, const __sentinel& __y)
        {
            return __x._M_outer == __y._M_end;
        }
    };

    _V                          _M_base =_V();
    views::all_t<_InnerRange>   _M_inner = views::all_t<_InnerRange>();

public:

    join_view() = default;

    constexpr explicit join_view(_V __base) :
        _M_base(std::move(__base))
    {
    }

    template <input_range _R>
        requires viewable_range<_R> && constructible_from<_V, views::all_t<_R>>
    constexpr explicit join_view(_R&& __r) :
        _M_base(views::all(std::forward<_R>(__r)))
    {
    }

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr auto begin()
    {
        return __iterator<__detail::__simple_view<_V>>{*this, ranges::begin(_M_base)};
    }

    constexpr auto begin() const
        requires input_range<const _V> && is_reference_v<range_reference_t<const _V>>
    {
        return __iterator<true>{*this, ranges::begin(_M_base)};
    }

    constexpr auto end()
    {
        if constexpr (forward_range<_V>
                      && is_reference_v<_InnerRange>
                      && forward_range<_InnerRange>
                      && common_range<_V>
                      && common_range<_InnerRange>)
        {
            return __iterator<__detail::__simple_view<_V>>{*this, ranges::end(_M_base)};
        }
        else
        {
            return __sentinel<__detail::__simple_view<_V>>{*this};
        }
    }

    constexpr auto end() const
        requires input_range<const _V> && is_reference_v<range_reference_t<const _V>>
    {
        if constexpr (forward_range<const _V>
                      && is_reference_v<range_reference_t<const _V>>
                      && forward_range<range_reference_t<const _V>>
                      && common_range<const _V>
                      && common_range<range_reference_t<const _V>>)
        {
            return __iterator<true>{*this, ranges::end(_M_base)};
        }
        else
        {
            return __sentinel<true>{*this};
        }
    }
};

template <class _R>
explicit join_view(_R&&) -> join_view<views::all_t<_R>>;

namespace __detail
{

struct __join
{
    template <class _E>
    constexpr decltype(auto) operator()(_E&& __e) const
        noexcept(noexcept(join_view(std::forward<_E>(__e))))
    {
        return join_view(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __join { inline constexpr __detail::__join join = {}; }

} // namespace views


namespace __detail
{

template <auto> struct __require_constant;

template <class _R>
concept __tiny_range = sized_range<_R>
    && requires { typename __require_constant<remove_reference_t<_R>::size()>; }
    && (remove_reference_t<_R>::size() <= 1);

template <class _T>
struct __split_view_current
{ 
    using __type = iterator_t<_T>;
};

template <class _T>
    requires forward_range<_T>
struct __split_view_current<_T>
{
    using __type = __split_view_current;
};

}

template <input_range _V, forward_range _Pattern>
    requires view<_V>
        && view<_Pattern>
        && indirectly_comparable<iterator_t<_V>, iterator_t<_Pattern>, __detail::__equal_to>
        && (forward_range<_V> || __detail::__tiny_range<_Pattern>)
class split_view :
    public view_interface<split_view<_V, _Pattern>>
{
private:

    using _CurrentT = typename __detail::__split_view_current<_V>::__type;

    _V                              _M_base = _V();
    _Pattern                        _M_pattern = _Pattern();
    [[no_unique_address]] _CurrentT _M_current = _CurrentT();

 
    template <bool> struct __inner_iterator;

    template <bool _Const>
    struct __outer_iterator
    {
        using _Parent       = conditional_t<_Const, const split_view, split_view>;
        using _Base         = conditional_t<_Const, const _V, _V>;

        _Parent             _M_parent = nullptr;
        iterator_t<_Base>   _M_current = iterator_t<_Base>();

    public:

        using iterator_concept      = conditional_t<forward_range<_Base>, forward_iterator_tag, input_iterator_tag>;
        using iterator_category     = input_iterator_tag;
        using difference_type       = range_difference_t<_Base>;

        struct value_type :
            view_interface<value_type>
        {
        private:

            __outer_iterator    _M_i = __outer_iterator();

        public:

            value_type() = default;

            constexpr explicit value_type(__outer_iterator __i) :
                _M_i(std::move(__i))
            {
            }

            constexpr __inner_iterator<_Const> begin() const
                requires copyable<__outer_iterator>
            {
                return __inner_iterator<_Const>{_M_i};
            }

            constexpr __inner_iterator<_Const> begin()
                requires (!copyable<__outer_iterator>)
            {
                return __inner_iterator<_Const>{std::move(_M_i)};
            }

            constexpr default_sentinel_t end() const
            {
                return default_sentinel;
            }
        };

        __outer_iterator() = default;

        constexpr explicit __outer_iterator(_Parent& __parent)
            requires (!forward_range<_Base>) :
            _M_parent(addressof(__parent))
        {
        }

        constexpr __outer_iterator(_Parent& __parent, iterator_t<_Base> __current)
            requires forward_range<_Base> :
            _M_parent(addressof(__parent)),
            _M_current(std::move(__current))
        {
        }

        constexpr __outer_iterator(__outer_iterator<!_Const> __i)
            requires _Const
                && convertible_to<iterator_t<_V>, iterator_t<const _V>> :
            _M_parent(__i._M_parent),
            _M_current(std::move(__i._M_current))
        {
        }

        constexpr value_type operator*() const
        {
            return value_type{*this};
        }

        // Defined in RangesAlgorithm.hh due to a circular dependency.
        constexpr __outer_iterator& operator++();

        constexpr decltype(auto) operator++(int)
        {
            if constexpr (forward_range<_Base>)
            {
                auto __tmp = *this;
                ++*this;
                return __tmp;
            }
            else
            {
                ++*this;
            }
        }

        friend constexpr bool operator==(const __outer_iterator& __x, const __outer_iterator& __y)
            requires forward_range<_Base>
        {
            return __x._M_current == __y._M_current;
        }

        friend constexpr bool operator==(const __outer_iterator& __x, default_sentinel_t)
        {
            return __x.__current() == ranges::end(__x._M_parent->_M_base);
        }

        constexpr auto& __current() const
        {
            if constexpr (forward_range<_V>)
                return const_cast<iterator_t<_Base>&>(_M_current);
            else
                return _M_parent->_M_current;
        }
    };

    template <bool _Const>
    struct __inner_iterator
    {
    private:

        using _Base         = conditional_t<_Const, const _V, _V>;

        static constexpr bool _C    = _Const;

        __outer_iterator<_Const>    _M_i = __outer_iterator<_C>();
        bool                        _M_incremented = false;

    public:

        using iterator_concept      = typename __outer_iterator<_Const>::iterator_concept;
        using iterator_category     = conditional_t<derived_from<typename iterator_traits<iterator_t<_Base>>::iterator_category, forward_iterator_tag>,
                                                    forward_iterator_tag,
                                                    typename iterator_traits<iterator_t<_Base>>::iterator_category>;
        using value_type            = range_value_t<_Base>;
        using difference_type       = range_difference_t<_Base>;

        __inner_iterator() = default;

        constexpr explicit __inner_iterator(__outer_iterator<_Const> __i) :
            _M_i(std::move(__i))
        {
        }

        constexpr decltype(auto) operator*() const
        {
            return *_M_i.__current();
        }

        constexpr __inner_iterator& operator++()
        {
            _M_incremented = true;

            if constexpr (!forward_range<_Base>)
            {
                if constexpr (_Pattern::size() == 0)
                    return *this;
            }

            ++_M_i.__current();

            return *this;
        }

        constexpr decltype(auto) operator++(int)
        {
            if constexpr (forward_range<_V>)
            {
                auto __tmp = *this;
                ++*this;
                return __tmp;
            }
            else
            {
                ++*this;
            }
        }

        friend constexpr bool operator==(const __inner_iterator& __x, const __inner_iterator& __y)
            requires forward_range<_Base>
        {
            return __x._M_i.__current() == __y._M_i.__current();
        }

        friend constexpr bool operator==(const __inner_iterator& __x, default_sentinel_t)
        {
            auto [__pcur, __pend] = subrange{__x._M_i._M_parent->_M_pattern};
            auto __end = ranges::end(__x._M_i._M_parent->_M_base);

            if constexpr (__detail::__tiny_range<_Pattern>)
            {
                const auto& __cur = __x._M_i.__current();

                if (__cur == __end)
                    return true;

                if (__pcur == __pend)
                    return __x._M_incremented;

                return *__cur == *__pcur;
            }
            else
            {
                auto __cur = __x._M_i.__current();

                if (__cur == __end)
                    return true;

                if (__pcur == __pend)
                    return __x._M_incremented;

                do
                {
                    if (*__cur != *__pcur)
                        return false;

                    if (++__pcur == __pend)
                        return true;
                }
                while (++__cur != __end);

                return false;
            }
        }

        friend constexpr decltype(auto) iter_move(const __inner_iterator& __i)
            noexcept(noexcept(ranges::iter_move(__i._M_i.__current())))
        {
            return ranges::iter_move(__i._M_i.__current());
        }

        friend constexpr void iter_swap(const __inner_iterator& __x, const __inner_iterator& __y)
            noexcept(noexcept(ranges::swap(__x._M_i.__current(), __y._M_i.__current())))
            requires indirectly_swappable<iterator_t<_Base>>
        {
            ranges::swap(__x._M_i.__current(), __y._M_i.__current());
        }
    };

public:

    split_view() = default;

    constexpr split_view(_V __base, _Pattern __pattern) :
        _M_base(std::move(__base)),
        _M_pattern(std::move(__pattern))
    {
    }

    template <input_range _R, forward_range _P>
        requires constructible_from<_V, views::all_t<_R>>
            && constructible_from<_Pattern, views::all_t<_P>>
    constexpr split_view(_R&& __r, _P&& __p) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_pattern(views::all(std::forward<_P>(__p)))
    {
    }

    template <input_range _R>
        requires constructible_from<_V, views::all_t<_R>>
            && constructible_from<_Pattern, single_view<range_value_t<_R>>>
    constexpr split_view(_R&& __r, range_value_t<_R> __e) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_pattern(single_view{std::move(__e)})
    {
    }

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr auto begin()
    {
        if constexpr (forward_range<_V>)
            return __outer_iterator<__detail::__simple_view<_V>>{*this, ranges::begin(_M_base)};
        else
        {
            _M_current = ranges::begin(_M_base);
            return __outer_iterator<false>{*this};
        }
    }

    constexpr auto begin() const
        requires forward_range<_V> && forward_range<const _V>
    {
        return __outer_iterator<true>{*this, ranges::begin(_M_base)};
    }

    constexpr auto end()
        requires forward_range<_V> && common_range<_V>
    {
        return __outer_iterator<__detail::__simple_view<_V>>{*this, ranges::end(_M_base)};
    }

    constexpr auto end() const
    {
        if constexpr (forward_range<_V> && forward_range<const _V> && common_range<const _V>)
            return __outer_iterator<true>{*this, ranges::end(_M_base)};
        else
            return default_sentinel;
    }
};

template <class _R, class _P>
split_view(_R&&, _P&&) -> split_view<views::all_t<_R>, views::all_t<_P>>;

template <input_range _R>
split_view(_R&&, range_value_t<_R>) -> split_view<views::all_t<_R>, single_view<range_value_t<_R>>>;

namespace __detail
{

struct __split
{
    template <class _Pattern>
    constexpr decltype(auto) operator()(_Pattern&& __pattern) const
    {
        return __make_range_closure_object([__pattern = std::forward<_Pattern>(__pattern)]<class _E>(_E&& __e) mutable
            {
                return split_view{std::forward<_E>(__e), std::forward<_Pattern>(__pattern)};
            });
    }

    template <class _E, class _P>
    constexpr decltype(auto) operator()(_E&& __e, _P&& __p) const
    {
        return operator()(std::forward<_P>(__p))(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __split { inline constexpr __detail::__split split = {}; }

} // namespace views


namespace __detail
{

struct __counted
{
    template <class _E, class _F, class _T = decay_t<_E>>
        requires (input_or_output_iterator<_T>
            && convertible_to<_F, iter_difference_t<_T>>
            && random_access_iterator<_T>)
    constexpr decltype(auto) operator()(_E&& __e, _F&& __f) const
        noexcept(noexcept(subrange{std::forward<_E>(__e), std::forward<_E>(__e) + static_cast<iter_difference_t<_T>>(__f)}))
    {
        return subrange{std::forward<_E>(__e), std::forward<_E>(__e) + static_cast<iter_difference_t<_T>>(__f)};
    }

    template <class _E, class _F, class _T = decay_t<_E>>
        requires (input_or_output_iterator<_T>
            && convertible_to<_F, iter_difference_t<_T>>
            && !random_access_iterator<_T>)
    constexpr decltype(auto) operator()(_E&& __e, _F&& __f) const
        noexcept(noexcept(subrange{counted_iterator{std::forward<_E>(__e), std::forward<_F>(__f)}, default_sentinel}))
    {
        subrange{counted_iterator{std::forward<_E>(__e), std::forward<_F>(__f)}, default_sentinel};
    }
};

} // namespace __detail

namespace views
{

inline namespace __counted { inline constexpr __detail::__counted counted = {}; }

} // namespace views


template <view _V>
    requires (!common_range<_V> && copyable<iterator_t<_V>>)
class common_view :
    public view_interface<common_view<_V>>
{
private:

    _V _M_base = _V();

public:

    common_view() = default;

    constexpr explicit common_view(_V __r) :
        _M_base(std::move(__r))
    {
    }

    template <viewable_range _R>
        requires (!common_range<_R> && constructible_from<_V, views::all_t<_R>>)
    constexpr explicit common_view(_R&& __r) :
        _M_base(views::all(std::forward<_R>(__r)))
    {
    }

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr auto size()
        requires sized_range<_V>
    {
        return ranges::size(_M_base);
    }

    constexpr auto size() const
        requires sized_range<const _V>
    {
        return ranges::size(_M_base);
    }

    constexpr auto begin()
    {
        if constexpr (random_access_range<_V> && sized_range<_V>)
            return ranges::begin(_M_base);
        else
            return common_iterator<iterator_t<_V>, sentinel_t<_V>>(ranges::begin(_M_base));
    }

    constexpr auto begin() const
    {
        if constexpr (random_access_range<const _V> && sized_range<const _V>)
            return ranges::begin(_M_base);
        else
            return common_iterator<iterator_t<const _V>, sentinel_t<const _V>>(ranges::begin(_M_base));
    }

    constexpr auto end()
    {
        if constexpr (random_access_range<_V> && sized_range<_V>)
            return ranges::begin(_M_base) + ranges::size(_M_base);
        else
            return common_iterator<iterator_t<_V>, sentinel_t<_V>>(ranges::end(_M_base));
    }

    constexpr auto end() const
    {
        if constexpr (random_access_range<const _V> && sized_range<const _V>)
            return ranges::begin(_M_base) + ranges::end(_M_base);
        else
            return common_iterator<iterator_t<const _V>, sentinel_t<const _V>>(ranges::end(_M_base));
    }
};

template <class _R>
common_view(_R&&) -> common_view<views::all_t<_R>>;

namespace __detail
{

struct __common
{
    template <class _E>
    constexpr decltype(auto) operator()(_E&& __e) const
        noexcept(noexcept(common_view(std::forward<_E>(__e))))
    {
        return common_view(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views
{

inline namespace __common { inline constexpr __detail::__common common = {}; }

} // namespace views


template <view _V>
    requires bidirectional_range<_V>
class reverse_view :
    public view_interface<reverse_view<_V>>
{
private:

    _V                          _M_base = _V();
    optional<iterator_t<_V>>    _M_begin = nullopt;

public:

    reverse_view() = default;

    constexpr explicit reverse_view(_V __r) :
        _M_base(std::move(__r))
    {
    }

    template <viewable_range _R>
        requires bidirectional_range<_R> && constructible_from<_V, views::all_t<_R>>
    constexpr explicit reverse_view(_R&& __r) :
        _M_base(views::all(std::forward<_R>(__r)))
    {
    }

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr reverse_iterator<iterator_t<_V>> begin()
    {
        if (!_M_begin)
            _M_begin = make_reverse_iterator(ranges::next(ranges::begin(_M_base), ranges::end(_M_base)));

        return _M_begin;
    }

    constexpr reverse_iterator<iterator_t<_V>> begin()
        requires common_range<_V>
    {
        return make_reverse_iterator(ranges::end(_M_base));
    }

    constexpr reverse_iterator<iterator_t<const _V>> begin() const
        requires common_range<const _V>
    {
        return make_reverse_iterator(ranges::end(_M_base));
    }

    constexpr reverse_iterator<iterator_t<_V>> end()
    {
        return make_reverse_iterator(ranges::begin(_M_base));
    }

    constexpr reverse_iterator<iterator_t<const _V>> end() const
        requires common_range<const _V>
    {
        return make_reverse_iterator(ranges::begin(_M_base));
    }

    constexpr auto size()
        requires sized_range<_V>
    {
        return ranges::size(_M_base);
    }

    constexpr auto size() const
        requires sized_range<const _V>
    {
        return ranges::size(_M_base);
    }
};

template <class _R>
reverse_view(_R&&) -> reverse_view<views::all_t<_R>>;

namespace __detail
{

template <class _E>
struct __reverse_helper
{
    template <class _F>
    constexpr decltype(auto) operator()(_F&& __f) const
        noexcept(noexcept(reverse_view{std::forward<_F>(__f)}))
    {
        return reverse_view{std::forward<_F>(__f)};
    }
};

template <class _E>
struct __reverse_helper<reverse_view<_E>>
{
    template <class _F>
    constexpr decltype(auto) operator()(_F&& __f) const
        noexcept(noexcept(std::forward<_F>(__f).base()))
    {
        return std::forward<_F>(__f).base();
    }
};

template <class _I>
struct __reverse_helper<subrange<reverse_iterator<_I>, reverse_iterator<_I>, subrange_kind::sized>>
{
    template <class _F>
    constexpr decltype(auto) operator()(_F&& __f) const
        noexcept(noexcept(subrange<_I, _I, subrange_kind::sized>(__f.end().base(), __f.begin().base(), __f.size())))
    {
        return subrange<_I, _I, subrange_kind::sized>(__f.end().base(), __f.begin().base(), __f.size());
    }
};

template <class _I, subrange_kind _K>
struct __reverse_helper<subrange<reverse_iterator<_I>, reverse_iterator<_I>, _K>>
{
    template <class _F>
    constexpr decltype(auto) operator()(_F&& __f) const
        noexcept(noexcept(subrange<_I, _I, _K>(__f.end().base(), __f.begin().base())))
    {
        return subrange<_I, _I, _K>(__f.end().base(), __f.begin().base());
    }
};

struct __reverse_view
{
    template <class _E, class _T = remove_cvref_t<_E>>
    constexpr decltype(auto) operator()(_E&& __e) const
        noexcept(noexcept(__reverse_helper<_T>()(std::forward<_E>(__e))))
    {
        __reverse_helper<_T>()(std::forward<_E>(__e));
    }
};

} // namespace __detail

namespace views::inline __reverse_view
{

inline constexpr __detail::__reverse_view reverse = {};

} // namespace views::__reverse


namespace __detail
{

template <class _T, size_t _N>
concept __has_tuple_element = requires(_T __t)
{
    typename tuple_size<_T>::type;
    requires _N < tuple_size_v<_T>;
    typename tuple_element_t<_N, _T>;
    { get<_N>(__t) } -> convertible_to<const tuple_element_t<_N, _T>&>;
};

} // namespace __detail

template <input_range _V, size_t _N>
    requires view<_V>
        && __detail::__has_tuple_element<range_value_t<_V>, _N>
        && __detail::__has_tuple_element<remove_reference_t<range_reference_t<_V>>, _N>
class elements_view :
    public view_interface<elements_view<_V, _N>>
{
private:

    template <bool _Const>
    class __iterator
    {
    private:

        using _Base = conditional_t<_Const, const _V, _V>;

        friend __iterator<!_Const>;

        iterator_t<_Base> _M_current = iterator_t<_Base>();

    public:

        using iterator_category     = typename iterator_traits<iterator_t<_Base>>::iterator_category;
        using value_type            = remove_cvref_t<tuple_element_t<_N, range_value_t<_Base>>>;
        using difference_type       = range_difference_t<_Base>;

        __iterator() = default;

        constexpr explicit __iterator(iterator_t<_Base> __current) :
            _M_current(std::move(__current))
        {
        }

        constexpr __iterator(__iterator<!_Const> __i)
            requires _Const && convertible_to<iterator_t<_V>, iterator_t<_Base>> :
            _M_current(std::move(__i._M_current))
        {
        }

        constexpr iterator_t<_Base> base() const &
            requires copyable<iterator_t<_Base>>
        {
            return _M_current;
        }

        constexpr iterator_t<_Base> base() &&
        {
            return std::move(_M_current);
        }

        constexpr decltype(auto) operator*() const
        {
            return get<_N>(*_M_current);
        }

        constexpr __iterator& operator++()
        {
            ++_M_current;
            return *this;
        }

        constexpr void operator++(int)
            requires (!forward_range<_Base>)
        {
            ++_M_current;
        }

        constexpr __iterator operator++(int)
            requires forward_range<_Base>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires bidirectional_range<_Base>
        {
            --_M_current;
            return *this;
        }

        constexpr __iterator operator--(int)
            requires bidirectional_range<_Base>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        constexpr __iterator& operator+=(difference_type __x)
            requires random_access_range<_Base>
        {
            _M_current += __x;
            return *this;
        }

        constexpr __iterator& operator-=(difference_type __x)
            requires random_access_range<_Base>
        {
            _M_current -= __x;
            return *this;
        }

        constexpr decltype(auto) operator[](difference_type __n) const
            requires random_access_range<_Base>
        {
            return get<_N>(*(_M_current + __n));
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires equality_comparable<iterator_t<_Base>>
        {
            return __x._M_current == __y._M_current;
        }

        friend constexpr bool operator==(const __iterator& __x, const sentinel_t<_Base>& __y)
        {
            return __x._M_current == __y;
        }

        friend constexpr bool operator<(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return __x._M_current < __y._M_current;
        }

        friend constexpr bool operator>(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return (__y < __x);
        }

        friend constexpr bool operator<=(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return !(__y < __x);
        }

        friend constexpr bool operator>=(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return !(__x < __y);
        }

        friend constexpr compare_three_way_result_t<iterator_t<_Base>> operator<=>(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
                && three_way_comparable<iterator_t<_Base>>
        {
            return __x._M_current <=> __y._M_current;
        }

        friend constexpr __iterator operator+(const __iterator& __x, difference_type __y)
            requires random_access_range<_Base>
        {
            return __iterator{__x} += __y;
        }

        friend constexpr __iterator operator+(difference_type __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return __y + __x;
        }

        friend constexpr difference_type operator-(const __iterator& __x, const __iterator& __y)
            requires random_access_range<_Base>
        {
            return __x._M_current - __y._M_current;
        }

        friend constexpr __iterator operator-(const __iterator& __x, difference_type __y)
            requires random_access_range<_Base>
        {
            return __iterator{__x} -= __y;
        }

        friend constexpr difference_type operator-(const __iterator<_Const>& __x, const sentinel_t<_Base>& __y)
            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<_Base>>
        {
            return __x._M_current - __y;
        }

        friend constexpr difference_type operator-(const sentinel_t<_Base>& __x, const __iterator<_Const>& __y)
            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<_Base>>
        {
            return -(__y - __x);
        }
    };

    _V _M_base = _V();

public:

    elements_view() = default;

    constexpr explicit elements_view(_V __base) :
        _M_base(std::move(__base))
    {
    }

    constexpr _V base() const &
        requires copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr auto begin()
        requires (!__detail::__simple_view<_V>)
    {
        return __iterator<false>(ranges::begin(_M_base));
    }

    constexpr auto begin() const
        requires __detail::__simple_view<_V>
    {
        return __iterator<true>(ranges::begin(_M_base));
    }

    constexpr auto end()
        requires (!__detail::__simple_view<_V>)
    {
        return ranges::end(_M_base);
    }

    constexpr auto end() const
        requires __detail::__simple_view<_V>
    {
        return ranges::end(_M_base);
    }

    constexpr auto size()
        requires sized_range<_V>
    {
        return ranges::size(_M_base);
    }

    constexpr auto size() const
        requires sized_range<const _V>
    {
        return ranges::size(_M_base);
    }
};

template <class _R>
using keys_view = elements_view<views::all_t<_R>, 0>;

template <class _R>
using values_view = elements_view<views::all_t<_R>, 1>;

namespace __detail
{

template <size_t _N>
struct __elements
{
    template <class _E>
    constexpr decltype(auto) operator()(_E&& __e) const
        noexcept(noexcept(elements_view<views::all_t<_E>, _N>{std::forward<_E>(__e)}))
    {
        return elements_view<views::all_t<_E>, _N>{std::forward<_E>(__e)};
    }
};

} // namespace __detail

namespace views::inline __elements
{

template <size_t _N>
inline constexpr __detail::__elements<_N> elements = {};

} // namespace views::__elements


} // namespace ranges
#endif // if __cpp_concepts


namespace views = ranges::views;


} // namespace __XVI_STD_UTILITY_NS


// Required for the implementation of ranges::find_if and we have a mutual dependency.
#include <System/C++/Utility/RangesAlgorithm.hh>


#endif /* ifndef __SYSTEM_CXX_UTILITY_RANGES_H */
