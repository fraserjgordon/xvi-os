#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_COMPARE_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_COMPARE_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


namespace __detail
{

enum class __eq { __equal = 0, __equivalent = __equal, __nonequal = 1, __nonequivalent = __nonequal };
enum class __ord { __less = -1, __greater = 1 };
enum class __ncmp { __unordered = -127 };

} // namespace __detail


class weak_equality
{
public:

    static const weak_equality equivalent;
    static const weak_equality nonequivalent;

    friend constexpr bool operator==(weak_equality __v, nullptr_t) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(weak_equality __v, nullptr_t) noexcept
        { return __v._M_value != 0; }
    friend constexpr bool operator==(nullptr_t, weak_equality __v) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(nullptr_t, weak_equality __v) noexcept
        { return __v._M_value != 0; }
    /*friend constexpr weak_equality operator<=>(weak_equality __v, nullptr_t) noexcept
        { return __v; }
    friend constexpr weak_equality operator<=>(nullptr_t, weak_equality __v) noexcept
        { return __v; }*/

private:

    int _M_value;

    constexpr explicit weak_equality(__detail::__eq __v) noexcept : _M_value(int(__v)) {}
};

inline constexpr weak_equality weak_equality::equivalent(__detail::__eq::__equivalent);
inline constexpr weak_equality weak_equality::nonequivalent(__detail::__eq::__nonequivalent);

class strong_equality
{
public:

    static const strong_equality equal;
    static const strong_equality nonequal;
    static const strong_equality equivalent;
    static const strong_equality nonequivalent;

    constexpr operator weak_equality() noexcept
        { return _M_value == 0 ? weak_equality::equivalent : weak_equality::nonequivalent; }

    friend constexpr bool operator==(strong_equality __v, nullptr_t) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(strong_equality __v, nullptr_t) noexcept
        { return __v._M_value != 0; }
    friend constexpr bool operator==(nullptr_t, strong_equality __v) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(nullptr_t, strong_equality __v) noexcept
        { return __v._M_value != 0; }
    /*friend constexpr strong_equality operator<=>(strong_equality __v, nullptr_t) noexcept
        { return __v; }
    friend constexpr strong_equality operator<=>(nullptr_t, strong_equality __v) noexcept
        { return __v; }*/

private:

    int _M_value;

    constexpr explicit strong_equality(__detail::__eq __v) noexcept : _M_value(int(__v)) {}
};

inline constexpr strong_equality strong_equality::equal(__detail::__eq::__equal);
inline constexpr strong_equality strong_equality::nonequal(__detail::__eq::__nonequal);
inline constexpr strong_equality strong_equality::equivalent(__detail::__eq::__equivalent);
inline constexpr strong_equality strong_equality::nonequivalent(__detail::__eq::__nonequivalent);

class partial_ordering
{
public:

    static const partial_ordering less;
    static const partial_ordering equivalent;
    static const partial_ordering greater;
    static const partial_ordering unordered;

    constexpr operator weak_equality() const noexcept
        { return _M_value == 0 ? weak_equality::equivalent : weak_equality::nonequivalent; }

    friend constexpr bool operator==(partial_ordering __v, nullptr_t) noexcept
        { return __v._M_is_ordered && __v._M_value == 0; }
    friend constexpr bool operator!=(partial_ordering __v, nullptr_t) noexcept
        { return !__v._M_is_ordered || __v._M_value != 0; }
    friend constexpr bool operator< (partial_ordering __v, nullptr_t) noexcept
        { return __v._M_is_ordered && __v._M_value < 0; }
    friend constexpr bool operator> (partial_ordering __v, nullptr_t) noexcept
        { return __v._M_is_ordered && __v._M_value > 0; }
    friend constexpr bool operator<=(partial_ordering __v, nullptr_t) noexcept
        { return __v._M_is_ordered && __v._M_value <= 0; }
    friend constexpr bool operator>=(partial_ordering __v, nullptr_t) noexcept
        { return __v._M_is_ordered && __v._M_value >= 0; }
    friend constexpr bool operator==(nullptr_t, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 == __v._M_value; }
    friend constexpr bool operator!=(nullptr_t, partial_ordering __v) noexcept
        { return !__v._M_is_ordered || 0 != __v._M_value; }
    friend constexpr bool operator< (nullptr_t, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 < __v._M_value; }
    friend constexpr bool operator> (nullptr_t, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 > __v._M_value; }
    friend constexpr bool operator<=(nullptr_t, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 <= __v._M_value; }
    friend constexpr bool operator>=(nullptr_t, partial_ordering __v) noexcept
        { return __v._M_is_ordered && 0 >= __v._M_value; };
    /*friend constexpr partial_ordering operator<=>(partial_ordering __v, nullptr_t) noexcept
        { return __v; }
    friend constexpr partial_ordering operator<=>(nullptr_t, partial_ordering __v) noexcept
        { return __v < 0 ? partial_ordering::greater : __v > 0 ? partial_ordering::less : __v; }*/

private:

    int _M_value;
    bool _M_is_ordered;

    constexpr explicit partial_ordering(__detail::__eq __v) noexcept : _M_value(int(__v)), _M_is_ordered(true) {}
    constexpr explicit partial_ordering(__detail::__ord __v) noexcept : _M_value(int(__v)), _M_is_ordered(true) {}
    constexpr explicit partial_ordering(__detail::__ncmp __v) noexcept : _M_value(int(__v)), _M_is_ordered(false) {}
};

inline constexpr partial_ordering partial_ordering::less(__detail::__ord::__less);
inline constexpr partial_ordering partial_ordering::equivalent(__detail::__eq::__equivalent);
inline constexpr partial_ordering partial_ordering::greater(__detail::__ord::__greater);
inline constexpr partial_ordering partial_ordering::unordered(__detail::__ncmp::__unordered);

class weak_ordering
{
public:

    static const weak_ordering less;
    static const weak_ordering equivalent;
    static const weak_ordering greater;

    constexpr operator weak_equality() const noexcept
        { return _M_value == 0 ? weak_equality::equivalent : weak_equality::nonequivalent; }
    constexpr operator partial_ordering() const noexcept
    { 
        return _M_value == 0 ? partial_ordering::equivalent :
               _M_value <  0 ? partial_ordering::less : 
                             partial_ordering::greater;
    }

    friend constexpr bool operator==(weak_ordering __v, nullptr_t) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(weak_ordering __v, nullptr_t) noexcept
        { return __v._M_value != 0; }
    friend constexpr bool operator< (weak_ordering __v, nullptr_t) noexcept
        { return __v._M_value < 0; }
    friend constexpr bool operator> (weak_ordering __v, nullptr_t) noexcept
        { return __v._M_value > 0; }
    friend constexpr bool operator<=(weak_ordering __v, nullptr_t) noexcept
        { return __v._M_value <= 0; }
    friend constexpr bool operator>=(weak_ordering __v, nullptr_t) noexcept
        { return __v._M_value >= 0; }
    friend constexpr bool operator==(nullptr_t, weak_ordering __v) noexcept
        { return 0 == __v._M_value; }
    friend constexpr bool operator!=(nullptr_t, weak_ordering __v) noexcept
        { return 0 != __v._M_value; }
    friend constexpr bool operator< (nullptr_t, weak_ordering __v) noexcept
        { return 0 < __v._M_value; }
    friend constexpr bool operator> (nullptr_t, weak_ordering __v) noexcept
        { return 0 > __v._M_value; }
    friend constexpr bool operator<=(nullptr_t, weak_ordering __v) noexcept
        { return 0 <= __v._M_value; }
    friend constexpr bool operator>=(nullptr_t, weak_ordering __v) noexcept
        { return 0 >= __v._M_value; }
    /*friend constexpr weak_ordering operator<=>(weak_ordering __v, nullptr_t) noexcept
        { return __v; }
    friend constexpr weak_ordering operator<=>(nullptr_t, weak_ordering __v) noexcept
        { return __v < 0 ? weak_ordering::greater : __v > 0 ? weak_ordering::less : __v; }*/

private:

    int _M_value;

    constexpr explicit weak_ordering(__detail::__eq __v) noexcept : _M_value(int(__v)) {}
    constexpr explicit weak_ordering(__detail::__ord __v) noexcept : _M_value(int(__v)) {}
};

inline constexpr weak_ordering weak_ordering::less(__detail::__ord::__less);
inline constexpr weak_ordering weak_ordering::equivalent(__detail::__eq::__equivalent);
inline constexpr weak_ordering weak_ordering::greater(__detail::__ord::__greater);

class strong_ordering
{
public:

    static const strong_ordering less;
    static const strong_ordering equal;
    static const strong_ordering equivalent;
    static const strong_ordering greater;

    constexpr operator weak_equality() const noexcept
        { return _M_value == 0 ? weak_equality::equivalent : weak_equality::nonequivalent; }
    constexpr operator strong_equality() const noexcept
        { return _M_value == 0 ? strong_equality::equal : strong_equality::nonequal; }
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

    friend constexpr bool operator==(strong_ordering __v, nullptr_t) noexcept
        { return __v._M_value == 0; }
    friend constexpr bool operator!=(strong_ordering __v, nullptr_t) noexcept
        { return __v._M_value != 0; }
    friend constexpr bool operator< (strong_ordering __v, nullptr_t) noexcept
        { return __v._M_value < 0; }
    friend constexpr bool operator> (strong_ordering __v, nullptr_t) noexcept
        { return __v._M_value > 0; }
    friend constexpr bool operator<=(strong_ordering __v, nullptr_t) noexcept
        { return __v._M_value <= 0; }
    friend constexpr bool operator>=(strong_ordering __v, nullptr_t) noexcept
        { return __v._M_value >= 0; }
    friend constexpr bool operator==(nullptr_t, strong_ordering __v) noexcept
        { return 0 == __v._M_value; }
    friend constexpr bool operator!=(nullptr_t, strong_ordering __v) noexcept
        { return 0 != __v._M_value; }
    friend constexpr bool operator< (nullptr_t, strong_ordering __v) noexcept
        { return 0 < __v._M_value; }
    friend constexpr bool operator> (nullptr_t, strong_ordering __v) noexcept
        { return 0 > __v._M_value; }
    friend constexpr bool operator<=(nullptr_t, strong_ordering __v) noexcept
        { return 0 <= __v._M_value; }
    friend constexpr bool operator>=(nullptr_t, strong_ordering __v) noexcept
        { return 0 >= __v._M_value; }
    /*friend constexpr strong_ordering operator<=>(strong_ordering __v, nullptr_t) noexcept
        { return __v; }
    friend constexpr strong_ordering operator<=>(nullptr_t, strong_ordering __v) noexcept
        { return __v < 0 ? strong_ordering::greater : __v > 0 ? strong_ordering::less : __v; }*/


private:

    int _M_value;

    constexpr explicit strong_ordering(__detail::__eq __v) : _M_value(int(__v)) {}
    constexpr explicit strong_ordering(__detail::__ord __v) : _M_value(int(__v)) {}
};


constexpr bool is_eq(weak_equality __cmp) noexcept
    { return __cmp == 0; }
constexpr bool is_neq(weak_equality __cmp) noexcept
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
template <> struct __is_comparison_category<weak_equality> : true_type {};
template <> struct __is_comparison_category<strong_equality> : true_type {};
template <> struct __is_comparison_category<partial_ordering> : true_type {};
template <> struct __is_comparison_category<weak_ordering> : true_type {};
template <> struct __is_comparison_category<strong_ordering> : true_type {};

template <class _T, class... _Ts> struct __type_in_pack : bool_constant<(is_same_v<_T, _Ts> || ...)> {};

template <class... _Ts>
struct __common_comparison_category
{
    static constexpr bool __other = (!__is_comparison_category<_Ts>::value || ...);
    static constexpr bool __weq = __type_in_pack<weak_equality, _Ts...>::value;
    static constexpr bool __seq = __type_in_pack<strong_equality, _Ts...>::value;
    static constexpr bool __pord = __type_in_pack<partial_ordering, _Ts...>::value;
    static constexpr bool __word = __type_in_pack<weak_ordering, _Ts...>::value;
    static constexpr bool __sord = __type_in_pack<strong_ordering, _Ts...>::value;

    using __type = conditional_t<__other, void,
                   conditional_t<__weq || (__seq && (__pord || __word)), weak_equality,
                   conditional_t<__seq, strong_equality,
                   conditional_t<__pord, partial_ordering,
                   conditional_t<__word, weak_ordering, strong_ordering>>>>>;
};

} // namespace __detail


template <class... _Ts> struct common_comparison_category
    { using type = typename __detail::__common_comparison_category<_Ts...>::__type; };

template <class... _Ts> using common_comparison_category_t = typename common_comparison_category<_Ts...>::type;


template <class _T> constexpr strong_ordering strong_order(const _T&, const _T&);
template <class _T> constexpr weak_ordering weak_order(const _T&, const _T&);
template <class _T> constexpr partial_ordering partial_order(const _T&, const _T&);
template <class _T> constexpr strong_equality strong_equal(const _T&, const _T&);
template <class _T> constexpr weak_equality weak_equal(const _T&, const _T&);


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_COMPARE_H */
