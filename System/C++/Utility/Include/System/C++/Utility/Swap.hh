#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SWAP_H
#define __SYSTEM_CXX_UTILITY_SWAP_H


#include <System/C++/LanguageSupport/Utility.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Note the extra class - comes from an enable_if in the TypeTraits lib. Noexcept conditions are specified there too.
template <class _T, class>
constexpr void swap(_T& __a, _T& __b)
    noexcept(__detail::is_nothrow_move_constructible<_T>::value && __detail::is_nothrow_move_assignable<_T>::value)
{
    _T __tmp = __XVI_STD_NS::move(__a);
    __a = __XVI_STD_NS::move(__b);
    __b = __XVI_STD_NS::move(__tmp);
}

template <class _T, class _U = _T>
constexpr _T exchange(_T& __obj, _U&& __new_val)
{
    _T __old_val = __XVI_STD_NS::move(__obj);
    __obj = std::forward<_U>(__new_val);
    return __old_val;
}


namespace ranges
{


namespace __detail
{


// Forward declarations.
struct __swap;
struct __swap_ranges;


template <class _T> void swap(_T&, _T&) = delete;


template <class _T>
concept __enumeration_or_class = std::is_enum_v<std::remove_reference_t<_T>> || std::is_class_v<std::remove_reference_t<_T>>;

template <class _T>
concept __array_lvalue = std::is_lvalue_reference_v<_T> && std::is_array_v<std::remove_reference_t<_T>>;

template <class _E1, class _E2>
concept __swap_alt1 = ((__enumeration_or_class<_E1> || __enumeration_or_class<_E2>) && requires (_E1&& __e1, _E2&& __e2) { (void)swap(std::forward<_E1>(__e1), std::forward<_E2>(__e2)); });

template <class _E1, class _E2>
concept __swap_alt2 = !__swap_alt1<_E1, _E2>
    && __array_lvalue<_E1>
    && __array_lvalue<_E2>
    && std::extent_v<std::remove_reference_t<_E1>> == std::extent_v<std::remove_reference_t<_E2>>
    && requires(_E1&& __e1, _E2&& __e2) { declval<ranges::__detail::__swap>()(*__e1, *__e2); };

template <class _E1, class _E2>
concept __swap_alt3 = !__swap_alt1<_E1, _E2> && !__swap_alt2<_E1, _E2>
    && std::is_lvalue_reference_v<_E1>
    && std::is_lvalue_reference_v<_E2>
    && std::is_same_v<std::remove_reference_t<_E1>, std::remove_reference_t<_E2>>
    && move_constructible<std::remove_reference_t<_E1>>
    && assignable_from<std::remove_reference_t<_E1>&, std::remove_reference_t<_E1>>;

struct __swap
{
    template <class _E1, class _E2>
        requires __swap_alt1<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const noexcept(noexcept((void)swap(__e1, __e2)))
    {
        (void)swap(std::forward<_E1>(__e1), std::forward<_E2>(__e2));
    }

    // Defined in RangesAlgorithm.hh after a definition of __swap_ranges is available.
    template <class _E1, class _E2>
        requires __swap_alt2<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const noexcept(noexcept(declval<ranges::__detail::__swap>()(std::forward<_E1>(__e1), std::forward<_E2>(__e2))));

    template <class _E1, class _E2>
        requires __swap_alt3<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const noexcept(std::is_nothrow_move_constructible_v<std::remove_reference_t<_E1>> && std::is_nothrow_move_assignable_v<std::remove_reference_t<_E1>>)
    {
        using _T = std::remove_reference_t<_E1>;

        _T __temp(std::move(__e1));
        __e1 = std::move(__e2);
        __e2 = std::move(__temp);
    }
};


} // namespace __detail


inline namespace __swap
{

inline constexpr __detail::__swap swap = {};

} // inline namespace __swap


} // namespace ranges


static_assert(swappable<int>);


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SWAP_H */
