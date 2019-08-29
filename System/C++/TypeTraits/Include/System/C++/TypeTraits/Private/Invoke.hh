#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_INVOKE_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_INVOKE_H


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
constexpr bool __is_invoke_noexcept()
{
    if constexpr (is_member_function_pointer<_F>::value)
    {
        using _C = typename __mem_fn_ptr_class<typename __strip_memfn_qualifiers<_F>::type>::type;

        if constexpr (is_base_of<_C, typename remove_reference<_T1>::type>::value)
            return noexcept((declval<_T1>().*declval<_F>())(declval<_Tn>()...));

        else if constexpr (__is_reference_wrapper_specialization<typename remove_cvref<_T1>::type>::value)
            return noexcept((declval<_T1>().get().*declval<_F>())(declval<_Tn>()...));

        else
            return noexcept(((*declval<_T1>()).*declval<_F>())(declval<_Tn>()...));
    }

    else if constexpr (is_member_object_pointer<_F>::value && sizeof...(_Tn) == 0)
    {
        using _C = typename __mem_obj_ptr_class<_F>::type;

        if constexpr (is_base_of<_C, typename remove_reference<_T1>::type>::value)
            return noexcept(declval<_T1>().*declval<_F>());

        else if constexpr (__is_reference_wrapper_specialization<typename remove_cvref<_T1>::type>::value)
            return noexcept(declval<_T1>().get().*declval<_F>());

        else
            return noexcept((*declval<_T1>()).*declval<_F>());
    }

    else
        return noexcept(declval<_F>()(declval<_T1>(), declval<_Tn>()...));
}


template <class _F, class _T1, class... _Tn>
constexpr int __invoke_type_id_impl()
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
        return 7;
}

template <class _F, class... _Tn>
constexpr int __invoke_type_id()
{
    if constexpr (sizeof...(_Tn) == 0)
        return 7;
    else
        return __invoke_type_id_impl<_F, _Tn...>();
}


template <int _InvokeType, class _Fn, class... Args>
struct __invoke_type;

template <class _F, class _T1, class... _Tn>
struct __invoke_type<1, _F, _T1, _Tn...>
{
    template <class _G, class...>
    using __detector = decltype((declval<_T1>().*declval<_G>())(declval<_Tn>()...));
};

template <class _F, class _T1, class... _Tn>
struct __invoke_type<2, _F, _T1, _Tn...>
{
    template <class _G, class...>
    using __detector = decltype((declval<_T1>().get().*declval<_G>())(declval<_Tn>()...));
};

template <class _F, class _T1, class... _Tn>
struct __invoke_type<3, _F, _T1, _Tn...>
{
    template <class _G, class _U1, class... _Un>
    using __detector = decltype(((*declval<_U1>()).*declval<_G>())(declval<_Un>()...));
};

template <class _F, class _T1>
struct __invoke_type<4, _F, _T1>
{
    template <class _G, class...>
    using __detector = decltype(declval<_T1>().*declval<_G>());
};

template <class _F, class _T1>
struct __invoke_type<5, _F, _T1>
{
    template <class _G, class...>
    using __detector = decltype(declval<_T1>().get().*declval<_G>());
};

template <class _F, class _T1>
struct __invoke_type<6, _F, _T1>
{
    template <class _G, class...>
    using __detector = decltype((*declval<_T1>()).*declval<_G>());
};

template <class _F, class... _Tn>
struct __invoke_type<7, _F, _Tn...>
{
    template <class _G, class...>
    using __detector = decltype(declval<_G>()(declval<_Tn>()...));
};


template <class _Fn, class... _Args>
struct __invoke_helper : __invoke_type<__invoke_type_id<_Fn, _Args...>(), _Fn, _Args...>
{
};

template <class _F, class _T1, class... _Tn>
constexpr decltype(auto) _INVOKE(_F&& __f, _T1&& __t1, _Tn&&... __tn) noexcept(__is_invoke_noexcept<_F, _T1, _Tn...>())
{
    if constexpr (is_member_function_pointer<_F>::value)
    {
        using _C = typename __mem_fn_ptr_class<typename __strip_memfn_qualifiers<_F>::type>::type;

        if constexpr (is_base_of<_C, typename remove_reference<_T1>::type>::value)
            return (forward<_T1>(__t1).*__f)(forward<_Tn>(__tn)...);

        else if constexpr (__is_reference_wrapper_specialization<typename remove_cvref<_T1>::type>::value)
            return (forward<_T1>(__t1).get().*__f)(forward<_Tn>(__tn)...);

        else
            return ((*forward<_T1>(__t1)).*__f)(forward<_Tn>(__tn)...);
    }

    else if constexpr (is_member_object_pointer<_F>::value && sizeof...(_Tn) == 0)
    {
        using _C = typename __mem_obj_ptr_class<_F>::type;

        if constexpr (is_base_of<_C, typename remove_reference<_T1>::type>::value)
            return forward<_T1>(__t1).*__f;

        else if constexpr (__is_reference_wrapper_specialization<typename remove_cvref<_T1>::type>::value)
            return forward<_T1>(__t1).get().*__f;

        else
            return (*forward<_T1>(__t1)).*__f;
    }

    else
        return forward<_F>(__f)(forward<_T1>(__t1), forward<_Tn>(__tn)...);
}

template <class _F>
constexpr auto _INVOKE(_F&& __f) noexcept(noexcept(declval<_F>()()))
{
    return forward<_F>(__f)();
}

template <bool, class _R, class... _Args>
struct __invoke_r_helper
{
    constexpr _R operator()(_Args&&... __args)
        noexcept(noexcept(_INVOKE(declval<_Args>()...)) && is_nothrow_convertible<decltype(_INVOKE(declval<_Args>()...)), _R>::value)
    {
        return _INVOKE(forward<_Args>(__args)...);
    }
};

template <class _R, class... _Args>
struct __invoke_r_helper<true, _R, _Args...>
{
    constexpr void operator()(_Args&&... __args)
        noexcept(noexcept(static_cast<void>(_INVOKE(declval<_Args>()...))))
    {
        static_cast<void>(_INVOKE(forward<_Args>(__args)...));
    }
};

template <class _R, class... _Args>
struct __invoke_r_t : __invoke_r_helper<is_void<typename remove_cv<_R>::type>::value, _R, _Args...> {};

template <class _R, class... _Args>
constexpr auto _INVOKE_R(_Args&&... __args)
    noexcept(noexcept(__invoke_r_t<_R, _Args...>()(declval<_Args>()...)))
{
    return __invoke_r_t<_R, _Args...>()(forward<_Args>(__args)...);
}


template <class _Fn, class... _Args> using __is_invocable_detector = __invoke_helper<_Fn, _Args...>::template __detector<_Fn, _Args...>;

} // namespace __detail


} // namespace __XVI_STD_TYPETRAITS_NS


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_INVOKE_H */
