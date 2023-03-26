#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ITERATOR_H
#define __SYSTEM_CXX_UTILITY_ITERATOR_H


#include <System/C++/Core/IteratorTraits.hh>
#include <System/C++/Core/IteratorUtils.hh>
#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Utility/Abs.hh>
#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/PointerTraits.hh>
#include <System/C++/Utility/Swap.hh>
#include <System/C++/Utility/Variant.hh>
#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/Concepts.hh>


namespace __XVI_STD_UTILITY_NS
{

namespace ranges::__detail
{
    struct __less;
}


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

template <class _Iterator1, three_way_comparable_with<_Iterator1> _Iterator2>
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

void rbegin(auto&) = delete;
void rbegin(const auto&) = delete;

struct __rbegin
{
    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && requires(_T& __t) { { __DECAY(__t).rbegin() } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(static_cast<_T&>(__t).begin())))
    {
        return __DECAY(static_cast<_T&>(__t).rbegin());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __DECAY(__t.rbegin()) } -> input_or_output_iterator; }
            && requires(_T& __t) { { __DECAY(rbegin(__t)) } -> input_or_output_iterator; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(rbegin(static_cast<_T&>(__t)))))
    {
        return __DECAY(rbegin(static_cast<_T&>(__t)));
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __DECAY(__t.rbegin()) } -> input_or_output_iterator; }
            && !requires(_T& __t) { { __DECAY(rbegin(__t)) } -> input_or_output_iterator; }
            && requires(_T& __t)
            {
                { ranges::begin(__t) } -> bidirectional_iterator;
                { ranges::end(__t)   } -> bidirectional_iterator;
            }
            && std::same_as<decltype(ranges::begin(declval<_T&>())), decltype(ranges::end(declval<_T&>()))>)
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

void rend(auto&) = delete;
void rend(const auto&) = delete;

struct __rend
{
    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && requires(_T& __t) { { __DECAY(__t.rend()) } -> sentinel_for<decltype(ranges::rbegin(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(static_cast<_T&>(__t).begin())))
    {
        return __DECAY(static_cast<_T&>(__t).begin());
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __DECAY(__t.rend()) } -> sentinel_for<decltype(ranges::rbegin(__t))>; }
            && requires(_T& __t) { { __DECAY(rend(__t)) } -> sentinel_for<decltype(ranges::rbegin(__t))>; })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(__DECAY(rbegin(static_cast<_T&>(__t)))))
    {
        return __DECAY(rbegin(static_cast<_T&>(__t)));
    }

    template <class _T>
        requires (__lvalue_or_borrowed_range<_T>
            && !requires(_T& __t) { { __DECAY(__t.rend()) } -> sentinel_for<decltype(ranges::rbegin(__t))>; }
            && !requires(_T& __t) { { __DECAY(rend(__t)) } -> sentinel_for<decltype(ranges::rbegin(__t))>; }
            && requires(_T& __t)
            {
                { ranges::begin(__t) } -> bidirectional_iterator;
                { ranges::end(__t)   } -> bidirectional_iterator;
            }
            && std::same_as<decltype(ranges::begin(declval<_T&>())), decltype(ranges::end(declval<_T&>()))>)
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
        requires (std::is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rbegin(static_cast<const _T&>(std::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rbegin(static_cast<const _T&>(std::forward<_T>(__t)))))
    {
        return ranges::rbegin(static_cast<const _T&>(std::forward<_T>(__t)));
    }

    template <class _T>
        requires (!std::is_lvalue_reference_v<_T>
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
        requires (std::is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rend(static_cast<const _T&>(std::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rend(static_cast<const _T&>(std::forward<_T>(__t)))))
    {
        return ranges::rend(static_cast<const _T&>(std::forward<_T>(__t)));
    }

    template <class _T>
        requires (!std::is_lvalue_reference_v<_T>
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

template <class _Iterator1, three_way_comparable_with<_Iterator1> _Iterator2>
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
    requires (!std::same_as<_I, _S>)
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
        requires std::convertible_to<const _I2&, _I>
            && std::convertible_to<const _S2&, _S>
    constexpr common_iterator(const common_iterator<_I2, _S2>& __x) :
        _M_variant{__x._M_variant.index() == 0 ? variant<_I, _S>{in_place_index<0>, get<0>(__x._M_variant)}
                                               : variant<_I, _S>{in_place_index<1>, get<1>(__x._M_variant)}}
    {
    }

    constexpr common_iterator& operator=(const common_iterator&) = default;
    constexpr common_iterator& operator=(common_iterator&&) = default;

    template <class _I2, class _S2>
        requires std::assignable_from<_I&, const _I2&>
            && std::assignable_from<_S&, const _S2&>
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
                _M_variant.template emplace<0>(get<0>(__x._M_variant));
            else
                _M_variant.template emplace<1>(get<1>(__x._M_variant));
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
                || std::is_reference_v<iter_reference_t<_I>>
                || std::constructible_from<iter_value_t<_I>, iter_reference_t<_I>>)
    {
        if constexpr (std::is_pointer_v<_I> || requires { get<_I>(_M_variant).operator->(); })
        {
            return get<_I>(_M_variant);
        }
        else if constexpr (std::is_reference_v<iter_reference_t<_I>>)
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
        requires sentinel_for<_S, _I2> && std::equality_comparable_with<_I, _I2>
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
    using iterator_category     = conditional_t<std::derived_from<typename iterator_traits<_I>::iterator_category, forward_iterator_tag>,
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
        requires std::assignable_from<_I&, const _I2&>
    constexpr counted_iterator& operator=(const counted_iterator<_I2>& __x)
    {
        _M_current = __x._M_current;
        _M_length = __x._M_length;
    }

    constexpr _I base() const &
        requires std::copy_constructible<_I>
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

    template <std::common_with<_I> _I2>
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

    template <std::common_with<_I> _I2>
    friend constexpr bool operator==(const counted_iterator& __x, const counted_iterator<_I2>& __y)
    {
        return __x._M_length == __y._M_length;
    }

    friend constexpr bool operator==(const counted_iterator& __x, default_sentinel_t)
    {
        return __x._M_length == 0;
    }

    template <std::common_with<_I> _I2>
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
template <class _C> constexpr auto cbegin(const _C& __c) noexcept(noexcept(std::begin(__c))) -> decltype(std::begin(__c))
    { return std::begin(__c); }
template <class _C> constexpr auto cend(const _C& __c) noexcept(noexcept(std::end(__c))) -> decltype(std::end(__c))
    { return std::end(__c); }
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
template <class _C> constexpr auto crbegin(const _C& __c) -> decltype(std::rbegin(__c))
    { return std::rbegin(__c); }
template <class _C> constexpr auto crend(const _C& __c) -> decltype(std::rend(__c))
    { return std::rend(__c); }


template <class _C> constexpr auto size(const _C& __c) -> decltype(__c.size())
    { return __c.size(); }
template <class _T, size_t _N> constexpr size_t size(const _T (&)[_N]) noexcept
    { return _N; }
template <class _C> constexpr auto ssize(const _C& __c) -> std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(__c.size())>>
    { return static_cast<std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(__c.size())>>>(__c.size()); }
template <class _T, std::size_t _N> constexpr std::ptrdiff_t ssize(const _T (&__array)[_N]) noexcept
    { static_assert(_N <= __XVI_STD_NS::numeric_limits<std::ptrdiff_t>::max()); return _N; }
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
