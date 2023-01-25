#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_INVOKE_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_INVOKE_H


#include <System/C++/TypeTraits/Private/CompilerBuiltins.hh>
#include <System/C++/TypeTraits/Private/Conditional.hh>
#include <System/C++/TypeTraits/Private/Namespace.hh>


namespace __XVI_STD_TYPETRAITS_NS
{


// Forward declarations.
template <class, class> struct is_base_of;
template <class> struct is_member_function_pointer;
template <class> struct is_member_object_pointer;
template <class> struct is_void;
template <class> struct remove_reference;
template <class> struct remove_cvref;
template <class> struct remove_cv;
template <class> struct add_rvalue_reference;
template <class, class> struct is_convertible;
template <class, class> struct is_nothrow_convertible;
template <class, class> struct reference_converts_from_temporary;
template <class _T> typename add_rvalue_reference<_T>::type declval() noexcept;
template <class _T> constexpr _T&& forward(typename remove_reference<_T>::type&) noexcept;
template <class _T> constexpr _T&& forward(typename remove_reference<_T>::type&&) noexcept;


namespace __detail
{

// Forward declarations.
template <class> struct __is_reference_wrapper_specialization;
template <class> struct __strip_memfn_qualifiers;


template <class _F> struct __mem_fn_ptr_class;
template <class _R, class _C, class... _Params> struct __mem_fn_ptr_class<_R (_C::*)(_Params...)> { using type = _C; };

template <class _F> struct __mem_obj_ptr_class;
template <class _T, class _C> struct __mem_obj_ptr_class<_T _C::*> { using type = _C; };


template <class _F, class _T1, class... _Tn>
    requires requires(_F&& __f, _T1&& __t1, _Tn&&... __tn) { (__XVI_STD_NS::forward<_T1>(__t1).*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...); }
constexpr decltype(auto) __invoke_memfn_ref(_F&& __f, _T1&& __t1, _Tn&&... __tn)
    noexcept(noexcept((__XVI_STD_NS::forward<_T1>(__t1).*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...)))
{
    return (__XVI_STD_NS::forward<_T1>(__t1).*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...);
}

template <class _F, class _T1, class... _Tn>
    requires requires(_F&& __f, _T1&& __t1, _Tn&&... __tn) { (__XVI_STD_NS::forward<_T1>(__t1).get().*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...); }
constexpr decltype(auto) __invoke_memfn_refwrapper(_F&& __f, _T1&& __t1, _Tn&&... __tn)
    noexcept(noexcept((__XVI_STD_NS::forward<_T1>(__t1).get().*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...)))
{
    return (__XVI_STD_NS::forward<_T1>(__t1).get().*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...);
}

template <class _F, class _T1, class... _Tn>
    requires requires(_F&& __f, _T1&& __t1, _Tn&&... __tn) { ((*__XVI_STD_NS::forward<_T1>(__t1)).*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...); }
constexpr decltype(auto) __invoke_memfn(_F&& __f, _T1&& __t1, _Tn&&... __tn)
    noexcept(noexcept(((*__XVI_STD_NS::forward<_T1>(__t1)).*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...)))
{
    return ((*__XVI_STD_NS::forward<_T1>(__t1)).*__f)(__XVI_STD_NS::forward<_Tn>(__tn)...);
}

template <class _F, class _T1>
    requires requires(_F&& __f, _T1&& __t1) { __XVI_STD_NS::forward<_T1>(__t1).*__f; }
constexpr decltype(auto) __invoke_memvar_ref(_F&& __f, _T1&& __t1)
    noexcept(noexcept(__XVI_STD_NS::forward<_T1>(__t1).*__f))
{
    return __XVI_STD_NS::forward<_T1>(__t1).*__f;
}

template <class _F, class _T1>
    requires requires(_F&& __f, _T1&& __t1) { __XVI_STD_NS::forward<_T1>(__t1).get().*__f; }
constexpr decltype(auto) __invoke_memvar_refwrapper(_F&& __f, _T1&& __t1)
    noexcept(noexcept(__XVI_STD_NS::forward<_T1>(__t1).get().*__f))
{
    return __XVI_STD_NS::forward<_T1>(__t1).get().*__f;
}

template <class _F, class _T1>
    requires requires(_F&& __f, _T1&& __t1) { (*__XVI_STD_NS::forward<_T1>(__t1)).*__f; }
constexpr decltype(auto) __invoke_memvar(_F&& __f, _T1&& __t1)
    noexcept(noexcept((*__XVI_STD_NS::forward<_T1>(__t1)).*__f))
{
    return (*__XVI_STD_NS::forward<_T1>(__t1)).*__f;
}

template <class _F, class... _Tn>
    requires requires(_F&& __f, _Tn&&... __tn) { __XVI_STD_NS::forward<_F>(__f)(__XVI_STD_NS::forward<_Tn>(__tn)...); }
constexpr decltype(auto) __invoke_other(_F&& __f, _Tn&&... __tn)
    noexcept(noexcept(__XVI_STD_NS::forward<_F>(__f)(__XVI_STD_NS::forward<_Tn>(__tn)...)))
{
    return __XVI_STD_NS::forward<_F>(__f)(__XVI_STD_NS::forward<_Tn>(__tn)...);
}


template <class _F, class _T1, class... _Tn>
consteval int __invoke_select_impl()
{
    if constexpr (is_member_function_pointer<_F>::value)
    {
        using _C = typename __mem_fn_ptr_class<typename __strip_memfn_qualifiers<_F>::type>::type;

        if constexpr (is_base_of<_C, typename remove_reference<_T1>::type>::value)
            return 1;
        else if constexpr (__is_reference_wrapper_specialization<typename remove_cvref<_T1>::type>::value)
            return 2;
        else
            return 3;
    }
    else if constexpr (is_member_object_pointer<_F>::value && sizeof...(_Tn) == 0)
    {
        using _C = typename __mem_obj_ptr_class<_F>::type;

        if constexpr (is_base_of<_C, typename remove_reference<_T1>::type>::value)
            return 4;
        else if constexpr (__is_reference_wrapper_specialization<typename remove_cvref<_T1>::type>::value)
            return 5;
        else
            return 6;
    }
    else
    {
        return 7;
    }
}

template <class _F, class... _Tn>
consteval int __invoke_select()
{
    if constexpr (sizeof...(_Tn) == 0)
        return 7;
    else
        return __invoke_select_impl<_F, _Tn...>();
}


template <class... _Args>
    requires (__invoke_select<_Args...>() == 1)
    && requires(_Args&&... __args) { __invoke_memfn_ref(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) __invoke(_Args&&... __args)
    noexcept(noexcept(__invoke_memfn_ref(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke_memfn_ref(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class... _Args>
    requires (__invoke_select<_Args...>() == 2)
    && requires(_Args&&... __args) { __invoke_memfn_refwrapper(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) __invoke(_Args&&... __args)
    noexcept(noexcept(__invoke_memfn_refwrapper(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke_memfn_refwrapper(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class... _Args>
    requires (__invoke_select<_Args...>() == 3)
    && requires(_Args&&... __args) { __invoke_memfn(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) __invoke(_Args&&... __args)
    noexcept(noexcept(__invoke_memfn(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke_memfn(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class... _Args>
    requires (__invoke_select<_Args...>() == 4)
    && requires(_Args&&... __args) { __invoke_memvar_ref(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) __invoke(_Args&&... __args)
    noexcept(noexcept(__invoke_memvar_ref(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke_memvar_ref(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class... _Args>
    requires (__invoke_select<_Args...>() == 5)
    && requires(_Args&&... __args) { __invoke_memvar_refwrapper(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) __invoke(_Args&&... __args)
    noexcept(noexcept(__invoke_memvar_refwrapper(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke_memvar_refwrapper(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class... _Args>
    requires (__invoke_select<_Args...>() == 6)
    && requires(_Args&&... __args) { __invoke_memvar(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) __invoke(_Args&&... __args)
    noexcept(noexcept(__invoke_memvar(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke_memvar(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class... _Args>
    requires (__invoke_select<_Args...>() == 7)
    && requires(_Args&&... __args) { __invoke_other(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) __invoke(_Args&&... __args)
    noexcept(noexcept(__invoke_other(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke_other(__XVI_STD_NS::forward<_Args>(__args)...);
}


template <class... _Args>
    requires requires(_Args&&... __args) { __invoke(__XVI_STD_NS::forward<_Args>(__args)...); }
constexpr decltype(auto) _INVOKE(_Args&&... __args)
    noexcept(noexcept(__invoke(__XVI_STD_NS::forward<_Args>(__args)...)))
{
    return __invoke(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class _R, class... _Args>
concept __invoke_r_implicit_conv = is_void<_R>::value || requires { [](_Args&&... __args) -> _R { return _INVOKE(__XVI_STD_NS::forward<_Args>(__args)...); }; };

template <class _R, class... _Args>
concept __invoke_r_implicit_conv_nothrow = __invoke_r_implicit_conv<_R, _Args...> && noexcept(_R{_INVOKE(declval<_Args>()...)});

template <class _R, class... _Args>
    requires requires(_Args&&... __args)
    {
        _INVOKE(__XVI_STD_NS::forward<_Args>(__args)...);
    } && __invoke_r_implicit_conv<_R, _Args...>
constexpr _R _INVOKE_R(_Args&&... __args)
    noexcept
    (
        noexcept(_INVOKE(__XVI_STD_NS::forward<_Args>(__args)...))
        && __invoke_r_implicit_conv_nothrow<_R, _Args...>
    )
{
    if constexpr (is_void<_R>::value)
    {
        static_cast<void>(_INVOKE(__XVI_STD_NS::forward<_Args>(__args)...));
    }
    else
    {
        static_assert(!reference_converts_from_temporary<_R, decltype(_INVOKE(__XVI_STD_NS::forward<_Args>(__args)...))>::value);
        return _INVOKE(__XVI_STD_NS::forward<_Args>(__args)...);
    }
}

template <class... _Args>
concept __invocable = requires(_Args&&... __args) { _INVOKE(__XVI_STD_NS::forward<_Args>(__args)...); };

template <class _R, class... _Args>
concept __invocable_r = requires(_Args&&... __args) { _INVOKE_R<_R>(__XVI_STD_NS::forward<_Args>(__args)...); };

template <class... _Args>
concept __nothrow_invocable = __invocable<_Args...> && noexcept(_INVOKE(declval<_Args>()...));

template <class _R, class... _Args>
concept __nothrow_invocable_r = __invocable_r<_R, _Args...> && noexcept(_INVOKE_R<_R>(declval<_Args>()...));


template <class... _Args>
struct __invoke_result {};

template <class... _Args>
    requires __invocable<_Args...>
struct __invoke_result<_Args...>
    { using type = decltype(_INVOKE(declval<_Args>()...)); };


} // namespace __detail


template <class... _Args>
struct is_invocable :
    public bool_constant<__detail::__invocable<_Args...>> {};

template <class _R, class... _Args>
struct is_invocable_r :
    public bool_constant<__detail::__invocable_r<_R, _Args...>> {};

template <class... _Args>
struct is_nothrow_invocable :
    public bool_constant<__detail::__nothrow_invocable<_Args...>> {};

template <class _R, class... _Args>
struct is_nothrow_invocable_r :
    public bool_constant<__detail::__nothrow_invocable_r<_R, _Args...>> {};


template <class... _Args>
inline constexpr auto is_invocable_v = is_invocable<_Args...>::value;

template <class _R, class... _Args>
inline constexpr auto is_invocable_r_v = is_invocable_r<_R, _Args...>::value;

template <class... _Args>
inline constexpr auto is_nothrow_invocable_v = is_nothrow_invocable<_Args...>::value;

template <class _R, class... _Args>
inline constexpr auto is_nothrow_invocable_r_v = is_nothrow_invocable_r<_R, _Args...>::value;


template <class _F, class... _Args>
struct invoke_result : public __detail::__invoke_result<_F, _Args...> {};

template <class _F, class... _Args>
using invoke_result_t = typename invoke_result<_F, _Args...>::type;


} // namespace __XVI_STD_TYPETRAITS_NS


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_INVOKE_H */
