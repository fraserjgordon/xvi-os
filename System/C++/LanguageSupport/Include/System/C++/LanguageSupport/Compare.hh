#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_COMPARE_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_COMPARE_H


#include <System/C++/TypeTraits/Concepts.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/Private/Namespace.hh>

// Needed for nteroperability with std types when hosted.
#if defined(__XVI_HOSTED)
#  include <compare>
#endif


namespace __XVI_STD_LANGSUPPORT_NS_DECL
{


namespace __detail
{

enum class __ord { __equal = 0, __equivalent = __equal, __less = -1, __greater = 1 };
enum class __ncmp { __unordered = -127 };

} // namespace __detail


class partial_ordering
{
public:

    static const partial_ordering less;
    static const partial_ordering equivalent;
    static const partial_ordering greater;
    static const partial_ordering unordered;

#if defined(__XVI_HOSTED)
    constexpr partial_ordering(std::partial_ordering __o) :
        _M_value((__o == std::partial_ordering::unordered)  ? -127 :
                 (__o == std::partial_ordering::equivalent) ?  0   :
                 (__o == std::partial_ordering::less)       ? -1   : 1),
        _M_is_ordered(__o != std::partial_ordering::unordered)
        {}
#endif

    friend constexpr bool operator==(partial_ordering, partial_ordering) noexcept = default;

    friend constexpr bool operator==(partial_ordering __v, int) noexcept
        { return __v._M_is_ordered && __v._M_value == 0; }
    friend constexpr bool operator!=(partial_ordering __v, int) noexcept
        { return !__v._M_is_ordered || __v._M_value != 0; }
    friend constexpr bool operator< (partial_ordering __v, int) noexcept
        { return __v._M_is_ordered && __v._M_value < 0; }
    friend constexpr bool operator> (partial_ordering __v, int) noexcept
        { return __v._M_is_ordered && __v._M_value > 0; }
    friend constexpr bool operator<=(partial_ordering __v, int) noexcept
        { return __v._M_is_ordered && __v._M_value <= 0; }
    friend constexpr bool operator>=(partial_ordering __v, int) noexcept
        { return __v._M_is_ordered && __v._M_value >= 0; }
    friend constexpr bool operator==(int, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 == __v._M_value; }
    friend constexpr bool operator!=(int, partial_ordering __v) noexcept
        { return !__v._M_is_ordered || 0 != __v._M_value; }
    friend constexpr bool operator< (int, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 < __v._M_value; }
    friend constexpr bool operator> (int, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 > __v._M_value; }
    friend constexpr bool operator<=(int, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 <= __v._M_value; }
    friend constexpr bool operator>=(int, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 >= __v._M_value; };

    friend constexpr partial_ordering operator<=>(partial_ordering __v, int) noexcept
        { return __v; }
    friend constexpr partial_ordering operator<=>(int, partial_ordering __v) noexcept
        { return __v < 0 ? partial_ordering::greater : __v > 0 ? partial_ordering::less : __v; }

private:

    int _M_value;
    bool _M_is_ordered;

    constexpr explicit partial_ordering(__detail::__ord __v) noexcept : _M_value(int(__v)), _M_is_ordered(true) {}
    constexpr explicit partial_ordering(__detail::__ncmp __v) noexcept : _M_value(int(__v)), _M_is_ordered(false) {}
};

inline constexpr partial_ordering partial_ordering::less(__detail::__ord::__less);
inline constexpr partial_ordering partial_ordering::equivalent(__detail::__ord::__equivalent);
inline constexpr partial_ordering partial_ordering::greater(__detail::__ord::__greater);
inline constexpr partial_ordering partial_ordering::unordered(__detail::__ncmp::__unordered);


class weak_ordering
{
public:

    static const weak_ordering less;
    static const weak_ordering equivalent;
    static const weak_ordering greater;

    constexpr operator partial_ordering() const noexcept
    { 
        return _M_value == 0 ? partial_ordering::equivalent :
               _M_value <  0 ? partial_ordering::less : 
                               partial_ordering::greater;
    }

#if defined(__XVI_HOSTED)
    constexpr weak_ordering(std::weak_ordering __o) :
        _M_value((__o == std::weak_ordering::equivalent) ? 0 : (__o == std::weak_ordering::less) ? -1 : 1) {}
#endif

    friend constexpr bool operator==(weak_ordering, weak_ordering) noexcept = default;

    friend constexpr bool operator==(weak_ordering __v, int) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(weak_ordering __v, int) noexcept
        { return __v._M_value != 0; }
    friend constexpr bool operator< (weak_ordering __v, int) noexcept
        { return __v._M_value < 0; }
    friend constexpr bool operator> (weak_ordering __v, int) noexcept
        { return __v._M_value > 0; }
    friend constexpr bool operator<=(weak_ordering __v, int) noexcept
        { return __v._M_value <= 0; }
    friend constexpr bool operator>=(weak_ordering __v, int) noexcept
        { return __v._M_value >= 0; }
    friend constexpr bool operator==(int, weak_ordering __v) noexcept
        { return 0 == __v._M_value; }
    friend constexpr bool operator!=(int, weak_ordering __v) noexcept
        { return 0 != __v._M_value; }
    friend constexpr bool operator< (int, weak_ordering __v) noexcept
        { return 0 < __v._M_value; }
    friend constexpr bool operator> (int, weak_ordering __v) noexcept
        { return 0 > __v._M_value; }
    friend constexpr bool operator<=(int, weak_ordering __v) noexcept
        { return 0 <= __v._M_value; }
    friend constexpr bool operator>=(int, weak_ordering __v) noexcept
        { return 0 >= __v._M_value; }

    friend constexpr weak_ordering operator<=>(weak_ordering __v, int) noexcept
        { return __v; }
    friend constexpr weak_ordering operator<=>(int, weak_ordering __v) noexcept
        { return __v < 0 ? weak_ordering::greater : __v > 0 ? weak_ordering::less : __v; }

private:

    int _M_value;

    constexpr explicit weak_ordering(__detail::__ord __v) noexcept : _M_value(int(__v)) {}
};

inline constexpr weak_ordering weak_ordering::less(__detail::__ord::__less);
inline constexpr weak_ordering weak_ordering::equivalent(__detail::__ord::__equivalent);
inline constexpr weak_ordering weak_ordering::greater(__detail::__ord::__greater);


class strong_ordering
{
public:

    static const strong_ordering less;
    static const strong_ordering equal;
    static const strong_ordering equivalent;
    static const strong_ordering greater;

    constexpr operator partial_ordering() const noexcept
    {
        return _M_value == 0 ? partial_ordering::equivalent :
               _M_value <  0 ? partial_ordering::less :
                               partial_ordering::greater;
    }

    constexpr operator weak_ordering() const noexcept
    {
        return _M_value == 0 ? weak_ordering::equivalent :
               _M_value <  0 ? weak_ordering::less :
                               weak_ordering::greater;
    }

#if __XVI_HOSTED
    constexpr strong_ordering(std::strong_ordering __o) :
        _M_value((__o == std::strong_ordering::equal) ? 0 : (__o == std::strong_ordering::less) ? -1 : 1) {}
#endif

    friend constexpr bool operator==(strong_ordering, strong_ordering) noexcept = default;

    friend constexpr bool operator==(strong_ordering __v, int) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(strong_ordering __v, int) noexcept
        { return __v._M_value != 0; }
    friend constexpr bool operator< (strong_ordering __v, int) noexcept
        { return __v._M_value < 0; }
    friend constexpr bool operator> (strong_ordering __v, int) noexcept
        { return __v._M_value > 0; }
    friend constexpr bool operator<=(strong_ordering __v, int) noexcept
        { return __v._M_value <= 0; }
    friend constexpr bool operator>=(strong_ordering __v, int) noexcept
        { return __v._M_value >= 0; }
    friend constexpr bool operator==(int, strong_ordering __v) noexcept
        { return 0 == __v._M_value; }
    friend constexpr bool operator!=(int, strong_ordering __v) noexcept
        { return 0 != __v._M_value; }
    friend constexpr bool operator< (int, strong_ordering __v) noexcept
        { return 0 < __v._M_value; }
    friend constexpr bool operator> (int, strong_ordering __v) noexcept
        { return 0 > __v._M_value; }
    friend constexpr bool operator<=(int, strong_ordering __v) noexcept
        { return 0 <= __v._M_value; }
    friend constexpr bool operator>=(int, strong_ordering __v) noexcept
        { return 0 >= __v._M_value; }

    friend constexpr strong_ordering operator<=>(strong_ordering __v, int) noexcept
        { return __v; }
    friend constexpr strong_ordering operator<=>(int, strong_ordering __v) noexcept
        { return __v < 0 ? strong_ordering::greater : __v > 0 ? strong_ordering::less : __v; }


private:

    int _M_value;

    constexpr explicit strong_ordering(__detail::__ord __v) : _M_value(int(__v)) {}
};

inline constexpr strong_ordering strong_ordering::less(__detail::__ord::__less);
inline constexpr strong_ordering strong_ordering::equal(__detail::__ord::__equal);
inline constexpr strong_ordering strong_ordering::equivalent(__detail::__ord::__equivalent);
inline constexpr strong_ordering strong_ordering::greater(__detail::__ord::__greater);


constexpr bool is_eq(partial_ordering __cmp) noexcept
    { return __cmp == 0; }
constexpr bool is_neq(partial_ordering __cmp) noexcept
    { return __cmp != 0; }
constexpr bool is_lt(partial_ordering __cmp) noexcept
    { return __cmp < 0; }
constexpr bool is_lteq(partial_ordering __cmp) noexcept
    { return __cmp <= 0; }
constexpr bool is_gt(partial_ordering __cmp) noexcept
    { return __cmp > 0; }
constexpr bool is_gteq(partial_ordering __cmp) noexcept
    { return __cmp >= 0; }


namespace __detail
{

template <class _T> struct __is_comparison_category : false_type {};
template <> struct __is_comparison_category<partial_ordering> : true_type {};
template <> struct __is_comparison_category<weak_ordering> : true_type {};
template <> struct __is_comparison_category<strong_ordering> : true_type {};

// Interoperability with std types when hosted.
#if defined(__XVI_HOSTED)
template <> struct __is_comparison_category<std::partial_ordering> : true_type {};
template <> struct __is_comparison_category<std::weak_ordering> : true_type {};
template <> struct __is_comparison_category<std::strong_ordering> : true_type {};
#endif

template <class _T, class... _Ts> struct __type_in_pack : bool_constant<(is_same_v<_T, _Ts> || ...)> {};

template <class... _Ts>
struct __common_comparison_category
{
    static constexpr bool __other = (!__is_comparison_category<_Ts>::value || ...);
    static constexpr bool __pord = __type_in_pack<partial_ordering, _Ts...>::value;
    static constexpr bool __word = __type_in_pack<weak_ordering, _Ts...>::value;

#if defined(__XVI_HOSTED)
    static constexpr bool __pord_std = __type_in_pack<std::partial_ordering, _Ts...>::value;
    static constexpr bool __word_std = __type_in_pack<std::weak_ordering, _Ts...>::value;

    using __type_std = conditional_t<__other, void,
                       conditional_t<__pord || __pord_std, partial_ordering,
                       conditional_t<__word || __word_std, weak_ordering, strong_ordering>>>;
#else
    using __type_std = void;
#endif

    // Note: defaults to strong_ordering when the pack is empty.
    using __type = conditional_t<!is_void_v<__type_std>, __type_std,
                   conditional_t<__other, void,
                   conditional_t<__pord, partial_ordering,
                   conditional_t<__word, weak_ordering, strong_ordering>>>>;
};

} // namespace __detail


template <class... _Ts> struct common_comparison_category
    { using type = typename __detail::__common_comparison_category<_Ts...>::__type; };

template <class... _Ts> using common_comparison_category_t = typename common_comparison_category<_Ts...>::type;


namespace __detail
{

template <class _T, class _Cat>
concept __compares_as = same_as<common_comparison_category_t<_T, _Cat>, _Cat>;

} // namespace __detail


template <class _T, class _Cat = partial_ordering>
concept three_way_comparable = 
       __detail::__weakly_equality_comparable_with<_T, _T>
    && __detail::__partially_ordered_with<_T, _T>
    && requires(const remove_reference_t<_T>& __a, const remove_reference_t<_T>& __b)
    {
        { __a <=> __b } -> __detail::__compares_as<_Cat>;
    };

template <class _T, class _U, class _Cat = partial_ordering>
concept three_way_comparable_with = 
       three_way_comparable<_T, _Cat>
    && three_way_comparable<_U, _Cat>
    && common_reference_with<const remove_reference_t<_T>&, const remove_reference_t<_U>&>
    && three_way_comparable<common_reference_t<const remove_reference_t<_T>&,
                                               const remove_reference_t<_U>&>,
                            _Cat>
    && requires(const remove_reference_t<_T>& __t, const remove_reference_t<_U>& __u)
    {
        { __t <=> __u } -> __detail::__compares_as<_Cat>;
        { __u <=> __t } -> __detail::__compares_as<_Cat>;
    };


namespace __detail
{

//! @TODO: implement
template <class _T, class _U>
constexpr bool __builtin_ptr_three_way = is_pointer_v<_T> && is_pointer_v<_U>;

} // namespace __detail


template <class _T, class _U = _T>
struct compare_three_way_result {};

template <class _T, class _U>
    requires requires(const remove_reference_t<_T>& __t, const remove_reference_t<_U>& __u) { __t <=> __u; }
struct compare_three_way_result<_T, _U>
{
    using type = decltype(declval<remove_reference_t<_T>&>() <=> declval<remove_reference_t<_U>&>());
};

template <class _T, class _U = _T>
using compare_three_way_result_t = typename compare_three_way_result<_T, _U>::type;

struct compare_three_way
{
    template <class _T, class _U>
        requires three_way_comparable_with<_T, _U> || __detail::__builtin_ptr_three_way<_T, _U>
    constexpr auto operator()(_T&& __t, _U&& __u) const
    {
        return __XVI_STD_NS::forward<_T>(__t) <=> __XVI_STD_NS::forward<_U>(__u);
    }

    using is_transparent = void;
};

inline namespace __compare
{

template <class _Ord, class _E, class _F>
concept __has_strong_order = requires(_E __e, _F __f)
{
    _Ord(strong_order(__e, __f));
};

template <class _Ord, class _E, class _F>
concept __has_weak_order = requires(_E __e, _F __f)
{
    _Ord(weak_order(__e, __f));
};

template <class _Ord, class _E, class _F>
concept __has_partial_order = requires(_E __e, _F __f)
{
    _Ord(partial_order(__e, __f));
};

template <class _E, class _F>
concept __has_eq_and_lt = requires(_E __e, _F __f)
{
    { __e == __f } -> convertible_to<bool>;
    { __e <  __f } -> convertible_to<bool>;
};

template <class _Ord, class _E, class _F>
concept __has_compare_three_way = requires(_E __e, _F __f)
{
    _Ord(compare_three_way()(__e, __f));
};

struct __strong_order
{
    template <class _E, class _F>
        requires (!same_as<decay_t<_E>, decay_t<_F>>)
    static constexpr void operator()(_E&&, _F&&) = delete;

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
         && __has_strong_order<strong_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return strong_ordering(strong_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_strong_order<strong_ordering, _E, _F>
        && __has_compare_three_way<strong_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return strong_ordering(compare_three_way()(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_strong_order<strong_ordering, _E, _F>
        && !__has_compare_three_way<strong_ordering, _E, _F>)
    static constexpr void operator()(_E&&, _F&&) = delete;
};

inline constexpr __strong_order strong_order = {};

struct __weak_order
{
    template <class _E, class _F>
        requires (!same_as<decay_t<_E>, decay_t<_F>>)
    static constexpr void operator()(_E&& __e, _F&& __f) = delete;

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && __has_weak_order<weak_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return weak_ordering(weak_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_weak_order<weak_ordering, _E, _F>
        && __has_compare_three_way<weak_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return weak_ordering(compare_three_way()(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_weak_order<weak_ordering, _E, _F>
        && !__has_compare_three_way<weak_ordering, _E, _F>
        && __has_strong_order<weak_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return weak_ordering(strong_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_weak_order<weak_ordering, _E, _F>
        && !__has_compare_three_way<weak_ordering, _E, _F>
        && !__has_strong_order<weak_ordering, _E, _F>)
    static constexpr void operator()(_E&& __e, _F&& __f) = delete;
};

inline constexpr __weak_order weak_order = {};

struct __partial_order
{
    template <class _E, class _F>
        requires (!same_as<decay_t<_E>, decay_t<_F>>)
    static constexpr void operator()(_E&& __e, _F&& __f) = delete;

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && __has_partial_order<partial_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return partial_ordering(partial_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_partial_order<partial_ordering, _E, _F>
        && __has_compare_three_way<partial_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return partial_ordering(compare_three_way()(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_partial_order<partial_ordering, _E, _F>
        && !__has_compare_three_way<partial_ordering, _E, _F>
        && __has_weak_order<partial_ordering, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return partial_ordering(weak_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f)));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_partial_order<partial_ordering, _E, _F>
        && !__has_compare_three_way<partial_ordering, _E, _F>
        && !__has_weak_order<partial_ordering, _E, _F>)
    static constexpr void operator()(_E&& __e, _F&& __f) = delete;
};

inline constexpr __partial_order partial_order = {};

struct __compare_strong_order_fallback
{
    template <class _E, class _F>
        requires (!same_as<decay_t<_E>, decay_t<_F>>)
    static constexpr void operator()(_E&& __E, _F&& __f) = delete;

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && __has_strong_order<void, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return strong_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_strong_order<void, _E, _F>
        && __has_eq_and_lt<_E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return (__e == __f) ? strong_ordering::equal :
               (__e <  __f) ? strong_ordering::less  :
                              strong_ordering::greater;
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_strong_order<void, _E, _F>
        && !__has_eq_and_lt<_E, _F>)
    static constexpr void operator()(_E&& __e, _F&& __f) = delete;
};

inline constexpr __compare_strong_order_fallback compare_strong_order_fallback = {};

struct __compare_weak_order_fallback
{
    template <class _E, class _F>
        requires (!same_as<decay_t<_E>, decay_t<_F>>)
    static constexpr void operator()(_E&& __E, _F&& __f) = delete;

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && __has_weak_order<void, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return weak_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_weak_order<void, _E, _F>
        && __has_eq_and_lt<_E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return (__e == __f) ? weak_ordering::equivalent :
               (__e <  __f) ? weak_ordering::less       :
                              weak_ordering::greater;
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_weak_order<void, _E, _F>
        && !__has_eq_and_lt<_E, _F>)
    static constexpr void operator()(_E&& __e, _F&& __f) = delete;
};

inline constexpr __compare_weak_order_fallback compare_weak_order_fallback = {};

struct __compare_partial_order_fallback
{
    template <class _E, class _F>
        requires (!same_as<decay_t<_E>, decay_t<_F>>)
    static constexpr void operator()(_E&& __E, _F&& __f) = delete;

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && __has_partial_order<void, _E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return partial_order(__XVI_STD_NS::forward<_E>(__e), __XVI_STD_NS::forward<_F>(__f));
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_partial_order<void, _E, _F>
        && __has_eq_and_lt<_E, _F>)
    static constexpr auto operator()(_E&& __e, _F&& __f)
    {
        return (__e == __f) ? partial_ordering::equivalent :
               (__e <  __f) ? partial_ordering::less       :
               (__f <  __e) ? partial_ordering::greater    :
                              partial_ordering::unordered;
    }

    template <class _E, class _F>
        requires (same_as<decay_t<_E>, decay_t<_F>>
        && !__has_partial_order<void, _E, _F>
        && !__has_eq_and_lt<_E, _F>)
    static constexpr void operator()(_E&& __e, _F&& __f) = delete;
};

inline constexpr __compare_partial_order_fallback compare_partial_order_fallback = {};

} // inline namespace __compare


namespace __detail
{

constexpr auto __synth_three_way = []<class _T, class _U>(const _T& __t, const _U& __u)
    requires requires
    {
        { __t < __u } -> __boolean_testable;
        { __u < __t } -> __boolean_testable;
    }
{
    if constexpr (three_way_comparable_with<_T, _U>)
    {
        // Force conversion from a std to __XVI_STD type when hosted.
        using _C = common_comparison_category_t<decltype(__t <=> __u)>;
        return _C(__t <=> __u);
    }
    else
    {
        if (__t < __u)
            return weak_ordering::less;
        else if (__u < __t)
            return weak_ordering::greater;
        else
            return weak_ordering::equivalent;
    }
};

template <class _T, class _U = _T>
using __synth_three_way_result = decltype(__synth_three_way(declval<_T&>(), declval<_U&>()));

} // namespace __detail


} // namespace __XVI_STD_LANGSUPPORT_NS_DECL


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_COMPARE_H */
