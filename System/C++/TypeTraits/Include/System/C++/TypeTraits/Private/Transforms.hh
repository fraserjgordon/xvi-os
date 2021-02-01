#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_TRANSFORMS_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_TRANSFORMS_H


#include <System/C++/TypeTraits/Private/CompilerBuiltins.hh>
#include <System/C++/TypeTraits/Private/Conditional.hh>
#include <System/C++/TypeTraits/Private/Detect.hh>
#include <System/C++/TypeTraits/Private/IntegralConstant.hh>
#include <System/C++/TypeTraits/Private/Invoke.hh>
#include <System/C++/TypeTraits/Private/Namespace.hh>


namespace __XVI_STD_TYPETRAITS_NS
{


// Forward declarations.
using size_t = decltype(sizeof(int));
template <class> struct is_reference;
template <class> struct is_function;
template <class> struct is_const;
template <class> struct is_volatile;
template <class> struct is_object;
template <class> struct is_integral;
template <class> struct is_enum;
template <class> struct is_void;
template <class> struct is_array;
template <class> struct is_lvalue_reference;
template <class> struct is_rvalue_reference;
template <class, class> struct is_same;
template <class, class> struct is_convertible;
template <class, class, template <class> class, template <class> class> struct basic_common_reference;


namespace __detail
{

// Forward declarations.
template <class> struct __strip_fn_qualifiers;  // In SimpleTraits.hh
template <class, class> struct __is_not_same;   // In Relations.hh 

template <class _T> struct __is_referenceable
    : bool_constant<is_object<_T>::value 
        || (is_function<_T>::value && is_same<_T, typename __strip_fn_qualifiers<_T>::type>::value)
        || is_reference<_T>::value> {};

template <class _T> struct remove_const { using type = _T; };
template <class _T> struct remove_const<const _T> { using type = _T; };

template <class _T> struct remove_volatile { using type = _T; };
template <class _T> struct remove_volatile<volatile _T> { using type = _T; };

template <class _T> struct remove_cv {using type = _T; };
template <class _T> struct remove_cv<const _T> { using type = _T; };
template <class _T> struct remove_cv<volatile _T> { using type = _T; };
template <class _T> struct remove_cv<const volatile _T> { using type = _T; };

template <class _T, bool = is_reference<_T>::value || is_function<_T>::value || is_const<_T>::value>
    struct add_const { using type = const _T; };
template <class _T> struct add_const<_T, true> { using type = _T; };

template <class _T, bool = is_reference<_T>::value || is_function<_T>::value || is_volatile<_T>::value>
    struct add_volatile { using type = volatile _T; };
template <class _T> struct add_volatile<_T, true> { using type = _T; };

template <class _T>
    struct add_cv { using type = typename add_const<typename add_volatile<_T>::type>::type; };


template <class _T> struct remove_reference { using type = _T; };
template <class _T> struct remove_reference<_T&> { using type = _T; };
template <class _T> struct remove_reference<_T&&> { using type = _T; };

template <class _T, bool = __is_referenceable<_T>::value>
    struct add_lvalue_reference { using type = _T&; };
template <class _T> struct add_lvalue_reference<_T, false> { using type = _T; };

template <class _T, bool = __is_referenceable<_T>::value>
    struct add_rvalue_reference { using type = _T&&; };
template <class _T> struct add_rvalue_reference<_T, false> { using type = _T; };


template <class _From, class _To> struct __copy_cv { using type = _To; };
template <class _From, class _To> struct __copy_cv<const _From, _To> { using type = typename add_const<_To>::type; };
template <class _From, class _To> struct __copy_cv<volatile _From, _To> { using type = typename add_volatile<_To>::type; };
template <class _From, class _To> struct __copy_cv<const volatile _From, _To> { using type = typename add_cv<_To>::type; };

template <size_t _Size, bool _Signed> struct __sized_integer
{
    static constexpr auto __return_sized_integer_signed()
    {
        if constexpr (_Size == sizeof(char))
            return char(0);
        else if constexpr (_Size == sizeof(short))
            return short(0);
        else if constexpr (_Size == sizeof(int))
            return 0;
        else if constexpr (_Size == sizeof(long))
            return 0L;
        else if constexpr (_Size == sizeof(long long))
            return 0LL;
    }

    static constexpr auto __return_sized_integer_unsigned()
    {
        using __uchar = unsigned char;
        using __ushort = unsigned short;
        
        if constexpr (_Size == sizeof(char))
            return __uchar(0);
        else if constexpr (_Size == sizeof(short))
            return __ushort(0);
        else if constexpr (_Size == sizeof(int))
            return 0U;
        else if constexpr (_Size == sizeof(long))
            return 0UL;
        else if constexpr (_Size == sizeof(long long))
            return 0ULL;
    }

    using __utype = decltype(__return_sized_integer_unsigned());
    using __stype = decltype(__return_sized_integer_signed());

    using type = conditional_t<_Signed, __stype, __utype>;
};

template <class _T> struct make_signed
{ 
    using type = enable_if_t<(is_integral<_T>::value && !is_same<_T, bool>::value) || is_enum<_T>::value,
                             typename __sized_integer<sizeof(_T), true>::type>;
};
template <> struct make_signed<char> { using type = signed char; };
template <> struct make_signed<signed char> { using type = signed char; };
template <> struct make_signed<short> { using type = short; };
template <> struct make_signed<int> { using type = int; };
template <> struct make_signed<long> { using type = long; };
template <> struct make_signed<long long> { using type = long long; };
template <> struct make_signed<unsigned char> { using type = signed char; };
template <> struct make_signed<unsigned short> { using type = short; };
template <> struct make_signed<unsigned int> { using type = int; };
template <> struct make_signed<unsigned long> { using type = long; };
template <> struct make_signed<unsigned long long> { using type = long long; };

template <class _T> struct make_unsigned
{ 
    using type = enable_if_t<(is_integral<_T>::value && !is_same<_T, bool>::value) || is_enum<_T>::value,
                             typename __sized_integer<sizeof(_T), false>::type>;
};
template <> struct make_unsigned<char> { using type = unsigned char; };
template <> struct make_unsigned<signed char> { using type = unsigned char; };
template <> struct make_unsigned<short> { using type = unsigned short; };
template <> struct make_unsigned<int> { using type = unsigned int; };
template <> struct make_unsigned<long> { using type = unsigned long; };
template <> struct make_unsigned<long long> { using type = unsigned long long; };
template <> struct make_unsigned<unsigned char> { using type = unsigned char; };
template <> struct make_unsigned<unsigned short> { using type = unsigned short; };
template <> struct make_unsigned<unsigned int> { using type = unsigned int; };
template <> struct make_unsigned<unsigned long> { using type = unsigned long; };
template <> struct make_unsigned<unsigned long long> { using type = unsigned long long; };


template <class _T> struct remove_extent { using type = _T; };
template <class _T> struct remove_extent<_T[]> { using type = _T; };
template <class _T, size_t _N> struct remove_extent<_T[_N]> { using type = _T; };

template <class _T> struct remove_all_extents { using type = _T; };
template <class _T> struct remove_all_extents<_T[]> { using type = typename remove_all_extents<_T>::type; };
template <class _T, size_t _N> struct remove_all_extents<_T[_N]> { using type = typename remove_all_extents<_T>::type; };

template <class _T, class _U = typename remove_cv<_T>::type>
    struct remove_pointer { using type = _T; };
template <class _T, class _U> struct remove_pointer<_T, _U*> { using type = _U; };

template <class _T, bool = __is_referenceable<_T>::value || is_void<_T>::value>
    struct add_pointer { using type = typename remove_reference<_T>::type*; };
template <class _T> struct add_pointer<_T, false> { using type = _T; };


template <class _T> struct type_identity { using type = _T; };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wduplicated-branches"
static constexpr size_t __get_default_alignment(size_t _Len)
{
    if (_Len <= sizeof(char))
        return alignof(char);
    else if (_Len <= sizeof(short))
        return alignof(short);
    else if (_Len <= sizeof(int))
        return alignof(int);
    else if (_Len <= sizeof(long))
        return alignof(long);
    else if (_Len <= sizeof(long long))
        return alignof(long long);
    else // TODO: probably not right.
        return alignof(long long);
}
#pragma GCC diagnostic pop

template <size_t _Len, size_t _Align>
struct aligned_storage
{
    struct __storage
    {
        alignas(_Align) unsigned char __bytes [_Len];
    };

    using type = __storage;
};


template <class _T, _T...> struct __max_constant {};
template <class _T, _T _First, _T _Second, _T... _Rest>
    struct __max_constant<_T, _First, _Second, _Rest...>
        : integral_constant<_T, __max_constant<_T, (_First < _Second ? _Second : _First), _Rest...>::value> {};
template <class _T, _T _Val> struct __max_constant<_T, _Val> : integral_constant<_T, _Val> {};

template <size_t _Len, class... _Types>
struct aligned_union
{
    static constexpr size_t __len = __max_constant<size_t, _Len, sizeof(_Types)...>::value;
    static constexpr size_t alignment_value = __max_constant<size_t, alignof(_Types)...>::value;
    
    using type = typename aligned_storage<__len, alignment_value>::type;
};


template <class _T> struct remove_cvref { using type = typename remove_cv<typename remove_reference<_T>::type>::type; };


template <class _T, class _U = typename remove_reference<_T>::type>
struct decay
{
    using type = conditional_t<is_array<typename remove_reference<_U>::type>::value,
                               typename remove_extent<typename remove_reference<_U>::type>::type*,
                               conditional_t<is_function<typename remove_reference<_U>::type>::value,
                                             typename add_pointer<typename remove_reference<_U>::type>::type,
                                             typename remove_cv<typename remove_reference<_U>::type>::type>>;
};


template <class _T> typename add_rvalue_reference<_T>::type declval() noexcept;

template <class _From, class _To> struct __copy_ref { using type = _To; };
template <class _From, class _To> struct __copy_ref<_From&, _To> { using type = _To&; };
template <class _From, class _To> struct __copy_ref<_From&&, _To> { using type = _To&&; };


// Common type/ref utilities.
struct __common_type_utils
{
    template <class _T> using _CREF = typename add_lvalue_reference<const typename remove_reference<_T>::type>::type;
    template <class _T, class _U> using _XREF_ = typename __copy_ref<_T, typename __copy_cv<_T, _U>::type>::type;
    template <class _T> struct _XREF { template <class _U> using __type = _XREF_<_T, _U>; };
    template <class _From, class _To> using _COPYCV = typename __copy_cv<_From, _To>::type;
    template <class _T, class _U> using _COND_RES = decltype(false ? declval<_T(&)()>()() : declval<_U(&)()>()());

    template <template <class...> class _D, class _T, class _U> struct __det
    {
        using __d = __detector<void, void, _D, _T, _U>;
        static constexpr bool value = __d::value_t::value;
        using type = typename __d::type;
    };
};


// Implementation of common_type follows.
template <class... _Types> struct common_type;

template <class... _Types> using __common_type_detector = typename common_type<_Types...>::type;

template <class _T1, class _T2>
struct __common_type_dual_helper : __common_type_utils
{
    using _D1 = typename decay<_T1>::type;
    using _D2 = typename decay<_T2>::type;

    template <class _U, class _V> using __alt0_detector = typename common_type<_U, _V>::type;
    template <class _U, class _V> using __alt1_detector = typename decay<decltype(false ? declval<_U>() : declval<_V>)>::type;
    template <class _U, class _V> using __alt2_detector = typename decay<_COND_RES<_CREF<_U>, _CREF<_V>>>::type;

    static constexpr bool __use_alt0 = conjunction_v<disjunction<__is_not_same<_T1, _D1>, __is_not_same<_T2, _D2>>, __is_detected_delayed<__alt0_detector, _D1, _D2>>;
    static constexpr bool __use_alt1 = conjunction_v<bool_constant<!__use_alt0>, is_detected<__alt1_detector, _D1, _D2>>;
    static constexpr bool __use_alt2 = conjunction_v<bool_constant<!__use_alt0>, bool_constant<!__use_alt1>, is_detected<__alt2_detector, _D1, _D2>>;
    static constexpr bool __has_type = __use_alt0 || __use_alt1 || __use_alt2;

    struct __alt0 { static constexpr bool value = __use_alt0; template <class _U, class _V> using __type = __alt0_detector<_U, _V>; };
    struct __alt1 { static constexpr bool value = __use_alt1; template <class _U, class _V> using __type = __alt1_detector<_U, _V>; };
    struct __alt2 { static constexpr bool value = __use_alt2; template <class _U, class _V> using __type = __alt2_detector<_U, _V>; };

    template <class _X = _D1, class _Y = _D2>
    using __type = typename disjunction<__alt0, __alt1, __alt2, nonesuch>::template __type<_X, _Y>;
};

template <class _T0, class _T1, bool = __common_type_dual_helper<_T0, _T1>::__has_type>
    struct __common_type_dual {};
template <class _T0, class _T1>
    struct __common_type_dual<_T0, _T1, true> { using type = typename __common_type_dual_helper<_T0, _T1>::template __type<>; };

template <class _T0>
    struct __common_type_single : __common_type_dual<_T0, _T0> {};

template <bool, class _T0, class _T1, class... _Rest>
    struct __common_type_multiple_helper : common_type<typename common_type<_T0, _T1>::type, _Rest...> {};
template <class _T0, class _T1, class... _Rest>
    struct __common_type_multiple_helper<false, _T0, _T1, _Rest...> {};

template <class _T0, class _T1, class... _Rest>
    struct __common_type_multiple
        : __common_type_multiple_helper<is_detected_v<__common_type_detector, _T0, _T1>, _T0, _T1, _Rest...> {};

template <> struct common_type<> {};
template <class _T0> struct common_type<_T0> : __common_type_single<_T0> {};
template <class _T0, class _T1> struct common_type<_T0, _T1> : __common_type_dual<_T0, _T1> {};
template <class... _Types> struct common_type : __common_type_multiple<_Types...> {};


// Implementation of common_reference follows.
template <class...> struct common_reference;

template <class _A, class _B> struct __common_ref;
template <class _A, class _B> using __common_ref_t = typename __common_ref<_A, _B>::type;

template <class _A, class _B> struct _COMMON_REF : __common_type_utils
{
    using _X = typename remove_reference<_A>::type;
    using _Y = typename remove_reference<_B>::type;

    template <class _T, class _U> using __alt0_detector = _COND_RES<_COPYCV<_T, _U>&, _COPYCV<_U, _T>&>;
    template <class _T, class _U> using __alt1_detector = typename remove_reference<typename _COMMON_REF<_T&, _U&>::type>::type&&;
    template <class _T, class _U> using __alt2_detector = typename _COMMON_REF<const _T&, _U&>::type;
    template <class _T, class _U> using __alt3_detector = typename _COMMON_REF<_U, _T>::type;

    template <class _T, class _U> struct __alt0_type
        { using type = __alt0_detector<_T, _U>; static constexpr bool value = is_reference<type>::value; };
    template <class _T, class _U> struct __alt1_type
        { using type = __alt1_detector<_T, _U>; static constexpr bool value = is_convertible<_A, type>::value && is_convertible<_B, type>::value; };
    template <class _T, class _U> struct __alt2_type
        { using type = __alt2_detector<_T, _U>; static constexpr bool value = is_convertible<_A, type>::value; };
    template <class _T, class _U> struct __alt3_type
        { using type = __alt3_detector<_T, _U>; static constexpr bool value = true; };

    static constexpr bool __use_alt0 =
        conjunction_v<is_lvalue_reference<_A>, is_lvalue_reference<_B>, is_detected<__alt0_detector, _X, _Y>, __alt0_type<_X, _Y>>;
    static constexpr bool __use_alt1 =
        conjunction_v<bool_constant<!__use_alt0>, is_rvalue_reference<_A>, is_rvalue_reference<_B>, is_detected<__alt1_detector, _X, _Y>, __alt1_type<_X, _Y>>;
    static constexpr bool __use_alt2 =
        conjunction_v<bool_constant<!__use_alt0 && !__use_alt1>, is_rvalue_reference<_A>, is_lvalue_reference<_B>, is_detected<__alt2_detector, _X, _Y>, __alt2_type<_X, _Y>>;
    static constexpr bool __use_alt3 =
        conjunction_v<bool_constant<!__use_alt0 && !__use_alt1 && !__use_alt2>, is_lvalue_reference<_A>, is_rvalue_reference<_B>, is_detected<__alt3_detector, _A, _B>, __alt3_type<_A, _B>>;
    static constexpr bool __has_type = __use_alt0 || __use_alt1 || __use_alt2 || __use_alt3;

    //static_assert(__has_type, "instantiation of __common_ref is ill-formed");

    struct __alt0 { static constexpr bool value = __use_alt0; template <class _T = _X, class _U = _Y> using __type = __alt0_detector<_T, _U>; };
    struct __alt1 { static constexpr bool value = __use_alt1; template <class _T = _X, class _U = _Y> using __type = __alt1_detector<_T, _U>; };
    struct __alt2 { static constexpr bool value = __use_alt2; template <class _T = _X, class _U = _Y> using __type = __alt2_detector<_T, _U>; };
    struct __alt3 { static constexpr bool value = __use_alt3; template <class _T = _A, class _U = _B> using __type = __alt3_detector<_T, _U>; };

    using type = typename disjunction<__alt0, __alt1, __alt2, __alt3>::template __type<>;
};

template <class... _Types> using __common_reference_detector = typename common_reference<_Types...>::type;

template <class _T0, class _T1>
struct __common_reference_dual_helper : __common_type_utils
{
    template <class _X, class _Y> using __alt0_detector = typename _COMMON_REF<_X, _Y>::type;
    template <class _X, class _Y> using __alt1_detector = typename basic_common_reference<typename remove_cvref<_X>::type, typename remove_cvref<_Y>::type, _XREF<_X>::template __type, _XREF<_Y>::template __type>::type;
    template <class _X, class _Y> using __alt2_detector = _COND_RES<_X, _Y>;
    template <class _X, class _Y> using __alt3_detector = typename common_type<_X, _Y>::type;

    static constexpr bool __use_alt0 = conjunction_v<is_reference<_T0>, is_reference<_T1>, is_detected<__alt0_detector, _T0, _T1>>;
    static constexpr bool __use_alt1 = conjunction_v<bool_constant<!__use_alt0>, is_detected<__alt1_detector, _T0, _T1>>;
    static constexpr bool __use_alt2 = conjunction_v<bool_constant<!__use_alt0 && !__use_alt1>, is_detected<__alt2_detector, _T0, _T1>>;
    static constexpr bool __use_alt3 = conjunction_v<bool_constant<!__use_alt0 && !__use_alt1 && !__use_alt2>, is_detected<__alt3_detector, _T0, _T1>>;
    static constexpr bool __has_type = __use_alt0 || __use_alt1 || __use_alt2 || __use_alt3;

    struct __alt0 { static constexpr bool value = __use_alt0; template <class _X, class _Y> using __type = __alt0_detector<_X, _Y>; };
    struct __alt1 { static constexpr bool value = __use_alt1; template <class _X, class _Y> using __type = __alt1_detector<_X, _Y>; };
    struct __alt2 { static constexpr bool value = __use_alt2; template <class _X, class _Y> using __Type = __alt2_detector<_X, _Y>; };
    struct __alt3 { static constexpr bool value = __use_alt3; template <class _X, class _Y> using __type = __alt3_detector<_X, _Y>; };

    template <class _X = _T0, class _Y = _T1>
    using __type = typename disjunction<__alt0, __alt1, __alt2, __alt3, nonesuch>::template __type<_X, _Y>;
};

template <class _T0, class _T1, bool = __common_reference_dual_helper<_T0, _T1>::__has_type>
    struct __common_reference_dual {};
template <class _T0, class _T1>
    struct __common_reference_dual<_T0, _T1, true> { using type = typename __common_reference_dual_helper<_T0, _T1>::template __type<>; };

template <bool, class _T0, class _T1, class... _Rest>
    struct __common_reference_multiple_helper : common_reference<typename common_reference<_T0, _T1>::type, _Rest...> {};
template <class _T0, class _T1, class... _Rest>
    struct __common_reference_multiple_helper<false, _T0, _T1, _Rest...> {};

template <class _T0, class _T1, class... _Rest>
    struct __common_reference_multiple
        : __common_reference_multiple_helper<is_detected_v<__common_reference_detector, _T0, _T1>, _T0, _T1, _Rest...> {};

template <> struct common_reference<> {};
template <class _T0> struct common_reference<_T0> { using type = _T0; };
template <class _T0, class _T1> struct common_reference<_T0, _T1> : __common_reference_dual<_T0, _T1> {};
template <class... _Types> struct common_reference : __common_reference_multiple<_Types...> {};


template <bool, class _Fn, class... _Args> struct __invoke_result {};
template <class _Fn, class... _Args>
    struct __invoke_result<true, _Fn, _Args...> { using type = __is_invocable_detector<_Fn, _Args...>; };

template <class _Fn, class... _Args>
    struct invoke_result : __invoke_result<is_detected_v<__is_invocable_detector, _Fn, _Args...>, _Fn, _Args...> {};

} // namespace __detail


template <class _T> struct remove_const : __detail::remove_const<_T> {};
template <class _T> using remove_const_t = typename remove_const<_T>::type;

template <class _T> struct remove_volatile : __detail::remove_volatile<_T> {};
template <class _T> using remove_volatile_t = typename remove_volatile<_T>::type;

template <class _T> struct remove_cv : __detail::remove_cv<_T> {};
template <class _T> using remove_cv_t = typename remove_cv<_T>::type;

template <class _T> struct add_const : __detail::add_const<_T> {};
template <class _T> using add_const_t = typename add_const<_T>::type;

template <class _T> struct add_volatile : __detail::add_volatile<_T> {};
template <class _T> using add_volatile_t = typename add_volatile<_T>::type;

template <class _T> struct add_cv : __detail::add_cv<_T> {};
template <class _T> using add_cv_t = typename add_cv<_T>::type;

template <class _T> struct remove_reference : __detail::remove_reference<_T> {};
template <class _T> using remove_reference_t = typename remove_reference<_T>::type;

template <class _T> struct add_lvalue_reference : __detail::add_lvalue_reference<_T> {};
template <class _T> using add_lvalue_reference_t = typename add_lvalue_reference<_T>::type;

template <class _T> struct add_rvalue_reference : __detail::add_rvalue_reference<_T> {};
template <class _T> using add_rvalue_reference_t = typename add_rvalue_reference<_T>::type;

template <class _T> struct make_signed : __detail::make_signed<_T> {};
template <class _T> using make_signed_t = typename make_signed<_T>::type;

template <class _T> struct make_unsigned : __detail::make_unsigned<_T> {};
template <class _T> using make_unsigned_t = typename make_unsigned<_T>::type;

template <class _T> struct remove_extent : __detail::remove_extent<_T> {};
template <class _T> using remove_extent_t = typename remove_extent<_T>::type;

template <class _T> struct remove_all_extents : __detail::remove_all_extents<_T> {};
template <class _T> using remove_all_extents_t = typename remove_all_extents<_T>::type;

template <class _T> struct remove_pointer : __detail::remove_pointer<_T> {};
template <class _T> using remove_pointer_t = typename remove_pointer<_T>::type;

template <class _T> struct add_pointer : __detail::add_pointer<_T> {};
template <class _T> using add_pointer_t = typename add_pointer<_T>::type;

template <class _T> struct type_identity : __detail::type_identity<_T> {};
template <class _T> using type_identity_t = typename type_identity<_T>::type;

template <size_t _Len, size_t _Align = __detail::__get_default_alignment(_Len)>
    struct aligned_storage : __detail::aligned_storage<_Len, _Align> {};
template <size_t _Len, size_t _Align = __detail::__get_default_alignment(_Len)>
    using aligned_storage_t = typename aligned_storage<_Len, _Align>::type;

template <size_t _Len, class... _Types> struct aligned_union : __detail::aligned_union<_Len, _Types...> {};
template <size_t _Len, class... _Types> using aligned_union_t = typename aligned_union<_Len, _Types...>::type;

template <class _T> struct remove_cvref : __detail::remove_cvref<_T> {};
template <class _T> using remove_cvref_t = typename remove_cvref<_T>::type;

template <class _T> struct decay : __detail::decay<_T> {};
template <class _T> using decay_t = typename decay<_T>::type;

template <class... _Types> struct common_type : __detail::common_type<_Types...> {};
template <class... _Types> using common_type_t = typename common_type<_Types...>::type;

template <class, class, template<class> class, template<class> class> struct basic_common_reference {};

template <class... _Types> struct common_reference : __detail::common_reference<_Types...> {};
template <class... _Types> using common_reference_t = typename common_reference<_Types...>::type;

template <class _T> struct underlying_type : __detail::underlying_type<_T> {};
template <class _T> using underlying_type_t = typename underlying_type<_T>::type;

template <class _Fn, class... _Args> struct invoke_result : __detail::invoke_result<_Fn, _Args...> {};
template <class _Fn, class... _Args> using invoke_result_t = typename invoke_result<_Fn, _Args...>::type;


} // namespace __XVI_STD_TYPETRAITS_NS


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_TRANSFORMS_H */
