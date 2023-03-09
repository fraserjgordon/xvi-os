#ifndef __SYSTEM_CXX_CORE_TUPLE_H
#define __SYSTEM_CXX_CORE_TUPLE_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/CtorTags.hh>
#include <System/C++/Core/IntegerSequence.hh>
#include <System/C++/Core/Invoke.hh>
#include <System/C++/Core/Pair.hh>
#include <System/C++/Core/ReferenceWrapper.hh>
#include <System/C++/Core/Swap.hh>
#include <System/C++/Core/TupleTraits.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class...> class tuple;


namespace __detail
{


template <class _TupleLike>
struct __convert_to_tuple
{
    template <size_t... _I>
    static auto __make(index_sequence<_I...>)
        -> tuple<tuple_element_t<_I, _TupleLike>...>;

    using type = decltype(__make(make_index_sequence<tuple_size_v<_TupleLike>>()));
};


template <template <class> class, template <class> class, class, class>
struct __has_tuple_common_reference_impl : false_type {};

template <template <class> class _TQual, template <class> class _UQual, class... _TTypes, class... _UTypes>
struct __has_tuple_common_reference_impl<_TQual, _UQual, tuple<_TTypes...>, tuple<_UTypes...>>
    : bool_constant<requires { typename tuple<common_reference_t<_TQual<_TTypes>, _UQual<_UTypes>>...>; }> {};

template <class _TTuple, class _UTuple, template <class> class _TQual, template <class> class _UQual>
concept __has_tuple_common_reference = __has_tuple_common_reference_impl<_TQual, _UQual, typename __convert_to_tuple<_TTuple>::type, typename __convert_to_tuple<_UTuple>::type>::value;

template <template <class> class, template <class> class, class, class> 
struct __tuple_common_reference_impl {};

template <template <class> class _TQual, template <class> class _UQual, class... _TTypes, class... _UTypes>
struct __tuple_common_reference_impl<_TQual, _UQual, tuple<_TTypes...>, tuple<_UTypes...>>
{
    using type = tuple<common_reference_t<_TQual<_TTypes>, _UQual<_UTypes>>...>;
};

template <template <class> class _TQual, template <class> class _UQual, class _TTuple, class _UTuple>
using __tuple_common_reference = typename __tuple_common_reference_impl<_TQual, _UQual, typename __convert_to_tuple<_TTuple>::type, typename __convert_to_tuple<_UTuple>::type>::type;


template <class, class>
struct __has_tuple_common_type_impl : false_type {};

template <class... _TTypes, class... _UTypes>
struct __has_tuple_common_type_impl<tuple<_TTypes...>, tuple<_UTypes...>>
    : bool_constant<requires { typename tuple<common_type_t<_TTypes, _UTypes>...>; }> {};

template <class _TTuple, class _UTuple>
concept __has_tuple_common_type = __has_tuple_common_type_impl<typename __convert_to_tuple<_TTuple>::type, typename __convert_to_tuple<_UTuple>::type>::value;

template <class, class>
struct __tuple_common_type_impl {};

template <class... _TTypes, class... _UTypes>
struct __tuple_common_type_impl<tuple<_TTypes...>, tuple<_UTypes...>>
{
    using type = tuple<common_type_t<_TTypes, _UTypes>...>;
};

template <class _TTuple, class _UTuple>
using __tuple_common_type = typename __tuple_common_type_impl<typename __convert_to_tuple<_TTuple>::type, typename __convert_to_tuple<_UTuple>::type>::type;


} // namespace __detail


template <__detail::__tuple_like _TTuple, __detail::__tuple_like _UTuple, template <class> class _TQual, template <class> class _UQual>
    requires (__detail::__is_tuple_specialisation<_TTuple>::value || __detail::__is_tuple_specialisation<_UTuple>::value)
        && is_same_v<_TTuple, decay_t<_TTuple>>
        && is_same_v<_UTuple, decay_t<_UTuple>>
        && (tuple_size_v<_TTuple> == tuple_size_v<_UTuple>)
        && __detail::__has_tuple_common_reference<_TTuple, _UTuple, _TQual, _UQual>
struct basic_common_reference<_TTuple, _UTuple, _TQual, _UQual>
{
    using type = typename __detail::__tuple_common_reference<_TQual, _UQual, _TTuple, _UTuple>;
};

template <__detail::__tuple_like _TTuple, __detail::__tuple_like _UTuple>
    requires (__detail::__is_tuple_specialisation<_TTuple>::value || __detail::__is_tuple_specialisation<_UTuple>::value)
        && is_same_v<_TTuple, decay_t<_TTuple>>
        && is_same_v<_UTuple, decay_t<_UTuple>>
        && (tuple_size_v<_TTuple> == tuple_size_v<_UTuple>)
        && __detail::__has_tuple_common_type<_TTuple, _UTuple>
struct common_type<_TTuple, _UTuple>
{
    using type = typename __detail::__tuple_common_type<_TTuple, _UTuple>;
};


template <class> struct tuple_size;
template <class... _Types> struct tuple_size<tuple<_Types...>> : integral_constant<size_t, sizeof...(_Types)> {};


template <size_t, class> struct tuple_element;
template <size_t _I, class... _Types> struct tuple_element<_I, tuple<_Types...>>
    { using type = typename __detail::__nth_type<_I, _Types...>::type; };


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

template <class... _Types> using __first_type_t = typename __first_type<_Types...>::type;
template <class... _Types> using __second_type_t = typename __second_type<_Types...>::type;


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

    constexpr const __tuple_storage& operator=(const __tuple_storage& __t) const
    {
        __first_base::operator=(static_cast<const __first_base&>(__t));
        __rest_base::operator=(static_cast<const __rest_base&>(__t));
        return *this;
    }

    constexpr const __tuple_storage& operator=(__tuple_storage&& __t) const
    {
        __first_base::operator=(static_cast<__first_base&&>(__t));
        __rest_base::operator=(static_cast<__rest_base&&>(__t));
        return *this;
    }

    ~__tuple_storage() = default;

    template <class _U, class... _URest>
    constexpr __tuple_storage(_U&& __u, _URest&&... __urest)
        : __first_base(__XVI_STD_NS::forward<_U>(__u)),
          __rest_base(__XVI_STD_NS::forward<_URest>(__urest)...) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(__tuple_storage<_J, _U, _URest...>& __u)
        : __first_base(static_cast<__tuple_storage<_J, _U>&>(__u)),
          __rest_base(static_cast<__tuple_storage<_J + 1, _URest...>&>(__u)) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(const __tuple_storage<_J, _U, _URest...>& __u)
        : __first_base(static_cast<const __tuple_storage<_J, _U>&>(__u)),
          __rest_base(static_cast<const __tuple_storage<_J + 1, _URest...>&>(__u)) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(__tuple_storage<_J, _U, _URest...>&& __u)
        : __first_base(static_cast<__tuple_storage<_J, _U>&&>(__u)),
          __rest_base(static_cast<__tuple_storage<_J + 1, _URest...>&&>(__u)) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(const __tuple_storage<_J, _U, _URest...>&& __u)
        : __first_base(static_cast<const __tuple_storage<_J, _U>&&>(__u)),
          __rest_base(static_cast<const __tuple_storage<_J + 1, _URest...>&&>(__u)) {}

    template <class _U1, class _U2>
    constexpr __tuple_storage(pair<_U1, _U2>& __p)
        : __first_base(__p.first),
          __rest_base(__p.second) {}

    template <class _U1, class _U2>
    constexpr __tuple_storage(const pair<_U1, _U2>& __p)
        : __first_base(__p.first),
          __rest_base(__p.second) {}

    template <class _U1, class _U2>
    constexpr __tuple_storage(pair<_U1, _U2>&& __p)
        : __first_base(static_cast<_U1&&>(__p.first)),
          __rest_base(static_cast<_U2&&>(__p.second)) {}

    template <class _U1, class _U2>
    constexpr __tuple_storage(const pair<_U1, _U2>&& __p)
        : __first_base(static_cast<const _U1&&>(__p.first)),
          __rest_base(static_cast<const _U2&&>(__p.second)) {}

    template <class _Alloc>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc)
        : __first_base(allocator_arg, __alloc),
          __rest_base(allocator_arg, __alloc) {}

    template <class _Alloc>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const __tuple_storage& __t)
        : __first_base(allocator_arg, __alloc, static_cast<const __first_base&>(__t)),
          __rest_base(allocator_arg, __alloc, static_cast<const __rest_base&>(__t)) {}

    template <class _Alloc>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, __tuple_storage&& __t)
        : __first_base(allocator_arg, __alloc, static_cast<__first_base&&>(__t)),
          __rest_base(allocator_arg, __alloc, static_cast<__rest_base&&>(__t)) {}

    template <class _Alloc, class _U, class... _URest>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, _U&& __u, _URest&&... __urest)
        : __first_base(allocator_arg, __alloc, __XVI_STD_NS::forward<_U>(__u)),
          __rest_base(allocator_arg, __alloc, __XVI_STD_NS::forward<_URest>(__urest)...) {}

    template <class _Alloc, size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, __tuple_storage<_J, _U, _URest...>& __u)
        : __first_base(allocator_arg, __alloc, static_cast<__tuple_storage<_J, _U>&>(__u)),
          __rest_base(allocator_arg, __alloc, static_cast<__tuple_storage<_J + 1, _URest...>&>(__u)) {}

    template <class _Alloc, size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const __tuple_storage<_J, _U, _URest...>& __u)
        : __first_base(allocator_arg, __alloc, static_cast<const __tuple_storage<_J, _U>&>(__u)),
          __rest_base(allocator_arg, __alloc, static_cast<const __tuple_storage<_J + 1, _URest...>&>(__u)) {}

    template <class _Alloc, size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, __tuple_storage<_J, _U, _URest...>&& __u)
        : __first_base(allocator_arg, __alloc, static_cast<__tuple_storage<_J, _U>&&>(__u)),
          __rest_base(allocator_arg, __alloc, static_cast<__tuple_storage<_J + 1, _URest...>&&>(__u)) {}

    template <class _Alloc, size_t _J, class _U, class... _URest>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const __tuple_storage<_J, _U, _URest...>&& __u)
        : __first_base(allocator_arg, __alloc, static_cast<const __tuple_storage<_J, _U>&&>(__u)),
          __rest_base(allocator_arg, __alloc, static_cast<const __tuple_storage<_J + 1, _URest...>&&>(__u)) {}

    template <class _Alloc, class _U1, class _U2>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, pair<_U1, _U2>& __p)
        : __first_base(allocator_arg, __alloc, __p.first),
          __rest_base(allocator_arg, __alloc, __p.second) {}

    template <class _Alloc, class _U1, class _U2>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const pair<_U1, _U2>& __p)
        : __first_base(allocator_arg, __alloc, __p.first),
          __rest_base(allocator_arg, __alloc, __p.second) {}

    template <class _Alloc, class _U1, class _U2>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, pair<_U1, _U2>&& __p)
        : __first_base(allocator_arg, __alloc, static_cast<_U1&&>(__p.first)),
          __rest_base(allocator_arg, __alloc, static_cast<_U2&&>(__p.second)) {}

    template <class _Alloc, class _U1, class _U2>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const pair<_U1, _U2>&& __p)
        : __first_base(allocator_arg, __alloc, static_cast<const _U1&&>(__p.first)),
          __rest_base(allocator_arg, __alloc, static_cast<const _U2&&>(__p.second)) {}

    template <size_t _J, class _U, class... _URest>
    constexpr __tuple_storage& operator=(const __tuple_storage<_J, _U, _URest...>& __u)
    {
        __first_base::operator=(static_cast<const __tuple_storage<_J, _U>&>(__u));
        __rest_base::operator=(static_cast<const __tuple_storage<_J + 1, _URest...>&>(__u));
        return *this;
    }

    template <size_t _J, class _U, class... _URest>
    constexpr const __tuple_storage& operator=(const __tuple_storage<_J, _U, _URest...>& __u) const
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

    template <size_t _J, class _U, class... _URest>
    constexpr const __tuple_storage& operator=(__tuple_storage<_J, _U, _URest...>&& __u) const
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
    constexpr const __tuple_storage& operator=(const pair<_U1, _U2>& __p) const
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

    template <class _U1, class _U2>
    constexpr const __tuple_storage& operator=(pair<_U1, _U2>&& __p) const
    {
        __first_base::operator=(static_cast<const _U1&&>(__p.first));
        __rest_base::operator=(static_cast<const _U2&&>(__p.second));
        return *this;
    }

    constexpr void swap(__tuple_storage& __x)
        noexcept(is_nothrow_swappable_v<_T> && (is_nothrow_swappable_v<_Rest> && ...))
    {
        __first_base::swap(__x);
        __rest_base::swap(__x);
    }

    constexpr void swap(const __tuple_storage& __x) const
        noexcept(is_nothrow_swappable_v<const _T> && (is_nothrow_swappable_v<const _Rest> && ...))
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

    constexpr const __tuple_storage& operator=(const __tuple_storage& __t) const
    {
        _M_elem = __t._M_elem;
        return *this;
    }

    constexpr const __tuple_storage& operator=(__tuple_storage&& __t) const
    {
        _M_elem = __XVI_STD_NS::move(__t._M_elem);
        return this;
    }

    ~__tuple_storage() = default;

    template <class _U>
    constexpr __tuple_storage(_U&& __u)
        : _M_elem(__XVI_STD_NS::forward<_U>(__u)) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage(__tuple_storage<_J, _U>& __u)
        : _M_elem(__u._M_elem) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage(const __tuple_storage<_J, _U>& __u)
        : _M_elem(__u._M_elem) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage(__tuple_storage<_J, _U>&& __u)
        : _M_elem(__XVI_STD_NS::move(__u._M_elem)) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage(const __tuple_storage<_J, _U>&& __u)
        : _M_elem(static_cast<const _U&&>(__u._M_elem)) {}

    template <class _Alloc>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc)
        : _M_elem(make_obj_using_allocator<_T>(__alloc)) {}

    template <class _Alloc>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const __tuple_storage& __t)
        : _M_elem(make_obj_using_allocator<_T>(__alloc, __t._M_elem)) {}

    template <class _Alloc>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, __tuple_storage&& __t)
        : _M_elem(make_obj_using_allocator<_T>(__alloc, __XVI_STD_NS::move(__t._M_elem))) {}

    template <class _Alloc, class _U>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, _U&& __u)
        : _M_elem(make_obj_using_allocator<_T>(__alloc, __XVI_STD_NS::forward<_U>(__u))) {}

    template <class _Alloc, size_t _J, class _U>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, __tuple_storage<_J, _U>& __u)
        : _M_elem(make_obj_using_allocator<_T>(__alloc, __u._M_elem)) {}

    template <class _Alloc, size_t _J, class _U>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const __tuple_storage<_J, _U>& __u)
        : _M_elem(make_obj_using_allocator<_T>(__alloc, __u._M_elem)) {}

    template <class _Alloc, size_t _J, class _U>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, __tuple_storage<_J, _U>&& __u)
        : _M_elem(make_obj_using_allocator<_T>(__alloc, __XVI_STD_NS::move(__u._M_elem))) {}

    template <class _Alloc, size_t _J, class _U>
    constexpr __tuple_storage(allocator_arg_t, const _Alloc& __alloc, const __tuple_storage<_J, _U>&& __u)
        : _M_elem(make_obj_using_allocator<_T>(__alloc, static_cast<const _U&&>(__u._M_elem))) {}

    template <size_t _J, class _U>
    constexpr __tuple_storage& operator=(const __tuple_storage<_J, _U>& __u)
    {
        _M_elem = __u._M_elem;
        return *this;
    }

    template <size_t _J, class _U>
    constexpr const __tuple_storage& operator=(const __tuple_storage<_J, _U>& __u) const
    {
        _M_elem = __u._M_elem;
        return *this;
    }

    template <size_t _J, class _U>
    constexpr __tuple_storage& operator=(__tuple_storage<_J, _U>&& __u)
    {
        _M_elem = __XVI_STD_NS::move(__u._M_elem);
        return *this;
    }

    template <size_t _J, class _U>
    constexpr const __tuple_storage& operator=(__tuple_storage<_J, _U>&& __u) const
    {
        _M_elem = __XVI_STD_NS::move(__u._M_elem);
        return *this;
    }

    constexpr void swap(__tuple_storage& __x) noexcept(is_nothrow_swappable_v<_T>)
    {
        swap(_M_elem, __x._M_elem);
    }

    constexpr void swap(const __tuple_storage& __x) const noexcept(is_nothrow_swappable_v<const _T>)
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
    return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), get<_I>(__XVI_STD_NS::forward<_Tuple>(__t))...);
}

template <class _R, class _F, class _Tuple, size_t... _I>
constexpr decltype(auto) __apply_r_helper(_F&& __f, _Tuple&& __t, index_sequence<_I...>)
{
    return __XVI_STD_NS::invoke<_R>(__XVI_STD_NS::forward<_F>(__f), get<_I>(__XVI_STD_NS::forward<_Tuple>(__t))...);
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
constexpr auto __tuple_compare_helper(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
    -> __XVI_STD_NS::common_comparison_category_t<__synth_three_way_result<_TTypes, _UTypes>...>
{
    auto __c = __synth_three_way(get<_Offset>(__x), get<_Offset>(__y));
    
    if constexpr (_Offset == (sizeof...(_TTypes) - 1))
    {
        return __c;
    }
    else
    {
        if (__c != 0)
            return __c;

        return __tuple_compare_helper<_Offset + 1>(__x, __y);
    }
}

template <class... _TTypes, class... _UTypes>
constexpr auto __tuple_compare(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return __tuple_compare_helper<0>(__x, __y);
}


template <class...> struct __tuple_ctor_disambiguation_constraint : false_type {};

template <class _Type, class _UType>
struct __tuple_ctor_disambiguation_constraint<tuple<_Type>, _UType> :
    negation<is_same<remove_cvref_t<_UType>, tuple<_Type>>> {};


template <class _T0, class _T1, class _U0, class _U1>
struct __tuple_ctor_disambiguation_constraint<tuple<_T0, _T1>, _U0, _U1> :
    bool_constant<!is_same_v<remove_cvref_t<_U0>, allocator_arg_t> || is_same_v<remove_cvref_t<_T0>, allocator_arg_t>> {};

template <class _T0, class _T1, class _T2, class _U0, class _U1, class _U2>
struct __tuple_ctor_disambiguation_constraint<tuple<_T0, _T1, _T2>, _U0, _U1, _U2> :
    __tuple_ctor_disambiguation_constraint<tuple<_T0, _T1>, _U0, _U1> {};


template <class _UTuple, class... _Types, __XVI_STD_NS::size_t... _I>
consteval bool __tuple_constructible_from(__XVI_STD_NS::index_sequence<_I...>)
{
    return (__XVI_STD_NS::is_constructible_v<_Types, decltype(get<_I>(declval<_UTuple>()))> && ...);
}

template <class _UTuple, class... _Types, __XVI_STD_NS::size_t... _I>
consteval bool __tuple_convertible_from(__XVI_STD_NS::index_sequence<_I...>)
{
    return (__XVI_STD_NS::is_convertible_v<decltype(get<_I>(declval<_UTuple>())), _Types> && ...);
}

template <class _UTuple, class... _Types, __XVI_STD_NS::size_t... _I>
consteval bool __tuple_ref_constructs_from_temporary(__XVI_STD_NS::index_sequence<_I...>)
{
    return (__XVI_STD_NS::reference_constructs_from_temporary_v<_Types, decltype(get<_I>(declval<_UTuple>()))> || ...);
}

template <class _UTuple, class... _Types>
inline constexpr bool __tuple_constructible_from_v = __tuple_constructible_from<_UTuple, _Types...>(__XVI_STD_NS::make_index_sequence<sizeof...(_Types)>());

template <class _UTuple, class... _Types>
inline constexpr bool __tuple_convertible_from_v = __tuple_convertible_from<_UTuple, _Types...>(__XVI_STD_NS::make_index_sequence<sizeof...(_Types)>());

template <class _UTuple, class... _Types>
inline constexpr bool __tuple_ref_constructs_from_temporary_v = __tuple_ref_constructs_from_temporary<_UTuple, _Types...>(__XVI_STD_NS::make_index_sequence<sizeof...(_Types)>());

template <class, class> struct __tuple_constructible_from_tuple_impl;

template <class... _Types, class _UTuple>
struct __tuple_constructible_from_tuple_impl<tuple<_Types...>, _UTuple>
    : bool_constant<
            __tuple_constructible_from_v<_UTuple, _Types...>
            && (sizeof...(_Types) != 1
                || (!__XVI_STD_NS::is_convertible_v<_UTuple, __first_type_t<_Types...>>
                    && !__XVI_STD_NS::is_constructible_v<__first_type_t<_Types...>, _UTuple>
                    && !__XVI_STD_NS::is_same_v<__first_type_t<_Types...>, tuple_element_t<0, remove_cvref_t<_UTuple>>>))> {};

template <class _Tuple, class _UTuple>
concept __tuple_constructible_from_tuple = __tuple_constructible_from_tuple_impl<_Tuple, _UTuple>::value;

template <class _Pair, class... _Types>
concept __tuple_ref_constructs_from_temporary_pair = __XVI_STD_NS::reference_constructs_from_temporary_v<__first_type_t<_Types...>, decltype(get<0>(declval<_Pair>()))>
    || __XVI_STD_NS::reference_constructs_from_temporary_v<__second_type_t<_Types...>, decltype(get<1>(declval<_Pair>()))>;

template <class _Pair, class... _Types>
concept __tuple_constructs_from_pair = (sizeof...(_Types) == 2)
    && __XVI_STD_NS::is_convertible_v<__first_type_t<_Types...>, decltype(get<0>(declval<_Pair>()))>
    && __XVI_STD_NS::is_convertible_v<__second_type_t<_Types...>, decltype(get<1>(declval<_Pair>()))>
    && (!__tuple_ref_constructs_from_temporary_pair<_Pair, _Types...>);

template <class _Pair, class... _Types>
inline constexpr bool __tuple_converts_from_pair_v = !__XVI_STD_NS::is_convertible_v<decltype(get<0>(declval<_Pair>())), __first_type_t<_Types...>>
    && !__XVI_STD_NS::is_convertible_v<decltype(get<1>(declval<_Pair>())), __second_type_t<_Types...>>;


} // namespace __detail


template <class... _Types> class tuple
{
public:

    constexpr explicit(!(__detail::__is_implicit_default_constructible_v<_Types> && ...)) 
    tuple()
        requires __XVI_STD_NS::conjunction_v<__XVI_STD_NS::is_default_constructible<_Types>...>
    = default;

    constexpr explicit(!conjunction_v<is_convertible<const _Types&, _Types>...>)
    tuple(const _Types&... __t)
        requires (sizeof...(_Types) >= 1) && __XVI_STD_NS::conjunction_v<__XVI_STD_NS::is_copy_constructible<_Types>...>
        : _M_elems(__t...) {}
    
    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && (sizeof...(_Types) >= 1)
            && __XVI_STD_NS::conjunction_v<__detail::__tuple_ctor_disambiguation_constraint<tuple, _UTypes...>, __XVI_STD_NS::is_constructible<_Types, _UTypes>...>
            && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<_Types, _UTypes&&> || ...))
    constexpr explicit(!conjunction_v<is_convertible<_UTypes, _Types>...>)
    tuple(_UTypes&&... __u)
        : _M_elems(__XVI_STD_NS::forward<_UTypes>(__u)...) {}

    template <class... _UTypes>
        requires (__XVI_STD_NS::reference_constructs_from_temporary_v<_Types, _UTypes&&> || ...)
    constexpr explicit(!conjunction_v<is_convertible<_UTypes, _Types>...>)
    tuple(_UTypes&&... __u) = delete;

    tuple(const tuple&) = default;
    
    tuple(tuple&&)
        requires __XVI_STD_NS::conjunction_v<__XVI_STD_NS::is_move_constructible<_Types>...>
    = default;

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, tuple<_UTypes...>&>
            && (!__detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&&, _Types...>)
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&, _Types...>)
    tuple(tuple<_UTypes...>& __u)
        : _M_elems(__u._M_elems) {}

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, tuple<_UTypes...>&>
            && __detail::__tuple_ref_constructs_from_temporary_v<tuple<_UTypes...>&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&, _Types...>)
    tuple(tuple<_UTypes...>&) = delete;

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, const tuple<_UTypes...>&>
            && (!__detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&&, _Types...>)
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&, _Types...>)
    tuple(const tuple<_UTypes...>& __u)
        : _M_elems(__u._M_elems) {}

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, const tuple<_UTypes...>&>
            && __detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&, _Types...>)
    tuple(const tuple<_UTypes...>&) = delete;

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, tuple<_UTypes...>&&>
            && (!__detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&&, _Types...>)
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&&, _Types...>)
    tuple(tuple<_UTypes...>&& __u)
        : _M_elems(__XVI_STD_NS::move(__u._M_elems)) {}

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, tuple<_UTypes...>&&>
            && __detail::__tuple_ref_constructs_from_temporary_v<tuple<_UTypes...>&&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&&, _Types...>)
    tuple(tuple<_UTypes...>&&) = delete;
  
    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, const tuple<_UTypes...>&&>
            && (!__detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&&, _Types...>)
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&&, _Types...>)
    tuple(const tuple<_UTypes...>&& __u)
        : _M_elems(static_cast<decltype(__u._M_elems)>(__u._M_elems)) {}

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, const tuple<_UTypes...>&&>
            && __detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&&, _Types...>)
    tuple(const tuple<_UTypes...>&&) = delete;

    template <class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&, _Types...>)
    tuple(pair<_U1, _U2>& __p)
        : _M_elems(__p) {}

    template <class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&, _Types...>)
    tuple (pair<_U1, _U2>&) = delete;

    template <class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<const pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<const pair<_U1, _U2>&, _Types...>)
    tuple(const pair<_U1, _U2>& __p)
        : _M_elems(__p) {}

    template <class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<const pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<const pair<_U1, _U2>&, _Types...>)
    tuple (const pair<_U1, _U2>&) = delete;

    template <class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&&, _Types...>)
    tuple(pair<_U1, _U2>&& __p)
        : _M_elems(__XVI_STD_NS::move(__p)) {}

    template <class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&&, _Types...>)
    tuple (pair<_U1, _U2>&&) = delete;

    template <class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<const pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<const pair<_U1, _U2>&&, _Types...>)
    tuple(const pair<_U1, _U2>&& __p)
        : _M_elems(static_cast<decltype(__p)>(__p)) {}

    template <class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<const pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&&, _Types...>)
    tuple (const pair<_U1, _U2>&&) = delete;


    template <class _Alloc>
    constexpr explicit(!(__detail::__is_implicit_default_constructible_v<_Types> && ...)) 
    tuple(allocator_arg_t, const _Alloc& __alloc)
        requires __XVI_STD_NS::conjunction_v<__XVI_STD_NS::is_default_constructible<_Types>...>
        : _M_elems(allocator_arg, __alloc)
    {
    }

    template <class _Alloc>
    constexpr explicit(!conjunction_v<is_convertible<const _Types&, _Types>...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, const _Types&... __t)
        requires (sizeof...(_Types) >= 1) && __XVI_STD_NS::conjunction_v<__XVI_STD_NS::is_copy_constructible<_Types>...>
        : _M_elems(allocator_arg, __alloc, __t...) {}
    
    template <class _Alloc, class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && (sizeof...(_Types) >= 1)
            && __XVI_STD_NS::conjunction_v<__detail::__tuple_ctor_disambiguation_constraint<tuple, _UTypes...>, __XVI_STD_NS::is_constructible<_Types, _UTypes>...>
            && (!(__XVI_STD_NS::reference_constructs_from_temporary_v<_Types, _UTypes&&> || ...))
    constexpr explicit(!conjunction_v<is_convertible<_UTypes, _Types>...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, _UTypes&&... __u)
        : _M_elems(allocator_arg, __alloc, __XVI_STD_NS::forward<_UTypes>(__u)...) {}

    template <class _Alloc, class... _UTypes>
        requires (__XVI_STD_NS::reference_constructs_from_temporary_v<_Types, _UTypes&&> || ...)
    constexpr explicit(!conjunction_v<is_convertible<_UTypes, _Types>...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, _UTypes&&... __u) = delete;

    template <class _Alloc>
    tuple(allocator_arg_t, const _Alloc& __alloc, const tuple& __t)
        : _M_elems(allocator_arg, __alloc, __t._M_elems) {}
    
    template <class _Alloc>
    tuple(allocator_arg_t, const _Alloc& __alloc, tuple&& __t)
        requires __XVI_STD_NS::conjunction_v<__XVI_STD_NS::is_move_constructible<_Types>...>
        : _M_elems(allocator_arg, __alloc, __XVI_STD_NS::move(__t._M_elems)) {}

    template <class _Alloc, class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, tuple<_UTypes...>&>
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, tuple<_UTypes...>& __u)
        : _M_elems(allocator_arg, __alloc, __u._M_elems) {}

    template <class _Alloc, class... _UTypes>
        requires __detail::__tuple_ref_constructs_from_temporary_v<tuple<_UTypes...>&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, tuple<_UTypes...>&) = delete;

    template <class _Alloc, class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, const tuple<_UTypes...>&>
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, const tuple<_UTypes...>& __u)
        : _M_elems(allocator_arg, __alloc, __u._M_elems) {}

    template <class _Alloc, class... _UTypes>
        requires __detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, const tuple<_UTypes...>&) = delete;

    template <class _Alloc, class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, tuple<_UTypes...>&&>
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, tuple<_UTypes...>&& __u)
        : _M_elems(allocator_arg, __alloc, __XVI_STD_NS::move(__u._M_elems)) {}

    template <class _Alloc, class... _UTypes>
        requires __detail::__tuple_ref_constructs_from_temporary_v<tuple<_UTypes...>&&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<tuple<_UTypes...>&&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, tuple<_UTypes...>&&) = delete;
  
    template <class _Alloc, class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && __detail::__tuple_constructible_from_tuple<tuple, const tuple<_UTypes...>&&>
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, const tuple<_UTypes...>&& __u)
        : _M_elems(allocator_arg, __alloc, static_cast<decltype(__u._M_elems)>(__u._M_elems)) {}

    template <class _Alloc, class... _UTypes>
        requires __detail::__tuple_ref_constructs_from_temporary_v<const tuple<_UTypes...>&&, _Types...>
    constexpr explicit(!__detail::__tuple_convertible_from_v<const tuple<_UTypes...>&&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, const tuple<_UTypes...>&&) = delete;

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, pair<_U1, _U2>& __p)
        : _M_elems(allocator_arg, __alloc, __p) {}

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&, _Types...>)
    tuple (allocator_arg_t, const _Alloc& __alloc, pair<_U1, _U2>&) = delete;

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<const pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<const pair<_U1, _U2>&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, const pair<_U1, _U2>& __p)
        : _M_elems(allocator_arg, __alloc, __p) {}

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<const pair<_U1, _U2>&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<const pair<_U1, _U2>&, _Types...>)
    tuple (allocator_arg_t, const _Alloc& __alloc, const pair<_U1, _U2>&) = delete;

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, pair<_U1, _U2>&& __p)
        : _M_elems(allocator_arg, __alloc, __XVI_STD_NS::move(__p)) {}

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&&, _Types...>)
    tuple (allocator_arg_t, const _Alloc& __alloc, pair<_U1, _U2>&&) = delete;

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_constructs_from_pair<const pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<const pair<_U1, _U2>&&, _Types...>)
    tuple(allocator_arg_t, const _Alloc& __alloc, const pair<_U1, _U2>&& __p)
        : _M_elems(allocator_arg, __alloc, static_cast<decltype(__p)>(__p)) {}

    template <class _Alloc, class _U1, class _U2>
        requires __detail::__tuple_ref_constructs_from_temporary_pair<const pair<_U1, _U2>&&, _Types...>
    constexpr explicit(!__detail::__tuple_converts_from_pair_v<pair<_U1, _U2>&&, _Types...>)
    tuple (allocator_arg_t, const _Alloc& __alloc, const pair<_U1, _U2>&&) = delete;


    constexpr tuple& operator=(const tuple&) = default;
    constexpr tuple& operator=(tuple&&) noexcept((is_nothrow_move_assignable_v<_Types> && ...)) = default;

    constexpr const tuple& operator=(const tuple& __t) const
        requires (__XVI_STD_NS::is_copy_assignable_v<const _Types> && ...)
    {
        _M_elems.operator=(__t._M_elems);
        return *this;
    }

    constexpr const tuple& operator=(tuple&& __t) const
        requires (__XVI_STD_NS::is_assignable_v<const _Types&, _Types> && ...)
    {
        _M_elems.operator=(__XVI_STD_NS::move(__t._M_elems));
        return *this;
    }

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && (__XVI_STD_NS::is_assignable_v<_Types&, const _UTypes&> && ...)
    constexpr tuple& operator=(const tuple<_UTypes...>& __u)
    {
        _M_elems.operator=(__u._M_elems);
        return *this;
    }

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && (__XVI_STD_NS::is_assignable_v<const _Types&, const _UTypes&> && ...)
    constexpr const tuple& operator=(const tuple<_UTypes...>& __u) const
    {
        _M_elems.operator=(__u._M_elems);
        return *this;
    }

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && (__XVI_STD_NS::is_assignable_v<_Types&, _UTypes> && ...)
    constexpr tuple& operator=(tuple<_UTypes...>&& __u)
    {
        _M_elems.operator=(__XVI_STD_NS::move(__u._M_elems));
        return *this;
    }

    template <class... _UTypes>
        requires (sizeof...(_Types) == sizeof...(_UTypes))
            && (__XVI_STD_NS::is_assignable_v<const _Types&, _UTypes> && ...)
    constexpr const tuple& operator=(tuple<_UTypes...>&& __u)
    {
        _M_elems.operator=(__XVI_STD_NS::move(__u._M_elems));
        return *this;
    }

    template <class _U1, class _U2>
        requires (sizeof...(_Types) == 2)
            && __XVI_STD_NS::is_assignable_v<__detail::__first_type_t<_Types...>&, const _U1&>
            && __XVI_STD_NS::is_assignable_v<__detail::__second_type_t<_Types...>&, const _U2&>
    constexpr tuple& operator=(const pair<_U1, _U2>& __p)
    {
        _M_elems.operator=(__p);
        return *this;
    }

    template <class _U1, class _U2>
        requires (sizeof...(_Types) == 2)
            && __XVI_STD_NS::is_assignable_v<const __detail::__first_type_t<_Types...>&, const _U1&>
            && __XVI_STD_NS::is_assignable_v<const __detail::__second_type_t<_Types...>&, const _U2&>
    constexpr const tuple& operator=(const pair<_U1, _U2>& __p) const
    {
        _M_elems.operator=(__p);
        return *this;
    }

    template <class _U1, class _U2>
        requires (sizeof...(_Types) == 2)
            && __XVI_STD_NS::is_assignable_v<__detail::__first_type_t<_Types...>&, _U1>
            && __XVI_STD_NS::is_assignable_v<__detail::__second_type_t<_Types...>&, _U2>
    constexpr tuple& operator=(pair<_U1, _U2>&& __p)
    {
        _M_elems.operator=(__XVI_STD_NS::move(__p));
        return *this;
    }

    template <class _U1, class _U2>
        requires (sizeof...(_Types) == 2)
            && __XVI_STD_NS::is_assignable_v<const __detail::__first_type_t<_Types...>&, _U1>
            && __XVI_STD_NS::is_assignable_v<const __detail::__second_type_t<_Types...>&, _U2>
    constexpr const tuple& operator=(pair<_U1, _U2>&& __p) const
    {
        _M_elems.operator=(__XVI_STD_NS::move(__p));
        return *this;
    }

    constexpr void swap(tuple& __rhs) noexcept((is_nothrow_swappable_v<_Types> && ...))
    {
        _M_elems.swap(__rhs._M_elems);
    }

    constexpr void swap(const tuple& __rhs) const noexcept((is_nothrow_swappable_v<const _Types> && ...))
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


    // Utility method that returns the 
};

template <class... _UTypes>
tuple(_UTypes...) -> tuple<_UTypes...>;

template<class _T1, class _T2>
tuple(pair<_T1, _T2>) -> tuple<_T1, _T2>;

template <class _Alloc, class... _UTypes>
tuple(allocator_arg_t, _Alloc, _UTypes...) -> tuple<_UTypes...>;

template <class _Alloc, class _T1, class _T2>
tuple(allocator_arg_t, _Alloc, pair<_T1, _T2>) -> tuple<_T1, _T2>;

template <class _Alloc, class... _UTypes>
tuple(allocator_arg_t, _Alloc, tuple<_UTypes...>) -> tuple<_UTypes...>;


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
    static_assert(tuple_size_v<__XVI_STD_NS::remove_reference_t<_Tuple>> != 1
        || !__XVI_STD_NS::reference_constructs_from_temporary_v<_T, decltype(get<0>(declval<_Tuple>()))>);

    return __detail::__make_from_tuple_helper<_T>(__XVI_STD_NS::forward<_Tuple>(__t),
                                                  make_index_sequence<tuple_size<remove_reference_t<_Tuple>>::value>{});
}


template <class... _TTypes, class... _UTypes>
constexpr bool operator==(const tuple<_TTypes...>& __x, const tuple<_UTypes...>& __y)
{
    return __detail::__tuple_equality(__x, __y);
}

template <class... _TTypes, class... _UTypes>
constexpr __XVI_STD_NS::common_comparison_category_t<__detail::__synth_three_way_result<_TTypes, _UTypes>...>
operator<=>(const tuple<_TTypes...>& __t, const tuple<_UTypes...>& __u)
{
    if constexpr (sizeof...(_TTypes) == 0 && sizeof...(_UTypes) == 0)
        return __XVI_STD_NS::strong_ordering::equal;
    else
        return __detail::__tuple_compare(__t, __u);
}


template <class, class> struct uses_allocator;

template <class... _Types, class _Alloc>
struct uses_allocator<tuple<_Types...>, _Alloc> : true_type {};


template <class... _Types>
constexpr void swap(tuple<_Types...>& __x, tuple<_Types...>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class... _Types>
constexpr void swap(const tuple<_Types...>& __x, const tuple<_Types...>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_TUPLE_H */
