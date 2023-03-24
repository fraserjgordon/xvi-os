#ifndef __SYSTEM_CXX_CORE_MEMORYALGORITHMS_H
#define __SYSTEM_CXX_CORE_MEMORYALGORITHMS_H


#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/IteratorTraits.hh>
#include <System/C++/Core/Pair.hh>
#include <System/C++/Core/Tuple.hh>
#include <System/C++/Core/TupleTraits.hh>


namespace __XVI_STD_CORE_NS_DECL
{


namespace __detail
{

template <class _T, class _Alloc>
struct __uses_allocator : false_type {};

template <class _T, class _Alloc>
    requires requires { typename _T::allocator_type; }
struct __uses_allocator<_T, _Alloc>
    : is_convertible<_Alloc, typename _T::allocator_type> {};


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
    requires (__detail::__is_pair_specialization_v<_T> && !__detail::__uaca_test_fun_valid<_U>)
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
    
    if constexpr (is_nothrow_default_constructible_v<_V>)
    {
        for (; __first != __last; ++__first)
            ::new (__voidify(*__first)) _V;
    }
    else
    {
        auto __initial_first = __first;
        try
        {
            for (; __first != __last; ++__first)
                ::new (__voidify(*__first)) _V;
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _ForwardIterator, class _Size>
void uninitialized_default_construct_n(_ForwardIterator __first, _Size __n)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_default_constructible_v<_V>)
    {
        for (; __n > 0; (void)++__first, --__n)
            ::new (__voidify(*__first)) _V;
    }
    else
    {
        auto __initial_first = __first;
        try
        {
            for (; __n > 0; (void)++__first, --__n)
                ::new (__voidify(*__first)) _V;
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _ForwardIterator>
void uninitialized_value_construct(_ForwardIterator __first, _ForwardIterator __last)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_default_constructible_v<_V>)
    {
        for (; __first != __last; ++__first)
            ::new (__voidify(*__first)) _V();
    }
    else
    {
        auto __initial_first = __first;
        try
        {
            for (; __first != __last; ++__first)
                ::new (__voidify(*__first)) _V();
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _ForwardIterator, class _Size>
void uninitialized_value_construct_n(_ForwardIterator __first, _Size __n)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_default_constructible_v<_V>)
    {
        for (; __n > 0; (void)++__first, --__n)
            ::new (__voidify(*__first)) _V;
    }
    else
    {
        auto __initial_first = __first;
        try
        {
            for (; __n > 0; (void)++__first, --__n)
                ::new (__voidify(*__first)) _V;
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_copy(_InputIterator __first, _InputIterator __last, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_copy_constructible_v<_V>)
    {
        for (; __first != __last; ++__result, (void)++__first)
            ::new (__voidify(*__result)) _V(*__first);
        return __result;
    }
    else
    {
        auto __initial_result = __result;
        try
        {
            for (; __first != __last; ++__result, (void)++__first)
                ::new (__voidify(*__result)) _V(*__first);
            return __result;
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _InputIterator, class _Size, class _ForwardIterator>
_ForwardIterator uninitialized_copy_n(_InputIterator __first, _Size __n, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_copy_constructible_v<_V>)
    {
        for (; __n > 0; ++__result, (void)++__first, --__n)
            ::new (__voidify(*__result)) _V(*__first);
        return __result;
    }
    else
    {
        auto __initial_result = __result;
        try
        {
            for (; __n > 0; ++__result, (void)++__first, --__n)
                ::new (__voidify(*__result)) _V(*__first);
            return __result;
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _InputIterator, class _ForwardIterator>
_ForwardIterator uninitialized_move(_InputIterator __first, _InputIterator __last, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_move_constructible_v<_V>)
    {
        for (;__first != __last; ++__result, (void)++__first)
            ::new (__voidify(*__result)) _V(__XVI_STD_NS::move(*__first));
        return __result;
    }
    else
    {
        auto __initial_result = __result;
        try
        {
            for (; __first != __last; ++__result, (void)++__first)
                ::new (__voidify(*__result)) _V(__XVI_STD_NS::move(*__first));
            return __result;
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _InputIterator, class _Size, class _ForwardIterator>
pair<_InputIterator, _ForwardIterator> uninitialized_move_n(_InputIterator __first, _Size __n, _ForwardIterator __result)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_move_constructible_v<_V>)
    {
        for (; __n > 0; ++__result, (void)++__first, --__n)
            ::new (__voidify(*__result)) _V(__XVI_STD_NS::move(*__first));
        return {__first, __result};
    }
    else
    {
        auto __initial_result = __result;
        try
        {
            for (; __n > 0; ++__result, (void)++__first, --__n)
                ::new (__voidify(*__result)) _V(__XVI_STD_NS::move(*__first));
            return {__first, __result};
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _ForwardIterator, class _T>
void uninitialized_fill(_ForwardIterator __first, _ForwardIterator __last, const _T& __x)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_constructible_v<_V, const _T&>)
    {
        for (; __first != __last; ++__first)
            ::new (__voidify(*__first)) _V(__x);
    }
    else
    {
        auto __initial_first = __first;
        try
        {
            for (; __first != __last; ++__first)
                ::new (__voidify(*__first)) _V(__x);
        }
        catch (...)
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

            throw;
        }
    }
}

template <class _ForwardIterator, class _Size, class _T>
void uninitialized_fill_n(_ForwardIterator __first, _Size __n, const _T& __x)
{
    using _V = typename iterator_traits<_ForwardIterator>::value_type;
    
    if constexpr (is_nothrow_constructible_v<_V, const _T&>)
    {
        for (; __n > 0; ++__first, (void)--__n)
            ::new (__voidify(*__first)) _V(__x);
    }
    else
    {
        auto __initial_first = __first;
        try
        {
            for (; __n > 0; ++__first, (void)--__n)
                ::new (__voidify(*__first)) _V(__x);
        }
        catch (...)
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

            throw;
        }
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


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_MEMORYALGORITHMS_H */
