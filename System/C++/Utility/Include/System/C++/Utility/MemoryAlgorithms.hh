#pragma once
#ifndef __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H
#define __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H


#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/Ranges.hh>
#include <System/C++/Utility/Private/AddressOf.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
        if constexpr (DerivedFrom<typename iterator_traits<_ForwardIterator>::iterator_category, bidirectional_iterator_tag>)
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
concept bool _NoThrowInputIterator = InputIterator<_I>
    && is_lvalue_reference_v<iter_reference_t<_I>>
    && Same<remove_cvref_t<iter_reference_t<_I>>, iter_value_t<_I>>;

template <class _S, class _I>
concept bool _NoThrowSentinel = Sentinel<_S, _I>;

template <class _R>
concept bool _NoThrowInputRange = Range<_R>
    && _NoThrowInputIterator<iterator_t<_R>>
    && _NoThrowSentinel<sentinel_t<_R>, iterator_t<_R>>;

template <class _I>
concept bool _NoThrowForwardIterator = _NoThrowInputIterator<_I>
    && ForwardIterator<_I>
    && _NoThrowSentinel<_I, _I>;

template <class _R>
concept bool _NoThrowForwardRange = _NoThrowInputRange<_R>
    && _NoThrowForwardIterator<iterator_t<_R>>;


struct __uninitialized_default_construct
{
    template <_NoThrowForwardIterator _I, _NoThrowSentinel<_I> _S>
        requires DefaultConstructible<iter_value_t<_I>>
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
            if constexpr (BidirectionalIterator<_I>)
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

    template <_NoThrowForwardRange _R>
        requires DefaultConstructible<iter_value_t<iterator_t<_R>>>
    safe_iterator_t<_R> operator()(_R&& __r) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r));
    }
};

inline constexpr __uninitialized_default_construct uninitialized_default_construct = {};


struct __uninitialized_default_construct_n
{
    template <_NoThrowForwardIterator _I>
        requires DefaultConstructible<iter_value_t<_I>>
    _I operator()(_I __first, iter_difference_t<_I> __n) const
    {
        return uninitialized_default_construct(counted_iterator(__first, __n), default_sentinel).base();
    }
};

inline constexpr __uninitialized_default_construct_n uninitialized_default_construct_n = {};


struct __uninitialized_value_construct
{
    template <_NoThrowForwardIterator _I, _NoThrowSentinel<_I> _S>
        requires DefaultConstructible<iter_value_t<_I>>
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
            if constexpr (BidirectionalIterator<_I>)
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

    template <_NoThrowForwardRange _R>
        requires DefaultConstructible<iter_value_t<iterator_t<_R>>>
    safe_iterator_t<_R> operator()(_R&& __r) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r));
    }
};

inline constexpr __uninitialized_value_construct uninitialized_value_construct = {};


struct __uninitialized_value_construct_n
{
    template <_NoThrowForwardIterator _I>
        requires DefaultConstructible<iter_value_t<_I>>
    _I operator()(_I __first, iter_difference_t<_I> __n) const
    {
        return uninitialized_value_construct(counted_iterator(__first, __n), default_sentinel).base();
    }
};

inline constexpr __uninitialized_value_construct_n uninitialized_value_construct_n = {};


struct __uninitialized_copy
{
    template <InputIterator _I, Sentinel<_I> _S1, _NoThrowForwardIterator _O, _NoThrowSentinel<_O> _S2>
        requires Constructible<iter_value_t<_O>, iter_reference_t<_I>>
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
            if constexpr (BidirectionalIterator<_I>)
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

    template <InputRange _IR, _NoThrowForwardRange _OR>
        requires Constructible<iter_value_t<iterator_t<_OR>>, iter_reference_t<iterator_t<_IR>>>
    uninitialized_copy_result<safe_iterator_t<_IR>, safe_iterator_t<_OR>> operator()(_IR&& __ir, _OR&& __or) const
    {
        return operator()(ranges::begin(__ir), ranges::end(__ir), ranges::begin(__or), ranges::end(__or));
    }
};

inline constexpr __uninitialized_copy uninitialized_copy = {};


struct __uninitialized_copy_n
{
    template <InputIterator _I, _NoThrowForwardIterator _O, _NoThrowSentinel<_O> _S>
        requires Constructible<iter_value_t<_O>, iter_reference_t<_I>>
    uninitialized_copy_n_result<_I, _O> operator()(_I __ifirst, iter_difference_t<_I> __n, _O __ofirst, _S __olast) const
    {
        auto __t = uninitialized_copy(counted_iterator(__ifirst, __n), default_sentinel, __ofirst, __olast);
        return {__t.in.base(), __t.out};
    }
};

inline constexpr __uninitialized_copy_n uninitialized_copy_n = {};


struct __uninitialized_move
{
    template <InputIterator _I, Sentinel<_I> _S1, _NoThrowForwardIterator _O, _NoThrowSentinel<_O> _S2>
        requires Constructible<iter_value_t<_O>, iter_rvalue_reference_t<_I>>
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
            if constexpr (BidirectionalIterator<_I>)
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

    template <InputRange _IR, _NoThrowForwardRange _OR>
        requires Constructible<iter_value_t<iterator_t<_OR>>, iter_rvalue_reference_t<iterator_t<_IR>>>
    uninitialized_move_result<safe_iterator_t<_IR>, safe_iterator_t<_OR>> operator()(_IR&& __ir, _OR&& __or) const
    {
        return operator()(ranges::begin(__ir), ranges::end(__ir), ranges::begin(__or), ranges::end(__or));
    }
};

inline constexpr __uninitialized_move uninitialized_move = {};


struct __uninitialized_move_n
{
    template <InputIterator _I, _NoThrowForwardIterator _O, _NoThrowSentinel<_O> _S>
        requires Constructible<iter_value_t<_O>, iter_rvalue_reference_t<_I>>
    uninitialized_move_n_result<_I, _O> operator()(_I __ifirst, iter_difference_t<_I> __n, _O __ofirst, _S __olast) const
    {
        auto __t = uninitialized_copy(counted_iterator(__ifirst, __n), default_sentinel, __ofirst, __olast);
        return {__t.in.base(), __t.out};
    }
};

inline constexpr __uninitialized_move_n uninitialized_move_n = {};


struct __uninitialized_fill
{
    template <_NoThrowForwardIterator _I, _NoThrowSentinel<_I> _S, class _T>
        requires Constructible<iter_value_t<_I>, const _T&>
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
            if constexpr (BidirectionalIterator<_I>)
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

    template <_NoThrowForwardRange _R, class _T>
        requires Constructible<iter_value_t<iterator_t<_R>>, const _T&>
    safe_iterator_t<_R> operator()(_R&& __r, const _T& __x) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __x);
    }
};

inline constexpr __uninitialized_fill uninitialized_fill = {};


struct __uninitialized_fill_n
{
    template <_NoThrowForwardIterator _I, class _T>
    _I operator()(_I __first, iter_difference_t<_I> __n, const _T& __x) const
    {
        return uninitialized_fill(counted_iterator(__first, __n), default_sentinel, __x);
    }
};

inline constexpr __uninitialized_fill_n uninitialized_fill_n = {};


struct __destroy_at
{
    template <Destructible _T>
    void operator()(_T* __location) const noexcept;
};

inline constexpr __destroy_at destroy_at = {};

struct __destroy
{
    template <_NoThrowInputIterator _I, _NoThrowSentinel<_I> _S>
        requires Destructible<iter_value_t<_I>>
    _I operator()(_I __first, _S __last) const noexcept
    {
        for (; __first != __last; ++__first)
            destroy_at(addressof(*__first));
        return __first;
    }

    template <_NoThrowInputRange _R>
        requires Destructible<iter_value_t<iterator_t<_R>>>
    safe_iterator_t<_R> operator()(_R&& __r) const noexcept
    {
        return operator()(ranges::begin(__r), ranges::end(__r));
    }
};

inline constexpr __destroy destroy = {};

template <Destructible _T>
void __destroy_at::operator()(_T* __location) const noexcept
{
    if constexpr (is_array_v<_T>)
        ranges::destroy(ranges::begin(*__location), ranges::end(*__location));
    else
        __location->~_T();
}

struct __destroy_n
{
    template <_NoThrowInputIterator _I>
        requires Destructible<iter_value_t<_I>>
    _I operator()(_I __first, iter_difference_t<_I> __n) const noexcept
    {
        return destroy(counted_iterator(__first, __n), default_sentinel).base();
    }
};

inline constexpr __destroy_n destroy_n = {};

} // namespace ranges


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H */
