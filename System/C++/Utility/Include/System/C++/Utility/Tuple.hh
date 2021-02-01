#pragma once
#ifndef __SYSTEM_CXX_UTILITY_TUPLE_H
#define __SYSTEM_CXX_UTILITY_TUPLE_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/IntegerSequence.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/ReferenceWrapper.hh>
#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/TupleTraits.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class...> class tuple;


template <class> struct tuple_size;
template <class... _Types> struct tuple_size<tuple<_Types...>> : integral_constant<size_t, sizeof...(_Types)> {};

template <class _T> inline constexpr size_t tuple_size_v = tuple_size<_T>::value;


template <size_t, class> struct tuple_element;
template <size_t _I, class... _Types> struct tuple_element<_I, tuple<_Types...>>
    { using type = typename __detail::__nth_type<_I, _Types...>::type; };

template <size_t _I, class _T> using tuple_element_t = typename tuple_element<_I, _T>::type;


template <size_t _I, class... _Types>
constexpr tuple_element_t<_I, tuple<_Types...>>&
get(tuple<_Types...>& __t) noexcept
{
    return __t.template __get<_I>();
}

template <size_t _I, class... _Types>
constexpr const tuple_element_t<_I, tuple<_Types...>>&
get(const tuple<_Types...>& __t) noexcept
{
    return __t.template __get<_I>();
}

template <size_t _I, class... _Types>
constexpr tuple_element_t<_I, tuple<_Types...>>&&
get(tuple<_Types...>&& __t) noexcept
{
    return static_cast<tuple<_Types...>&&>(__t).template __get<_I>();
}

template <size_t _I, class... _Types>
constexpr const tuple_element_t<_I, tuple<_Types...>>&&
get(const tuple<_Types...>&& __t) noexcept
{
    return static_cast<const tuple<_Types...>&&>(__t).template __get<_I>();
}


template <class _T, class... _Types>
constexpr _T& get(tuple<_Types...>& __t) noexcept
{
    return __t.template __get<_T>();
}

template <class _T, class... _Types>
constexpr const _T& get(const tuple<_Types...>& __t) noexcept
{
    return __t.template __get<_T>();
}

template <class _T, class... _Types>
constexpr _T&& get(tuple<_Types...>&& __t) noexcept
{
    return static_cast<tuple<_Types...>&&>(__t).template __get<_T>();
}

template <class _T, class... _Types>
constexpr const _T&& get(const tuple<_Types...>&& __t) noexcept
{
    return static_cast<const tuple<_Types...>&&>(__t).template __get<_T>();
}



namespace __detail
{

struct __no_such_type {};

template <class...> struct __first_type { using type = __no_such_type; };
template <class _T, class... _Rest> struct __first_type<_T, _Rest...> { using type = _T; };

template <class...> struct __second_type { using type = __no_such_type; };
template <class _T, class _U, class... _Rest> struct __second_type<_T, _U, _Rest...> { using type = _U; };


struct __ignore_t
{ 
    explicit constexpr __ignore_t() = default;
    template <class _T> __ignore_t& operator==(_T&&) noexcept { return *this; }
};


template <size_t _I, class... _Types> struct __tuple_storage;

template <size_t _I, class _T, class... _Rest>
struct __tuple_storage<_I, _T, _Rest...> : __tuple_storage<_I, _T>, __tuple_storage<_I + 1, _Rest...>
{
    using __first_base = __tuple_storage<_I, _T>;
    using __rest_base = __tuple_storage<_I + 1, _Rest...>;
    
    constexpr __tuple_storage() = default;
    
    constexpr __tuple_storage(const __tuple_storage&) = default;
    constexpr __tuple_storage(__tuple_storage&&) = default;

    constexpr __tuple_storage& operator=(const __tuple_storage&) = default;
    constexpr __tuple_storage& operator=(__tuple_storage&&) 
        noexcept(is_nothrow_move_assignable_v<_T> && (is_nothrow_move_assignable_v<_Rest> && ...)) = default;

    ~__tuple_storage() = default;

    constexpr __tuple_storage(const _T& __t, const _Rest&&... __rest)
        : __first_base(__t), __rest_base(__rest...) {}

    template <class _U, class... _URest>
    constexpr __tuple_storage(_U&& __u, _URest&&... __urest)
        : __first_base(__XVI_STD_NS::forward<_U>(__u)), __rest_base(__XVI_STD_NS::forward<_URest>(__urest)...) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(const __tuple_storage<_J, _U, _URest...>& __u)
        : __first_base(static_cast<const __tuple_storage<_J, _U>&>(__u)),
          __rest_base(static_cast<const __tuple_storage<_J + 1, _URest...>&>(__u)) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(__tuple_storage<_J, _U, _URest...>&& __u)
        : __first_base(static_cast<__tuple_storage<_J, _U>&&>(__u)),
          __rest_base(static_cast<__tuple_storage<_J + 1, _URest...>&&>(__u)) {}

    template <class _U1, class _U2>
    constexpr __tuple_storage(const pair<_U1, _U2>& __p)
        : __first_base(__p.first),
          __rest_base(__p.second) {}

    template <class _U1, class _U2>
    constexpr __tuple_storage(pair<_U1, _U2>&& __p)
        : __first_base(static_cast<_U1&&>(__p.first)),
          __rest_base(static_cast<_U2&&>(__p.second)) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage& operator=(const __tuple_storage<_J, _U, _URest...>& __u)
    {
        __first_base::operator=(static_cast<const __tuple_storage<_J, _U>&>(__u));
        __rest_base::operator=(static_cast<const __tuple_storage<_J + 1, _URest...>&>(__u));
        return *this;
    }

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage& operator=(__tuple_storage<_J, _U, _URest...>&& __u)
    {
        __first_base::operator=(static_cast<__tuple_storage<_J, _U>&&>(__u));
        __rest_base::operator=(static_cast<__tuple_storage<_J + 1, _URest...>&&>(__u));
        return *this;
    }

    template <class _U1, class _U2>
    constexpr __tuple_storage& operator=(const pair<_U1, _U2>& __p)
    {
        __first_base::operator=(__p.first);
        __rest_base::operator=(__p.second);
        return *this;
    }

    template <class _U1, class _U2>
    constexpr __tuple_storage& operator=(pair<_U1, _U2>&& __p)
    {
        __first_base::operator=(static_cast<_U1&&>(__p.first));
        __rest_base::operator=(static_cast<_U2&&>(__p.second));
        return *this;
    }

    constexpr void swap(__tuple_storage& __x)
        noexcept(is_nothrow_swappable_v<_T> && (is_nothrow_swappable_v<_Rest> && ...))
    {
        __first_base::swap(__x);
        __rest_base::swap(__x);
    }

    template <size_t _J> constexpr auto& get() & noexcept
    {
        if constexpr (_J == 0)
            return __first_base::template get<_J>();
        else
            return __rest_base::template get<_J-1>();
    }

    template <size_t _J> constexpr const auto& get() const & noexcept
    {
        if constexpr (_J == 0)
            return __first_base::template get<_J>();
        else
            return __rest_base::template get<_J-1>();
    }

    template <size_t _J> constexpr auto&& get() && noexcept
    {
        if constexpr (_J == 0)
            return static_cast<__first_base&&>(*this).template get<_J>();
        else
            return static_cast<__rest_base&&>(*this).template get<_J-1>();
    }

    template <size_t _J> constexpr const auto&& get() const && noexcept
    {
        if constexpr (_J == 0)
            return static_cast<const __first_base&&>(*this).template get<_J>();
        else
            return static_cast<const __rest_base&&>(*this).template get<_J-1>();
    }

    template <class _U> constexpr _U& get() & noexcept
    {
        if constexpr (is_same_v<_T, _U>)
            return __first_base::template get<_U>();
        else
            return __rest_base::template get<_U>();
    }

    template <class _U> constexpr const _U& get() const & noexcept
    {
        if constexpr (is_same_v<_T, _U>)
            return __first_base::template get<_U>();
        else
            return __rest_base::template get<_U>();
    }

    template <class _U> constexpr _U&& get() && noexcept
    {
        if constexpr (is_same_v<_T, _U>)
            return static_cast<__first_base&&>(*this).template get<_U>();
        else
            return static_cast<__rest_base&&>(*this).template get<_U>();
    }

    template <class _U> constexpr const _U&& get() const && noexcept
    {
        if constexpr (is_same_v<_T, _U>)
            return static_cast<const __first_base&&>(*this).template get<_U>();
        else
            return static_cast<const __rest_base&&>(*this).template get<_U>();
    }
};

template <size_t _I, class _T>
struct __tuple_storage<_I, _T>
{
    _T _M_elem;

    constexpr __tuple_storage() = default;
    constexpr __tuple_storage(const __tuple_storage&) = default;
    constexpr __tuple_storage(__tuple_storage&&) = default;

    constexpr __tuple_storage& operator=(const __tuple_storage&) = default;
    constexpr __tuple_storage& operator=(__tuple_storage&&) noexcept(is_nothrow_move_assignable_v<_T>) = default;

    ~__tuple_storage() = default;

    constexpr __tuple_storage(const _T& __t)
        : _M_elem(__t) {}

    template <class _U>
    constexpr __tuple_storage(_U&& __u)
        : _M_elem(__XVI_STD_NS::forward<_U>(__u)) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage(const __tuple_storage<_J, _U>& __u)
        : _M_elem(__u._M_elem) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage(__tuple_storage<_J, _U>&& __u)
        : _M_elem(__XVI_STD_NS::forward<_U>(__u._M_elem)) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage& operator=(const __tuple_storage<_J, _U>& __u)
    {
        _M_elem = __u._M_elem;
        return *this;
    }

    template <size_t _J, class _U>
    constexpr __tuple_storage& operator=(__tuple_storage<_J, _U>&& __u)
    {
        _M_elem = __XVI_STD_NS::forward<_U>(__u._M_elem);
        return *this;
    }

    constexpr void swap(__tuple_storage& __x) noexcept(is_nothrow_swappable_v<_T>)
    {
        swap(_M_elem, __x._M_elem);
    }

    template <size_t _J> constexpr _T& get() & noexcept
    {
        static_assert(_J == 0);
        return _M_elem;
    }

    template <size_t _J> constexpr const _T& get() const & noexcept
    {
        static_assert(_J == 0);
        return _M_elem;
    }

    template <size_t _J> constexpr _T&& get() && noexcept
    {
        static_assert(_J == 0);
        return static_cast<_T&&>(_M_elem);
    }

    template <size_t _J> constexpr const _T&& get() const && noexcept
    {
        static_assert(_J == 0);
        return static_cast<const _T&&>(_M_elem);
    }

    template <class _U> constexpr _T& get() & noexcept
    {
        static_assert(is_same_v<_T, _U>);
        return _M_elem;
    }

    template <class _U> constexpr const _T& get() const & noexcept
    {
        static_assert(is_same_v<_T, _U>);
        return _M_elem;
    }

    template <class _U> constexpr _T&& get() && noexcept
    {
        static_assert(is_same_v<_T, _U>);
        return static_cast<_T&&>(_M_elem);
    }

    template <class _U> constexpr const _T&& get() const && noexcept
    {
        static_assert(is_same_v<_T, _U>);
        return static_cast<const _T&&>(_M_elem);
    }
};

template <> struct __tuple_storage<0> {};


template <class _MetaTuple, size_t... _ElemIdx, size_t... _TupleIdx>
tuple<typename tuple_element<_ElemIdx, remove_cvref_t<typename tuple_element<_TupleIdx, remove_cvref_t<_MetaTuple>>::type>>::type...>
__tuple_cat_t_helper_fn(index_sequence<_ElemIdx...>, index_sequence<_TupleIdx...>);

template <class _MetaTuple, class _ElemIdxSeq, class _TupleIdxSeq>
using __tuple_cat_t = decltype(__tuple_cat_t_helper_fn<_MetaTuple>(_ElemIdxSeq(), _TupleIdxSeq()));

template <class _MetaTuple, size_t... _ElemIdx, size_t... _TupleIdx>
constexpr __tuple_cat_t<_MetaTuple, index_sequence<_ElemIdx...>, index_sequence<_TupleIdx...>>
__tuple_cat(_MetaTuple&& __m, index_sequence<_ElemIdx...>, index_sequence<_TupleIdx...>)
{
    using __return_t = __tuple_cat_t<_MetaTuple, index_sequence<_ElemIdx...>, index_sequence<_TupleIdx...>>;
    return __return_t(get<_ElemIdx>(get<_TupleIdx>(__m))...);
}


template <class _F, class _Tuple, size_t... _I>
constexpr decltype(auto) __apply_helper(_F&& __f, _Tuple&& __t, index_sequence<_I...>)
{
    return _INVOKE(__XVI_STD_NS::forward<_F>(__f), get<_I>(__XVI_STD_NS::forward<_Tuple>(__t))...);
}

template <class _R, class _F, class _Tuple, size_t... _I>
constexpr decltype(auto) __apply_r_helper(_F&& __f, _Tuple&& __t, index_sequence<_I...>)
{
    return _INVOKE_R<_R>(__XVI_STD_NS::forward<_F>(__f), get<_I>(__XVI_STD_NS::forward<_Tuple>(__t))...);
}

template <class _R, class _F, class _Tuple>
constexpr decltype(auto) __apply_r(_F&& __f, _Tuple&& __t)
{
    return __detail::__apply_r_helper(__XVI_STD_NS::forward<_F>(__f), 
                                      __XVI_STD_NS::forward<_Tuple>(__t),
                                      make_index_sequence<tuple_size<remove_reference_t<_Tuple>>::value>{});
}


template <class _T, class _Tuple, size_t... _I>
constexpr _T __make_from_tuple_helper(_Tuple&& __t, index_sequence<_I...>)
{
    return _T(get<_I>(__XVI_STD_NS::forward<_Tuple>(__t))...);
}


template <class... _TTypes, class... _UTypes, size_t... _Idx>
constexpr bool __tuple_equality_helper(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y, index_sequence<_Idx...>)
{
    return ((get<_Idx>(__x) == get<_Idx>(__y)) && ...);
}

template <class... _TTypes, class... _UTypes>
constexpr bool __tuple_equality(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return __tuple_equality_helper(__x, __y, make_index_sequence<sizeof...(_TTypes)>{});
}

template <size_t _Offset, class... _TTypes, class... _UTypes>
constexpr bool __tuple_lt_helper(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    if constexpr (_Offset >= sizeof...(_TTypes))
        return false;
    else
    {
        if (bool(get<_Offset>(__x) < get<_Offset>(__y)))
            // x < y
            return true;
        else if (bool(get<_Offset>(__y) < get<_Offset>(__x)))
            // y > x
            return false;
        else
            // !(y < x) && !(x < y) (therefore, x == y) so we need to compare the remaining elements
            return __tuple_lt_helper<_Offset + 1>(__x, __y);
    }
}

template <class... _TTypes, class... _UTypes>
constexpr bool __tuple_lt(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return __tuple_lt_helper<0>(__x, __y);
}


} // namespace __detail


template <class... _Types> class tuple
{
public:

    constexpr explicit(!(__detail::__is_implicit_default_constructible_v<_Types> && ...)) 
    tuple() = default;

    template <size_t _N = sizeof...(_Types),
              class = enable_if_t<_N >= 1 && (is_copy_constructible_v<_Types> && ...), void>>
    constexpr explicit(!conjunction_v<is_convertible<const _Types&, _Types>...>)
    tuple(const _Types&... __t)
        : _M_elems(__t...) {}
    
    template <class... _UTypes,
              class = enable_if_t<sizeof...(_Types) == sizeof...(_UTypes)
                                  && (is_constructible_v<_Types, _UTypes> && ...), void>>
    constexpr explicit(!conjunction_v<is_convertible<_UTypes, _Types>...>)
    tuple(_UTypes&&... __u)
        : _M_elems(__XVI_STD_NS::forward<_UTypes>(__u)...) {}

    tuple(const tuple&) = default;
    tuple(tuple&&) = default;

    template <class... _UTypes,
              class = enable_if_t<sizeof...(_Types) == sizeof...(_UTypes)
                                  && (is_constructible_v<_Types, const _UTypes&> && ...)
                                  && (sizeof...(_Types) != 1
                                      || (!is_convertible_v<const tuple<_UTypes...>&, typename __detail::__first_type<_Types...>::type>
                                          && !is_constructible_v<typename __detail::__first_type<_Types...>::type, const tuple<_UTypes...>&>
                                          && !is_same_v<typename __detail::__first_type<_Types...>::type, typename __detail::__first_type<_UTypes...>::type>)),
                                  void>>
    constexpr explicit(!conjunction_v<is_convertible<const _UTypes&, _Types>...>)
    tuple(const tuple<_UTypes...>& __u)
        : _M_elems(__u._M_elems) {}
    
    template <class... _UTypes,
              class = enable_if_t<sizeof...(_Types) == sizeof...(_UTypes)
                                  && (is_constructible_v<_Types, _UTypes&&> && ...)
                                  && (sizeof...(_Types) != 1
                                      || (!is_convertible_v<tuple<_UTypes...>, typename __detail::__first_type<_Types...>::type>
                                          && !is_constructible_v<typename __detail::__first_type<_Types...>::type, tuple<_UTypes...>>
                                          && !is_same_v<typename __detail::__first_type<_Types...>::type, typename __detail::__first_type<_UTypes...>::type>)),
                                  void>>
    constexpr explicit(!conjunction_v<is_convertible<_UTypes, _Types>...>)
    tuple(tuple<_UTypes...>&& __u)
        : _M_elems(__XVI_STD_NS::move(__u._M_elems)) {}

    template <class _U1, class _U2,
              class = enable_if_t<sizeof...(_Types) == 2
                                  && is_constructible_v<typename __detail::__first_type<_Types...>::type, const _U1&>
                                  && is_constructible_v<typename __detail::__second_type<_Types...>::type, const _U2&>,
                                  void>>
    constexpr explicit(!is_convertible_v<const _U1&, typename __detail::__first_type<_Types...>::type>
                       || !is_convertible_v<const _U2&, typename __detail::__second_type<_Types...>::type>)
    tuple(const pair<_U1, _U2>& __p)
        : _M_elems(__p) {}

    template <class _U1, class _U2,
              class = enable_if_t<sizeof...(_Types) == 2
                                  && is_constructible_v<typename __detail::__first_type<_Types...>::type, _U1&&>
                                  && is_constructible_v<typename __detail::__second_type<_Types...>::type, _U2&&>,
                                  void>>
    constexpr explicit(!is_convertible_v<_U1, typename __detail::__first_type<_Types...>::type>
                       || !is_convertible_v<_U2, typename __detail::__second_type<_Types...>::type>)
    tuple(pair<_U1, _U2>&& __p)
        : _M_elems(__XVI_STD_NS::move(__p)) {}

    //! @TODO: uses-allocator constructors.

    constexpr tuple& operator=(const tuple&) = default;
    constexpr tuple& operator=(tuple&&) noexcept((is_nothrow_move_assignable_v<_Types> && ...)) = default;

    template <class... _UTypes,
              class = enable_if_t<sizeof...(_Types) == sizeof...(_UTypes)
                                  && (is_assignable_v<_Types&, const _UTypes&> && ...),
                                  void>>
    constexpr tuple& operator=(const tuple<_UTypes...>& __u)
    {
        _M_elems.operator=(__u._M_elems);
        return *this;
    }

    template <class... _UTypes,
              class = enable_if_t<sizeof...(_Types) == sizeof...(_UTypes)
                                  && (is_assignable_v<_Types&, _UTypes&&> && ...),
                                  void>>
    constexpr tuple& operator=(tuple<_UTypes...>&& __u)
    {
        _M_elems.operator=(__XVI_STD_NS::move(__u._M_elems));
        return *this;
    }

    template <class _U1, class _U2,
              class = enable_if_t<sizeof...(_Types) == 2
                                  && is_assignable_v<typename __detail::__first_type<_Types...>::type&, const _U1&>
                                  && is_assignable_v<typename __detail::__second_type<_Types...>::type&, const _U2&>,
                                  void>>
    constexpr tuple& operator=(const pair<_U1, _U2>& __p)
    {
        _M_elems.operator=(__p);
        return *this;
    }

    template <class _U1, class _U2,
              class = enable_if_t<sizeof...(_Types) == 2
                                  && is_assignable_v<typename __detail::__first_type<_Types...>::type&, _U1&&>
                                  && is_assignable_v<typename __detail::__second_type<_Types...>::type&, _U2&&>,
                                  void>>
    constexpr tuple& operator=(pair<_U1, _U2>&& __p)
    {
        _M_elems.operator=(__XVI_STD_NS::move(__p));
        return *this;
    }

    constexpr void swap(tuple& __rhs) noexcept((is_nothrow_swappable_v<_Types> && ...))
    {
        _M_elems.swap(__rhs._M_elems);
    }

    template <size_t _I> constexpr auto& __get() & noexcept
    {
        return _M_elems.template get<_I>();
    }

    template <size_t _I> constexpr const auto& __get() const & noexcept
    {
        return _M_elems.template get<_I>();
    }

    template <size_t _I> constexpr auto&& __get() && noexcept
    {
        return static_cast<__storage_t&&>(_M_elems).template get<_I>();
    }

    template <size_t _I> constexpr const auto&& __get() const && noexcept
    {
        return static_cast<const __storage_t&&>(_M_elems).template get<_I>();
    }

    template <class _T> constexpr _T& __get() & noexcept
    {
        static_assert(__detail::__type_unique_in_pack<_T, _Types...>::value);
        return _M_elems.template get<_T>();
    }

    template <class _T> constexpr const _T& __get() const & noexcept
    {
        static_assert(__detail::__type_unique_in_pack<_T, _Types...>::value);
        return _M_elems.template get<_T>();
    }

    template <class _T> constexpr _T&& __get() && noexcept
    {
        static_assert(__detail::__type_unique_in_pack<_T, _Types...>::value);
        return static_cast<__storage_t&&>(_M_elems).template get<_T>();
    }

    template <class _T> constexpr const _T&& __get() const && noexcept
    {
        static_assert(__detail::__type_unique_in_pack<_T, _Types...>::value);
        return static_cast<const __storage_t&&>(_M_elems).template get<_T>();
    }

private:

    template <class... _UTypes> friend class tuple;

    using __storage_t = __detail::__tuple_storage<0, _Types...>;
    __storage_t _M_elems;
};


inline constexpr __detail::__ignore_t ignore {};


template <class... _Types>
constexpr tuple<unwrap_ref_decay_t<_Types>...> make_tuple(_Types&&... __t)
{
    return tuple<unwrap_ref_decay_t<_Types>...>(__XVI_STD_NS::forward<_Types>(__t)...);
}

template <class... _Types>
constexpr tuple<_Types&&...> forward_as_tuple(_Types&&... __t) noexcept
{
    return tuple<_Types&&...>(__XVI_STD_NS::forward<_Types>(__t)...);
}

template <class... _Types>
constexpr tuple<_Types&...> tie(_Types&... __t) noexcept
{
    return tuple<_Types&...>(__t...);
}

template <class... _Tuples>
constexpr auto tuple_cat(_Tuples&&... __tpls)
{
    auto __meta_tuple = forward_as_tuple(__XVI_STD_NS::forward<_Tuples>(__tpls)...);
    
    // Generate a concatenated list of indices within each tuple. For example, if there are three input tuples with
    // lengths X, Y and Z, this will generate an index list containing {1..X, 1..Y, 1..Z}.
    using __elem_indices = __detail::__cat_integer_sequences<make_index_sequence<tuple_size<_Tuples>::value>...>;

    // And again but for the index of the tuple itself. Continuing the above example, this index sequence will contain
    // X integers with value 0 then Y integers with value 1 then Z integers with value 2.
    using __tuple_indices = __detail::__make_repeated_integer_sequence<size_t, tuple_size<_Tuples>::value...>;

    // The __elem_indices and __tuple_indices are index sequences with the same length and together are paired
    // subscripts into the __meta_tuple and then the individual tuples.
    return __detail::__tuple_cat(__meta_tuple, __elem_indices(), __tuple_indices());
}

template <class _F, class _Tuple>
constexpr decltype(auto) apply(_F&& __f, _Tuple&& __t)
{
    return __detail::__apply_helper(__XVI_STD_NS::forward<_F>(__f), 
                                    __XVI_STD_NS::forward<_Tuple>(__t),
                                    make_index_sequence<tuple_size<remove_reference_t<_Tuple>>::value>{});
}

template <class _T, class _Tuple>
constexpr _T make_from_tuple(_Tuple&& __t)
{
    return __detail::__make_from_tuple_helper<_T>(__XVI_STD_NS::forward<_Tuple>(__t),
                                                  make_index_sequence<tuple_size<remove_reference_t<_Tuple>>::value>{});
}


template <class... _TTypes, class... _UTypes>
constexpr bool operator==(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return __detail::__tuple_equality(__x, __y);
}

template <class... _TTypes, class... _UTypes>
constexpr bool operator!=(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return !(__x == __y);
}

template <class... _TTypes, class... _UTypes>
constexpr bool operator<(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return __detail::__tuple_lt(__x, __y);
}

template <class... _TTypes, class... _UTypes>
constexpr bool operator>(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return (__y < __x);
}

template <class... _TTypes, class... _UTypes>
constexpr bool operator<=(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return !(__y < __x);
}

template <class... _TTypes, class... _UTypes>
constexpr bool operator>=(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return !(__x < __y);
}


template <class, class> struct uses_allocator;

template <class... _Types, class _Alloc>
struct uses_allocator<tuple<_Types...>, _Alloc> : true_type {};


template <class... _Types>
constexpr void swap(tuple<_Types...>& __x, tuple<_Types...>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_TUPLE_H */
