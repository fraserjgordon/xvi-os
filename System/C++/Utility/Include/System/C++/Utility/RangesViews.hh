#ifndef __SYSTEM_CXX_UTILITY_RANGESVIEWS_H
#define __SYSTEM_CXX_UTILITY_RANGESVIEWS_H


#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Algorithm.hh>
#include <System/C++/Utility/Ranges.hh>
#include <System/C++/Utility/Span.hh>
#include <System/C++/Utility/StringFwd.hh>
#include <System/C++/Utility/Variant.hh>


//! @todo ensure all enable_borrowed_view specializations are present!!!
//! @todo same with enable_view


namespace __XVI_STD_UTILITY_NS
{


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
struct __find;
struct __min;
struct __mismatch;
struct __search;

// Algorithm CPO wrapper forward declarations.
constexpr const __find& __make_find();
constexpr const __min& __make_min();
constexpr const __mismatch& __make_mismatch();
constexpr const __search& __make_search();

} // namespace __detail


template <class _T>
    requires std::is_object_v<_T>
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

template <template <class> class _Template, class _T> struct __is_specialization_of : std::false_type {};

template <template <class> class _Template, class... _T> struct __is_specialization_of<_Template, _Template<_T...>> : std::true_type {};

template <template <class> class _Template, class _T>
inline constexpr bool __is_specialization_of_v = __is_specialization_of<_Template, _T>::value;


template <class _T>
    requires std::copy_constructible<_T> && std::is_object_v<_T>
class __copyable_box
    : public optional<_T>
{
public:

    using optional<_T>::optional;

    constexpr __copyable_box()
        noexcept(std::is_nothrow_default_constructible_v<_T>)
        requires std::default_initializable<_T>
        : __copyable_box{in_place}
    {
    }

    __copyable_box& operator=(const __copyable_box& __that)
        noexcept(std::is_nothrow_copy_constructible_v<_T>)
        requires (!std::copyable<_T>)
    {
        if (__that)
            optional<_T>::emplace(*__that);
        else
            optional<_T>::reset();
        return *this;
    }

    __copyable_box& operator=(__copyable_box&& __that)
        noexcept(std::is_nothrow_move_constructible_v<_T>)
        requires (std::movable<_T>)
    {
        if (__that)
            optional<_T>::emplace(std::move(*__that));
        else
            optional<_T>::reset();
        return *this;
    }
};

template <class _T>
    requires std::is_object_v<_T>
class __non_propagating_cache
    : public optional<_T>
{
public:

    using optional<_T>::optional;

    constexpr __non_propagating_cache(const __non_propagating_cache&) noexcept {}

    constexpr __non_propagating_cache(__non_propagating_cache&& __other) noexcept
    {
        __other.reset();
    }

    __non_propagating_cache& operator=(const __non_propagating_cache& __other) noexcept
    {
        if (addressof(__other) != this)
            optional<_T>::reset();

        return *this;
    }

    __non_propagating_cache& operator=(__non_propagating_cache&& __other) noexcept
    {
        optional<_T>::reset();
        __other.reset();
        return *this;
    }

    template <class _I>
    constexpr _T& __emplace_deref(const _I& __i)
    {
        optional<_T>::reset();
        return emplace(*__i);
    }
};

} // namespace __detail


template <std::copy_constructible _T>
    requires std::is_object_v<_T>
class single_view
    : public view_interface<single_view<_T>>
{
public:

    single_view()
        requires std::default_initializable<_T> = default;

    constexpr explicit single_view(const _T& __t)
        : _M_value(__t)
    {
    }

    constexpr explicit single_view(_T&& __t)
        : _M_value(__XVI_STD_NS::move(__t))
    {
    }

    template <class... _Args>
        requires std::constructible_from<_T, _Args...>
    constexpr single_view(in_place_t, _Args&&... __args)
        : _M_value{in_place, std::forward<_Args>(__args)...}
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

    __detail::__copyable_box<_T> _M_value;
};

template <class _T>
single_view(_T) -> single_view<_T>;


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
        { --__i } -> std::same_as<_I&>;
        { __i-- } -> std::same_as<_I>;
    };

template <class _I>
concept __advanceable = __decrementable<_I>
    && std::totally_ordered<_I>
    && requires (_I __i, const _I __j, const __iota_diff_t<_I> __n)
    {
        { __i += __n } -> std::same_as<_I&>;
        { __i -= __n } -> std::same_as<_I&>;
        _I(__j + __n);
        _I(__n + __j);
        _I(__j - __n);
        { __j -  __j } -> std::convertible_to<__iota_diff_t<_I>>;
    };

} // namespace __detail

template <weakly_incrementable _W, semiregular _Bound = unreachable_sentinel_t>
    requires std::__detail::__weakly_equality_comparable_with<_W, _Bound>
        && std::copyable<_W>
class iota_view
    : public view_interface<iota_view<_W, _Bound>>
{
private:

    struct __iterator
    {
    public:

        using iterator_concept = conditional_t<__detail::__advanceable<_W>,
                                                random_access_iterator_tag,
                                                conditional_t<__detail::__decrementable<_W>,
                                                              bidirectional_iterator_tag,
                                                              conditional_t<incrementable<_W>,
                                                                            forward_iterator_tag,
                                                                            input_iterator_tag>
                                                               >
                                                >;
        using iterator_category = input_iterator_tag;
        using value_type = _W;
        using difference_type = __detail::__iota_diff_t<_W>;

        __iterator()
            requires default_initializable<_W> = default;

        constexpr explicit __iterator(_W __value)
            : _M_value(std::move(__value))
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

        _Bound __bound() const
        {
            return _M_bound;
        }

    private:

        _Bound _M_bound = _Bound();
    };

public:

    iota_view()
        requires std::default_initializable<_W> = default;

    constexpr explicit iota_view(_W __value)
        : _M_value(std::move(__value))
    {
    }

    constexpr iota_view(type_identity_t<_W> __value, type_identity_t<_Bound> __bound)
        : _M_value(std::move(__value)), _M_bound(std::move(__bound))
    {
    }

    constexpr iota_view(__iterator __first, __iterator __last)
        requires same_as<_W, _Bound>
        : iota_view(*__first, *__last)
    {
    }

    constexpr iota_view(__iterator __first, unreachable_sentinel_t __last)
        requires (!same_as<_W, _Bound>)
            && same_as<_Bound, unreachable_sentinel_t>
        : iota_view(*__first, __last)
    {
    }

    constexpr iota_view(__iterator __first, __sentinel __last)
        requires (!same_as<_W, _Bound> && !same_as<_Bound, unreachable_sentinel_t>)
        : iota_view(*__first, __last.__bound())
    {
    }

    constexpr __iterator begin() const
    {
        return __iterator{_M_value};
    }

    constexpr auto end() const
    {
        if constexpr (std::same_as<_Bound, unreachable_sentinel_t>)
            return unreachable_sentinel;
        else
            return __sentinel{_M_bound};
    }

    constexpr __iterator end() const
        requires std::same_as<_W, _Bound>
    {
        return __iterator{_M_bound};
    }

    constexpr auto size() const
        requires (std::same_as<_W, _Bound> && __detail::__advanceable<_W>)
            || (std::integral<_W> && std::integral<_Bound>)
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
    requires (!std::__detail::__is_integer_like<_W>
              || !std::__detail::__is_integer_like<_Bound>
              || (std::__detail::__is_signed_integer_like<_W> == std::__detail::__is_signed_integer_like<_Bound>))
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
        return iota_view{std::forward<_E>(__e)};
    }

    template <class _E, class _F>
    constexpr auto operator()(_E&& __e, _F&& __f) const
    {
        return iota_view{std::forward<_E>(__e), std::forward<_F>(__f)};
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

template <movable _Val, class _CharT, class _Traits = char_traits<_CharT>>
    requires default_initializable<_Val>
        && __detail::__stream_extractable<_Val, _CharT, _Traits>
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

template <class _Val>
using istream_view = basic_istream_view<_Val, char>;

template <class _Val>
using wistream_view = basic_istream_view<_Val, wchar_t>;


template <range _R>
    requires std::is_object_v<_R>
class ref_view :
    public view_interface<ref_view<_R>>
{
public:

    // Test functions for the _T&& constructor.
    static void __fun(_R&);
    static void __fun(_R&&) = delete;

    template <__detail::__different_from<ref_view> _T>
        requires (std::convertible_to<_T, _R&>
            && requires { __fun(declval<_T>()); })
    constexpr ref_view(_T&& __t) :
        _M_r(addressof(static_cast<_R&>(std::forward<_T>(__t))))
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

private:

    _R* _M_r = nullptr;
};

template <class _R>
ref_view(_R&&) -> ref_view<_R>;


template <class _T>
inline constexpr bool enable_borrowed_range<ref_view<_T>> = true;


template <range _R>
    requires std::movable<_R> && (!__detail::__is_initializer_list_v<_R>)
class owning_view :
    public view_interface<owning_view<_R>>
{
private:

    _R _M_r = _R();

public:

    owning_view()
        requires std::default_initializable<_R> = default;

    constexpr owning_view(_R&& __t) :
        _M_r(std::move(__t))
    {
    }

    owning_view(owning_view&&) = default;
    owning_view& operator=(owning_view&&) = default;

    constexpr _R& base() & noexcept
        { return _M_r; }

    constexpr const _R& base() const & noexcept
        { return _M_r; }

    constexpr _R&& base() && noexcept
        { return std::move(_M_r); }
    
    constexpr const _R&& base() const && noexcept
        { return std::move(_M_r); }

    constexpr iterator_t<_R> begin()
        { return ranges::begin(_M_r); }

    constexpr sentinel_t<_R> end()
        { return ranges::end(_M_r); }

    constexpr bool empty()
        requires requires { ranges::empty(_M_r); }
        { return ranges::empty(_M_r); }

    constexpr bool empty() const
        requires requires { ranges::empty(_M_r); }
        { return ranges::empty(_M_r); }

    constexpr auto size() requires sized_range<_R>
        { return ranges::size(_M_r); }

    constexpr auto size() const requires sized_range<const _R>
        { return ranges::size(_M_r); }

    constexpr auto data() requires contiguous_range<_R>
        { return ranges::data(_M_r); }

    constexpr auto data() const requires contiguous_range<const _R>
        { return ranges::data(_M_r); }
};

template <class _T>
inline constexpr bool enable_borrowed_range<owning_view<_T>> = enable_borrowed_range<_T>;


namespace __detail
{

// Marker class that can be used to detect inheritance from multiple specializations of range_adaptor_closure<>.
struct __range_adaptor_closure_base {};

// False if there are multiple paths to __range_adaptor_closure_base.
template <class _T>
concept __uniquely_derived_from_range_adaptor_closure = std::derived_from<_T, __range_adaptor_closure_base>;

} // namespace __detail


template <class _D>
    requires std::is_class_v<_D>
        && std::same_as<_D, std::remove_cv_t<_D>>
class range_adaptor_closure : public __detail::__range_adaptor_closure_base {};


namespace __detail
{

// Wrapper functions so that range adaptors capture by-ref arguments properly.
template <class _T>
constexpr auto __range_adaptor_arg(_T& __t)
{
    return reference_wrapper<_T>(__t);
}

template <class _T>
constexpr auto __range_adaptor_arg(const _T& __t)
{
    return reference_wrapper<const _T>(__t);
}

template <class _T>
constexpr decltype(auto) __range_adaptor_arg(_T&& __t)
{
    return std::forward<_T>(__t);
}

template <class _T>
constexpr decltype(auto) __range_adaptor_arg_unwrap(_T&& __t)
{
    return std::forward<_T>(__t);
}

template <class _T>
constexpr decltype(auto) __range_adaptor_arg_unwrap(reference_wrapper<_T> __t)
{
    return __t.get();
}

template <class _Callable>
class __range_adaptor_closure;

template <class _Callable>
class __range_adaptor
{
public:

    __range_adaptor()
        requires std::default_initializable<_Callable> = default;

    constexpr __range_adaptor(const _Callable& __c) :
        _M_callable(__c) {}

    constexpr __range_adaptor(_Callable&& __c) :
        _M_callable(std::move(__c)) {}

    template <class... _Args>
        requires (sizeof...(_Args) > 0)
    constexpr auto operator()(_Args&&... __args) const
    {
        // If the callable cannot be invoked with these arguments, assume that the range argument is missing and return
        // a closure that takes that range as its only argument.
        //
        // Otherwise, it can be invoked so the range is already present.
        if constexpr (std::is_invocable_v<_Callable, _Args...>)
        {
            // Arguments are complete - invoke the callable.
            return _M_callable(std::forward<_Args>(__args)...);
        }
        else
        {
            // Arguments are incomplete - return a closure. Because that closure only takes a single argument, it is a
            // valid range adaptor closure object.
            auto __closure = [__callable = _M_callable, ...__args(__range_adaptor_arg(std::forward<_Args>(__args)))]<class _Range>(_Range&& __r)
            {
                return __callable(std::forward<_Range>(__r), __range_adaptor_arg_unwrap(__args)...);
            };

            return __range_adaptor_closure<decltype(__closure)>(std::move(__closure));
        }
    }

protected:

    [[no_unique_address]] _Callable _M_callable = {};
};

template <class _Callable>
__range_adaptor(_Callable) -> __range_adaptor<_Callable>;

template <class _Callable>
class __range_adaptor_closure :
    public __range_adaptor<_Callable>,
    public range_adaptor_closure<__range_adaptor_closure<_Callable>>
{
public:

    using __range_adaptor<_Callable>::__range_adaptor;

    using __range_adaptor<_Callable>::operator();

    template <viewable_range _R>
        requires requires { declval<_Callable>()(declval<_R>()); }
    constexpr auto operator()(_R&& __r) const
    {
        return _M_callable(std::forward<_R>(__r));
    }
};

template <class _Callable>
__range_adaptor_closure(_Callable) -> __range_adaptor_closure<_Callable>;

} // namespace __detail


template <viewable_range _R, class _T>
    requires std::derived_from<_T, range_adaptor_closure<_T>>
        && __detail::__uniquely_derived_from_range_adaptor_closure<_T>
constexpr auto operator|(_R&& __r, const _T& __adaptor)
{
    return __adaptor(std::forward<_R>(__r));
}

template <class _T, class _U>
    requires std::derived_from<_T, range_adaptor_closure<_T>>
        && std::derived_from<_U, range_adaptor_closure<_U>>
        && __detail::__uniquely_derived_from_range_adaptor_closure<_T>
        && __detail::__uniquely_derived_from_range_adaptor_closure<_U>
constexpr auto operator|(const _T& __first, const _U& __second)
{
    // Return a new range adaptor closure object.
    auto __closure = [__first, __second]<class _R>(_R&& __r)
    {
        return std::forward<_R>(__r) | __first | __second;
    };

    return __detail::__range_adaptor_closure<decltype(__closure)>(std::move(__closure));
}


namespace views
{

inline constexpr __detail::__range_adaptor_closure all = []<class _T>(_T&& __t)
{
    if constexpr (view<std::decay_t<_T>>)
        return auto(std::forward<_T>(__t));
    else if constexpr (requires { ref_view{std::forward<_T>(__t)}; })
        return ref_view{std::forward<_T>(__t)};
    else
        return owning_view{std::forward<_T>(__t)};
};

template <viewable_range _R>
using all_t = decltype(views::all(declval<_R>()));

} // namespace views


template <input_range _V, indirect_unary_predicate<iterator_t<_V>> _Pred>
    requires view<_V> && std::is_object_v<_Pred>
class filter_view :
    public view_interface<filter_view<_V, _Pred>>
{
private:

    _V                              _M_base = _V();
    __detail::__copyable_box<_Pred> _M_pred = {};

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

        __iterator()
            requires std::default_initializable<iterator_t<_V>> = default;

        constexpr __iterator(filter_view& __parent, iterator_t<_V> __current) :
            _M_current(__current),
            _M_parent(addressof(__parent))
        {
        }

        constexpr const iterator_t<_V>& base() const & noexcept
        {
            return _M_current;
        }

        constexpr iterator_t<_V> base() &&
        {
            return std::move(_M_current);
        }

        constexpr range_reference_t<_V> operator*() const
        {
            return *_M_current;
        }

        constexpr iterator_t<_V> operator->() const
            requires __detail::__has_arrow<iterator_t<_V>> && std::copyable<iterator_t<_V>>
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

        sentinel_t<_V> _M_end = sentinel_t<_V>();

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

    filter_view()
        requires std::default_initializable<_V> && std::default_initializable<_Pred> = default;

    constexpr filter_view(_V __base, _Pred __pred) :
        _M_base(std::move(__base)),
        _M_pred(std::move(__pred))
    {
    }

    constexpr _V base() const
        requires std::copy_constructible<_V>
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

    constexpr __iterator begin()
    {
        //! @todo: cache the result to achieve amortised constant time.
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


namespace views
{

inline constexpr __detail::__range_adaptor_closure filter = []<class _E, class _P>(_E&& __e, _P&& __p)
{
    return filter_view{std::forward<_E>(__e), std::forward<_P>(__p)};
};

} // namespace views


template <input_range _V, std::copy_constructible _F>
    requires view<_V>
        && std::is_object_v<_F>
        && std::regular_invocable<_F&, range_reference_t<_V>>
        && __XVI_STD_UTILITY_NS::__detail::__can_reference<std::invoke_result_t<_F, range_reference_t<_V>>>
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

    transform_view()
        requires std::default_initializable<_V> && std::default_initializable<_F> = default;

    constexpr transform_view(_V __base, _F __fun) :
        _M_base(std::move(__base)),
        _M_fun(std::move(__fun))
    {
    }

    constexpr _V base() const &
        requires std::copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
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
    __detail::__copyable_box<_F>    _M_fun;
};

template <class _R, class _F>
transform_view(_R&&, _F) -> transform_view<views::all_t<_R>, _F>;


namespace views
{

inline constexpr __detail::__range_adaptor_closure transform = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    return transform_view{std::forward<_E>(__e), std::forward<_F>(__f)};
};

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

        using _Base     = std::conditional_t<_Const, const _V, _V>;

        template <bool _OtherConst>
        using _CI       = counted_iterator<iterator_t<std::conditional_t<_Const, const _V, _V>>>;

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

        friend constexpr bool operator==(const _CI<_Const>& __y, const __sentinel& __x)
        {
            return __y.count() == 0 || __y.base() == __x._M_end;
        }

        template <bool _OtherConst = !_Const>
            requires sentinel_for<sentinel_t<_Base>, iterator_t<std::conditional_t<_OtherConst, const _V, _V>>>
        friend constexpr bool operator==(const _CI<_OtherConst>& __y, const __sentinel& __x)
        {
            return __y.count() == 0 || __y.base() == __x._M_end;
        }
    };

public:

    take_view()
        requires std::default_initializable<_V> = default;

    constexpr take_view(_V __base, range_difference_t<_V> __count) :
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


namespace views
{

inline constexpr __detail::__range_adaptor_closure take = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    using _T = std::remove_cvref_t<_E>;
    using _D = range_difference_t<_F>;

    if constexpr (__detail::__is_specialization_of_v<empty_view, _T>)
    {
        return ((void) std::forward<_F>(__f), auto(std::forward<_E>(__e)));
    }
    else if constexpr (random_access_range<_T> && sized_range<_T>
        && (__detail::__is_specialization_of_v<span, _T>
            || __detail::__is_specialization_of_v<basic_string_view, _T>
            || __detail::__is_specialization_of_v<subrange, _T>
            || __detail::__is_specialization_of_v<iota_view, _T>))
    {
        if constexpr (__detail::__is_specialization_of_v<span, _T>)
            return span<typename _T::element_type>(ranges::begin(std::forward<_E>(__e)), ranges::begin(std::forward<_E>(__e)) + std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f));
        else if constexpr (__detail::__is_specialization_of_v<basic_string_view, _T>)
            return _T(ranges::begin(std::forward<_E>(__e)), ranges::begin(std::forward<_E>(__e)) + std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f));
        else if constexpr (__detail::__is_specialization_of_v<subrange, _T>)
            return ranges::subrange<iterator_t<_T>>(ranges::begin(std::forward<_E>(__e)), ranges::begin(std::forward<_E>(__e)) + std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f));  
        else
            return ranges::iota_view(*ranges::begin(std::forward<_E>(__e)), *(ranges::begin(std::forward<_E>(__e)) + std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f))); 
    }
    else
    {
        return ranges::take_view(std::forward<_E>(__e), std::forward<_F>(__f));
    }
};

} // namespace views


template <view _V, class _Pred>
    requires input_range<_V>
        && std::is_object_v<_Pred>
        && indirect_unary_predicate<const _Pred, iterator_t<_V>>
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
            return __y._M_end == __x || !std::invoke(*__y._M_pred, *__x);
        }

        template <bool _OtherConst = !_Const>
            requires sentinel_for<sentinel_t<__base_t>, iterator_t<std::conditional<_OtherConst, const _V, _V>>>
        friend constexpr bool operator==(const iterator_t<std::conditional_t<_OtherConst, const _V, _V>>& __x, const __sentinel& __y)
        {
            return __y._M_end == __x || !std::invoke(*__y._M_pred, *__x);
        }
    };

public:

    take_while_view()
        requires std::default_initializable<_V>
            && std::default_initializable<_Pred>
        = default;

    constexpr take_while_view(_V __base, _Pred __pred) :
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
        requires (!__detail::__simple_view<_V>)
    {
        return ranges::begin(_M_base);
    }

    constexpr auto begin() const
        requires range<const _V>
            && indirect_unary_predicate<const _Pred, iterator_t<const _V>>
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
            && indirect_unary_predicate<const _Pred, iterator_t<const _V>>
    {
        return __sentinel<true>(ranges::end(_M_base), addressof(*_M_pred));
    }

private:

    _V                              _M_base;
    __detail::__copyable_box<_Pred> _M_pred;
};

template <class _R, class _Pred>
take_while_view(_R&&, _Pred) -> take_while_view<views::all_t<_R>, _Pred>;

namespace views
{

inline constexpr __detail::__range_adaptor_closure take_while = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    return take_while_view(std::forward<_E>(__e), std::forward<_F>(__f));
};

} // namespace views


template <view _V>
class drop_view :
    public view_interface<drop_view<_V>>
{
public:

    drop_view()
        requires std::default_initializable<_V> = default;

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
        requires (!(__detail::__simple_view<_V> && random_access_range<_V> && sized_range<_V>))
    {
        // The range concept requires amortised constant-time complexity so cache the result.
        if (!_M_begin)
            _M_begin = ranges::next(ranges::begin(_M_base), _M_count, ranges::end(_M_base));

        return _M_begin;
    }

    constexpr auto begin() const
        requires random_access_range<const _V> && sized_range<const _V>
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


namespace views
{

inline constexpr __detail::__range_adaptor_closure drop = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    using _T = std::remove_cvref_t<_E>;
    using _D = range_difference_t<_E>;

    static_assert(std::convertible_to<_F, _D>);

    if constexpr (__detail::__is_specialization_of_v<empty_view, _T>)
    {
        return ((void) std::forward<_F>(__f), auto(std::forward<_E>(__e)));
    }
    else if constexpr (random_access_range<_T> && sized_range<_T>
        && (__detail::__is_specialization_of_v<span, _T>
            || __detail::__is_specialization_of_v<basic_string_view, _T>
            || __detail::__is_specialization_of_v<subrange, _T>
            || __detail::__is_specialization_of_v<iota_view, _T>))
    {
        if constexpr (__detail::__is_specialization_of_v<span, _T>)
            return span<typename _T::element_type>(ranges::begin(std::forward<_E>(__e)) + std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f), ranges::end(std::forward<_E>(__e)));
        else if constexpr (!__detail::__is_specialization_of_v<subrange, _T> || !_T::_StoreSize)
            return _T(ranges::begin(std::forward<_E>(__e)) + std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f), ranges::end(std::forward<_E>(__e)));
        else
            return _T(ranges::begin(std::forward<_E>(__e)) + std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f), ranges::end(std::forward<_E>(__e)), __detail::__to_unsigned_like(ranges::distance(std::forward<_E>(__e)) - std::min<_D>(ranges::distance(std::forward<_E>(__e)), __f)));
    }
    else
    {
        return ranges::drop_view(std::forward<_E>(__e), std::forward<_F>(__f));
    }
};

} // namespace views


template <view _V, class _Pred>
    requires input_range<_V>
        && std::is_object_v<_Pred>
        && indirect_unary_predicate<const _Pred, iterator_t<_V>>
class drop_while_view :
    public view_interface<drop_while_view<_V, _Pred>>
{
public:

    drop_while_view()
        requires std::default_initializable<_V>
            && std::default_initializable<_Pred>
        = default;

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
    __detail::__copyable_box<_Pred>     _M_pred;
    optional<iterator_t<_V>>            _M_begin = nullopt;
};

template <class _R, class _Pred>
drop_while_view(_R&&, _Pred) -> drop_while_view<views::all_t<_R>, _Pred>;


namespace views
{

inline constexpr __detail::__range_adaptor_closure drop_while = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    return drop_while_view(std::forward<_E>(__e), std::forward<_F>(__f));
};

} // namespace views


template <input_range _V>
    requires view<_V>
        && input_range<range_reference_t<_V>>
class join_view :
    public view_interface<join_view<_V>>
{
private:

    using _InnerRange       = range_reference_t<_V>;

    template <bool _Const>
    struct __iterator
    {
    private:

        using _Parent       = std::conditional_t<_Const, const join_view, join_view>;
        using _Base         = std::conditional_t<_Const, const _V, _V>;
        using _OuterIter    = iterator_t<_Base>;
        using _InnerIter    = iterator_t<range_reference_t<_Base>>;

        using _InnerC       = typename iterator_traits<iterator_t<_Base>>::iterator_category;
        using _OuterC       = typename iterator_traits<iterator_t<range_reference_t<_Base>>>::iterator_category;

        static constexpr bool __ref_is_glvalue = std::is_reference_v<range_reference_t<_Base>>;

        _OuterIter                              _M_outer = _OuterIter();
        _InnerIter                              _M_inner = _InnerIter();
        _Parent*                                _M_parent = nullptr;

        constexpr void __satisfy()
        {
            auto __update_inner = [this](const iterator_t<_Base> __x) -> auto&&
            {
                if constexpr (__ref_is_glvalue)
                    return *__x;
                else
                    return _M_parent->_M_inner.__emplace_deref(__x);
            };

            for (; _M_outer != ranges::end(_M_parent->_M_base); ++_M_outer)
            {
                auto&& __inner = __update_inner(_M_outer);
                _M_inner = ranges::begin(__inner);

                if (_M_inner != ranges::end(__inner))
                    return;
            }

            if constexpr (__ref_is_glvalue)
                _M_inner = _InnerIter();
        }

    public:

        using iterator_concept      = conditional_t<__ref_is_glvalue && bidirectional_range<_Base> && bidirectional_range<range_reference_t<_Base>> && common_range<range_reference_t<_Base>>,
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

        __iterator()
            requires std::default_initializable<_OuterIter>
                && std::default_initializable<_InnerIter>
            = default;

        constexpr __iterator(_Parent& __parent, iterator_t<_V> __outer) :
            _M_outer(std::move(__outer)),
            _M_parent(addressof(__parent))
        {
            __satisfy();
        }

        constexpr __iterator(__iterator<!_Const> __i)
            requires _Const
                && convertible_to<iterator_t<_V>, _OuterIter>
                && convertible_to<iterator_t<_InnerRange>, _InnerIter> :
            _M_outer(std::move(__i._M_outer)),
            _M_inner(std::move(__i._M_inner)),
            _M_parent(__i._M_parent)
        {
        }

        constexpr decltype(auto) operator*() const
        {
            return *_M_inner;
        }

        constexpr _InnerIter operator->() const
            requires __detail::__has_arrow<_InnerIter> && copyable<_InnerIter>
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
                    return *_M_parent->_M_inner;
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
            requires indirectly_swappable<_InnerIter>
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

    struct __empty_inner {};

    using __inner_t = std::conditional<std::is_reference_v<_InnerRange>, __empty_inner, __detail::__non_propagating_cache<std::remove_cv_t<_InnerRange>>>;

    _V                          _M_base =_V();
    __inner_t                   _M_inner;

public:

    join_view() 
        requires std::default_initializable<_V> = default;

    constexpr explicit join_view(_V __base) :
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
    {
        constexpr bool _UseConst = __detail::__simple_view<_V> && std::is_reference_v<range_reference_t<_V>>;
        return __iterator<_UseConst>{*this, ranges::begin(_M_base)};
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


namespace views
{

inline constexpr __detail::__range_adaptor_closure join = []<class _E>(_E&& __e)
{
    return join_view<views::all_t<_E>>{std::forward<_E>(__e)};
};

} // namespace views


namespace __detail
{

template <class _R, class _P>
concept __compatible_joinable_ranges =
    common_with<range_value_t<_R>, range_value_t<_P>>
    && common_reference_with<range_reference_t<_R>, range_reference_t<_P>>
    && common_reference_with<range_rvalue_reference_t<_R>, range_rvalue_reference_t<_P>>;

template <class _R>
concept __bidirectional_common =
    bidirectional_range<_R> && common_range<_R>;

} // namespace __detail


template <input_range _V, forward_range _Pattern>
    requires view<_V>
        && input_range<range_reference_t<_V>>
        && view<_Pattern>
        && __detail::__compatible_joinable_ranges<range_reference_t<_V>, _Pattern>
class join_with_view
{
private:

    using _InnerRange = range_reference_t<_V>;

    template <bool _Const>
    struct __iterator
    {
        using _Parent           = std::conditional_t<_Const, const join_with_view, join_with_view>;
        using _Base             = std::conditional_t<_Const, const _V, _V>;
        using _InnerBase        = range_reference_t<_Base>;
        using _PatternBase      = std::conditional_t<_Const, const _Pattern, _Pattern>;

        using _OuterIter        = iterator_t<_Base>;
        using _InnerIter        = iterator_t<_InnerBase>;
        using _PatternIter      = iterator_t<_PatternBase>;

        static constexpr bool __ref_is_glvalue = std::is_reference_v<_InnerBase>;


        _Parent*                            _M_parent = nullptr;
        _OuterIter                          _M_outer = _OuterIter();
        variant<_PatternIter, _InnerIter>   _M_inner;


        constexpr __iterator(_Parent& __parent, iterator_t<_Base> __outer) :
            _M_parent(addressof(__parent)),
            _M_outer(std::move(__outer))
        {
            if (_M_outer != ranges::end(_M_parent->_M_base))
            {
                auto&& __inner = __update_inner(_M_outer);
                _M_inner.template emplace<1>(ranges::begin(__inner));
                __satisfy();
            }
        }

        constexpr auto&& __update_inner(const _OuterIter& __i)
        {
            if constexpr (__ref_is_glvalue)
                return *__i;
            else
                return _M_parent->_M_inner.__emplace_deref(__i);
        }

        constexpr auto&& __get_inner(const _OuterIter& __i)
        {
            if constexpr (__ref_is_glvalue)
                return *__i;
            else
                return *_M_parent->_M_inner;
        }

        constexpr void __satisfy()
        {
            while (true)
            {
                if (_M_inner.index() == 0)
                {
                    if (std::get<0>(_M_inner) != ranges::end(_M_parent->_M_pattern))
                        break;

                    auto&& __inner = __update_inner(_M_outer);
                    _M_inner.template emplace<1>(ranges::begin(__inner));
                }
                else
                {
                    auto&& __inner = __get_inner(_M_outer);

                    if (std::get<1>(_M_inner) != ranges::end(_M_inner))
                        break;

                    if (++_M_outer == ranges::end(_M_parent->_M_base))
                    {
                        if constexpr (__ref_is_glvalue)
                            _M_inner.template emplace<0>();
                        break;
                    }

                    _M_inner.template emplace<0>(ranges::begin(_M_parent->_M_pattern));
                }
            }
        }

    public:

        using iterator_concept          = std::conditional_t<__ref_is_glvalue && bidirectional_range<_Base> && __detail::__bidirectional_common<_InnerBase> && __detail::__bidirectional_common<_PatternBase>,
                                                             bidirectional_iterator_tag,
                                                             std::conditional_t<__ref_is_glvalue && forward_range<_Base> && forward_range<_InnerBase>,
                                                                                forward_iterator_tag,
                                                                                input_iterator_tag>>;
        /*using iterator_category         = ...;*/ //! @todo: define this.
        using value_type                = std::common_type_t<iter_value_t<_InnerIter>, iter_value_t<_PatternIter>>;
        using difference_type           = std::common_type_t<iter_difference_t<_OuterIter>, iter_difference_t<_InnerIter>, iter_difference_t<_PatternIter>>;

        __iterator()
            requires std::default_initializable<_OuterIter> = default;

        constexpr __iterator(__iterator<!_Const> __i)
            requires _Const
                && std::convertible_to<iterator_t<_V>, _OuterIter>
                && std::convertible_to<iterator_t<_InnerRange>, _InnerIter>
                && std::convertible_to<iterator_t<_Pattern>, _PatternIter> :
            _M_parent(__i._M_parent),
            _M_outer(std::move(__i._M_outer))
        {
            if (__i._M_inner.index() == 0)
                _M_inner.template emplace<0>(std::get<0>(std::move(__i._M_inner)));
            else
                _M_inner.template emplace<1>(std::get<1>(std::move(__i._M_inner)));
        }

        constexpr decltype(auto) operator*() const
        {
            using __reference = std::common_reference_t<iter_reference_t<_InnerIter>, iter_reference_t<_PatternIter>>;
            return visit([](auto& __it) -> __reference { return *__it; }, _M_inner);
        }

        constexpr __iterator& operator++()
        {
            visit([](auto& __it) { ++__it; }, _M_inner);
            __satisfy();
            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        constexpr __iterator operator++(int)
            requires __ref_is_glvalue && forward_iterator<_OuterIter> && forward_iterator<_InnerIter>
        {
            __iterator __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires __ref_is_glvalue
                && bidirectional_range<_Base>
                && __detail::__bidirectional_common<_InnerBase>
                && __detail::__bidirectional_common<_PatternBase>
        {
            if (_M_outer == ranges::end(_M_parent->_M_base))
            {
                auto&& __inner = *--_M_outer;
                _M_inner.template emplace<1>(ranges::end(__inner));
            }

            while (true)
            {
                if (_M_inner.index() == 0)
                {
                    auto& __it = std::get<0>(_M_inner);
                    if (__it = ranges::begin(_M_parent->_M_pattern))
                    {
                        auto&& __inner = *--_M_outer;
                        _M_inner.template emplace<1>(ranges::end(__inner));
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    auto& __it = std::get<1>(_M_inner);
                    auto&& __inner = *_M_outer;
                    if (__it = ranges::begin(__inner))
                    {
                        _M_inner.template emplace<0>(ranges::end(_M_parent->_M_pattern));
                    }
                    else
                    {
                        break;
                    }
                }
            }

            visit([](auto& __it) { --__it; }, _M_inner);

            return *this;
        }

        constexpr __iterator operator--(int)
            requires __ref_is_glvalue
                && bidirectional_range<_Base>
                && __detail::__bidirectional_common<_InnerBase>
                && __detail::__bidirectional_common<_PatternBase>
        {
            __iterator __tmp = *this;
            --*this;
            return __tmp;
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires __ref_is_glvalue
                && std::equality_comparable<_OuterIter>
                && std::equality_comparable<_InnerIter>
        {
            return __x._M_outer == __y._M_outer && __x._M_inner == __y._M_inner;
        }

        friend constexpr decltype(auto) iter_move(const __iterator& __x)
        {
            using __rvalue_reference = std::common_reference_t<iter_rvalue_reference_t<_InnerIter>, iter_rvalue_reference_t<_PatternIter>>;

            return visit<__rvalue_reference>(ranges::iter_move, __x._M_inner);
        }

        friend constexpr void iter_swap(const __iterator& __x, const __iterator& __y)
            requires indirectly_swappable<_InnerIter, _PatternIter>
        {
            visit(ranges::iter_swap, __x._M_inner, __y._M_inner);
        }
    };

    template <bool _Const>
    struct __sentinel
    {
        using _Parent           = std::conditional_t<_Const, const join_with_view, join_with_view>;
        using _Base             = std::conditional_t<_Const, const _V, _V>;

        sentinel_t<_Base>   _M_end = sentinel_t<_Base>();

        constexpr explicit __sentinel(_Parent& __p) :
            _M_end(ranges::end(__p._M_base))
        {
        }

    public:

        __sentinel() = default;

        constexpr __sentinel(__sentinel<!_Const> __s)
            requires _Const
                && std::convertible_to<sentinel_t<_V>, sentinel_t<_Base>> :
            _M_end(std::move(__s._M_end))
        {
        }

        template <bool _OtherConst>
            requires sentinel_for<sentinel_t<_Base>, iterator_t<std::conditional_t<_OtherConst, const _V, _V>>>
        friend constexpr bool operator==(const __iterator<_OtherConst>& __x, const __sentinel& __y)
        {
            return __x._M_outer == __y._M_end;
        }
    };

public:

    join_with_view()
        requires std::default_initializable<_V>
            && std::default_initializable<_Pattern>
        = default;

    constexpr join_with_view(_V __base, _Pattern __pattern) :
        _M_base(std::move(__base)),
        _M_pattern(std::move(__pattern))
    {
    }

    template <input_range _R>
        requires std::constructible_from<_V, views::all_t<_R>>
            && std::constructible_from<_Pattern, single_view<range_value_t<_InnerRange>>>
    constexpr join_with_view(_R&& __r, range_value_t<_InnerRange> __e) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_pattern(views::single(std::move(__e)))
    {
    }

    constexpr auto begin()
    {
        constexpr bool _UseConst = __detail::__simple_view<_V> && std::is_reference_v<_InnerRange> && __detail::__simple_view<_Pattern>;
        return __iterator<_UseConst>{*this, ranges::begin(_M_base)};
    }

    constexpr auto begin() const
        requires input_range<const _V>
            && forward_range<const _Pattern>
            && std::is_reference_v<range_reference_t<const _V>>
    {
        return __iterator<true>{*this, ranges::begin(_M_base)};
    }

    constexpr auto end()
    {
        if constexpr (forward_range<_V>
                      && std::is_reference_v<_InnerRange>
                      && forward_range<_InnerRange>
                      && common_range<_V>
                      && common_range<_InnerRange>)
            return __iterator<__detail::__simple_view<_V> && __detail::__simple_view<_Pattern>>{*this, ranges::end(_M_base)};
        else
            return __sentinel<__detail::__simple_view<_V> && __detail::__simple_view<_Pattern>>{*this};
    }

    constexpr auto end() const
        requires input_range<const _V>
            && forward_range<const _Pattern>
            && std::is_reference_v<range_reference_t<const _V>>
    {
        using _InnerConstRange = range_reference_t<const _V>;

        if constexpr (forward_range<const _V>
                      && forward_range<_InnerConstRange>
                      && common_range<const _V>
                      && common_range<_InnerConstRange>)
            return __iterator<true>{*this, ranges::end(_M_base)};
        else
            return __sentinel<true>{*this};
    }

private:

    _V                                                                  _M_base = _V();
    __detail::__non_propagating_cache<std::remove_cv_t<_InnerRange>>    _M_inner;
    _Pattern                                                            _M_pattern = _Pattern();
};

template <class _R, class _P>
join_with_view(_R&&, _P&&) -> join_with_view<views::all_t<_R>, views::all_t<_P>>;

template <input_range _R>
join_with_view(_R&&, range_value_t<range_reference_t<_R>>)
    -> join_with_view<views::all_t<_R>, single_view<range_value_t<range_reference_t<_R>>>>;


namespace views
{

inline constexpr __detail::__range_adaptor_closure join_with = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    return join_with_view(std::forward<_E>(__e), std::forward<_F>(__f));
};

} // namespace views


namespace __detail
{

template <auto> struct __require_constant;

template <class _R>
concept __tiny_range = sized_range<_R>
    && requires { typename __require_constant<remove_reference_t<_R>::size()>; }
    && (remove_reference_t<_R>::size() <= 1);

} // namespace __detail


template <input_range _V, forward_range _Pattern>
    requires view<_V>
        && view<_Pattern>
        && indirectly_comparable<iterator_t<_V>, iterator_t<_Pattern>, __detail::__equal_to>
        && (forward_range<_V> || __detail::__tiny_range<_Pattern>)
class lazy_split_view :
    public view_interface<lazy_split_view<_V, _Pattern>>
{
private:

    template <bool> struct __inner_iter;

    template <bool _Const>
    struct __outer_iter
    {
    private:

        friend class __inner_iter<_Const>;

        using _Parent       = std::conditional_t<_Const, const lazy_split_view, lazy_split_view>;
        using _Base         = std::conditional_t<_Const, const _V, _V>;

        _Parent*            _M_parent = nullptr;
        iterator_t<_Base>   _M_current = iterator_t<_Base>();   //! @todo: shouldn't be present if !forward_range<_V>.
        bool                _M_trailing_empty = false;

    public:

        using iterator_concept = std::conditional_t<forward_range<_Base>, forward_iterator_tag, input_iterator_tag>;

        //! @todo: shouldn't be defined if not a forward range.
        using iterator_category = input_iterator_tag;

        struct value_type :
            public view_interface<value_type>
        {
        private:

            __outer_iter _M_i = __outer_iter();

        public:

            value_type() = default;

            constexpr explicit value_type(__outer_iter __i) :
                _M_i(std::move(__i))
            {
            }

            constexpr __inner_iter<_Const> begin() const
            {
                return __inner_iter<_Const>{_M_i};
            }

            constexpr default_sentinel_t end() const noexcept
            {
                return default_sentinel;
            }
        };

        using difference_type = range_difference_t<_Base>;

        constexpr decltype(auto) __get_current()
        {
            if constexpr (forward_range<_V>)
                return _M_current;
            else
                return _M_parent->_M_current;
        }

        constexpr decltype(auto) __get_current() const
        {
            if constexpr (forward_range<_V>)
                return _M_current;
            else
                return _M_parent->_M_current;
        }


        __outer_iter() = default;

        constexpr explicit __outer_iter(_Parent& __parent)
            requires (!forward_range<_Base>) :
            _M_parent(addressof(__parent))
        {
        }

        constexpr __outer_iter(_Parent& __parent, iterator_t<_Base> __current)
            requires (forward_range<_Base>) :
            _M_parent(addressof(__parent))
        {
            if constexpr (forward_range<_V>)
                _M_current = std::move(__current);
            else
                _M_parent->_M_current = std::move(__current);
        }

        constexpr __outer_iter(__outer_iter<!_Const> __i)
            requires _Const && std::convertible_to<iterator_t<_V>, iterator_t<_Base>> :
            _M_parent(__i._M_parent)
        {
            if constexpr (forward_range<_V>)
                _M_current = std::move(__i._M_current);
            else
                _M_parent->_M_current = std::move(__i._M_current);
        }

        constexpr value_type operator*() const
        {
            return value_type{*this};
        }

        constexpr __outer_iter& operator++()
        {
            auto __end = ranges::end(_M_parent->_M_base);
            auto& __current = __get_current();

            if (__current == __end)
            {
                _M_trailing_empty = true;
                return *this;
            }

            const auto [__pbegin, __pend] = subrange{_M_parent->_M_pattern};
            if (__pbegin == __pend)
                ++__current;
            else if constexpr (__detail::__tiny_range<_Pattern>)
            {
                __current = ranges::__detail::__make_find()(std::move(__current), __end, __pbegin);
                if (__current != __end)
                {
                    ++__current;
                    if (__current == __end)
                        _M_trailing_empty = true;
                }
            }
            else
            {
                do
                {
                    auto [__b, __p] = ranges::__detail::__make_mismatch()(__current, __end, __pbegin, __pend);
                    if (__p == __pend)
                    {
                        __current = __b;
                        if (__current == __end)
                            _M_trailing_empty = true;
                        break;
                    }
                } while (++__current != __end);
            }

            return *this;
        }

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

        friend constexpr bool operator==(const __outer_iter& __x, const __outer_iter& __y)
            requires forward_range<_Base>
        {
            return __x.__get_current() == __y.__get_current() && __x._M_trailing_empty == __y._M_trailing_empty;
        }

        friend constexpr bool operator==(const __outer_iter& __x, default_sentinel_t)
        {
            return __x.__get_current() == ranges::end(__x._M_parent->_M_base) && !__x._M_trailing_empty;
        }
    };

    template <bool _Const>
    struct __inner_iter
    {
    private:

        using _Base     = std::conditional_t<_Const, const _V, _V>;

        __outer_iter<_Const>        _M_i = {};
        bool                        _M_incremented = false;

    public:

        using iterator_concept = typename __outer_iter<_Const>::iterator_concept;

        //! @todo: should only be present if forward_range<_Base>.
        using iterator_category = std::conditional_t<derived_from<typename iterator_traits<iterator_t<_Base>>::iterator_category, forward_iterator_tag>,
                                                     forward_iterator_tag,
                                                     typename iterator_traits<iterator_t<_Base>>::iterator_category>; 

        using value_type = range_value_t<_Base>;
        using difference_type = range_difference_t<_Base>;

        __inner_iter() = default;

        constexpr explicit __inner_iter(__outer_iter<_Const> __i) :
            _M_i(std::move(__i))
        {
        }

        constexpr const iterator_t<_Base>& base() const & noexcept
        {
            return _M_i.__get_current();
        }

        constexpr iterator_t<_Base> base() &&
            requires forward_range<_V>
        {
            return std::move(_M_i.__get_current());
        }

        constexpr decltype(auto) operator*() const
        {
            return *_M_i->__get_current();
        }

        constexpr __inner_iter& operator++()
        {
            _M_incremented = true;
            if constexpr (!forward_range<_Base>)
            {
                if constexpr (_Pattern::size() == 0)
                    return *this;
            }

            ++_M_i.__get_current();
            return *this;
        }

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

        friend constexpr bool operator==(const __inner_iter& __x, const __inner_iter& __y)
            requires forward_range<_Base>
        {
            return __x._M_i.__get_current() == __y._M_i.__get_current();
        }

        friend constexpr bool operator==(const __inner_iter& __x, default_sentinel_t)
        {
            auto [__pcur, __pend] = subrange{__x._M_i._M_parent->_M_pattern};
            auto __end = ranges::end(__x._M_i._M_parent->_M_base);

            if constexpr (__detail::__tiny_range<_Pattern>)
            {
                const auto& __cur = __x._M_i.__get_current();
                if (__cur == __end)
                    return true;
                if (__pcur == __pend)
                    return __x._M_incremented;

                return *__cur == *__pcur;
            }
            else
            {
                auto __cur = __x._M_i.__get_current();
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
                } while (++__cur != __end);

                return false;
            }
        }

        friend constexpr decltype(auto) iter_move(const __inner_iter& __i)
            noexcept(noexcept(ranges::iter_move(__i._M_i.__get_current())))
        {
            return ranges::iter_move(__i._M_i.__get_current());
        }

        friend constexpr void iter_swap(const __inner_iter& __x, const __inner_iter& __y)
            noexcept(noexcept(ranges::iter_swap(__x._M_i.__get_current(), __y._M_i.__get_current())))
            requires indirectly_swappable<iterator_t<_Base>>
        {
            ranges::iter_swap(__x._M_i.__get_current(), __y._M_i.__get_current());
        }
    };

public:

    lazy_split_view()
        requires std::default_initializable<_V>
            && std::default_initializable<_Pattern>
        = default;

    constexpr lazy_split_view(_V __base, _Pattern __pattern) :
        _M_base(std::move(__base)),
        _M_pattern(std::move(__pattern))
    {
    }

    template <input_range _R>
        requires std::constructible_from<_V, views::all_t<_R>>
            && std::constructible_from<_Pattern, single_view<range_value_t<_R>>>
    constexpr lazy_split_view(_R&& __r, range_value_t<_R> __e) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_pattern(views::single(std::move(__e)))
    {
    }

    constexpr _V base() const &
        requires std::copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr auto begin()
    {
        if constexpr(forward_range<_V>)
        {
            return __outer_iter<__detail::__simple_view<_V> && __detail::__simple_view<_Pattern>>{*this, ranges::begin(_M_base)};
        }
        else
        {
            _M_current = ranges::begin(_M_base);
            return __outer_iter<false>{*this};
        }
    }

    constexpr auto begin() const
        requires forward_range<_V> && forward_range<const _V>
    {
        return __outer_iter<true>{*this, ranges::begin(_M_base)};
    }

    constexpr auto end()
        requires forward_range<_V> && common_range<_V>
    {
        return __outer_iter<__detail::__simple_view<_V> && __detail::__simple_view<_Pattern>>{*this, ranges::end(_M_base)};
    }

    constexpr auto end() const
    {
        if constexpr (forward_range<_V> && forward_range<const _V> && common_range<const _V>)
            return __outer_iter<true>{*this, ranges::end(_M_base)};
        else
            return default_sentinel;
    }

private:

    _V          _M_base = _V();
    _Pattern    _M_pattern = _Pattern();

    //! @todo: shouldn't be present if forward_range<_V>.
    __detail::__non_propagating_cache<iterator_t<_V>>   _M_current;
};

template <class _R, class _P>
lazy_split_view(_R&&, _P&&) -> lazy_split_view<views::all_t<_R>, views::all_t<_P>>;

template <input_range _R>
lazy_split_view(_R&&, range_value_t<_R>) -> lazy_split_view<views::all_t<_R>, single_view<range_value_t<_R>>>;


namespace views
{

inline constexpr __detail::__range_adaptor_closure lazy_split = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    return lazy_split_view(std::forward<_E>(__e), std::forward<_F>(__f));
};

} // namespace views


template <input_range _V, forward_range _Pattern>
    requires view<_V>
        && view<_Pattern>
        && indirectly_comparable<iterator_t<_V>, iterator_t<_Pattern>, __detail::__equal_to>
class split_view :
    public view_interface<split_view<_V, _Pattern>>
{
private:

    struct __sentinel;

    struct __iterator
    {
    private:

        friend class __sentinel;

        split_view*                 _M_parent = nullptr;
        iterator_t<_V>              _M_cur = iterator_t<_V>();
        subrange<iterator_t<_V>>    _M_next = subrange<iterator_t<_V>>();
        bool                        _M_trailing_empty = false;

    public:

        using iterator_concept      = forward_iterator_tag;
        using iterator_category     = input_iterator_tag;
        using value_type            = subrange<iterator_t<_V>>;
        using difference_type       = range_difference_t<_V>;

        __iterator() = default;

        constexpr __iterator(split_view& __parent, iterator_t<_V> __current, subrange<iterator_t<_V>> __next) :
            _M_parent(addressof(__parent)),
            _M_cur(std::move(__current)),
            _M_next(std::move(__next))
        {
        }

        constexpr iterator_t<_V> base() const
        {
            return _M_cur;
        }

        constexpr value_type operator*() const
        {
            return {_M_cur, _M_next.begin()};
        }

        constexpr __iterator& operator++()
        {
            _M_cur = _M_next.begin();
            if (_M_cur != ranges::end(_M_parent->_M_base))
            {
                _M_cur = _M_next.end();
                if (_M_cur == ranges::end(_M_parent->_M_base))
                {
                    _M_trailing_empty = true;
                    _M_next = {_M_cur, _M_cur};
                }
                else
                {
                    _M_next = _M_parent->__find_next(_M_cur);
                }
            }
            else
            {
                _M_trailing_empty = false;
            }

            return *this;
        }

        constexpr __iterator operator++(int)
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
        {
            return __x._M_cur == __y._M_cur && __x._M_trailing_empty == __y._M_trailing_empty;
        }
    };

    struct __sentinel
    {
    private:

        sentinel_t<_V>  _M_end = sentinel_t<_V>();

    public:

        __sentinel() = default;

        constexpr explicit __sentinel(split_view& __parent) :
            _M_end(ranges::end(__parent._M_base))
        {
        }

        friend constexpr bool operator==(const __iterator& __x, const __sentinel& __y)
        {
            return __x._M_cur == __y._M_end && !__x._M_trailing_empty;
        }
    };


    _V          _M_base = _V();
    _Pattern    _M_pattern = _Pattern();

    __detail::__non_propagating_cache<__iterator> _M_begin;


public:

    split_view()
        requires std::default_initializable<_V>
            && std::default_initializable<_Pattern>
        = default;

    constexpr split_view(_V __v, _Pattern __pattern) :
        _M_base(std::move(__v)),
        _M_pattern(std::move(__pattern))
    {
    }

    template <forward_range _R>
        requires std::constructible_from<_V, views::all_t<_R>>
            && std::constructible_from<_Pattern, single_view<range_value_t<_R>>>
    constexpr split_view(_R&& __r, range_value_t<_R> __e) :
        _M_base(views::all(std::forward<_R>(__r))),
        _M_pattern(views::single(std::move(__e)))
    {
    }

    constexpr _V base() const &
        requires std::copy_constructible<_V>
    {
        return _M_base;
    }

    constexpr _V base() &&
    {
        return std::move(_M_base);
    }

    constexpr __iterator begin()
    {
        if (!_M_begin)
            _M_begin = __iterator{*this, ranges::begin(_M_base), __find_next(ranges::begin(_M_base))};

        return *_M_begin;
    }

    constexpr auto __end()
    {
        if constexpr (common_range<_V>)
            return __iterator{*this, ranges::end(_M_base), {}};
        else
            return __sentinel{*this};
    }

    constexpr subrange<iterator_t<_V>> __find_next(iterator_t<_V> __it)
    {
        auto [__b, __e] = ranges::__detail::__make_search()(subrange(__it, ranges::end(_M_base)), _M_pattern);
        if (__b != ranges::end(_M_base) && ranges::empty(_M_pattern))
        {
            ++__b;
            ++__e;
        }

        return {__b, __e};
    }
};

template <class _R, class _P>
split_view(_R&&, _P&&) -> split_view<views::all_t<_R>, views::all_t<_P>>;

template <input_range _R>
split_view(_R&&, range_value_t<_R>) -> split_view<views::all_t<_R>, single_view<range_value_t<_R>>>;


namespace views
{

inline constexpr __detail::__range_adaptor_closure split = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    return split_view(std::forward<_E>(__e), std::forward<_F>(__f));
};

} // namespace views


namespace views
{

inline constexpr __detail::__range_adaptor_closure counted = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    using _T = std::decay_t<_E>;
    using _D = iter_difference_t<_T>;

    static_assert(std::convertible_to<_F, _D>);

    if constexpr (contiguous_iterator<_T>)
        return span(to_address(std::forward<_E>(__e)), static_cast<std::size_t>(static_cast<_D>(std::forward<_F>(__f))));
    else if constexpr (random_access_iterator<_T>)
        return subrange(std::forward<_E>(__e), std::forward<_E>(__e) + static_cast<_D>(std::forward<_F>(__f)));
    else
        return subrange(counted_iterator(std::forward<_E>(__e), std::forward<_F>(__f)), default_sentinel);
};

} // namespace views


template <view _V>
    requires (!common_range<_V> && copyable<iterator_t<_V>>)
class common_view :
    public view_interface<common_view<_V>>
{
private:

    _V _M_base = _V();

public:

    common_view()
        requires std::default_initializable<_V> = default;

    constexpr explicit common_view(_V __r) :
        _M_base(std::move(__r))
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


namespace views
{

inline constexpr __detail::__range_adaptor_closure common = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    if constexpr (common_range<_E> && requires { views::all(std::forward<_E>(__e)); })
        return views::all(std::forward<_E>(__e));
    else
        return common_view{std::forward<_E>(__e)};
};

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

    reverse_view()
        requires std::default_initializable<_V> = default;

    constexpr explicit reverse_view(_V __r) :
        _M_base(std::move(__r))
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

template <class _T>
struct __is_reverse_subrange : std::false_type {};

template <class _I, subrange_kind _K>
struct __is_reverse_subrange<subrange<reverse_iterator<_I>, reverse_iterator<_I>, _K>> : std::true_type {};

template <class _T>
inline constexpr bool __is_reverse_subrange_v = __is_reverse_subrange<_T>::value();

} // namespace __detail

namespace views
{

inline constexpr __detail::__range_adaptor_closure reverse = []<class _E>(_E&& __e)
{
    using _T = std::remove_cvref_t<_E>;

    if constexpr (__detail::__is_specialization_of_v<reverse_view, _T>)
        return std::forward<_E>(__e).base();
    else if constexpr (__detail::__is_reverse_subrange_v<_T>)
    {
        using _I = typename _T::_Iter;
        constexpr auto _K = _T::_Kind;
        if constexpr (_K == subrange_kind::sized)
            return subrange<_I, _I, _K>(__e.end().base(), __e.begin().base(), __e.size());
        else
            return subrange<_I, _I, _K>(__e.end().base(), __e.begin().base());
    }
    else
        return reverse_view{std::forward<_E>(__e)};
};

} // namespace views


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

template <class _T, size_t _N>
concept __returnable_element =
    std::is_reference_v<_T> || std::move_constructible<tuple_element_t<_N, _T>>;

} // namespace __detail

template <input_range _V, size_t _N>
    requires view<_V>
        && __detail::__has_tuple_element<range_value_t<_V>, _N>
        && __detail::__has_tuple_element<std::remove_reference_t<range_reference_t<_V>>, _N>
        && __detail::__returnable_element<range_reference_t<_V>, _N>
class elements_view :
    public view_interface<elements_view<_V, _N>>
{
private:

    template <bool> class __sentinel;

    template <bool _Const>
    class __iterator
    {
    private:

        template <bool> friend class __sentinel;

        using _Base = std::conditional_t<_Const, const _V, _V>;

        iterator_t<_Base> _M_current = iterator_t<_Base>();

        static constexpr decltype(auto) __get_element(const iterator_t<_Base>& __i)
        {
            if constexpr (std::is_reference_v<range_reference_t<_Base>>)
                return std::get<_N>(*__i);
            else
            {
                using _E = std::remove_cv_t<tuple_element_t<_N, range_reference_t<_Base>>>;
                return static_cast<_E>(std::get<_N>(*__i));
            }
        }

    public:

        using iterator_concept      = std::conditional_t<random_access_range<_Base>,
                                                         random_access_iterator_tag,
                                                         std::conditional_t<bidirectional_range<_Base>,
                                                                            bidirectional_iterator_tag,
                                                                            std::conditional_t<forward_range<_Base>,
                                                                                               forward_iterator_tag,
                                                                                               input_iterator_tag>>>;

        //! @todo: should only be defined if forward_range<_Base>.
        using iterator_category     = std::conditional_t<std::is_rvalue_reference_v<decltype(std::get<_N>(*declval<iterator_t<_Base>&>()))>,
                                                         input_iterator_tag,
                                                         std::conditional_t<std::derived_from<typename iterator_traits<iterator_t<_Base>>::iterator_category, random_access_iterator_tag>,
                                                                            random_access_iterator_tag,
                                                                            typename iterator_traits<iterator_t<_Base>>::iterator_category>>;

        using value_type            = std::remove_cvref_t<tuple_element_t<_N, range_value_t<_Base>>>;
        using difference_type       = range_difference_t<_Base>;

        __iterator()
            requires std::default_initializable<iterator_t<_Base>> = default;

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
            return __get_element(_M_current);
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

        friend constexpr auto operator<=>(const __iterator& __x, const __iterator& __y)
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

    template <bool _Const>
    class __sentinel
    {
    private:

        using _Base     = std::conditional_t<_Const, const _V, _V>;

        sentinel_t<_Base>   _M_end = sentinel_t<_Base>();

    public:

        __sentinel() = default;

        constexpr explicit __sentinel(sentinel_t<_Base> __end) :
            _M_end(std::move(__end))
        {
        }

        constexpr __sentinel(__sentinel<!_Const> __other)
            requires _Const && std::convertible_to<sentinel_t<_V>, sentinel_t<_Base>> :
            _M_end(std::move(__other._M_end))
        {
        }

        constexpr sentinel_t<_Base> base() const
        {
            return _M_end;
        }

        template <bool _OtherConst>
            requires sentinel_for<sentinel_t<_Base>, iterator_t<std::conditional_t<_OtherConst, const _V, _V>>>
        friend constexpr bool operator==(const __iterator<_OtherConst>& __x, const __sentinel& __y)
        {
            return __x._M_current == __y._M_end;
        }

        template <bool _OtherConst>
            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<std::conditional_t<_OtherConst, const _V, _V>>>
        friend constexpr range_difference_t<std::conditional_t<_OtherConst, const _V, _V>> operator-(const __iterator<_OtherConst>& __x, const __sentinel& __y)
        {
            return __x._M_current - __y._M_end;
        }

        template <bool _OtherConst>
            requires sized_sentinel_for<sentinel_t<_Base>, iterator_t<std::conditional_t<_OtherConst, const _V, _V>>>
        friend constexpr range_difference_t<std::conditional_t<_OtherConst, const _V, _V>> operator-(const __sentinel& __x, const __iterator<_OtherConst>& __y)
        {
            return __x._M_end - __y._M_current;
        }
    };

    _V _M_base = _V();

public:

    elements_view()
        requires std::default_initializable<_V> = default;

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
        requires (!__detail::__simple_view<_V> && !common_range<_V>)
    {
        return __sentinel<false>{ranges::end(_M_base)};
    }

    constexpr auto end()
        requires (!__detail::__simple_view<_V> && common_range<_V>)
    {
        return __iterator<false>{ranges::end(_M_base)};
    }

    constexpr auto end() const
        requires range<const _V>
    {
        return __sentinel<true>{ranges::end(_M_base)};
    }

    constexpr auto end() const
        requires common_range<const _V>
    {
        return __iterator<true>{ranges::end(_M_base)};
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


namespace views::inline __elements
{

template <size_t _N>
inline constexpr __detail::__range_adaptor_closure elements = []<class _E>(_E&& __e)
{
    return elements_view<views::all_t<_E>, _N>{std::forward<_E>(__e)};
};

inline constexpr auto keys = elements<0>;
inline constexpr auto values = elements<1>;

} // namespace views::__elements


namespace __detail
{

template <class... _Rs>
concept __zip_is_common =
    (sizeof...(_Rs) == 1 && (common_range<_Rs> && ...))
    || (!(bidirectional_range<_Rs> && ...) && (common_range<_Rs> && ...))
    || ((random_access_range<_Rs> && ...) && (sized_range<_Rs> && ...));


template <class... _Ts>
struct __tuple_or_pair { using type = tuple<_Ts...>; };

template <class _T1, class _T2>
struct __tuple_or_pair<_T1, _T2> { using type = pair<_T1, _T2>; };

template <class... _Ts>
using __tuple_or_pair_t = typename __tuple_or_pair<_Ts...>::type;

template <class _F, class _Tuple>
constexpr auto __tuple_transform(_F&& __f, _Tuple&& __tuple)
{
    return apply([&]<class... _Ts>(_Ts&&... __elements)
    {
        return __tuple_or_pair_t<std::invoke_result_t<_F&, _Ts>...>(std::invoke(__f, std::forward<_Ts>(__elements))...);
    }, std::forward<_Tuple>(__tuple));
}

template <class _F, class _Tuple>
constexpr void __tuple_for_each(_F&& __f, _Tuple&& __tuple)
{
    apply([&]<class... _Ts>(_Ts&&... __elements)
    {
        (std::invoke(__f, std::forward<_Ts>(__elements)), ...);
    }, std::forward<_Tuple>(__tuple));
}

template <bool _Const, class... _Views>
concept __all_random_access = (random_access_range<std::conditional_t<_Const, const _Views, _Views>> && ...);

template <bool _Const, class... _Views>
concept __all_bidirectional = (bidirectional_range<std::conditional_t<_Const, const _Views, _Views>> && ...);

template <bool _Const, class... _Views>
concept __all_forward = (forward_range<std::conditional_t<_Const, const _Views, _Views>> && ...);

} // namespace __detail


template <input_range... _Views>
    requires (view<_Views> && ...)
        && (sizeof...(_Views) > 0)
class zip_view :
    public view_interface<zip_view<_Views...>>
{
private:

    tuple<_Views...>    _M_views;

    template <bool> class __sentinel;

    template <bool _Const>
    class __iterator
    {
        friend class zip_view;
        template <bool> friend class __sentinel;

    public:

        //! @todo: should only be present if __detail::__all_forward<_Const, _Views...>
        using iterator_category         = input_iterator_tag;
        using iterator_concept          = std::conditional_t<__detail::__all_random_access<_Const, _Views...>,
                                                             random_access_iterator_tag,
                                                             std::conditional_t<__detail::__all_bidirectional<_Const, _Views...>,
                                                                                bidirectional_iterator_tag,
                                                                                std::conditional_t<__detail::__all_forward<_Const, _Views...>,
                                                                                                   forward_iterator_tag,
                                                                                                   input_iterator_tag>>>;
        using value_type                = __detail::__tuple_or_pair<range_value_t<std::conditional_t<_Const, const _Views, _Views>>...>;
        using difference_type           = std::common_type_t<range_difference_t<std::conditional_t<_Const, const _Views, _Views>>...>;

    private:

        using __tuple = __detail::__tuple_or_pair<iterator_t<std::conditional_t<_Const, const _Views, _Views>>...>;

        __tuple _M_current;

        constexpr explicit __iterator(__tuple __i);

        template <std::size_t... _Idx>
        static constexpr bool __any_piecewise_equal(const __tuple& __x, const __tuple& __y, std::index_sequence<_Idx...>)
        {
            return (bool(std::get<_Idx>(__x) == std::get<_Idx>(__y)) || ...);
        }

        template <std::size_t... _Idx>
        static constexpr difference_type __min_distance(const __tuple& __x, const __tuple& __y, std::index_sequence<_Idx...>)
        {
            //! @todo: should be the value with the smallest *absolute* value.
            return ranges::__detail::__make_min()({difference_type(std::get<_Idx>(__x) - std::get<_Idx>(__y))...});
        }

        template <std::size_t... _Idx>
        static constexpr void __iter_swap(const __tuple& __x, const __tuple& __y)
            noexcept(noexcept((ranges::iter_swap(std::get<_Idx>(__x), std::get<_Idx>(__y)), ...)))
        {
            (ranges::iter_swap(std::get<_Idx>(__x), std::get<_Idx>(__y)), ...);
        }

    public:
    
        //! @todo: exception behaviour: set a singular value if any non-const method exits via exception.

        __iterator() = default;

        constexpr __iterator(__iterator<!_Const> __i)
            requires _Const && (std::convertible_to<iterator_t<_Views>, iterator_t<std::conditional_t<_Const, const _Views, _Views>>> && ...) :
            _M_current(std::move(__i._M_current))
        {
        }

        constexpr auto operator*() const
        {
            return __detail::__tuple_transform([](auto& __i) -> decltype(auto)
            {
                return *__i;
            }, _M_current);
        }

        constexpr __iterator& operator++()
        {
            __detail::__tuple_for_each([](auto& __i) { ++__i; }, _M_current);
            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        constexpr __iterator operator++(int)
            requires __detail::__all_forward<_Const, _Views...>
        {
            auto __tmp = *this;
            ++*this;
            return __tmp;
        }

        constexpr __iterator& operator--()
            requires __detail::__all_bidirectional<_Const, _Views...>
        {
            __detail::__tuple_transform([](auto& __i) { --__i; }, _M_current);
            return *this;
        }

        constexpr __iterator operator--(int)
            requires __detail::__all_bidirectional<_Const, _Views...>
        {
            auto __tmp = *this;
            --*this;
            return __tmp;
        }

        constexpr __iterator& operator+=(difference_type __x)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            __detail::__tuple_for_each([&]<class _I>(_I& __i)
            {
                __i += iter_difference_t<_I>(__x);
            }, _M_current);
            return *this;
        }

        constexpr __iterator& operator-=(difference_type __x)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            __detail::__tuple_for_each([&]<class _I>(_I& __i)
            {
                __i -= __iter_difference_t<_I>(__x);
            }, _M_current);
            return *this;
        }

        constexpr auto operator[](difference_type __n) const
            requires __detail::__all_random_access<_Const, _Views...>
        {
            return __detail::__tuple_transform([&]<class _I>(_I& __i) -> decltype(auto)
            {
                return __i[iter_difference_t<_I>(__n)];
            }, _M_current);
        }

        friend constexpr bool operator==(const __iterator& __x, const __iterator& __y)
            requires (std::equality_comparable<iterator_t<std::conditional_t<_Const, const _Views, _Views>>> && ...)
        {
            if constexpr (__detail::__all_bidirectional<_Const, _Views...>)
                return __x._M_current == __y._M_current;

            return __any_piecewise_equal(__x._M_current, __y._M_current, std::make_index_sequence<sizeof...(_Views)>());
        }

        friend constexpr bool operator<(const __iterator& __x, const __iterator& __y)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            return __x._M_current < __y._M_current;
        }

        friend constexpr bool operator>(const __iterator& __x, const __iterator& __y)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            return __y < __x;
        }

        friend constexpr bool operator<=(const __iterator& __x, const __iterator& __y)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            return !(__y < __x);
        }

        friend constexpr bool operator>=(const __iterator& __x, const __iterator& __y)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            return !(__x < __y);
        }

        friend constexpr auto operator<=>(const __iterator& __x, const __iterator& __y)
            requires __detail::__all_random_access<_Const, _Views...>
                && (std::three_way_comparable<iterator_t<std::conditional_t<_Const, const _Views, _Views>>> && ...)
        {
            return __x._M_current <=> __y._M_current;
        }

        friend constexpr __iterator operator+(const __iterator& __i, difference_type __n)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            auto __r = __i;
            __r += __n;
            return __r;
        }

        friend constexpr __iterator operator+(difference_type __n, const __iterator& __i)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            auto __r = __i;
            __r += __n;
            return __r;
        }

        friend constexpr __iterator operator-(const __iterator& __i, difference_type __n)
            requires __detail::__all_random_access<_Const, _Views...>
        {
            auto __r = __i;
            __r -= __n;
            return __r;
        }

        friend constexpr difference_type operator-(const __iterator& __x, const __iterator& __y)
            requires (sized_sentinel_for<iterator_t<std::conditional_t<_Const, const _Views, _Views>>, iterator_t<std::conditional_t<_Const, const _Views, _Views>>> && ...)
        {
            return __min_distance(__x._M_current, __y._M_current, std::make_index_sequence<sizeof...(_Views)>());
        }

        friend constexpr auto iter_move(const __iterator& __i)
            noexcept((noexcept(ranges::iter_move(declval<const iterator_t<std::conditional_t<_Const, const _Views, _Views>>&>())) && ...)
                && (std::is_nothrow_move_constructible_v<range_rvalue_reference_t<std::conditional_t<_Const, const _Views, _Views>>> && ...))
        {
            return __detail::__tuple_transform(ranges::iter_move, __i._M_current);
        }

        friend constexpr void iter_swap(const __iterator& __l, const __iterator& __r)
            noexcept(noexcept(__iter_swap(__l._M_current, __r._M_current)))
            requires (indirectly_swappable<iterator_t<std::conditional_t<_Const, const _Views, _Views>>> && ...)
        {
            __iter_swap(__l._M_current, __r._M_current);
        }
    };

    template <bool _Const>
    class __sentinel
    {
        friend class zip_view;

    private:

        using __tuple = __detail::__tuple_or_pair_t<sentinel_t<std::conditional_t<_Const, const _Views, _Views>>...>;

        __tuple _M_end;

        constexpr explicit __sentinel(__tuple __i) :
            _M_end(std::move(__i))
        {
        }

    public:

        __sentinel() = default;

        constexpr __sentinel(__sentinel<!_Const> __i)
            requires _Const && (std::convertible_to<sentinel_t<_Views>, sentinel_t<std::conditional_t<_Const, const _Views, _Views>>> && ...) :
            _M_end(std::move(__i._M_end))
        {
        }

        //! @todo: implement
        template <bool _OtherConst>
            requires (sentinel_for<sentinel_t<std::conditional_t<_Const, const _Views, _Views>>, iterator_t<std::conditional_t<_OtherConst, const _Views, _Views>>> && ...)
        friend constexpr bool operator==(const __iterator<_OtherConst>& __x, const __sentinel& __y);

        //! @todo: implement
        template <bool _OtherConst>
            requires (sized_sentinel_for<sentinel_t<std::conditional_t<_Const, const _Views, _Views>>, iterator_t<std::conditional_t<_OtherConst, const _Views, _Views>>> && ...)
        friend constexpr common_type_t<range_difference_t<std::conditional_t<_OtherConst, const _Views, _Views>>...> operator-(const __iterator<_OtherConst>& __x, const __sentinel& __y);

        template <bool _OtherConst>
            requires (sized_sentinel_for<sentinel_t<std::conditional_t<_Const, const _Views, _Views>>, iterator_t<std::conditional_t<_OtherConst, const _Views, _Views>>> && ...)
        friend constexpr common_type_t<range_difference_t<std::conditional_t<_OtherConst, const _Views, _Views>>...> operator-(const __sentinel& __y, const __iterator<_OtherConst>& __x)
        {
            return -(__x - __y);
        }
    };

public:

    zip_view() = default;

    constexpr explicit zip_view(_Views... __views) :
        _M_views(std::move(__views)...)
    {
    }

    constexpr auto begin()
        requires (!(__detail::__simple_view<_Views> && ...))
    {
        return __iterator<false>(__detail::__tuple_transform(ranges::begin, _M_views));
    }

    constexpr auto begin() const
        requires (range<const _Views> && ...)
    {
        return __iterator<true>(__detail::__tuple_transform(ranges::begin, _M_views));
    }

    constexpr auto end()
        requires (!(__detail::__simple_view<_Views> && ...))
    {
        if constexpr (!__detail::__zip_is_common<_Views...>)
            return __sentinel<false>(__detail::__tuple_transform(ranges::end, _M_views));
        else if constexpr ((random_access_range<_Views> && ...))
            return begin() + iter_difference_t<__iterator<false>>(size());
        else
            return __iterator<false>(__detail::__tuple_transform(ranges::end, _M_views));
    }

    constexpr auto end() const
        requires (range<const _Views> && ...)
    {
        if constexpr (!__detail::__zip_is_common<_Views...>)
            return __sentinel<true>(__detail::__tuple_transform(ranges::end, _M_views));
        else if constexpr ((random_access_range<_Views> && ...))
            return begin() + iter_difference_t<__iterator<true>>(size());
        else
            return __iterator<true>(__detail::__tuple_transform(ranges::end, _M_views));
    }

    constexpr auto size()
        requires (sized_range<_Views> && ...)
    {
        return apply([](auto... __sizes)
        {
            using _CT = std::make_unsigned_t<std::common_type_t<decltype(__sizes)...>>;
            return ranges::__detail::__make_min()({_CT(__sizes)...});
        }, __detail::__tuple_transform(ranges::size, _M_views));
    }

    constexpr auto size() const
        requires (sized_range<const _Views> && ...)
    {
        return apply([](auto... __sizes)
        {
            using _CT = std::make_unsigned_t<std::common_type_t<decltype(__sizes)...>>;
            return ranges::__detail::__make_min()({_CT(__sizes)...});
        }, __detail::__tuple_transform(ranges::size, _M_views));
    }
};

template <class... _Rs>
zip_view(_Rs&&...) -> zip_view<views::all_t<_Rs>...>;

template <class... _Views>
inline constexpr bool enable_borrowed_range<zip_view<_Views...>> = (enable_borrowed_range<_Views> && ...);

namespace views
{

inline constexpr __detail::__range_adaptor_closure zip = []<class... _Es>(_Es&&... __es)
{
    if constexpr (sizeof...(_Es) == 0)
        return auto(views::empty<tuple<>>);
    else
        return zip_view<views::all_t<_Es>...>(std::forward<_Es>(__es)...);
};

} // namespace views


//! @todo: implement zip_transform_view
template <copy_constructible _F, input_range... _Views>
    requires (view<_Views> && ...)
        && (sizeof...(_Views) > 0)
        && std::is_object_v<_F>
        && std::regular_invocable<_F&, range_reference_t<_Views>...>
        && __XVI_STD_UTILITY_NS::__detail::__can_reference<std::invoke_result_t<_F&, range_reference_t<_Views>...>>
class zip_transform_view;

namespace views
{

inline constexpr __detail::__range_adaptor_closure zip_transform = []<class _F, class... _Es>(_F&& __f, _Es&&... __es)
{
    if constexpr (sizeof...(_Es) == 0)
    {
        using _FD = std::decay_t<_F>;

        static_assert(std::copy_constructible<_FD> && std::regular_invocable<_FD&> && std::is_object_v<std::decay_t<std::invoke_result_t<_FD&>>>);

        ((void)std::forward<_F>(__f), auto(views::empty<std::decay_t<std::invoke_result_t<_FD&>>>));
    }
    else
    {
        return zip_transform_view(std::forward<_F>(__f), std::forward<_Es>(__es)...);
    }
};

} // namespace views


//! @todo: implement adjacent_view
template <forward_range _V, std::size_t _N>
    requires view<_V>
        && (_N > 0)
class adjacent_view;

template <class _V, std::size_t _N>
inline constexpr bool enable_borrowed_range<adjacent_view<_V, _N>> = enable_borrowed_range<_V>;

namespace views
{

template <std::size_t _N>
inline constexpr __detail::__range_adaptor_closure adjacent = []<class _E>(_E&& __e)
{
    if constexpr (_N == 0)
        ((void)std::forward<_E>(__e), auto(views::empty<tuple<>>));
    else
        adjancent_view<views::all_t<_E>, _N>(std::forward<_E>(__e));
};

inline constexpr auto pairwise = adjacent<2>;

} // namespace views


//! @todo: implement adjacent_transform_view
template <forward_range _V, copy_constructible _F, std::size_t _N>
    requires (false)
class adjacent_transform_view;

namespace views
{

template <std::size_t _N>
inline constexpr __detail::__range_adaptor_closure adjacent_transform = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    if constexpr (_N == 0)
        ((void)std::forward<_E>(__e), views::zip_transform(std::forward<_F>(__f)));
    else
        adjacent_transform_view<views::all_t<_E>, std::decay_t<_F>, _N>(std::forward<_E>(__e), std::forward<_F>(__f));
};

inline constexpr auto pairwise_transform = adjacent_transform<2>;

}


//! @todo: implement chunk_view
template <view _V>
    requires input_range<_V>
class chunk_view;

template <view _V>
    requires forward_range<_V>
class chunk_view<_V>;

template <class _V>
inline constexpr bool enable_borrowed_range<chunk_view<_V>> = forward_range<_V> && enable_borrowed_range<_V>;

namespace views
{

inline constexpr __detail::__range_adaptor_closure chunk = []<class _E, class _N>(_E&& __e, _N&& __n)
{
    return chunk_view(std::forward<_E>(__e), std::forward<_N>(__n));
};

} // namespace views


//! @todo: implement slide_view
template <view _V>
    requires forward_range<_V>
class slide_view;

template <class _V>
inline constexpr bool enable_borrowed_range<slide_view<_V>> = enable_borrowed_range<_V>;

namespace views
{

inline constexpr __detail::__range_adaptor_closure slide = []<class _E, class _N>(_E&& __e, _N&& __n)
{
    return slide_view(std::forward<_E>(__e), std::forward<_N>(__n));
};

} // namespace views


//! @todo: implement chunk_by_view
template <forward_range _V, indirect_binary_predicate<iterator_t<_V>, iterator_t<_V>> _Pred>
    requires view<_V>
        && std::is_object_v<_Pred>
class chunk_by_view;

namespace views
{

inline constexpr __detail::__range_adaptor_closure chunk_by = []<class _E, class _F>(_E&& __e, _F&& __f)
{
    return chunk_by_view(std::forward<_E>(__e), std::forward<_F>(__f));
};

} // namespace views


} // namespace ranges


namespace views = ranges::views;


template <class _T> struct tuple_size;
template <std::size_t _I, class _T> struct tuple_element;

template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_size<ranges::subrange<_I, _S, _K>> : std::integral_constant<std::size_t, 2> {};

template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_element<0, ranges::subrange<_I, _S, _K>>
    { using type = _I; };

template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_element<1, ranges::subrange<_I, _S, _K>>
    { using type = _S; };

template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_element<0, const ranges::subrange<_I, _S, _K>>
    { using type = _I; };

template <class _I, class _S, ranges::subrange_kind _K>
struct tuple_element<1, const ranges::subrange<_I, _S, _K>>
    { using type = _S; };


} // namespace __XVI_STD_UTILITY_NS


// Required for the implementation of ranges::find_if and we have a mutual dependency.
#include <System/C++/Utility/RangesAlgorithm.hh>


#endif /* ifndef __SYSTEM_CXX_UTILITY_RANGESVIEWS_H */
