#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_COMPILERBUILTINS_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_COMPILERBUILTINS_H


#include <System/C++/TypeTraits/Private/IntegralConstant.hh>


namespace __XVI_STD_TYPETRAITS_NS
{


// Forward declarations.
template <class, class> struct is_same;
template <class> struct is_reference;
template <class> struct is_destructible;


namespace __detail
{

template <class _T> struct is_enum : bool_constant<__is_enum(_T)> {};

template <class _T> struct is_union : bool_constant<__is_union(_T)> {};

template <class _T> struct is_class : bool_constant<__is_class(_T)> {};

template <class _T> struct is_trivially_copyable
    : bool_constant<__has_trivial_copy(_T) && !is_reference<_T>::value> {};

template <class _T> struct is_empty : bool_constant<__is_empty(_T)> {};

template <class _T> struct is_polymorphic : bool_constant<__is_polymorphic(_T)> {};

template <class _T> struct is_abstract : bool_constant<__is_abstract(_T)> {};

template <class _T> struct is_final : bool_constant<__is_final(_T)> {};

template <class _T> struct is_aggregate : bool_constant<__is_aggregate(_T)> {};

template <class _T> struct is_standard_layout : bool_constant<__is_standard_layout(_T)> {};

template <class _T> struct is_trivial : bool_constant<__is_trivial(_T)> {};

template <class _T, class... _Args> struct is_trivially_constructible
    : bool_constant<__is_trivially_constructible(_T, _Args...)> {};

template <class _T, class _U> struct is_trivially_assignable
    : bool_constant<__is_trivially_assignable(_T, _U)> {};

template <class _T> struct is_trivially_destructible
    : bool_constant<__has_trivial_destructor(_T) && is_destructible<_T>::value> {};

template <class _T> struct has_virtual_destructor : bool_constant<__has_virtual_destructor(_T)> {};

template <class _T> struct has_unique_object_representations
    : bool_constant<__has_unique_object_representations(_T)> {};

template <class _T, class _U> struct is_base_of
    : bool_constant<__is_base_of(_T, _U)> {};

template <class _T> struct underlying_type { using type = __underlying_type(_T); };

} // namespace __detail


constexpr bool is_constant_evaluated() noexcept
{
    return __builtin_is_constant_evaluated();
}


} // namespace __XVI_STD_TYPETRAITS_NS


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_COMPILERBUILTINS_H */
