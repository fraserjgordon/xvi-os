#pragma once
#ifndef __SYSTEM_CXX_UTILITY_BIND_H
#define __SYSTEM_CXX_UTILITY_BIND_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Invoke.hh>
#include <System/C++/Utility/ReferenceWrapper.hh>
#include <System/C++/Utility/Tuple.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _F>
auto not_fn(_F&& __f)
{
    return [__f = std::forward<_F>(__f)]<class... _CallArgs>(_CallArgs&&... __call_args) mutable
    {
        return !invoke(std::forward<_F>(__f), std::forward<_CallArgs>(__call_args)...);
    };
}


template <class _R, class _T>
auto mem_fn(_R _T::* __pm) noexcept
{
    return [__pm]<class... _CallArgs>(_T* __t, _CallArgs&&... __call_args)
    {
        return __detail::_INVOKE(__pm, __t, std::forward<_CallArgs>(__call_args)...);
    };
}


template <class _F, class... _Args>
auto bind_front(_F&& __f, _Args&&... __args)
{
    return [__f = std::forward<_F>(__f), ...__bound_args = unwrap_ref_decay_t<_Args>(__args)]<class... _CallArgs>(_CallArgs&&... __call_args) mutable
    {
        return invoke(std::forward<_F>(__f), std::forward<unwrap_ref_decay_t<_Args>>(__bound_args)..., std::forward<_CallArgs>(__call_args)...);
    };
}


template <class _F, class... _Args>
auto bind_back(_F&& __f, _Args&&... __args)
{
    return [__f = std::forward<_F>(__f), ...__bound_args = unwrap_ref_decay_t<_Args>(__args)]<class... _CallArgs>(_CallArgs&&... __call_args) mutable
    {
        return invoke(std::forward<_F>(__f), std::forward<_CallArgs>(__call_args)..., std::forward<unwrap_ref_decay_t<_Args>>(__bound_args)...);
    };
}


namespace placeholders
{

namespace __detail
{

template <int _N> struct __placeholder {};

} // namespace __detail


inline constexpr __detail::__placeholder<1>  _1{};
inline constexpr __detail::__placeholder<2>  _2{};
inline constexpr __detail::__placeholder<3>  _3{};
inline constexpr __detail::__placeholder<4>  _4{};
inline constexpr __detail::__placeholder<5>  _5{};
inline constexpr __detail::__placeholder<6>  _6{};
inline constexpr __detail::__placeholder<7>  _7{};
inline constexpr __detail::__placeholder<8>  _8{};
inline constexpr __detail::__placeholder<9>  _9{};
inline constexpr __detail::__placeholder<10> _10{};
inline constexpr __detail::__placeholder<11> _11{};
inline constexpr __detail::__placeholder<12> _12{};
inline constexpr __detail::__placeholder<13> _13{};
inline constexpr __detail::__placeholder<14> _14{};
inline constexpr __detail::__placeholder<15> _15{};
inline constexpr __detail::__placeholder<16> _16{};

} // namespace placeholders


template <class _T> struct is_bind_expression : false_type {};

template <class _T> struct is_placeholder : integral_constant<int, 0> {};
template <int _N> struct is_placeholder<placeholders::__detail::__placeholder<_N>> : integral_constant<int, _N> {};

template <class _T> inline constexpr bool is_bind_expression_v = is_bind_expression<_T>::value;
template <class _T> inline constexpr int  is_placeholder_v = is_placeholder<_T>::value;


namespace __detail
{

template <int _I, class _FirstArg, class... _CallArgs>
decltype(auto) __ith_call_arg(_FirstArg&& __f, _CallArgs&&... __args)
{
    if constexpr (_I == 1)
        return std::forward<_FirstArg>(__f);
    else
        return __ith_call_arg<_I - 1>(std::forward<_CallArgs...>(__args)...);
}

template <class _F, class... _BoundArgs>
struct __bind_expression_base
{
    using __fn_t    = std::decay_t<_F>;
    using __tuple_t = tuple<std::decay_t<_BoundArgs>...>;
    
    __fn_t      _M_fn;
    __tuple_t   _M_bound_args;

    template <class _FD, int _I, class... _CallArgs> using _V = decltype
    (
        []()
        {
            using _TDi = tuple_element_t<_I, __tuple_t>;
            if constexpr (__detail::__is_reference_wrapper_specialization<_TDi>::value)
                return declval<typename _TDi::type&>();
            else if constexpr (is_bind_expression_v<_TDi>)
                return declval<invoke_result_t<typename __copy_cv<_FD, _TDi>::type&, _CallArgs...>>();
            else if constexpr (is_placeholder_v<_TDi>)
                return declval<typename __nth_type<is_placeholder_v<_TDi>, _CallArgs...>::type&&>();
            else
                return declval<typename __copy_cv<_FD, _TDi>::type&>();
        }()
    );

    template <int _I, class _FD, class _Bound, class... _CallArgs>
    static _V<_FD, _I, _CallArgs...> __v(_Bound&& __bound_args, _CallArgs&&... __call_args)
    {
        using _TDi = tuple_element_t<_I, __tuple_t>;
        _TDi& __tdi = get<_I>(__bound_args);
        
        if constexpr (__detail::__is_reference_wrapper_specialization<_TDi>::value)
            return __tdi.get();
        else if constexpr (is_bind_expression_v<_TDi>)
            return __tdi(std::forward<_CallArgs>(__call_args)...);
        else if constexpr (is_placeholder_v<_TDi>)
            return __ith_call_arg<is_placeholder_v<_TDi>>(std::forward<_CallArgs>(__call_args)...);
        else
            return __tdi;
    }

    template <class _FD, class _Bound, class... _CallArgs, int... _Idx>
    static auto __make_arg_tuple_helper(integer_sequence<int, _Idx...>, _Bound&& __bound_args, _CallArgs&&... __call_args)
    {
        using __ret_t = tuple<decltype(__v<_Idx, _FD>(__bound_args, std::forward<_CallArgs>(__call_args)...))...>;
        return __ret_t(__v<_Idx, _FD>(__bound_args, std::forward<_CallArgs>(__call_args)...)...);
    }

    template <class _FD, class _Bound, class... _CallArgs>
    static auto __make_arg_tuple(_FD&&, _Bound&& __bound_args, _CallArgs&&... __call_args)
    {
        return __make_arg_tuple_helper<_FD>(make_integer_sequence<int, tuple_size_v<__tuple_t>>(), __bound_args, std::forward<_CallArgs>(__call_args)...);
    }
};

template <class _F, class... _BoundArgs>
struct __bind_expression : __bind_expression_base<_F, _BoundArgs...>
{
    using __base = __bind_expression_base<_F, _BoundArgs...>;

    template <class... _CallArgs>
    decltype(auto) operator()(_CallArgs&&... __call_args)
    {
        return apply(__base::_M_fn, __base::__make_arg_tuple(__base::_M_fn, __base::_M_bound_args, std::forward<_CallArgs>(__call_args)...));
    }

    template <class... _CallArgs>
    decltype(auto) operator()(_CallArgs&&... __call_args) const
    {
        return apply(__base::_M_fn, __base::__make_arg_tuple(__base::_M_fn, __base::_M_bound_args, std::forward<_CallArgs>(__call_args)...));
    }
};

template <class _R, class _F, class... _BoundArgs>
struct __bind_r_expression : __bind_expression_base<_F, _BoundArgs...>
{
       using __base = __bind_expression_base<_F, _BoundArgs...>;

    template <class... _CallArgs>
    decltype(auto) operator()(_CallArgs&&... __call_args)
    {
        return __apply_r<_R>(__base::_M_fn, __base::__make_arg_tuple(__base::_M_fn, __base::_M_bound_args, std::forward<_CallArgs>(__call_args)...));
    }

    template <class... _CallArgs>
    decltype(auto) operator()(_CallArgs&&... __call_args) const
    {
        return __apply_r<_R>(__base::_M_fn, __base::__make_arg_tuple(__base::_M_fn, __base::_M_bound_args, std::forward<_CallArgs>(__call_args)...));
    }
};

} // namespace __detail


template <class _F, class... _BoundArgs>
struct is_bind_expression<__detail::__bind_expression<_F, _BoundArgs...>> : true_type {};

template <class _R, class _F, class... _BoundArgs>
struct is_bind_expression<__detail::__bind_r_expression<_R, _F, _BoundArgs...>> : true_type {};


template <class _F, class... _BoundArgs>
auto bind(_F&& __f, _BoundArgs&&... __bound_args)
{
    return __detail::__bind_expression<_F, _BoundArgs...>{std::forward<_F>(__f), {std::forward<_BoundArgs>(__bound_args)...}};
}

template <class _R, class _F, class... _BoundArgs>
auto bind(_F&& __f, _BoundArgs&&... __bound_args)
{
    return __detail::__bind_r_expression<_R, _F, _BoundArgs...>{std::forward<_F>(__f), {std::forward<_BoundArgs>(__bound_args)...}};
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_BIND_H */
