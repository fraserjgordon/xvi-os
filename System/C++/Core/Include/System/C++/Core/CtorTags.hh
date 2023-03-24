#ifndef __SYSTEM_CXX_CORE_INPLACE_H
#define __SYSTEM_CXX_CORE_INPLACE_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


struct allocator_arg_t { explicit allocator_arg_t() = default; };

inline constexpr allocator_arg_t allocator_arg {};


struct piecewise_construct_t
    { explicit piecewise_construct_t() = default; };
inline constexpr piecewise_construct_t piecewise_construct {};

struct in_place_t
    { explicit in_place_t() = default; };
inline constexpr in_place_t in_place {};

template <class _T> struct in_place_type_t
    { explicit in_place_type_t() = default; };
template <class _T> inline constexpr in_place_type_t<_T> in_place_type {};

template <size_t _I> struct in_place_index_t
    { explicit in_place_index_t() = default; };
template <size_t _I> inline constexpr in_place_index_t<_I> in_place_index {};


namespace __detail
{

template <class _T> void __implicit_default_construct_detect_fn(const _T&);
template <class _T> using __implicit_default_construct_detect = decltype(__implicit_default_construct_detect_fn<_T>({}));
template <class _T> struct __is_implicit_default_constructible : bool_constant<is_detected_v<__implicit_default_construct_detect, _T>> {};
template <class _T> inline constexpr bool __is_implicit_default_constructible_v = __is_implicit_default_constructible<_T>::value;

template <class _T> void __implicit_construct_detect_fn(const _T&);
template <class _T, class... _Args> using __implicit_construct_detect = decltype(__implicit_construct_detect_fn<_T>({declval<_Args>()...}));
template <class _T, class... _Args> struct __is_implicit_constructible : bool_constant<is_detected_v<__implicit_construct_detect, _T, _Args...>> {};
template <class _T, class... _Args> inline constexpr bool __is_implicit_constructible_v = __is_implicit_constructible<_T, _Args...>::value;

template <class _T> struct __is_in_place_type_specialization : false_type {};
template <class _T> struct __is_in_place_type_specialization<in_place_type_t<_T>> : true_type {};
template <class _T> inline constexpr bool __is_in_place_type_specialization_v = __is_in_place_type_specialization<_T>::value;

template <class _T> struct __is_in_place_index_specialization : false_type {};
template <size_t _I> struct __is_in_place_index_specialization<in_place_index_t<_I>> : true_type {};
template <class _T> inline constexpr bool __is_in_place_index_specialization_v = __is_in_place_index_specialization<_T>::value;

template <class _T> inline constexpr bool __is_in_place_specialization_v = __is_in_place_type_specialization_v<_T> || __is_in_place_index_specialization_v<_T>;

template <class _T, class _W>
constexpr bool __converts_from_any_cvref = disjunction_v<
    is_constructible<_T, _W&>, is_convertible<_W&, _T>,
    is_constructible<_T, _W>, is_convertible<_W, _T>,
    is_constructible<_T, const _W&>, is_convertible<const _W&, _T>,
    is_constructible<_T, const _W&&>, is_convertible<const _W, _T>>;

} // namespace __detail


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_INPLACE_H */
