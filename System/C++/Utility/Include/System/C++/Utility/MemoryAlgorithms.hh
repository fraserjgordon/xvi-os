#pragma once
#ifndef __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H
#define __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H


#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/AllocatorTraits.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/Ranges.hh>
#include <System/C++/Utility/Private/AddressOf.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{

template <class _T, class _Alloc, class = void_t<>>
struct __uses_allocator : false_type {};

template <class _T, class _Alloc>
struct __uses_allocator<_T, _Alloc, void_t<typename _T::allocator_type>>
    : is_convertible<_Alloc, typename _T::allocator_type> {};

template <class _T>
struct __is_pair_specialization : false_type {};

template <class _T, class _U>
struct __is_pair_specialization<pair<_T, _U>> : true_type {};

template <class _T>
inline constexpr bool __is_pair_specialization_v = __is_pair_specialization<_T>::value;


template <class _A, class _B>
void __uaca_test_fun(const pair<_A, _B>&);

template <class _T>
concept __uaca_test_fun_valid = __is_pair_specialization_v<_T> && requires (_T&& __t) { __uaca_test_fun(std::forward<_T>(__t)); };

}


template <class _T, class _Alloc>
struct uses_allocator : __detail::__uses_allocator<_T, _Alloc> {};

template <class _T, class _Alloc>
inline constexpr bool uses_allocator_v = uses_allocator<_T, _Alloc>::value;


template <class _T, class _Alloc, class... _Args>
    requires (!__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, _Args&&... __args)
{
    if constexpr (!uses_allocator_v<_T, _Alloc>
                  && is_constructible_v<_T, _Args...>)
    {
        return forward_as_tuple(std::forward<_Args>(__args)...);
    }
    else if constexpr (uses_allocator_v<_T, _Alloc>
                       && is_constructible_v<_T, allocator_arg_t, _Alloc, _Args...>)
    {
        return tuple<allocator_arg_t, const _Alloc&, _Args&&...>(allocator_arg, __alloc, std::forward<_Args>(__args)...);
    }
    else if constexpr (uses_allocator_v<_T, _Alloc>
                       && is_constructible_v<_T, _Args..., _Alloc>)
    {
        return forward_as_tuple(std::forward<_Args>(__args)..., __alloc);
    }
    else
    {
        static_assert(!same_as<_T, _T>, "invalid types for uses_allocator_construction_args");
    }
}

template <class _T, class _Alloc, class _Tuple1, class _Tuple2>
    requires (__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, piecewise_construct_t, _Tuple1&& __x, _Tuple2&& __y)
{
    using _T1 = typename _T::first_type;
    using _T2 = typename _T::second_type;

    return make_tuple
    (
        piecewise_construct,
        apply([&__alloc](auto&&... __args1)
            {
                return uses_allocator_construction_args<_T1>(__alloc, std::forward<decltype(__args1)>(__args1)...);
            }, std::forward<_Tuple1>(__x)),
        apply([&__alloc](auto&&... __args2)
            {
                return uses_allocator_construction_args<_T2>(__alloc, std::forward<decltype(__args2)>(__args2)...);
            }, std::forward<_Tuple2>(__y))
    );
}

template <class _T, class _Alloc>
    requires (__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc)
{
    return uses_allocator_construction_args<_T>(__alloc, piecewise_construct, tuple<>{}, tuple<>{});
}

template <class _T, class _Alloc, class _U, class _V>
    requires (__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, _U&& __u, _V&& __v)
{
    return uses_allocator_construction_args<_T>(__alloc, piecewise_construct, forward_as_tuple(std::forward<_U>(__u)), forward_as_tuple(std::forward<_V>(__v)));
}

template <class _T, class _Alloc, class _U, class _V>
    requires (__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, pair<_U, _V>& __pr)
{
    return uses_allocator_construction_args(__alloc, piecewise_construct, forward_as_tuple(__pr.first), forward_as_tuple(__pr.second));
}

template <class _T, class _Alloc, class _U, class _V>
    requires (__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, const pair<_U, _V>& __pr)
{
    return uses_allocator_construction_args(__alloc, piecewise_construct, forward_as_tuple(__pr.first), forward_as_tuple(__pr.second));
}

template <class _T, class _Alloc, class _U, class _V>
    requires (__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, pair<_U, _V>&& __pr)
{
    return uses_allocator_construction_args(__alloc, piecewise_construct, forward_as_tuple(std::move(__pr.first)), forward_as_tuple(std::move(__pr.second)));
}

template <class _T, class _Alloc, class _U, class _V>
    requires (__detail::__is_pair_specialization_v<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, const pair<_U, _V>&& __pr)
{
    return uses_allocator_construction_args(__alloc, piecewise_construct, forward_as_tuple(std::move(__pr.first)), forward_as_tuple(std::move(__pr.second)));
}


template <class _T, class _Alloc, class... _Args>
constexpr _T make_obj_using_allocator(const _Alloc& __alloc, _Args&&... __args)
{
    return make_from_tuple<_T>(uses_allocator_construction_args<_T>(__alloc, std::forward<_Args>(__args)...));
}


template <class _T, class _Alloc, class _U>
    requires (__detail::__is_pair_specialization_v<_T> && !__detail::__uaca_test_fun_valid<_T>)
constexpr auto uses_allocator_construction_args(const _Alloc& __alloc, _U&& __u) noexcept
{
    class __pair_constructor
    {
    private:

        using __pair_type = std::remove_cv_t<_T>;

        constexpr auto __do_construct(const __pair_type& __p) const
        {
            return make_obj_using_allocator<__pair_type>(_M_alloc, __p);
        }

        constexpr auto __do_construct(__pair_type&& __p) const
        {
            return make_obj_using_allocator<__pair_type>(_M_alloc, std::move(__p));
        }

        _Alloc _M_alloc;
        _U& _M_u;

    public:

        constexpr operator __pair_type() const
        {
            return __do_construct(std::forward<_U>(_M_u));
        }

        constexpr __pair_constructor(const _Alloc& __alloc, _U& __u) :
            _M_alloc{__alloc},
            _M_u{__u}
        {
        }
    };

    return make_tuple(__pair_constructor{__alloc, __u});
}

template <class _T, class _Alloc, class... _Args>
constexpr _T* uninitialized_construct_using_allocator(_T* __p, const _Alloc& __alloc, _Args&&... __args)
{
    return apply([&]<class... _U>(_U&&... __us)
    {
        return construct_at(__p, std::forward<_U>(__us)...);
    }, uses_allocator_construction_args<_T>(__alloc, std::forward<_Args>(__args)...));
}


template <class _T>
void* __voidify(_T& __ptr) noexcept
{
    return const_cast<void*>(static_cast<const volatile void*>(addressof(__ptr)));
}


template <class _ForwardIterator>
void uninitialized_default_construct(_ForwardIterator __first, _ForwardIterator __last)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_first = __first;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __first != __last; ++__first)
            ::new (__voidify(*__first)) _V;
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __first != __initial_first; --__first)
                (*__first).~_V();
        }
        else
        {
            for (; __initial_first != __first; ++__initial_first)
                (*__initial_first).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _ForwardIterator, class _Size>
void uninitialized_default_construct_n(_ForwardIterator __first, _Size __n)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_first = __first;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __n > 0; (void)++__first, --__n)
            ::new (__voidify(*__first)) _V;
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __first != __initial_first; --__first)
                (*__first).~_V();
        }
        else
        {
            for (; __initial_first != __first; ++__initial_first)
                (*__initial_first).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _ForwardIterator>
void uninitialized_value_construct(_ForwardIterator __first, _ForwardIterator __last)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_first = __first;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __first != __last; ++__first)
            ::new (__voidify(*__first)) _V();
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __first != __initial_first; --__first)
                (*__first).~_V();
        }
        else
        {
            for (; __initial_first != __first; ++__initial_first)
                (*__initial_first).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _ForwardIterator, class _Size>
void uninitialized_value_construct_n(_ForwardIterator __first, _Size __n)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_first = __first;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __n > 0; (void)++__first, --__n)
            ::new (__voidify(*__first)) _V;
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __first != __initial_first; --__first)
                (*__first).~_V();
        }
        else
        {
            for (; __initial_first != __first; ++__initial_first)
                (*__initial_first).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_copy(_InputIterator __first, _InputIterator __last, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_result = __result;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __first != __last; ++__result, (void)++__first)
            ::new (__voidify(*__result)) _V(*__first);
        return __result;
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __result != __initial_result; --__result)
                (*__result).~_V();
        }
        else
        {
            for (; __initial_result != __result; ++__initial_result)
                (*__initial_result).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _InputIterator, class _Size, class _ForwardIterator>
_ForwardIterator uninitialized_copy_n(_InputIterator __first, _Size __n, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_result = __result;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __n > 0; ++__result, (void)++__first, --__n)
            ::new (__voidify(*__result)) _V(*__first);
        return __result;
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __result != __initial_result; --__result)
                (*__result).~_V();
        }
        else
        {
            for (; __initial_result != __result; ++__initial_result)
                (*__initial_result).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_move(_InputIterator __first, _InputIterator __last, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_result = __result;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __first != __last; ++__result, (void)++__first)
            ::new (__voidify(*__result)) _V(__XVI_STD_NS::move(*__first));
        return __result;
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __result != __initial_result; --__result)
                (*__result).~_V();
        }
        else
        {
            for (; __initial_result != __result; ++__initial_result)
                (*__initial_result).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _InputIterator, class _Size, class _ForwardIterator>
pair<_InputIterator, _ForwardIterator> uninitialized_move_n(_InputIterator __first, _Size __n, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_result = __result;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __n > 0; ++__result, (void)++__first, --__n)
            ::new (__voidify(*__result)) _V(__XVI_STD_NS::move(*__first));
        return {__first, __result};
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __result != __initial_result; --__result)
                (*__result).~_V();
        }
        else
        {
            for (; __initial_result != __result; ++__initial_result)
                (*__initial_result).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _ForwardIterator, class _T>
void uninitialized_fill(_ForwardIterator __first, _ForwardIterator __last, const _T& __x)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_first = __first;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __first != __last; ++__first)
            ::new (__voidify(*__first)) _V(__x);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __first != __initial_first; --__first)
                (*__first).~_V();
        }
        else
        {
            for (; __initial_first != __first; ++__initial_first)
                (*__initial_first).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}

template <class _ForwardIterator, class _Size, class _T>
void uninitialized_fill_n(_ForwardIterator __first, _Size __n, const _T& __x)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    auto __initial_first = __first;
    __XVI_CXX_UTILITY_TRY
    {
        for (; __n > 0; ++__first, (void)--__n)
            ::new (__voidify(*__first)) _V(__x);
    }
    __XVI_CXX_UTILITY_CATCH(...)
    {
        if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
        {
            for (; __first != __initial_first; --__first)
                (*__first).~_V();
        }
        else
        {
            for (; __initial_first != __first; ++__initial_first)
                (*__initial_first).~_V();
        }

        __XVI_CXX_UTILITY_RETHROW;
    }
}


template <class _T, class... _Args>
    requires requires { ::new (declval<void*>()) _T(declval<_Args>()...); }
constexpr _T* construct_at(_T* __location, _Args&&... __args)
{
    return ::new(__voidify(*__location)) _T(std::forward<_Args>(__args)...);
}


template <class _ForwardIterator> void destroy(_ForwardIterator, _ForwardIterator);

template <class _T>
void destroy_at(_T* __location)
{
    if constexpr (is_array_v<_T>)
        destroy(begin(*__location), end(*__location));
    else
        __location->~_T();
}

template <class _ForwardIterator>
void destroy(_ForwardIterator __first, _ForwardIterator __last)
{
    for (; __first != __last; ++__last)
        destroy_at(addressof(*__first));
}

template <class _ForwardIterator, class _Size>
_ForwardIterator destroy_n(_ForwardIterator __first, _Size __n)
{
    for (; __n > 0; ++__first, (void)--__n)
        destroy_at(addressof(*__first));
    return __first;
}


namespace ranges
{

template <class _I, class _O>
using uninitialized_copy_result = copy_result<_I, _O>;

template <class _I, class _O>
using uninitialized_copy_n_result = uninitialized_copy_result<_I, _O>;

template <class _I, class _O>
using uninitialized_move_result = uninitialized_copy_result<_I, _O>;

template <class _I, class _O>
using uninitialized_move_n_result = uninitialized_move_result<_I, _O>;

template <class _I>
concept __nothrow_input_iterator = input_iterator<_I>
    && is_lvalue_reference_v<iter_reference_t<_I>>
    && same_as<remove_cvref_t<iter_reference_t<_I>>, iter_value_t<_I>>;

template <class _S, class _I>
concept __nothrow_sentinel = sentinel_for<_S, _I>;

template <class _R>
concept __nothrow_input_range = range<_R>
    && __nothrow_input_iterator<iterator_t<_R>>
    && __nothrow_sentinel<sentinel_t<_R>, iterator_t<_R>>;

template <class _I>
concept __nothrow_forward_iterator = __nothrow_input_iterator<_I>
    && forward_iterator<_I>
    && __nothrow_sentinel<_I, _I>;

template <class _R>
concept __nothrow_forward_range = __nothrow_input_range<_R>
    && __nothrow_forward_iterator<iterator_t<_R>>;


struct __uninitialized_default_construct
{
    template <__nothrow_forward_iterator _I, __nothrow_sentinel<_I> _S>
        requires default_initializable<iter_value_t<_I>>
    _I operator()(_I __first, _S __last) const
    {
        using _V = remove_reference_t<iter_reference_t<_I>>;
        
        auto __initial_first = __first;
        __XVI_CXX_UTILITY_TRY
        {
            for (; __first != __last; ++__first)
                ::new (__voidify(*__first)) _V;
            return __first;
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            if constexpr (bidirectional_iterator<_I>)
            {
                for (; __first != __initial_first; --__first)
                    (*__first).~_V();
            }
            else
            {
                for (; __initial_first != __first; ++__initial_first)
                    (*__initial_first).~_V();
            }

            __XVI_CXX_UTILITY_RETHROW;
        }
    }

    template <__nothrow_forward_range _R>
        requires default_initializable<iter_value_t<iterator_t<_R>>>
    safe_iterator_t<_R> operator()(_R&& __r) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r));
    }
};

inline constexpr __uninitialized_default_construct uninitialized_default_construct = {};


struct __uninitialized_default_construct_n
{
    template <__nothrow_forward_iterator _I>
        requires default_initializable<iter_value_t<_I>>
    _I operator()(_I __first, iter_difference_t<_I> __n) const
    {
        return uninitialized_default_construct(counted_iterator(__first, __n), default_sentinel).base();
    }
};

inline constexpr __uninitialized_default_construct_n uninitialized_default_construct_n = {};


struct __uninitialized_value_construct
{
    template <__nothrow_forward_iterator _I, __nothrow_sentinel<_I> _S>
        requires default_initializable<iter_value_t<_I>>
    _I operator()(_I __first, _S __last) const
    {
        using _V = remove_reference_t<iter_reference_t<_I>>;
        
        auto __initial_first = __first;
        __XVI_CXX_UTILITY_TRY
        {
            for (; __first != __last; ++__first)
                ::new (__voidify(*__first)) _V();
            return __first;
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            if constexpr (bidirectional_iterator<_I>)
            {
                for (; __first != __initial_first; --__first)
                    (*__first).~_V();
            }
            else
            {
                for (; __initial_first != __first; ++__initial_first)
                    (*__initial_first).~_V();
            }

            __XVI_CXX_UTILITY_RETHROW;
        }
    }

    template <__nothrow_forward_range _R>
        requires default_initializable<iter_value_t<iterator_t<_R>>>
    safe_iterator_t<_R> operator()(_R&& __r) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r));
    }
};

inline constexpr __uninitialized_value_construct uninitialized_value_construct = {};


struct __uninitialized_value_construct_n
{
    template <__nothrow_forward_iterator _I>
        requires default_initializable<iter_value_t<_I>>
    _I operator()(_I __first, iter_difference_t<_I> __n) const
    {
        return uninitialized_value_construct(counted_iterator(__first, __n), default_sentinel).base();
    }
};

inline constexpr __uninitialized_value_construct_n uninitialized_value_construct_n = {};


struct __uninitialized_copy
{
    template <input_iterator _I, sentinel_for<_I> _S1, __nothrow_forward_iterator _O, __nothrow_sentinel<_O> _S2>
        requires constructible_from<iter_value_t<_O>, iter_reference_t<_I>>
    uninitialized_copy_result<_I, _O> operator()(_I __ifirst, _S1 __ilast, _O __ofirst, _S2 __olast) const
    {
        using _V = remove_reference_t<iter_reference_t<_O>>;
        
        auto __initial_first = __ofirst;
        __XVI_CXX_UTILITY_TRY
        {
            for (; __ifirst != __ilast && __ofirst != __olast; ++__ofirst, (void)++__ifirst)
                ::new (__voidify(*__ofirst)) _V(*__ifirst);
            return {__ifirst, __ofirst};
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            if constexpr (bidirectional_iterator<_I>)
            {
                for (; __ofirst != __initial_first; --__ofirst)
                    (*__ofirst).~_V();
            }
            else
            {
                for (; __initial_first != __ofirst; ++__initial_first)
                    (*__initial_first).~_V();
            }

            __XVI_CXX_UTILITY_RETHROW;
        }
    }

    template <input_range _IR, __nothrow_forward_range _OR>
        requires constructible_from<iter_value_t<iterator_t<_OR>>, iter_reference_t<iterator_t<_IR>>>
    uninitialized_copy_result<safe_iterator_t<_IR>, safe_iterator_t<_OR>> operator()(_IR&& __ir, _OR&& __or) const
    {
        return operator()(ranges::begin(__ir), ranges::end(__ir), ranges::begin(__or), ranges::end(__or));
    }
};

inline constexpr __uninitialized_copy uninitialized_copy = {};


struct __uninitialized_copy_n
{
    template <input_iterator _I, __nothrow_forward_iterator _O, __nothrow_sentinel<_O> _S>
        requires constructible_from<iter_value_t<_O>, iter_reference_t<_I>>
    uninitialized_copy_n_result<_I, _O> operator()(_I __ifirst, iter_difference_t<_I> __n, _O __ofirst, _S __olast) const
    {
        auto __t = uninitialized_copy(counted_iterator(__ifirst, __n), default_sentinel, __ofirst, __olast);
        return {__t.in.base(), __t.out};
    }
};

inline constexpr __uninitialized_copy_n uninitialized_copy_n = {};


struct __uninitialized_move
{
    template <input_iterator _I, sentinel_for<_I> _S1, __nothrow_forward_iterator _O, __nothrow_sentinel<_O> _S2>
        requires constructible_from<iter_value_t<_O>, iter_rvalue_reference_t<_I>>
    uninitialized_move_result<_I, _O> operator()(_I __ifirst, _S1 __ilast, _O __ofirst, _S2 __olast) const
    {
        using _V = remove_reference_t<iter_reference_t<_O>>;
        
        auto __initial_first = __ofirst;
        __XVI_CXX_UTILITY_TRY
        {
            for (; __ifirst != __ilast && __ofirst != __olast; ++__ofirst, (void)++__ifirst)
                ::new (__voidify(*__ofirst)) _V(__XVI_STD_NS::move(*__ifirst));
            return {__ifirst, __ofirst};
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            if constexpr (bidirectional_iterator<_I>)
            {
                for (; __ofirst != __initial_first; --__ofirst)
                    (*__ofirst).~_V();
            }
            else
            {
                for (; __initial_first != __ofirst; ++__initial_first)
                    (*__initial_first).~_V();
            }

            __XVI_CXX_UTILITY_RETHROW;
        }
    }

    template <input_range _IR, __nothrow_forward_range _OR>
        requires constructible_from<iter_value_t<iterator_t<_OR>>, iter_rvalue_reference_t<iterator_t<_IR>>>
    uninitialized_move_result<safe_iterator_t<_IR>, safe_iterator_t<_OR>> operator()(_IR&& __ir, _OR&& __or) const
    {
        return operator()(ranges::begin(__ir), ranges::end(__ir), ranges::begin(__or), ranges::end(__or));
    }
};

inline constexpr __uninitialized_move uninitialized_move = {};


struct __uninitialized_move_n
{
    template <input_iterator _I, __nothrow_forward_iterator _O, __nothrow_sentinel<_O> _S>
        requires constructible_from<iter_value_t<_O>, iter_rvalue_reference_t<_I>>
    uninitialized_move_n_result<_I, _O> operator()(_I __ifirst, iter_difference_t<_I> __n, _O __ofirst, _S __olast) const
    {
        auto __t = uninitialized_copy(counted_iterator(__ifirst, __n), default_sentinel, __ofirst, __olast);
        return {__t.in.base(), __t.out};
    }
};

inline constexpr __uninitialized_move_n uninitialized_move_n = {};


struct __uninitialized_fill
{
    template <__nothrow_forward_iterator _I, __nothrow_sentinel<_I> _S, class _T>
        requires constructible_from<iter_value_t<_I>, const _T&>
    _I operator()(_I __first, _S __last, const _T& __x) const
    {
        using _V = remove_reference_t<iter_reference_t<_I>>;
        
        auto __initial_first = __first;
        __XVI_CXX_UTILITY_TRY
        {
            for (; __first != __last; ++__first)
                ::new (__voidify(*__first)) _V(__x);
            return __first;
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            if constexpr (bidirectional_iterator<_I>)
            {
                for (; __first != __initial_first; --__first)
                    (*__first).~_V();
            }
            else
            {
                for (; __initial_first != __first; ++__initial_first)
                    (*__initial_first).~_V();
            }

            __XVI_CXX_UTILITY_RETHROW;
        }
    }

    template <__nothrow_forward_range _R, class _T>
        requires constructible_from<iter_value_t<iterator_t<_R>>, const _T&>
    safe_iterator_t<_R> operator()(_R&& __r, const _T& __x) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __x);
    }
};

inline constexpr __uninitialized_fill uninitialized_fill = {};


struct __uninitialized_fill_n
{
    template <__nothrow_forward_iterator _I, class _T>
    _I operator()(_I __first, iter_difference_t<_I> __n, const _T& __x) const
    {
        return uninitialized_fill(counted_iterator(__first, __n), default_sentinel, __x);
    }
};

inline constexpr __uninitialized_fill_n uninitialized_fill_n = {};

struct __construct_at
{
    template <class _T, class... _Args>
        requires requires { ::new (declval<void*>()) _T(declval<_Args>()...); }
    constexpr _T* operator()(_T* __location, _Args&&... __args)
    {
        return ::new (__voidify(*__location)) _T(std::forward<_Args>(__args)...);
    }
};

inline constexpr __construct_at construct_at = {};

struct __destroy_at
{
    template <destructible _T>
    void operator()(_T* __location) const noexcept;
};

inline constexpr __destroy_at destroy_at = {};

struct __destroy
{
    template <__nothrow_input_iterator _I, __nothrow_sentinel<_I> _S>
        requires destructible<iter_value_t<_I>>
    _I operator()(_I __first, _S __last) const noexcept
    {
        for (; __first != __last; ++__first)
            destroy_at(addressof(*__first));
        return __first;
    }

    template <__nothrow_input_range _R>
        requires destructible<iter_value_t<iterator_t<_R>>>
    safe_iterator_t<_R> operator()(_R&& __r) const noexcept
    {
        return operator()(ranges::begin(__r), ranges::end(__r));
    }
};

inline constexpr __destroy destroy = {};

template <destructible _T>
void __destroy_at::operator()(_T* __location) const noexcept
{
    if constexpr (is_array_v<_T>)
        ranges::destroy(ranges::begin(*__location), ranges::end(*__location));
    else
        __location->~_T();
}

struct __destroy_n
{
    template <__nothrow_input_iterator _I>
        requires destructible<iter_value_t<_I>>
    _I operator()(_I __first, iter_difference_t<_I> __n) const noexcept
    {
        return destroy(counted_iterator(__first, __n), default_sentinel).base();
    }
};

inline constexpr __destroy_n destroy_n = {};


} // namespace ranges


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H */
