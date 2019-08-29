#pragma once
#ifndef __SYSTEM_CXX_UTILITY_FUNCTIONALUTILS_H
#define __SYSTEM_CXX_UTILITY_FUNCTIONALUTILS_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Utility/Invoke.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _T = void> struct plus
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x + __y;
    }
};

template <class _T = void> struct minus
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x - __y;
    }
};

template <class _T = void> struct multiplies
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x * __y;
    }
};

template <class _T = void> struct divides
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x / __y;
    }
};

template <class _T = void> struct modulus
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x % __y;
    }
};

template <class _T = void> struct negate
{
    constexpr _T operator()(const _T& __x) const
    {
        return -__x;
    }
};


template <> struct plus<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u) const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) + __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) + __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct minus<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u) const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) - __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) - __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct multiplies<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u) const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) * __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) * __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct divides<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u) const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) / __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) / __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct modulus<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u) const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) % __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) % __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct negate<void>
{
    template <class _T>
    constexpr auto operator()(_T&& __t) const
        -> decltype(-__XVI_STD_NS::forward<_T>(__t))
    {
        return -__XVI_STD_NS::forward<_T>(__t);
    }

    using is_transparent = void;
};


template <class _T = void> struct equal_to
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x == __y;
    }
};

template <class _T = void> struct not_equal_to
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x != __y;
    }
};

template <class _T = void> struct greater
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x > __y;
    }
};

template <class _T = void> struct less
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x < __y;
    }
};

template <class _T = void> struct greater_equal
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x >= __y;
    }
};

template <class _T = void> struct less_equal
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x <= __y;
    }
};

template <> struct equal_to<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) == __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) == __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct not_equal_to<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) != __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) != __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct greater<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) > __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) > __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct less<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) < __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) < __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct greater_equal<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) >= __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) >= __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct less_equal<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) <= __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) <= __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};


template <class _T = void> struct logical_and
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x && __y;
    }
};

template <class _T = void> struct logical_or
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x || __y;
    }
};

template <class _T = void> struct logical_not
{
    constexpr bool operator()(const _T& __x) const
    {
        return !__x;
    }
};

template <> struct logical_and<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) && __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) && __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct logical_or<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) || __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) || __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct logical_not<void>
{
    template <class _T>
    constexpr auto operator()(_T&& __t)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t))
    {
        return !__XVI_STD_NS::forward<_T>(__t);
    }

    using is_transparent = void;
};


template <class _T = void> struct bit_and
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x & __y;
    }
};

template <class _T = void> struct bit_or
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x | __y;
    }
};

template <class _T = void> struct bit_xor
{
    constexpr _T operator()(const _T& __x, const _T& __y) const
    {
        return __x ^ __y;
    }
};

template <class _T = void> struct bit_not
{
    constexpr bool operator()(const _T& __x) const
    {
        return ~__x;
    }
};


template <> struct bit_and<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) & __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) & __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct bit_or<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) | __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) | __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct bit_xor<void>
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u)  const
        -> decltype(__XVI_STD_NS::forward<_T>(__t) ^ __XVI_STD_NS::forward<_U>(__u))
    {
        return __XVI_STD_NS::forward<_T>(__t) ^ __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct bit_not<void>
{
    template <class _T>
    constexpr auto operator()(_T&& __t)  const
        -> decltype(~__XVI_STD_NS::forward<_T>(__t))
    {
        return ~__XVI_STD_NS::forward<_T>(__t);
    }

    using is_transparent = void;
};


struct identity
{
    template <class _T>
    constexpr _T&& operator()(_T&& __t) const noexcept
    {
        return __XVI_STD_NS::forward<_T>(__t);
    }

    using is_transparent = void;
};


namespace __detail
{


template <class _T, class _U>
concept bool _BUILTIN_PTR_CMP = is_pointer_v<remove_cvref_t<_T>> && is_pointer_v<remove_cvref_t<_U>>;


} // namespace __detail


namespace ranges
{

template <class _T = void>
    requires EqualityComparable<_T> || Same<_T, void> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
struct equal_to
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x == __y;
    }
};

template <class _T = void>
    requires EqualityComparable<_T> || Same<_T, void> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
struct not_equal_to
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return !(__x == __y);
    }
};

template <class _T = void>
    requires StrictTotallyOrdered<_T> || Same<_T, void> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
struct greater
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __y < __x;
    }
};

template <class _T = void>
    requires StrictTotallyOrdered<_T> || Same<_T, void> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
struct less
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return __x < __y;
    }
};

template <class _T = void>
    requires StrictTotallyOrdered<_T> || Same<_T, void> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
struct greater_equal
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return !(__x < __y);
    }
};

template <class _T = void>
    requires StrictTotallyOrdered<_T> || Same<_T, void> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
struct less_equal
{
    constexpr bool operator()(const _T& __x, const _T& __y) const
    {
        return !(__y < __x);
    }
};


template <> struct equal_to<void>
{
    template <class _T, class _U>
        requires EqualityComparableWith<_T, _U> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return __XVI_STD_NS::forward<_T>(__t) == __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct not_equal_to<void>
{
    template <class _T, class _U>
        requires EqualityComparableWith<_T, _U> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return !(__XVI_STD_NS::forward<_T>(__t) == __XVI_STD_NS::forward<_U>(__u));
    }

    using is_transparent = void;
};

template <> struct greater<void>
{
    template <class _T, class _U>
        requires StrictTotallyOrderedWith<_T, _U> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return __XVI_STD_NS::forward<_U>(__u) < __XVI_STD_NS::forward<_T>(__t);
    }

    using is_transparent = void;
};

template <> struct less<void>
{
    template <class _T, class _U>
        requires StrictTotallyOrderedWith<_T, _U> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return __XVI_STD_NS::forward<_T>(__t) < __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

template <> struct greater_equal<void>
{
    template <class _T, class _U>
        requires StrictTotallyOrderedWith<_T, _U> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return !(__XVI_STD_NS::forward<_T>(__t) < __XVI_STD_NS::forward<_U>(__u));
    }

    using is_transparent = void;
};

template <> struct less_equal<void>
{
    template <class _T, class _U>
        requires StrictTotallyOrderedWith<_T, _U> || __detail::_BUILTIN_PTR_CMP<const _T&, const _T&>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return !(__XVI_STD_NS::forward<_U>(__u) < __XVI_STD_NS::forward<_T>(__t));
    }

    using is_transparent = void;
};

} // namespace ranges


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_FUNCTIONALUTILS_H */
