#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_SIMPLETRAITS_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_SIMPLETRAITS_H


#include <System/C++/TypeTraits/Private/CompilerBuiltins.hh>
#include <System/C++/TypeTraits/Private/Conditional.hh>
#include <System/C++/TypeTraits/Private/Detect.hh>
#include <System/C++/TypeTraits/Private/IntegralConstant.hh>
#include <System/C++/TypeTraits/Private/Transforms.hh>


namespace __XVI_STD_TYPETRAITS_NS
{


// Forward declarations.
using size_t = decltype(sizeof(int));
template <class> class reference_wrapper;


namespace __detail
{

#if defined(__clang__)
//using char8_t = enum class __fake_char8_t : unsigned char {};
#endif

template <class _T> struct is_void : false_type {};
template <> struct is_void<void> : true_type {};

template <class _T> struct is_null_pointer : false_type {};
template <> struct is_null_pointer<decltype(nullptr)> : true_type {};

template <class _T> struct is_integral : false_type {};
template <> struct is_integral<bool> : true_type {};
template <> struct is_integral<char> : true_type {};
template <> struct is_integral<signed char> : true_type {};
template <> struct is_integral<unsigned char> : true_type {};
template <> struct is_integral<short> : true_type {};
template <> struct is_integral<unsigned short> : true_type {};
template <> struct is_integral<int> : true_type {};
template <> struct is_integral<unsigned int> : true_type {};
template <> struct is_integral<long> : true_type {};
template <> struct is_integral<unsigned long> : true_type {};
template <> struct is_integral<long long> : true_type {};
template <> struct is_integral<unsigned long long> : true_type {};
template <> struct is_integral<wchar_t> : true_type {};
template <> struct is_integral<char8_t> : true_type {};
template <> struct is_integral<char16_t> : true_type {};
template <> struct is_integral<char32_t> : true_type {};

template <class _T> struct is_floating_point : false_type {};
template <> struct is_floating_point<float> : true_type {};
template <> struct is_floating_point<double> : true_type {};
template <> struct is_floating_point<long double> : true_type {};

template <class _T> struct is_array : false_type {};
template <class _T> struct is_array<_T[]> : true_type {};
template <class _T, size_t N> struct is_array<_T[N]> : true_type {};

template <class _T> struct is_pointer : false_type {};
template <class _T> struct is_pointer<_T*> : true_type {};

template <class _T> struct is_lvalue_reference : false_type {};
template <class _T> struct is_lvalue_reference<_T&> : true_type {};
template <class _T> struct is_lvalue_reference<_T&&> : false_type {};

template <class _T> struct is_rvalue_reference : false_type {};
template <class _T> struct is_rvalue_reference<_T&> : false_type {};
template <class _T> struct is_rvalue_reference<_T&&> : true_type {};

template <class _T> struct is_member_object_pointer : false_type {};
template <class _T, class _C> struct is_member_object_pointer<_T _C::*> : true_type {};
template <class _R, class _C, class... _Args> struct is_member_object_pointer<_R (_C::*)(_Args...)> : false_type {};

template <class _T> struct is_member_function_pointer : false_type {};
template <class _R, class _C, class... _Args> struct is_member_function_pointer<_R (_C::*)(_Args...)> : true_type {};
template <class _R, class _C, class... _Args> struct is_member_function_pointer<_R (_C::*)(_Args...) noexcept> : true_type {};

// is_enum requires a compiler intrinsic
template <class> struct is_enum;

// is_union requires a compiler intrinsic
template <class> struct is_union;

// is_class requires a compiler intrinsic
template <class> struct is_class;

template <class _T> struct is_function : false_type {};
template <class _R, class... _Args> struct is_function<_R(_Args...)> : true_type {};
template <class _R, class... _Args> struct is_function<_R(_Args...) noexcept> : true_type {};

template <class _T> struct is_const : false_type {};
template <class _T> struct is_const<_T const> : true_type {};

template <class _T> struct is_volatile : false_type {};
template <class _T> struct is_volatile<_T volatile> : true_type {};

template <class _T, bool _IsArithmetic> struct is_signed : false_type {};
template <class _T> struct is_signed<_T, true> : bool_constant<(_T(-1) < _T(0))> {};

template <class _T, bool _IsArithmetic> struct is_unsigned : false_type {};
template <class _T> struct is_unsigned<_T, true> : bool_constant<(_T(0) < _T(-1))> {};


// Strips top-level CV qualifiers.
template <class _T> struct __strip_cv { using type = _T; };
template <class _T> struct __strip_cv<const _T> { using type = _T; };
template <class _T> struct __strip_cv<volatile _T> { using type = _T; };
template <class _T> struct __strip_cv<const volatile _T> { using type = _T; };
template <class _T> using __strip_cv_t = typename __strip_cv<_T>::type;

// Strips qualifers from non-member functions. Note that most of these types are not possible for real objects but are
// required to be handled anyway.
template <class _T> struct __strip_fn_qualifiers { using type = _T; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...)>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) volatile>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const volatile>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) &>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const &>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) volatile &>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const volatile &>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) &&>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const &&>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) volatile &&>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const volatile &&>
        { using type = _R(_Args...); };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) volatile noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const volatile noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) & noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const & noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) volatile & noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const volatile & noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) && noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const && noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) volatile && noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _R, class... _Args>
    struct __strip_fn_qualifiers<_R (_Args...) const volatile && noexcept>
        { using type = _R(_Args...) noexcept; };
template <class _T> using __strip_fn_qualifiers_t = typename __strip_fn_qualifiers<_T>::type;

// Strips qualifiers from a member function pointer.
template <class _T> struct __strip_memfn_qualifiers { using type = _T; };
template <class _R, class _C, class... _Args>
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...)>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args>
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const>
        {using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) volatile>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const volatile>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) &>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args>
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const &>
        {using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) volatile &>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const volatile &>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) &&>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args>
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const &&>
        {using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) volatile &&>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const volatile &&>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args>
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const noexcept>
        {using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) volatile noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const volatile noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) & noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args>
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const & noexcept>
        {using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) volatile & noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const volatile & noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) && noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args>
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const && noexcept>
        {using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) volatile && noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _R, class _C, class... _Args> 
    struct __strip_memfn_qualifiers<_R (_C::*)(_Args...) const volatile && noexcept>
        { using type = _R(_C::*)(_Args...); };
template <class _T> using __strip_memfn_qualifiers_t = typename __strip_memfn_qualifiers<_T>::type;


template <class _T, class... _Args> using __constructible_detector_t = decltype(_T(declval<_Args>()...));

template <class _T, class... _Args> using is_constructible = is_detected<__constructible_detector_t, _T, _Args...>;
template <class _T> using is_default_constructible = is_constructible<_T>;
template <class _T> using is_copy_constructible = is_constructible<_T, const _T&>;
template <class _T> using is_move_constructible = is_constructible<_T, _T&&>;


template <class _T, class _U> using __assign_detector_t = decltype(declval<_T>() = declval<_U>());

template <class _T, class _U> using is_assignable = is_detected<__assign_detector_t, _T, _U>;
template <class _T> using is_copy_assignable = is_assignable<_T&, const _T&>;
template <class _T> using is_move_assignable = is_assignable<_T&, _T&&>;


template <class _T, class _U = remove_all_extents_t<_T>> 
    using __destructible_detector_t = decltype(declval<_U&>().~_U());

template <class _T> struct is_destructible : is_detected<__destructible_detector_t, _T> {};


template <class _T> struct is_trivially_default_constructible : is_trivially_constructible<_T> {};
template <class _T> struct is_trivially_copy_constructible : is_trivially_constructible<_T, const _T&> {};
template <class _T> struct is_trivially_move_constructible : is_trivially_constructible<_T, _T&&> {};

template <class _T> struct is_trivially_copy_assignable : is_trivially_assignable<_T&, const _T&> {};
template <class _T> struct is_trivially_move_assignable : is_trivially_assignable<_T&, _T&&> {};


template <class _T, class... _Args>
    struct __is_nothrow_constructible_helper : conditional_t<noexcept(_T(declval<_Args>()...)), true_type, false_type> {}; 

template <class _T, class... _Args>
    using is_nothrow_constructible = typename conjunction<is_constructible<_T, _Args...>, __is_nothrow_constructible_helper<_T, _Args...>>::type;
template <class _T> using is_nothrow_default_constructible = is_nothrow_constructible<_T>;
template <class _T> using is_nothrow_copy_constructible = is_nothrow_constructible<_T, const _T&>;
template <class _T> using is_nothrow_move_constructible = is_nothrow_constructible<_T, _T&&>;


template <class _T, class _U>
    struct __is_nothrow_assignable_helper : bool_constant<noexcept(declval<_T>() = declval<_U>())> {};

template <class _T, class _U>
    using is_nothrow_assignable = typename conjunction<is_assignable<_T, _U>, __is_nothrow_assignable_helper<_T, _U>>::type;
template <class _T> using is_nothrow_copy_assignable = is_nothrow_assignable<_T&, const _T&>;
template <class _T> using is_nothrow_move_assignable = is_nothrow_assignable<_T&, _T&&>;


template <class _T, class _U = remove_all_extents_t<_T>>
    struct __is_nothrow_destructible_helper : bool_constant<noexcept(declval<_U&>().~_U())> {};

template <class _T>
    using is_nothrow_destructible = typename conjunction<is_destructible<_T>, __is_nothrow_destructible_helper<_T>>::type;

} // namespace __detail


// Forward declarations
template <class _T,
          class = enable_if_t<__detail::is_move_constructible<_T>::value && __detail::is_move_assignable<_T>::value, void>>
constexpr void swap(_T&, _T&)
    noexcept(__detail::is_nothrow_move_constructible<_T>::value && __detail::is_nothrow_move_assignable<_T>::value);


namespace __detail
{

template <class _T, class _U> using __swap_detector_t = decltype(swap(declval<_T>(), declval<_U>()));

template <class _T, class _U> 
    using is_swappable_with = typename conjunction<is_detected<__swap_detector_t, _T, _U>, is_detected<__swap_detector_t, _U, _T>>::type;
template <class _T> using is_swappable = is_swappable_with<_T&, _T&>;

template <class _T, class _U>
    struct __is_nothrow_swappable_helper : bool_constant<noexcept(swap(declval<_T>(), declval<_U>()))> {};

template <class _T, class _U>
    using is_nothrow_swappable_with = conjunction<is_swappable_with<_T, _U>, __is_nothrow_swappable_helper<_T, _U>, __is_nothrow_swappable_helper<_U, _T>>;
template <class _T> using is_nothrow_swappable = is_nothrow_swappable_with<_T&, _T&>;


template <class _T> struct alignment_of : integral_constant<size_t, alignof(_T)> {};


template <class _T> struct rank : integral_constant<size_t, 0> {};
template <class _T> struct rank<_T[]> : integral_constant<size_t, rank<_T>::value + 1> {};
template <class _T, size_t _N> struct rank<_T[_N]> : integral_constant<size_t, rank<_T>::value + 1> {};

template <class _T, unsigned _I> struct extent : integral_constant<size_t, 0> {};
template <class _T, size_t _N> struct extent<_T[_N], 0> : integral_constant<size_t, _N> {};
template <class _T, unsigned _I, size_t _N> struct extent<_T[_N], _I> : integral_constant<size_t, extent<_T, _I-1>::value> {};
template <class _T> struct extent<_T[], 0> : integral_constant<size_t, 0> {};
template <class _T, unsigned _I> struct extent<_T[], _I> : integral_constant<size_t, extent<_T, _I-1>::value> {};


template <class _T> struct __is_reference_wrapper_specialization : false_type {};
template <class _T> struct __is_reference_wrapper_specialization<reference_wrapper<_T>> : true_type {};

} // namespace __detail


template <class _T> struct is_void : __detail::is_void<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_null_pointer : __detail::is_null_pointer<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_integral : __detail::is_integral<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_floating_point : __detail::is_floating_point<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_array : __detail::is_array<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_pointer : __detail::is_pointer<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_lvalue_reference : __detail::is_lvalue_reference<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_rvalue_reference : __detail::is_rvalue_reference<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_member_object_pointer : __detail::is_member_object_pointer<__detail::__strip_memfn_qualifiers_t<__detail::__strip_cv_t<_T>>> {};
template <class _T> struct is_member_function_pointer : __detail::is_member_function_pointer<__detail::__strip_memfn_qualifiers_t<__detail::__strip_cv_t<_T>>> {};
template <class _T> struct is_enum : __detail::is_enum<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_union : __detail::is_union<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_class : __detail::is_class<__detail::__strip_cv_t<_T>> {};
template <class _T> struct is_function : __detail::is_function<__detail::__strip_fn_qualifiers_t<__detail::__strip_cv_t<_T>>> {};

template <class _T> inline constexpr bool is_void_v = is_void<_T>::value;
template <class _T> inline constexpr bool is_null_pointer_v = is_null_pointer<_T>::value;
template <class _T> inline constexpr bool is_integral_v = is_integral<_T>::value;
template <class _T> inline constexpr bool is_floating_point_v = is_floating_point<_T>::value;
template <class _T> inline constexpr bool is_array_v = is_array<_T>::value;
template <class _T> inline constexpr bool is_pointer_v = is_pointer<_T>::value;
template <class _T> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<_T>::value;
template <class _T> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<_T>::value;
template <class _T> inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<_T>::value;
template <class _T> inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<_T>::value;
template <class _T> inline constexpr bool is_enum_v = is_enum<_T>::value;
template <class _T> inline constexpr bool is_union_v = is_union<_T>::value;
template <class _T> inline constexpr bool is_class_v = is_class<_T>::value;
template <class _T> inline constexpr bool is_function_v = is_function<_T>::value;

template <class _T> struct is_reference : bool_constant<is_lvalue_reference_v<_T> || is_rvalue_reference_v<_T>> {};
template <class _T> inline constexpr bool is_reference_v = is_reference<_T>::value;

template <class _T> struct is_arithmetic : bool_constant<is_integral_v<_T> || is_floating_point_v<_T>> {};
template <class _T> inline constexpr bool is_arithmetic_v = is_arithmetic<_T>::value;

template <class _T> struct is_fundamental : bool_constant<is_arithmetic_v<_T> || is_void_v<_T> || is_null_pointer_v<_T>> {};
template <class _T> inline constexpr bool is_fundamental_v = is_fundamental<_T>::value;

template <class _T> struct is_member_pointer : bool_constant<is_member_object_pointer_v<_T> || is_member_function_pointer_v<_T>> {};
template <class _T> inline constexpr bool is_member_pointer_v = is_member_pointer<_T>::value;

template <class _T> struct is_object : bool_constant<!is_void_v<_T> && !is_reference_v<_T> && !is_function_v<_T>> {};
template <class _T> inline constexpr bool is_object_v = is_object<_T>::value;

template <class _T> struct is_scalar
    : bool_constant<is_arithmetic_v<_T> || is_enum_v<_T> || is_pointer_v<_T> || is_member_pointer_v<_T> || is_null_pointer_v<_T>> {};
template <class _T> inline constexpr bool is_scalar_v = is_scalar<_T>::value;

template <class _T> struct is_compound
    : bool_constant<is_array_v<_T> || is_function_v<_T> || is_pointer_v<_T> || is_reference_v<_T> || is_enum_v<_T> || is_union_v<_T> || is_class_v<_T> || is_member_pointer_v<_T>> {};
template <class _T> inline constexpr bool is_compound_v = is_compound<_T>::value;


template <class _T> struct is_const : __detail::is_const<_T> {};
template <class _T> inline constexpr bool is_const_v = is_const<_T>::value;

template <class _T> struct is_volatile : __detail::is_volatile<_T> {};
template <class _T> inline constexpr bool is_volatile_v = is_volatile<_T>::value;

template <class _T> struct is_signed : __detail::is_signed<_T, is_arithmetic_v<_T>> {};
template <class _T> inline constexpr bool is_signed_v = is_signed<_T>::value;

template <class _T> struct is_unsigned : __detail::is_unsigned<_T, is_arithmetic_v<_T>> {};
template <class _T> inline constexpr bool is_unsigned_v = is_unsigned<_T>::value;

template <class _T> struct is_trivial : __detail::is_trivial<_T> {};
template <class _T> inline constexpr bool is_trivial_v = is_trivial<_T>::value;

template <class _T> struct is_scoped_enum : __detail::is_scoped_enum<_T> {};
template <class _T> inline constexpr bool is_scoped_enum_v = is_scoped_enum<_T>::value;

template <class _T> struct is_trivially_copyable : __detail::is_trivially_copyable<_T> {};
template <class _T> inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<_T>::value;

template <class _T, class... _Args> struct is_trivially_constructible : __detail::is_trivially_constructible<_T, _Args...> {};
template <class _T, class... _Args> inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<_T, _Args...>::value;

template <class _T> struct is_trivially_default_constructible : __detail::is_trivially_default_constructible<_T> {};
template <class _T> inline constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<_T>::value;

template <class _T> struct is_trivially_copy_constructible : __detail::is_trivially_copy_constructible<_T> {};
template <class _T> inline constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<_T>::value;

template <class _T> struct is_trivially_move_constructible : __detail::is_trivially_move_constructible<_T> {};
template <class _T> inline constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<_T>::value;

template <class _T, class _U> struct is_trivially_assignable : __detail::is_trivially_assignable<_T, _U> {};
template <class _T, class _U> inline constexpr bool is_trivially_assignable_v = is_trivially_assignable<_T, _U>::value;

template <class _T> struct is_trivially_copy_assignable : __detail::is_trivially_copy_assignable<_T> {};
template <class _T> inline constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<_T>::value;

template <class _T> struct is_trivially_move_assignable : __detail::is_trivially_move_assignable<_T> {};
template <class _T> inline constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<_T>::value;

template <class _T> struct is_trivially_destructible : __detail::is_trivially_destructible<_T> {};
template <class _T> inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<_T>::value;

template <class _T> struct is_standard_layout : __detail::is_standard_layout<_T> {};
template <class _T> inline constexpr bool is_standard_layout_v = is_standard_layout<_T>::value;

template <class _T> struct is_empty : __detail::is_empty<_T> {};
template <class _T> inline constexpr bool is_empty_v = is_empty<_T>::value;

template <class _T> struct is_polymorphic : __detail::is_polymorphic<_T> {};
template <class _T> inline constexpr bool is_polymorphic_v = is_polymorphic<_T>::value;

template <class _T> struct is_abstract : __detail::is_abstract<_T> {};
template <class _T> inline constexpr bool is_abstract_v = is_abstract<_T>::value;

template <class _T> struct is_final : __detail::is_final<_T> {};
template <class _T> inline constexpr bool is_final_v = is_final<_T>::value;

template <class _T> struct is_aggregate : __detail::is_aggregate<_T> {};
template <class _T> inline constexpr bool is_aggregate_v = is_aggregate<_T>::value;

template <class _T, class... _Args> struct is_constructible : __detail::is_constructible<_T, _Args...> {};
template <class _T, class... _Args> inline constexpr bool is_constructible_v = is_constructible<_T, _Args...>::value;

template <class _T> struct is_default_constructible : __detail::is_default_constructible<_T> {};
template <class _T> inline constexpr bool is_default_constructible_v = is_default_constructible<_T>::value;

template <class _T> struct is_copy_constructible : __detail::is_copy_constructible<_T> {};
template <class _T> inline constexpr bool is_copy_constructible_v = is_copy_constructible<_T>::value;

template <class _T> struct is_move_constructible : __detail::is_move_constructible<_T> {};
template <class _T> inline constexpr bool is_move_constructible_v = is_move_constructible<_T>::value;

template <class _T, class _U> struct is_assignable : __detail::is_assignable<_T, _U> {};
template <class _T, class _U> inline constexpr bool is_assignable_v = is_assignable<_T, _U>::value;

template <class _T> struct is_copy_assignable : __detail::is_copy_assignable<_T> {};
template <class _T> inline constexpr bool is_copy_assignable_v = is_copy_assignable<_T>::value;

template <class _T> struct is_move_assignable : __detail::is_move_assignable<_T> {};
template <class _T> inline constexpr bool is_move_assignable_v = is_move_assignable<_T>::value;

template <class _T, class... _Args> struct is_nothrow_constructible : __detail::is_nothrow_constructible<_T, _Args...> {};
template <class _T, class... _Args> inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<_T, _Args...>::value;

template <class _T> struct is_nothrow_default_constructible : __detail::is_nothrow_default_constructible<_T> {};
template <class _T> inline constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<_T>::value;

template <class _T> struct is_nothrow_copy_constructible : __detail::is_nothrow_copy_constructible<_T> {};
template <class _T> inline constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<_T>::value;

template <class _T> struct is_nothrow_move_constructible : __detail::is_nothrow_move_constructible<_T> {};
template <class _T> inline constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<_T>::value;

template <class _T, class _U> struct is_nothrow_assignable : __detail::is_nothrow_assignable<_T, _U> {};
template <class _T, class _U> inline constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<_T, _U>::value;

template <class _T> struct is_nothrow_copy_assignable : __detail::is_nothrow_copy_assignable<_T> {};
template <class _T> inline constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<_T>::value;

template <class _T> struct is_nothrow_move_assignable : __detail::is_nothrow_move_assignable<_T> {};
template <class _T> inline constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<_T>::value;

template <class _T> struct is_destructible : __detail::is_destructible<_T> {};
template <class _T> inline constexpr bool is_destructible_v = is_destructible<_T>::value;

template <class _T> struct is_nothrow_destructible : __detail::is_nothrow_destructible<_T> {};
template <class _T> inline constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<_T>::value;

template <class _T> struct has_virtual_destructor : __detail::has_virtual_destructor<_T> {};
template <class _T> inline constexpr bool has_virtual_destructor_v = has_virtual_destructor<_T>::value;

template <class _T> struct has_unique_object_representations : __detail::has_unique_object_representations<_T> {};
template <class _T> inline constexpr bool has_unique_object_representations_v = has_unique_object_representations<_T>::value;

template <class _T, class _U> struct is_swappable_with : __detail::is_swappable_with<_T, _U> {};
template <class _T, class _U> inline constexpr bool is_swappable_with_v = is_swappable_with<_T, _U>::value;

template <class _T> struct is_swappable : __detail::is_swappable<_T> {};
template <class _T> inline constexpr bool is_swappable_v = is_swappable<_T>::value;

template <class _T, class _U> struct is_nothrow_swappable_with : __detail::is_nothrow_swappable_with<_T, _U> {};
template <class _T, class _U> inline constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<_T, _U>::value;

template <class _T> struct is_nothrow_swappable : __detail::is_nothrow_swappable<_T> {};
template <class _T> inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<_T>::value;

template <class _T> struct alignment_of : __detail::alignment_of<_T> {};
template <class _T> inline constexpr size_t alignment_of_v = alignment_of<_T>::value;

template <class _T> struct rank : __detail::rank<_T> {};
template <class _T> inline constexpr size_t rank_v = rank<_T>::value;

template <class _T, unsigned _I = 0> struct extent : __detail::extent<_T, _I> {};
template <class _T, unsigned _I = 0> inline constexpr size_t extent_v = extent<_T, _I>::value;

template <class _T> struct is_bounded_array : bool_constant<is_array_v<_T> &&  extent_v<_T> != 0> {};
template <class _T> inline constexpr bool is_bounded_array_v = is_bounded_array<_T>::value;

template <class _T> struct is_unbounded_array : bool_constant<is_array_v<_T> && extent_v<_T> == 0> {};
template <class _T> inline constexpr bool is_unbounded_array_v = is_unbounded_array<_T>::value;

template <class _T, class _U> struct reference_constructs_from_temporary : __detail::reference_constructs_from_temporary<_T, _U> {};
template <class _T, class _U> inline constexpr bool reference_constructs_from_temporary_v = reference_constructs_from_temporary<_T, _U>::value;

template <class _T, class _U> struct reference_converts_from_temporary : __detail::reference_converts_from_temporary<_T, _U> {};
template <class _T, class _U> inline constexpr bool reference_converts_from_temporary_v = reference_converts_from_temporary<_T, _U>::value;


} // namespace __XVI_STD_TYPETRAITS_NS


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_SIMPLETRAITS_H */
