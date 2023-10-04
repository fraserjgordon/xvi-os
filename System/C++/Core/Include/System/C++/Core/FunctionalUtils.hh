#ifndef __SYSTEM_CXX_CORE_FUNCTIONALUTILS_H
#define __SYSTEM_CXX_CORE_FUNCTIONALUTILS_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/Invoke.hh>


namespace __XVI_STD_CORE_NS_DECL
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
        -> decltype(!__XVI_STD_NS::forward<_T>(__t))
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
    constexpr _T operator()(const _T& __x) const
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


namespace ranges
{


namespace __detail
{

template <class _T, class _U>
concept _BUILTIN_PTR_CMP = 
    is_pointer_v<remove_cvref_t<_T>> 
    && same_as<remove_cvref_t<_T>, remove_cvref_t<_U>>;

} // namespace __detail


struct equal_to
{
    template <class _T, class _U>
        requires equality_comparable_with<_T, _U> || __detail::_BUILTIN_PTR_CMP<_T, _U>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return __XVI_STD_NS::forward<_T>(__t) == __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

struct not_equal_to
{
    template <class _T, class _U>
        requires equality_comparable_with<_T, _U> || __detail::_BUILTIN_PTR_CMP<_T, _U>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return !ranges::equal_to{}(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_U>(__u));
    }

    using is_transparent = void;
};

struct less
{
    template <class _T, class _U>
        requires totally_ordered_with<_T, _U> || __detail::_BUILTIN_PTR_CMP<_T, _U>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return __XVI_STD_NS::forward<_T>(__t) < __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

struct greater
{
    template <class _T, class _U>
        requires totally_ordered_with<_T, _U> || __detail::_BUILTIN_PTR_CMP<_T, _U>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return ranges::less{}(__XVI_STD_NS::forward<_U>(__u), __XVI_STD_NS::forward<_T>(__t));
    }

    using is_transparent = void;
};

struct greater_equal
{
    template <class _T, class _U>
        requires totally_ordered_with<_T, _U> || __detail::_BUILTIN_PTR_CMP<_T, _U>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return !ranges::less{}(__XVI_STD_NS::forward<_T>(__t), __XVI_STD_NS::forward<_U>(__u));
    }

    using is_transparent = void;
};

struct less_equal
{
    template <class _T, class _U>
        requires totally_ordered_with<_T, _U> || __detail::_BUILTIN_PTR_CMP<_T, _U>
    constexpr bool operator()(_T&& __t, _U&& __u) const
    {
        return !ranges::less{}(__XVI_STD_NS::forward<_U>(__u), __XVI_STD_NS::forward<_T>(__t));
    }

    using is_transparent = void;
};

//! @TODO: not specified in the standard.
struct __compare_3way
{
    template <class _T, class _U>
    constexpr auto operator()(_T&& __t, _U&& __u) const
    {
        return __XVI_STD_NS::forward<_T>(__t) <=> __XVI_STD_NS::forward<_U>(__u);
    }
};

} // namespace ranges


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_FUNCTIONALUTILS_H */
