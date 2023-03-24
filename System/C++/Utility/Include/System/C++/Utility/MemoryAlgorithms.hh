#pragma once
#ifndef __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H
#define __SYSTEM_CXX_UTILITY_MEMORYALGORITHMS_H


#include <System/C++/Core/MemoryAlgorithms.hh>

#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/AllocatorTraits.hh>
#include <System/C++/Utility/Pair.hh>
#include <System/C++/Utility/RangesAlgorithm.hh>
#include <System/C++/Utility/Private/AddressOf.hh>


namespace __XVI_STD_UTILITY_NS
{

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
    borrowed_iterator_t<_R> operator()(_R&& __r) const
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
    borrowed_iterator_t<_R> operator()(_R&& __r) const
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
    uninitialized_copy_result<borrowed_iterator_t<_IR>, borrowed_iterator_t<_OR>> operator()(_IR&& __ir, _OR&& __or) const
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
    uninitialized_move_result<borrowed_iterator_t<_IR>, borrowed_iterator_t<_OR>> operator()(_IR&& __ir, _OR&& __or) const
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
    borrowed_iterator_t<_R> operator()(_R&& __r, const _T& __x) const
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
    borrowed_iterator_t<_R> operator()(_R&& __r) const noexcept
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
