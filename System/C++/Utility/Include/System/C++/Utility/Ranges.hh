#pragma once
#ifndef __SYSTEM_CXX_UTILITY_RANGES_H
#define __SYSTEM_CXX_UTILITY_RANGES_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>

#include <System/C++/Utility/Iterator.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Tuple.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace ranges
{

namespace __detail
{

template <class _R>
concept __simple_view = view<_R> && range<const _R>
    && std::same_as<iterator_t<_R>, iterator_t<const _R>>
    && std::same_as<sentinel_t<_R>, sentinel_t<const _R>>;

template <class _I>
concept __has_arrow = input_iterator<_I> && (std::is_pointer_v<_I> || requires(_I __i) { __i.operator->(); });

template <class _T, class _U>
concept __different_from = !std::same_as<std::remove_cvref_t<_T>, std::remove_cvref_t<_U>>;

} // namespace __detail


template <class _D>
    requires std::is_class_v<_D> && std::same_as<_D, std::remove_cv_t<_D>>
class view_interface :
    public __detail::__view_interface_base
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
    constexpr decltype(auto) operator[](range_difference_t<_R> __n)
    {
        return ranges::begin(__derived())[__n];
    }

    template <random_access_range _R = const _D>
    constexpr decltype(auto) operator[](range_difference_t<_R> __n) const
    {
        return ranges::begin(__derived())[__n];
    }
};


namespace __detail
{

template <class _From, class _To>
concept __uses_nonqualification_pointer_conversion =
    std::is_pointer_v<_To>
    && std::is_pointer_v<_From>
    && !std::convertible_to<std::remove_pointer_t<_From>(*)[], std::remove_pointer_t<_To>(*)[]>;

template <class _From, class _To>
concept __convertible_to_non_slicing =
    std::convertible_to<_From, _To>
    && !__uses_nonqualification_pointer_conversion<std::decay_t<_From>, std::decay_t<_To>>;

template <class _T>
concept __pair_like = !std::is_reference_v<_T>
    && requires(_T __t)
    {
        typename tuple_size<_T>::type;
        requires std::derived_from<tuple_size<_T>, std::integral_constant<size_t, 2>>;
        typename tuple_element_t<0, std::remove_const_t<_T>>;
        typename tuple_element_t<1, std::remove_const_t<_T>>;
        { get<0>(__t) } -> std::convertible_to<const tuple_element_t<0, _T>&>;
        { get<1>(__t) } -> std::convertible_to<const tuple_element_t<1, _T>&>;  
    };

template <class _T, class _U, class _V>
concept __pair_like_convertible_from = !range<_T>
    && __pair_like<_T>
    && std::constructible_from<_T, _U, _V>
    && __convertible_to_non_slicing<_U, tuple_element_t<0, _T>>
    && std::convertible_to<_V, tuple_element_t<1, _T>>;

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
public:

    // Needs to be public as some subrange-specialized views need to check this.
    static constexpr bool _StoreSize = (_K == subrange_kind::sized && !sized_sentinel_for<_S, _I>);
    static constexpr subrange_kind _Kind = _K;

    using _Iter = _I;

private:

    using __size_storage_t = __detail::__subrange_store_size<_I, _StoreSize>;
    using __size_type = typename __size_storage_t::__type;

    _I _M_begin = _I();
    _S _M_end   = _S();
    [[no_unique_address]] __size_storage_t _M_size;


public:

    subrange() requires std::default_initializable<_I> = default;

    constexpr subrange(__detail::__convertible_to_non_slicing<_I> auto __i, _S __s)
        requires (!_StoreSize)
        : _M_begin(std::move(__i)), _M_end(__s)
    {
    }

    constexpr subrange(__detail::__convertible_to_non_slicing<_I> auto __i, _S __s, std::make_unsigned_t<iter_difference_t<_I>> __n)
        requires (_K == subrange_kind::sized)
        : _M_begin(std::move(__i)), _M_end(__s)
    {
        if constexpr (_StoreSize)
            _M_size.__size = __n;
    }

    template<__detail::__different_from<subrange> _R>
        requires borrowed_range<_R>
            && __detail::__convertible_to_non_slicing<iterator_t<_R>, _I>
            && std::convertible_to<sentinel_t<_R>, _S>
    constexpr subrange(_R&& __r)
        requires (!_StoreSize || sized_range<_R>)
        : _M_begin(ranges::begin(__r)),
          _M_end(ranges::end(__r))
    {
        if constexpr (_StoreSize)
            _M_size.__size = static_cast<__size_type>(ranges::size(__r));
    }

    template <borrowed_range _R>
        requires __detail::__convertible_to_non_slicing<iterator_t<_R>, _I>
            && convertible_to<sentinel_t<_R>, _I>
    constexpr subrange(_R&& __r, __size_type __n)
        requires (_K == subrange_kind::sized)
        : subrange{ranges::begin(__r), ranges::end(__r), __n}
    {
    }

    template <__detail::__different_from<subrange> _PairLike>
        requires __detail::__pair_like_convertible_from<_PairLike, const _I&, const _S&>
    constexpr operator _PairLike() const
    {
        return _PairLike(_M_begin, _M_end);
    }

    constexpr _I begin() const
        requires std::copyable<_I>
    {
        return _M_begin;
    }

    [[nodiscard]] constexpr _I begin()
        requires (!std::copyable<_I>)
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

    [[nodiscard]] constexpr subrange prev(iter_difference_t<_I> __n = 1) const
        requires bidirectional_iterator<_I>
    {
        auto __tmp = *this;
        __tmp.advance(-__n);
        return __tmp;
    }
    
    constexpr subrange& advance(iter_difference_t<_I> __n)
    {
        if constexpr (bidirectional_iterator<_I>)
        {
            if (__n < 0)
            {
                ranges::advance(_M_begin, __n);
                if constexpr (_StoreSize)
                    _M_size += static_cast<__size_type>(-__n);
                return *this;
            }
        }

        auto __d = __n - ranges::advance(_M_begin, __n, _M_end);
        if constexpr (_StoreSize)
            _M_size -= static_cast<__size_type>(__d);
        return *this;
    }
};

template <input_or_output_iterator _I, sentinel_for<_I> _S>
subrange(_I, _S) -> subrange<_I, _S>;

template <input_or_output_iterator _I, sentinel_for<_I> _S>
subrange(_I, _S, std::make_unsigned_t<iter_difference_t<_I>>) -> subrange<_I, _S, subrange_kind::sized>;

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

template <range _R>
using borrowed_subrange_t = std::conditional_t<borrowed_range<_R>, subrange<iterator_t<_R>>, dangling>;


namespace __detail
{


template <class _Container>
constexpr bool __reservable_container =
    sized_range<_Container>
    && requires (_Container& __c, range_size_t<_Container> __n)
    {
        __c.reserve(__n);
        { __c.capacity() } -> std::same_as<decltype(__n)>;
        { __c.max_size() } -> std::same_as<decltype(__n)>;
    };

template <class _Container, class _Ref>
constexpr bool __container_insertable =
    requires (_Container& __c, _Ref&& __ref)
    {
        requires (requires { __c.push_back(std::forward<_Ref>(__ref)); }
                  || requires { __c.insert(__c.end(), std::forward<_Ref>(__ref)); });
    };

template <class _Ref, class _C>
auto __container_inserter(_C& __c)
{
    if constexpr (requires { __c.push_back(declval<_Ref>()); })
        return back_inserter(__c);
    else
        return inserter(__c, __c.end());
}


} // namespace __detail


template <range _R, class _Allocator = allocator<byte>>
struct elements_of
{
    [[no_unique_address]] _R range;
    [[no_unique_address]] _Allocator allocator = _Allocator();
};

template <class _R, class _Allocator = allocator<byte>>
elements_of(_R&&, _Allocator = _Allocator()) -> elements_of<_R&&, _Allocator>;


} // namespace ranges


namespace __detail
{


template <input_or_output_iterator _I, sentinel_for<_I> _S, ranges::subrange_kind _K>
struct __is_tuple_like<ranges::subrange<_I, _S, _K>> : true_type {};

template <input_or_output_iterator _I, sentinel_for<_I> _S, ranges::subrange_kind _K>
struct __is_ranges_subrange_specialisation<ranges::subrange<_I, _S, _K>> : true_type {};


} // namespace __detail


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_RANGES_H */
