#ifndef __SYSTEM_CXX_CORE_ITERATORADAPTORS_H
#define __SYSTEM_CXX_CORE_ITERATORADAPTORS_H


#include <System/C++/LanguageSupport/InitializerList.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/IteratorTraits.hh>
#include <System/C++/Core/IteratorUtils.hh>
#include <System/C++/Core/Variant.hh>


namespace __XVI_STD_CORE_NS_DECL
{


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
        return _M_current[-__n - 1];
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
            && requires(_T&& __t) { ranges::rbegin(static_cast<const _T&>(__XVI_STD_NS::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rbegin(static_cast<const _T&>(__XVI_STD_NS::forward<_T>(__t)))))
    {
        return ranges::rbegin(static_cast<const _T&>(__XVI_STD_NS::forward<_T>(__t)));
    }

    template <class _T>
        requires (!is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rbegin(static_cast<const _T&&>(__XVI_STD_NS::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rbegin(static_cast<const _T&&>(__XVI_STD_NS::forward<_T>(__t)))))
    {
        return ranges::rbegin(static_cast<const _T&&>(__XVI_STD_NS::forward<_T>(__t)));
    }
};

struct __crend
{
    template <class _T>
        requires (is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rend(static_cast<const _T&>(__XVI_STD_NS::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rend(static_cast<const _T&>(__XVI_STD_NS::forward<_T>(__t)))))
    {
        return ranges::rend(static_cast<const _T&>(__XVI_STD_NS::forward<_T>(__t)));
    }

    template <class _T>
        requires (!is_lvalue_reference_v<_T>
            && requires(_T&& __t) { ranges::rend(static_cast<const _T&&>(__XVI_STD_NS::forward<_T>(__t))); })
    constexpr decltype(auto) operator()(_T&& __t) const
        noexcept(noexcept(ranges::rend(static_cast<const _T&&>(__XVI_STD_NS::forward<_T>(__t)))))
    {
        return ranges::rend(static_cast<const _T&&>(__XVI_STD_NS::forward<_T>(__t)));
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
        _M_container->push_back(__XVI_STD_NS::move(__value));
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
        _M_container->push_front(__XVI_STD_NS::move(__value));
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
        iter = container->insert(iter, __XVI_STD_NS::move(__value));
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


template <input_iterator> class basic_const_iterator;

template <indirectly_readable _It>
using iter_const_reference_t = common_reference_t<const iter_value_t<_It>&&, iter_reference_t<_It>>;

namespace __detail
{

template <class _It>
concept __constant_iterator = input_iterator<_It> && same_as<iter_const_reference_t<_It>, iter_reference_t<_It>>;

template <input_iterator _I>
struct __const_iterator { using type = basic_const_iterator<_I>; };

template <input_iterator _I>
    requires __constant_iterator<_I>
struct __const_iterator<_I> { using type = _I; };

template <semiregular _S>
struct __const_sentinel { using type = _S; };

template <semiregular _S>
    requires input_iterator<_S>
struct __const_sentinel<_S> { using type = __const_iterator<_S>::type; };

template <class _T>
struct __is_basic_const_iterator_specialization : false_type {};

template <input_iterator _T>
struct __is_basic_const_iterator_specialization<basic_const_iterator<_T>> : true_type {};

template <class _I>
concept __not_a_const_iterator = !__is_basic_const_iterator_specialization<_I>::value;

template <indirectly_readable _I>
using __iter_const_rvalue_reference_t = common_reference_t<const iter_value_t<_I>&&, iter_rvalue_reference_t<_I>>;

template <input_iterator _I>
struct __basic_const_iterator_category {};

template <forward_iterator _I>
struct __basic_const_iterator_category<_I> { using iterator_category = iterator_traits<_I>::iterator_category; };

template <input_iterator _I>
struct __basic_const_iterator_concept { using type = input_iterator_tag; };

template <forward_iterator _I>
struct __basic_const_iterator_concept<_I> { using type = forward_iterator_tag; };

template <bidirectional_iterator _I>
struct __basic_const_iterator_concept<_I> { using type = bidirectional_iterator_tag; };

template <random_access_iterator _I>
struct __basic_const_iterator_concept<_I> { using type = random_access_iterator_tag; };

template <contiguous_iterator _I>
struct __basic_const_iterator_concept<_I> { using type = contiguous_iterator_tag; };

template <class _T, class _U>
concept __different_from = !same_as<remove_cvref_t<_T>, remove_cvref_t<_U>>;

} // namespace __detail

template <input_iterator _I>
using const_iterator = __detail::__const_iterator<_I>::type;

template <semiregular _S>
using const_sentinel = __detail::__const_sentinel<_S>::type;

template <input_iterator _Iterator>
class basic_const_iterator :
    public __detail::__basic_const_iterator_category<_Iterator>
{
    using __reference = iter_const_reference_t<_Iterator>;
    using __rvalue_reference = __detail::__iter_const_rvalue_reference_t<_Iterator>;

public:

    using iterator_concept = __detail::__basic_const_iterator_concept<_Iterator>::type;
    using value_type = iter_value_t<_Iterator>;
    using difference_type = iter_difference_t<_Iterator>;

    basic_const_iterator()
        requires default_initializable<_Iterator> = default;

    constexpr basic_const_iterator(_Iterator __current) :
        _M_current(__XVI_STD_NS::move(__current))
    {
    }

    template <convertible_to<_Iterator> _U>
    constexpr basic_const_iterator(basic_const_iterator<_U> __current) :
        _M_current(__XVI_STD_NS::move(__current._M_current))
    {
    }

    template <__detail::__different_from<basic_const_iterator> _T>
        requires convertible_to<_T, _Iterator>
    constexpr basic_const_iterator(_T&& __current) :
        _M_current(__XVI_STD_NS::forward<_T>(__current))
    {
    }

    constexpr const _Iterator& base() const & noexcept
    {
        return _M_current;
    }

    constexpr _Iterator base() &&
    {
        return __XVI_STD_NS::move(_M_current);
    }

    constexpr __reference operator*() const
    {
        return static_cast<__reference>(*_M_current);
    }

    constexpr const auto* operator->() const
        requires is_lvalue_reference_v<iter_reference_t<_Iterator>>
            && same_as<remove_cvref_t<iter_reference_t<_Iterator>>, value_type>
    {
        if constexpr (contiguous_iterator<_Iterator>)
            return __XVI_STD_NS::to_address(_M_current);
        else
            return __XVI_STD_NS::addressof(*_M_current);
    }

    constexpr basic_const_iterator& operator++()
    {
        ++_M_current;
        return *this;
    }

    constexpr void operator++(int)
    {
        ++_M_current;
    }

    constexpr basic_const_iterator operator++(int)
        requires forward_iterator<_Iterator>
    {
        auto __tmp = *this;
        ++*this;
        return __tmp;
    }

    constexpr basic_const_iterator& operator--()
        requires bidirectional_iterator<_Iterator>
    {
        --_M_current;
        return *this;
    }

    constexpr basic_const_iterator operator--(int)
        requires bidirectional_iterator<_Iterator>
    {
        auto __tmp = *this;
        --*this;
        return __tmp;
    }

    constexpr basic_const_iterator& operator+=(difference_type __n)
        requires random_access_iterator<_Iterator>
    {
        _M_current += __n;
        return *this;
    }

    constexpr basic_const_iterator& operator-=(difference_type __n)
        requires random_access_iterator<_Iterator>
    {
        _M_current -= __n;
        return *this;
    }

    constexpr __reference operator[](difference_type __n) const
        requires random_access_iterator<_Iterator>
    {
        return static_cast<__reference>(_M_current[__n]);
    }

    template <sentinel_for<_Iterator> _S>
    constexpr bool operator==(const _S& __s) const
    {
        return _M_current == __s;
    }

    constexpr bool operator<(const basic_const_iterator& __y) const
        requires random_access_iterator<_Iterator>
    {
        return _M_current < __y._M_current;
    }

    constexpr bool operator>(const basic_const_iterator& __y) const
        requires random_access_iterator<_Iterator>
    {
        return _M_current > __y._M_current;
    }

    constexpr bool operator<=(const basic_const_iterator& __y) const
        requires random_access_iterator<_Iterator>
    {
        return _M_current <= __y._M_current;
    }

    constexpr bool operator>=(const basic_const_iterator& __y) const
        requires random_access_iterator<_Iterator>
    {
        return _M_current >= __y._M_current;
    }

    constexpr auto operator<=>(const basic_const_iterator& __y) const
        requires random_access_iterator<_Iterator> && three_way_comparable<_Iterator>
    {
        return _M_current <=> __y._M_current;
    }

    template <__detail::__different_from<basic_const_iterator> _I>
    constexpr bool operator<(const _I& __y) const
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return _M_current < __y;
    }

    template <__detail::__different_from<basic_const_iterator> _I>
    constexpr bool operator>(const _I& __y) const
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return _M_current > __y;
    }

    template <__detail::__different_from<basic_const_iterator> _I>
    constexpr bool operator<=(const _I& __y) const
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return _M_current <= __y;
    }

    template <__detail::__different_from<basic_const_iterator> _I>
    constexpr bool operator>=(const _I& __y) const
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return _M_current >= __y;
    }

    template <__detail::__different_from<basic_const_iterator> _I>
    constexpr auto operator<=>(const _I& __y) const
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I> && three_way_comparable_with<_Iterator, _I>
    {
        return _M_current <=> __y;
    }

    template <__detail::__not_a_const_iterator _I>
    friend constexpr bool operator<(const _I& __x, const basic_const_iterator& __y)
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return __x < __y._M_current;
    }

    template <__detail::__not_a_const_iterator _I>
    friend constexpr bool operator>(const _I& __x, const basic_const_iterator& __y)
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return __x > __y._M_current;
    }

    template <__detail::__not_a_const_iterator _I>
    friend constexpr bool operator<=(const _I& __x, const basic_const_iterator& __y)
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return __x <= __y._M_current;
    }

    template <__detail::__not_a_const_iterator _I>
    friend constexpr bool operator>=(const _I& __x, const basic_const_iterator& __y)
        requires random_access_iterator<_Iterator> && totally_ordered_with<_Iterator, _I>
    {
        return __x >= __y._M_current;
    }

    friend constexpr basic_const_iterator operator+(const basic_const_iterator& __i, difference_type __n)
        requires random_access_iterator<_Iterator>
    {
        return basic_const_iterator(__i._M_current + __n);
    }

    friend constexpr basic_const_iterator operator+(difference_type __n, const basic_const_iterator& __i)
        requires random_access_iterator<_Iterator>
    {
        return basic_const_iterator(__i._M_current + __n);
    }

    friend constexpr basic_const_iterator operator-(const basic_const_iterator& __i, difference_type __n)
        requires random_access_iterator<_Iterator>
    {
        return basic_const_iterator(__i._M_current - __n);
    }

    template <sized_sentinel_for<_Iterator> _S>
    constexpr difference_type operator-(const _S& __s) const
    {
        return _M_current - __s;
    }

    template <__detail::__not_a_const_iterator _S>
        requires sized_sentinel_for<_S, _Iterator>
    friend constexpr difference_type operator-(const _S& __x, const basic_const_iterator& __y)
    {
        return __x - __y._M_current;
    }

    friend constexpr __rvalue_reference iter_move(const basic_const_iterator& __i)
        noexcept(noexcept(static_cast<__rvalue_reference>(ranges::iter_move(__i._M_current))))
    {
        return static_cast<__rvalue_reference>(ranges::iter_move(__i._M_current));
    }

private:

    template <input_iterator> friend class basic_const_iterator;

    _Iterator _M_current = _Iterator();
};

template <class _T, common_with<_T> _U>
    requires input_iterator<common_type_t<_T, _U>>
struct common_type<basic_const_iterator<_T>, _U>
    { using type = basic_const_iterator<common_type_t<_T, _U>>; };

template <class _T, common_with<_T> _U>
    requires input_iterator<common_type_t<_T, _U>>
struct common_type<_U, basic_const_iterator<_T>>
    { using type = basic_const_iterator<common_type_t<_T, _U>>; };

template <class _T, common_with<_T> _U>
    requires input_iterator<common_type_t<_T, _U>>
struct common_type<basic_const_iterator<_T>, basic_const_iterator<_U>>
    { using type = basic_const_iterator<common_type_t<_T, _U>>; };

template <input_iterator _I>
constexpr const_iterator<_I> make_const_iterator(_I __it)
{
    return __it;
}

template <semiregular _S>
constexpr const_sentinel<_S> make_const_sentinel(_S __s)
{
    return __s;
}


template <semiregular _S>
class move_sentinel
{
public:

    constexpr move_sentinel() = default;
    constexpr move_sentinel(const move_sentinel&) = default;
    constexpr move_sentinel(move_sentinel&&) = default;

    constexpr explicit move_sentinel(_S __s) :
        _M_last(__XVI_STD_NS::move(__s))
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
        _M_current(__XVI_STD_NS::move(__i))
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
        return __XVI_STD_NS::move(_M_current);
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
    requires (!same_as<_I, _S>)
class common_iterator
{
public:

    constexpr common_iterator() = default;
    constexpr common_iterator(const common_iterator&) = default;
    constexpr common_iterator(common_iterator&&) = default;

    constexpr common_iterator(_I __i) :
        _M_variant{in_place_type<_I>, __XVI_STD_NS::move(__i)}
    {
    }

    constexpr common_iterator(_S __s) :
        _M_variant{in_place_type<_S>, __XVI_STD_NS::move(__s)}
    {
    }

    template <class _I2, class _S2>
        requires convertible_to<const _I2&, _I>
            && convertible_to<const _S2&, _S>
    constexpr common_iterator(const common_iterator<_I2, _S2>& __x) :
        _M_variant{__x._M_variant.index() == 0 ? variant<_I, _S>{in_place_index<0>, get<0>(__x._M_variant)}
                                               : variant<_I, _S>{in_place_index<1>, get<1>(__x._M_variant)}}
    {
    }

    constexpr common_iterator& operator=(const common_iterator&) = default;
    constexpr common_iterator& operator=(common_iterator&&) = default;

    template <class _I2, class _S2>
        requires assignable_from<_I&, const _I2&>
            && assignable_from<_S&, const _S2&>
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
                    _M_keep(__XVI_STD_NS::move(__x))
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
        return __XVI_STD_NS::move(_M_current);
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
        if constexpr (noexcept(_M_current++))
        {
            return _M_current++;
        }
        else
        {
            try
            {
                return _M_current++;
            }
            catch (...)
            {
                ++_M_length;
                throw;
            }
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
template <class _E> constexpr reverse_iterator<const _E*> rbegin(std::initializer_list<_E> __il)
    { return reverse_iterator<const _E*>(__il.end()); }
template <class _E> constexpr reverse_iterator<const _E*> rend(std::initializer_list<_E> __il)
    { return reverse_iterator<const _E*>(__il.begin()); }
template <class _C> constexpr auto crbegin(const _C& __c) -> decltype(__XVI_STD_NS::rbegin(__c))
    { return __XVI_STD_NS::rbegin(__c); }
template <class _C> constexpr auto crend(const _C& __c) -> decltype(__XVI_STD_NS::rend(__c))
    { return __XVI_STD_NS::rend(__c); }


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_ITERATORADAPTORS_H */
