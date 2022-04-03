#ifndef __SYSTEM_CXX_UTILITY_PRIVATE_CONCEPTS_H
#define __SYSTEM_CXX_UTILITY_PRIVATE_CONCEPTS_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class> struct pointer_traits;


namespace __detail
{


template <class _T>
concept __has_member_value_type = requires { typename _T::value_type; };

template <class _T>
concept __has_member_element_type = requires { typename _T::element_type; };

template <class _T>
concept __has_member_difference_type = requires { typename _T::difference_type; };

template <class _T>
concept __has_member_pointer_type = requires { typename _T::pointer; };


template <class _T> struct __pointer_member_detected
    { using type = typename _T::pointer; };

template <class _T> struct __pointer_traits_element_type_detected
    { using type = typename _T::element_type; };

template <class _T> struct __pointer_traits_element_type_pointer_indirect
    { using type = typename pointer_traits<_T*>::element_type*; };

template <class _T> struct __pointer_of
    : conditional_t<__has_member_pointer_type<_T>,
                    __pointer_member_detected<_T>, 
                    conditional_t<__has_member_element_type<_T>,
                                  __pointer_traits_element_type_detected<_T>,
                                  __pointer_traits_element_type_pointer_indirect<_T>>> {};

template <class _T>
using __pointer_of_t = typename __pointer_of<_T>::type;

template <class _T, class _U>
struct __pointer_of_or { using type = _U; };

template <class _T, class _U>
    requires requires { typename __pointer_of_t<_T>; }
struct __pointer_of_or<_T, _U> { using type = __pointer_of_t<_T>; };

template <class _T, class _U>
using __pointer_of_or_t = typename __pointer_of_or<_T, _U>::type;


} // namespace __detail


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_PRIVATE_CONCEPTS_H */
