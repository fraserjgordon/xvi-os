#ifndef __SYSTEM_CXX_CORE_SWAP_H
#define __SYSTEM_CXX_CORE_SWAP_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/Concepts.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


template <class _T>
    requires __detail::is_move_constructible<_T>::value && __detail::is_move_assignable<_T>::value
constexpr void swap(_T& __a, _T& __b)
    noexcept(__detail::is_nothrow_move_constructible<_T>::value && __detail::is_nothrow_move_assignable<_T>::value)
{
    _T __tmp = std::move(__a);
    __a = std::move(__b);
    __b = std::move(__tmp);
}


template <class _T, class _U = _T>
constexpr _T exchange(_T& __obj, _U&& __new_val)
{
    _T __old_val = std::move(__obj);
    __obj = std::forward<_U>(__new_val);
    return __old_val;
}


namespace ranges
{


namespace __detail
{

struct __swap;

template <class _T> void swap(_T&, _T&) = delete;

template <class _T>
concept __enumeration_or_class = is_enum_v<remove_reference_t<_T>> || is_class_v<remove_reference_t<_T>>;

template <class _T>
concept __array_lvalue = is_lvalue_reference_v<_T> && is_array_v<remove_reference_t<_T>>;

template <class _E1, class _E2>
concept __swap_alt1 = ((__enumeration_or_class<_E1> || __enumeration_or_class<_E2>) && requires (_E1&& __e1, _E2&& __e2) { (void)swap(__XVI_STD_NS::forward<_E1>(__e1), __XVI_STD_NS::forward<_E2>(__e2)); });

template <class _E1, class _E2>
concept __swap_alt2 = !__swap_alt1<_E1, _E2>
    && __array_lvalue<_E1>
    && __array_lvalue<_E2>
    && extent_v<remove_reference_t<_E1>> == extent_v<remove_reference_t<_E2>>
    && requires(_E1&& __e1, _E2&& __e2) { declval<ranges::__detail::__swap>()(*__e1, *__e2); };

template <class _E1, class _E2>
concept __swap_alt3 = !__swap_alt1<_E1, _E2> && !__swap_alt2<_E1, _E2>
    && is_lvalue_reference_v<_E1>
    && is_lvalue_reference_v<_E2>
    && is_same_v<remove_reference_t<_E1>, remove_reference_t<_E2>>
    && move_constructible<remove_reference_t<_E1>>
    && assignable_from<remove_reference_t<_E1>&, remove_reference_t<_E1>>;

struct __swap
{
    // There's a circular dependency involving ranges::swap, ranges::swap_ranges and various iterator & range concepts
    // so the implementation of the call operators on this struct ends up being a bit strange.

    template <class _E1, class _E2>
        requires __swap_alt1<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const noexcept(noexcept((void)swap(__e1, __e2)))
    {
        (void)swap(__XVI_STD_NS::forward<_E1>(__e1), __XVI_STD_NS::forward<_E2>(__e2));
    }

    // Can't be defined until after ranges::swap_ranges is available.
    template <class _E1, class _E2>
        requires __swap_alt2<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const noexcept(noexcept(declval<const __swap&>()(*__XVI_STD_NS::forward<_E1>(__e1), *__XVI_STD_NS::forward<_E2>(__e2))));

    template <class _E1, class _E2>
        requires __swap_alt3<_E1, _E2>
    constexpr void operator()(_E1&& __e1, _E2&& __e2) const noexcept(is_nothrow_move_constructible_v<remove_reference_t<_E1>> && is_nothrow_move_assignable_v<remove_reference_t<_E1>>)
    {
        using _T = remove_reference_t<_E1>;

        _T __temp(__XVI_STD_NS::move(__e1));
        __e1 = __XVI_STD_NS::move(__e2);
        __e2 = __XVI_STD_NS::move(__temp);
    }
};

} // namespace __detail

inline namespace __swap { inline constexpr __detail::__swap swap = {}; }

// Macro allows the missing ranges::swap method's code to be in this file with the other methods.
//
// The IteratorUtils.hh header emits it once all the related iterator and range machinery is defined.
#define __XVI_STD_CORE_SWAP_RANGES_IMPL \
template <class _E1, class _E2> \
    requires __swap_alt2<_E1, _E2> \
constexpr void __swap::operator()(_E1&& __e1, _E2&& __e2) const noexcept(noexcept(declval<const __swap&>()(*__XVI_STD_NS::forward<_E1>(__e1), *__XVI_STD_NS::forward<_E2>(__e2)))) \
{ \
    (void)ranges::swap_ranges(__XVI_STD_NS::forward<_E1>(__e1), __XVI_STD_NS::forward<_E2>(__e2)); \
}


} // namespace ranges


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_SWAP_H */