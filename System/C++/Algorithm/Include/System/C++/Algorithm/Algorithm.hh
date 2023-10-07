#ifndef __SYSTEM_CXX_ALGORITHM_ALGORITHM_H
#define __SYSTEM_CXX_ALGORITHM_ALGORITHM_H


#include <System/C++/LanguageSupport/Compare.hh>

#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Bit.hh>
#include <System/C++/Core/FunctionalUtils.hh>
#include <System/C++/Core/IteratorAdaptors.hh>
#include <System/C++/Core/IteratorUtils.hh>
#include <System/C++/Core/Pair.hh>
#include <System/C++/Core/Swap.hh>

#include <System/C++/Algorithm/Private/Config.hh>


namespace __XVI_STD_ALGORITHM_NS_DECL
{


template <class _InputIterator, class _Predicate>
constexpr bool all_of(_InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first) == false)
            return false;
    return true;
}

template <class _InputIterator, class _Predicate>
constexpr bool any_of(_InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first) != false)
            return true;
    return false;
}

template <class _InputIterator, class _Predicate>
constexpr bool none_of(_InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first) != false)
            return false;
    return true;
}

template <class _InputIterator, class _Function>
constexpr _Function for_each(_InputIterator __first, _InputIterator __last, _Function __f)
{
    for (; __first != __last; ++__first)
        __f(*__first);
    return __XVI_STD_NS::move(__f);
}

template <class _InputIterator, class _Size, class _Function>
constexpr _InputIterator for_each_n(_InputIterator __first, _Size __n, _Function __f)
{
    for (; __n > 0; ++__first, (void)--__n)
        __f(*__first);
    return __first;
}

template <class _InputIterator, class _T>
constexpr _InputIterator find(_InputIterator __first, _InputIterator __last, const _T& __value)
{
    for (; __first != __last; ++__first)
        if (*__first == __value)
            return __first;
    return __last;
}

template <class _InputIterator, class _Predicate>
constexpr _InputIterator find_if(_InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first) != false)
            return __first;
    return __last;
}

template <class _InputIterator, class _Predicate>
constexpr _InputIterator find_if_not(_InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first) == false)
            return __first;
    return __last;
}

template <class _InputIterator, class _ForwardIterator>
constexpr _InputIterator find_first_of(_InputIterator __first1, _InputIterator __last1, _ForwardIterator __first2, _ForwardIterator __last2)
{
    for (; __first1 != __last1; ++__first1)
    {
        for (_ForwardIterator __i = __first2; __i != __last2; ++__i)
            if (*__first1 == *__i)
                return __first1;
    }
    
    return __last1;
}

template <class _InputIterator, class _ForwardIterator, class _BinaryPredicate>
constexpr _InputIterator find_first_of(_InputIterator __first1, _InputIterator __last1, _ForwardIterator __first2, _ForwardIterator __last2, _BinaryPredicate __pred)
{
    for (; __first1 != __last1; ++__first1)
    {
        for (_ForwardIterator __i = __first2; __i != __last2; ++__i)
            if (__pred(*__first1, *__i) != false)
                return __first1;
    }

    return __last1;
}

template <class _ForwardIterator>
constexpr _ForwardIterator adjacent_find(_ForwardIterator __first, _ForwardIterator __last)
{
    if (__first == __last)
        return __last;

    for (_ForwardIterator __second = __XVI_STD_NS::next(__first); __second != __last; ++__first, (void)++__second)
        if (*__first == *__second)
            return __first;

    return __last;
}

template <class _ForwardIterator, class _BinaryPredicate>
constexpr _ForwardIterator adjacent_find(_ForwardIterator __first, _ForwardIterator __last, _BinaryPredicate __pred)
{
    if (__first == __last)
        return __last;

    for (_ForwardIterator __second = __XVI_STD_NS::next(__first); __second != __last; ++__first, (void)++__second)
        if (__pred(*__first, *__second) != false)
            return __first;

    return __last;
}

template <class _InputIterator, class _T>
constexpr typename iterator_traits<_InputIterator>::difference_type count(_InputIterator __first, _InputIterator __last, const _T& __x)
{
    typename iterator_traits<_InputIterator>::difference_type __n = 0;
    for (; __first != __last; ++__first)
        if (*__first == __x)
            ++__n;

    return __n;
}

template <class _InputIterator, class _Predicate>
constexpr typename iterator_traits<_InputIterator>::difference_type count_if(_InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    typename iterator_traits<_InputIterator>::difference_type __n = 0;
    for (; __first != __last; ++__first)
        if (__pred(*__first))
            ++__n;

    return __n;
}

template <class _InputIterator1, class _InputIterator2>
constexpr pair<_InputIterator1, _InputIterator2> mismatch(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2)
{
    for (; __first1 != __last1; ++__first1, (void)++__first2)
        if (!(*__first1 == *__first2))
            return {__first1, __first2};

    return {__first1, __first2};
}

template <class _InputIterator1, class _InputIterator2>
constexpr pair<_InputIterator1, _InputIterator2> mismatch(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2)
{
    for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
        if (!(*__first1 == *__first2))
            return {__first1, __first2};

    return {__first1, __first2};
}

template <class _InputIterator1, class _InputIterator2, class _BinaryPredicate>
constexpr pair<_InputIterator1, _InputIterator2> mismatch(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _BinaryPredicate __pred)
{
    for (; __first1 != __last1; ++__first1, (void)++__first2)
        if (__pred(*__first1, *__first2) == false)
            return {__first1, __first2};

    return {__first1, __first2};
}

template <class _InputIterator1, class _InputIterator2, class _BinaryPredicate>
constexpr pair<_InputIterator1, _InputIterator2> mismatch(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _BinaryPredicate __pred)
{
    for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
        if (__pred(*__first1, *__first2) == false)
            return {__first1, __first2};

    return {__first1, __first2};
}

template <class _InputIterator1, class _InputIterator2, class _BinaryPredicate>
constexpr bool equal(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _BinaryPredicate __pred)
{
    for (; __first1 != __last1; ++__first1, (void)++__first2)
        if (__pred(*__first1, *__first2) == false)
            return false;

    return true;
}

template <class _InputIterator1, class _InputIterator2>
constexpr bool equal(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2)
{
    return __XVI_STD_ALGORITHM_NS::equal(__first1, __last1, __first2, equal_to{});
}

template <class _InputIterator1, class _InputIterator2, class _BinaryPredicate>
constexpr bool equal(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _BinaryPredicate __pred)
{
    if constexpr (is_base_of_v<random_access_iterator_tag, typename iterator_traits<_InputIterator1>::iterator_category>
                  && is_base_of_v<random_access_iterator_tag, typename iterator_traits<_InputIterator2>::iterator_category>)
    {
        if (__XVI_STD_NS::distance(__first1, __last1) != __XVI_STD_NS::distance(__first2, __last2))
            return false;
    }

    for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
        if (__pred(*__first1, *__first2) == false)
            return false;

    return true;
}

template <class _InputIterator1, class _InputIterator2>
constexpr bool equal(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2)
{
    return __XVI_STD_ALGORITHM_NS::equal(__first1, __last1, __first2, __last2, equal_to{});
}

template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
constexpr _ForwardIterator1 find_end(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2, _BinaryPredicate __pred)
{
    if (__first2 == __last2)
        return __last1;

    _ForwardIterator1 __result = __last1;

    for (; __first1 != __last1; ++__first1)
    {
        auto [__i, __j] = __XVI_STD_ALGORITHM_NS::mismatch(__first1, __last1, __first2, __last2, __pred);
        if (__j == __last2)
            __result = __first1;
        if (__i == __last1)
            break;
    }

    return __result;
}

template <class _ForwardIterator1, class _ForwardIterator2>
constexpr _ForwardIterator1 find_end(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2)
{
    return __XVI_STD_ALGORITHM_NS::find_end(__first1, __last1, __first2, __last2, equal_to{});
}

template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
constexpr bool is_permutation(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2, _BinaryPredicate __pred)
{
        if constexpr (is_base_of_v<random_access_iterator_tag, typename iterator_traits<_ForwardIterator1>::iterator_category>
                  && is_base_of_v<random_access_iterator_tag, typename iterator_traits<_ForwardIterator2>::iterator_category>)
    {
        if (__XVI_STD_NS::distance(__first1, __last1) != __XVI_STD_NS::distance(__first2, __last2))
            return false;
    }

    auto [__start1, __start2] = __XVI_STD_ALGORITHM_NS::mismatch(__first1, __last1, __first2, __last2, __pred);
    if (__start1 == __last1 && __start2 == __last2)
        return true;

    for (; __start1 != __last1; ++__start1)
    {
        auto __n1 = __XVI_STD_ALGORITHM_NS::count_if(__start1, __last1, [__pred, __start1 = __start1]<class _T>(_T&& __t)
        {
            return __pred(*__start1, std::forward<_T>(__t));
        });

        auto __n2 = __XVI_STD_ALGORITHM_NS::count_if(__start2, __last2, [__pred, __start1 = __start1]<class _T>(_T&& __t)
        {
            return __pred(*__start1, std::forward<_T>(__t));
        });

        if (__n1 != __n2)
            return false;
    }

    return true;
}

template <class _ForwardIterator1, class _ForwardIterator2>
constexpr bool is_permutation(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2)
{
    return __XVI_STD_ALGORITHM_NS::is_permutation(__first1, __last1, __first2, __last2, equal_to{});
}

template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
constexpr bool is_permutation(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _BinaryPredicate __pred)
{
    return __XVI_STD_ALGORITHM_NS::is_permutation(__first1, __last1, __first2, __XVI_STD_NS::next(__first2, __XVI_STD_NS::distance(__first1, __first2)), __pred);
}

template <class _ForwardIterator1, class _ForwardIterator2>
constexpr bool is_permutation(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2)
{
    return __XVI_STD_ALGORITHM_NS::is_permutation(__first1, __last1, __first2, equal_to{});
}

template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
constexpr _ForwardIterator1 search(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2, _BinaryPredicate __pred)
{
    if (__first2 == __last2)
        return __first1;
    
    for (; __first1 != __last1; ++__first1)
    {
        if (auto [__miss1, __miss2] = __XVI_STD_ALGORITHM_NS::mismatch(__first1, __last1, __first2, __last2, __pred); __miss2 == __last2)
            return __first1;
    }

    return __last1;
}

template <class _ForwardIterator1, class _ForwardIterator2>
constexpr _ForwardIterator1 search(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2, _ForwardIterator2 __last2)
{
    return __XVI_STD_ALGORITHM_NS::search(__first1, __last1, __first2, __last2, equal_to{});
}

template <class _ForwardIterator, class _Size, class _T, class _BinaryPredicate>
constexpr _ForwardIterator search_n(_ForwardIterator __first, _ForwardIterator __last, _Size __n, const _T& __x, _BinaryPredicate __pred)
{
    while (__first != __last)
    {
        _ForwardIterator __iter = __first;
        _Size __i = 0;
        for (; __i < __n && __iter != __last; ++__i, ++__iter)
            if (__pred(*__iter, __x) == false)
                break;

        if (__i == __n)
            return __first;

        __first = __iter;
    }

    return __last;
}

template <class _ForwardIterator, class _Size, class _T>
constexpr _ForwardIterator search_n(_ForwardIterator __first, _ForwardIterator __last, _Size __n, const _T& __x)
{
    return __XVI_STD_ALGORITHM_NS::search_n(__first, __last, __n, __x, equal_to{});
}

template <class _InputIterator, class _OutputIterator>
constexpr _OutputIterator copy(_InputIterator __first, _InputIterator __last, _OutputIterator __result)
{
    for (; __first != __last; ++__first, (void)++__result)
        *__result = *__first;

    return __result;
}

template <class _InputIterator, class _Size, class _OutputIterator>
constexpr _OutputIterator copy_n(_InputIterator __first, _Size __n, _OutputIterator __result)
{
    for (; __n > 0; --__n, (void)++__first, ++__result)
        *__result = *__first;

    return __result;
}

template <class _InputIterator, class _OutputIterator, class _Predicate>
constexpr _OutputIterator copy_if(_InputIterator __first, _InputIterator __last, _OutputIterator __result, _Predicate __pred)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first))
        {
            *__result = *__first;
            ++__result;
        }

    return __result;
}

template <class _BidirectionalIterator1, class _BidirectionalIterator2>
constexpr _BidirectionalIterator2 copy_backward(_BidirectionalIterator1 __first, _BidirectionalIterator1 __last, _BidirectionalIterator2 __result)
{
    for (; __first != __last; --__last, (void)--__result)
        *__XVI_STD_NS::prev(__result) = *__XVI_STD_NS::prev(__last);

    return __result;
}

template <class _InputIterator, class _OutputIterator>
constexpr _OutputIterator move(_InputIterator __first, _InputIterator __last, _OutputIterator __result)
{
    for (; __first != __last; ++__first, (void)++__result)
        *__result = __XVI_STD_NS::move(*__first);

    return __result;
}

template <class _BidirectionalIterator1, class _BidirectionalIterator2>
constexpr _BidirectionalIterator2 move_backward(_BidirectionalIterator1 __first, _BidirectionalIterator1 __last, _BidirectionalIterator2 __result)
{
    for (; __first != __last; --__last, (void)--__result)
        *__XVI_STD_NS::prev(__result) = __XVI_STD_NS::move(*__XVI_STD_NS::prev(__last));

    return __result;
}

template <class _ForwardIterator1, class _ForwardIterator2>
constexpr _ForwardIterator2 swap_ranges(_ForwardIterator1 __first1, _ForwardIterator1 __last1, _ForwardIterator2 __first2)
{
    using __XVI_STD_NS::swap;
    for (; __first1 != __last1; ++__first1, (void)++__first2)
        swap(*__first1, *__first2);

    return __first2;
}

template <class _ForwardIterator1, class _ForwardIterator2>
constexpr void iter_swap(_ForwardIterator1 __a, _ForwardIterator2 __b)
{
    using __XVI_STD_NS::swap;
    swap(*__a, *__b);
}

template <class _InputIterator, class _OutputIterator, class _UnaryOperation>
constexpr _OutputIterator transform(_InputIterator __first, _InputIterator __last, _OutputIterator __result, _UnaryOperation __op)
{
    for (; __first != __last; ++__first, (void)++__result)
        *__result = __op(*__first);

    return __result;
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator, class _BinaryOperation>
constexpr _OutputIterator transform(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _OutputIterator __result, _BinaryOperation __op)
{
    for (; __first1 != __last1; ++__first1, (void)++__first2, ++__result)
        *__result = __op(*__first1, *__first2);
    
    return __result;
}

template <class _ForwardIterator, class _T>
constexpr void replace(_ForwardIterator __first, _ForwardIterator __last, const _T& __old, const _T& __new)
{
    for (; __first != __last; ++__first)
        if (*__first == __old)
            *__first = __new;
}

template <class _ForwardIterator, class _Predicate, class _T>
constexpr void replace_if(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred, const _T& __value)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first))
            *__first = __value;
}

template <class _InputIterator, class _OutputIterator, class _T>
constexpr _OutputIterator replace_copy(_InputIterator __first, _InputIterator __last, _OutputIterator __result, const _T& __old, const _T& __new)
{
    for (; __first != __last; ++__first, (void)++__result)
        if (!(*__first == __old))
            *__result = *__first;
        else
            *__result = __new;

    return __result;
}

template <class _InputIterator, class _OutputIterator, class _Predicate, class _T>
constexpr _OutputIterator replace_copy_if(_InputIterator __first, _InputIterator __last, _OutputIterator __result, _Predicate __pred, const _T& __value)
{
    for (; __first != __last; ++__first, (void)++__result)
        if (__pred(*__first) == false)
            *__result = *__first;
        else
            *__result = __value;

    return __result;
}

template <class _ForwardIterator, class _T>
constexpr void fill(_ForwardIterator __first, _ForwardIterator __last, const _T& __value)
{
    for (; __first != __last; ++__first)
        *__first = __value;
}

template <class _OutputIterator, class _Size, class _T>
constexpr _OutputIterator fill_n(_OutputIterator __result, _Size __n, const _T& __value)
{
    for (; __n > 0; --__n, (void)++__result)
        *__result = __value;

    return __result;
}

template <class _ForwardIterator, class _Generator>
constexpr void generate(_ForwardIterator __first, _ForwardIterator __last, _Generator __gen)
{
    for (; __first != __last; ++__first)
        *__first = __gen();
}

template <class _OutputIterator, class _Size, class _Generator>
constexpr _OutputIterator generate_n(_OutputIterator __result, _Size __n, _Generator __gen)
{
    for (; __n > 0; --__n, (void)++__result)
        *__result = __gen();

    return __result;
}

template <class _ForwardIterator, class _T>
constexpr _ForwardIterator remove(_ForwardIterator __first, _ForwardIterator __last, const _T& __value)
{
    _ForwardIterator __result = __first;
    for (; __first != __last; ++__first)
    {
        if (*__first == __value)
            continue;

        if (__first != __result)
            *__result = __XVI_STD_NS::move(*__first);

        ++__result;
    }

    return __result;
}

template <class _ForwardIterator, class _Predicate>
constexpr _ForwardIterator remove_if(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred)
{
    _ForwardIterator __result = __first;
    for (; __first != __last; ++__first)
    {
        if (__pred(*__first) != false)
            continue;

        if (__first != __result)
            *__result = __XVI_STD_NS::move(*__first);

        ++__result;
    }

    return __result;
}

template <class _InputIterator, class _OutputIterator, class _T>
constexpr _OutputIterator remove_copy(_InputIterator __first, _InputIterator __last, _OutputIterator __result, const _T& __value)
{
    for (; __first != __last; ++__first)
    {
        if (*__first == __value)
            continue;

        *__result = *__first;
        ++__result;
    }

    return __result;
}

template <class _InputIterator, class _OutputIterator, class _Predicate>
constexpr _OutputIterator remove_copy_if(_InputIterator __first, _InputIterator __last, _OutputIterator __result, _Predicate __pred)
{
    for (; __first != __last; ++__first)
    {
        if (__pred(*__first) != false)
            continue;

        *__result = *__first;
        ++__result;
    }

    return __result;
}

template <class _ForwardIterator, class _BinaryPredicate>
constexpr _ForwardIterator unique(_ForwardIterator __first, _ForwardIterator __last, _BinaryPredicate __pred)
{
    _ForwardIterator __cmp = __first;
    _ForwardIterator __result = __first;

    for (; __first != __last; ++__first)
    {
        if (__first == __cmp || __pred(*__first, *__cmp) != false)
            continue;

        if (__result != __first)
            *__result = __XVI_STD_NS::move(*__first);

        __cmp = __result;
        ++__result;
    }

    return __result;
}

template <class _ForwardIterator>
constexpr _ForwardIterator unique(_ForwardIterator __first, _ForwardIterator __last)
{
    return unique(__first, __last, equal_to{});
}

template <class _InputIterator, class _OutputIterator, class _BinaryPredicate>
constexpr _OutputIterator unique_copy(_InputIterator __first, _InputIterator __last, _OutputIterator __result, _BinaryPredicate __pred)
{
    if (__first == __last)
        return __result;
    
    if constexpr (is_base_of_v<forward_iterator_tag, typename iterator_traits<_InputIterator>::iterator_category>)
    {
        _InputIterator __cmp = __first;
        for (; __first != __last; ++__first)
        {
            if (__first == __cmp || __pred(*__first, *__cmp) != false)
                continue;

            *__result = *__first;
            __cmp = __first;
            ++__result;
        }
    }
    else if constexpr (is_base_of_v<forward_iterator_tag, typename iterator_traits<_OutputIterator>::iterator_category>
                       && is_same_v<typename iterator_traits<_InputIterator>::value_type, typename iterator_traits<_OutputIterator>::value_type>)
    {
        *__result = *__first;
        ++__first;

        for (; __first != __last; ++__first)
        {
            if (__pred(*__first, *__result) != false)
                continue;

            ++__result;
            *__result = *__first;
        }

        ++__result;
    }
    else
    {
        typename iterator_traits<_InputIterator>::value_type __t = *__first;
        *__result = *__first;
        ++__result;
        ++__first;

        for (; __first != __last; ++__first)
        {
            if (__pred(*__first, __t) != false)
                continue;

            __t = *__first;
            *__result = __t;
            ++__result;
        }
    }

    return __result;
}

template <class _InputIterator, class _OutputIterator>
constexpr _OutputIterator unique_copy(_InputIterator __first, _InputIterator __last, _OutputIterator __result)
{
    return __XVI_STD_ALGORITHM_NS::unique_copy(__first, __last, __result, equal_to{});
}

template <class _BidirectionalIterator>
constexpr void reverse(_BidirectionalIterator __first, _BidirectionalIterator __last)
{
    --__last;
    for (; __first != __last; ++__first, (void)--__last)
    {
        __XVI_STD_NS::iter_swap(__first, __last);
        if (next(__first) == __last)
            break;
    }
}

template <class _BidirectionalIterator, class _OutputIterator>
constexpr _OutputIterator reverse_copy(_BidirectionalIterator __first, _BidirectionalIterator __last, _OutputIterator __result)
{
    for (; __first != __last; --__last, (void)++__result)
        *__result = *__XVI_STD_NS::prev(__last);
    return __result;
}

template <class _ForwardIterator>
constexpr _ForwardIterator rotate(_ForwardIterator __first, _ForwardIterator __middle, _ForwardIterator __last)
{
    if (__first == __middle)
        return __last;
    if (__middle == __last)
        return __first;

    _ForwardIterator __read = __middle;
    _ForwardIterator __write = __first;
    _ForwardIterator __victim_ptr = __first;

    for (; __read != __last; ++__read, (void)++__write)
    {
        __XVI_STD_NS::iter_swap(__read, __write);
        
        if (__write == __victim_ptr)
            __victim_ptr = __read;
    }

    __XVI_STD_ALGORITHM_NS::rotate(__write, __victim_ptr, __read);

    return __write;
}

template <class _ForwardIterator, class _OutputIterator>
constexpr _OutputIterator rotate_copy(_ForwardIterator __first, _ForwardIterator __middle, _ForwardIterator __last, _OutputIterator __result)
{
    for (_ForwardIterator __i = __middle; __i != __last; ++__i, (void)++__result)
        *__result = *__i;
    for (_ForwardIterator __i = __first; __i != __middle; ++__i, (void)++__result)
        *__result = *__i;

    return __result;
}

template <class _ForwardIterator>
constexpr _ForwardIterator shift_left(_ForwardIterator __first, _ForwardIterator __last, typename iterator_traits<_ForwardIterator>::difference_type __n)
{
    if (__n <= 0)
        return __last;
    
    _ForwardIterator __read = __first;
    for (; __n > 0; --__n)
        if (++__read == __last)
            return __first;

    for (; __read != __last; ++__read, (void)++__first)
        *__first = __XVI_STD_NS::move(*__read);

    return __first;
}

template <class _ForwardIterator>
constexpr _ForwardIterator shift_right(_ForwardIterator __first, _ForwardIterator __last, typename iterator_traits<_ForwardIterator>::difference_type __n)
{
    if (__n <= 0)
        return __first;

    if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
    {
        _ForwardIterator __read = __last;
        for (; __n > 0; --__n)
            if (--__read == __first)
                return __last;

        return __XVI_STD_NS::move_backward(__first, __read, __last);
    }
    else
    {
        _ForwardIterator __read = __first;
        _ForwardIterator __write = __XVI_STD_NS::next(__first, __n);
        _ForwardIterator __pivot = __write;

        for (; __write != __last; ++__read, (void)++__write)
        {
            if (__read == __pivot)
                __read = __first;

            __XVI_STD_NS::iter_swap(__read, __write);
        }

        return __pivot;
    }
}


template <class _ForwardIterator, class _Compare>
constexpr _ForwardIterator is_sorted_until(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
    if (__first == __last)
        return __last;

    _ForwardIterator __second = next(__first);
    for (; __second != __last; ++__first, (void)++__second)
        if (__comp(*__second, *__first) != false)
            return __second;

    return __last;
}

template <class _ForwardIterator>
constexpr _ForwardIterator is_sorted_until(_ForwardIterator __first, _ForwardIterator __last)
{
    return __XVI_STD_ALGORITHM_NS::is_sorted_until(__first, __last, less{});
}

template <class _ForwardIterator, class _Compare>
constexpr bool is_sorted(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
    return __XVI_STD_ALGORITHM_NS::is_sorted_until(__first, __last, __comp) == __last;
}

template <class _ForwardIterator>
constexpr bool is_sorted(_ForwardIterator __first, _ForwardIterator __last)
{
    return __XVI_STD_ALGORITHM_NS::is_sorted(__first, __last, less{});
}

template <class _ForwardIterator, class _T, class _Compare>
constexpr _ForwardIterator lower_bound(_ForwardIterator __first, _ForwardIterator __last, const _T& __value, _Compare __comp)
{
    auto __length = __XVI_STD_NS::distance(__first, __last);   
    while (true)
    {
        if (__first == __last)
            return __first;

        auto __mid = __length / 2;
        auto __middle = __XVI_STD_NS::next(__first, __mid);

        if (bool(__XVI_STD_NS::invoke(__comp, *__middle, __value)))
        {
            __first = __XVI_STD_NS::next(__middle);
            __length -= (__mid + 1);
        }
        else
        {
            __last = __middle;
            __length = __mid;
        }
    }
}

template <class _ForwardIterator, class _T>
constexpr _ForwardIterator lower_bound(_ForwardIterator __first, _ForwardIterator __last, const _T& __value)
{
    return __XVI_STD_ALGORITHM_NS::lower_bound(__first, __last, __value, less{});
}

template <class _ForwardIterator, class _T, class _Compare>
constexpr _ForwardIterator upper_bound(_ForwardIterator __first, _ForwardIterator __last, const _T& __value, _Compare __comp)
{
    auto __length = __XVI_STD_NS::distance(__first, __last);   
    while (true)
    {
        if (__first == __last)
            return __first;

        auto __mid = __length / 2;
        auto __middle = __XVI_STD_NS::next(__first, __mid);

        if (!bool(__XVI_STD_NS::invoke(__comp, __value, *__middle)))
        {
            __first = __XVI_STD_NS::next(__middle);
            __length -= (__mid + 1);
        }
        else
        {
            __last = __middle;
            __length = __mid;
        }
    }
}

template <class _ForwardIterator, class _T>
constexpr _ForwardIterator upper_bound(_ForwardIterator __first, _ForwardIterator __last, const _T& __value)
{
    return __XVI_STD_ALGORITHM_NS::upper_bound(__first, __last, __value, less{});
}

template <class _ForwardIterator, class _T, class _Compare>
constexpr pair<_ForwardIterator, _ForwardIterator> equal_range(_ForwardIterator __first, _ForwardIterator __last, const _T& __value, _Compare __comp)
{
    return {__XVI_STD_ALGORITHM_NS::lower_bound(__first, __last, __value, __comp), __XVI_STD_ALGORITHM_NS::upper_bound(__first, __last, __value, __comp)};
}

template <class _ForwardIterator, class _T>
constexpr pair<_ForwardIterator, _ForwardIterator> equal_range(_ForwardIterator __first, _ForwardIterator __last, const _T& __value)
{
    return __XVI_STD_ALGORITHM_NS::equal_range(__first, __last, __value, less{});
}

template <class _ForwardIterator, class _T, class _Compare>
constexpr bool binary_search(_ForwardIterator __first, _ForwardIterator __last, const _T& __value, _Compare __comp)
{
    auto __length = __XVI_STD_NS::distance(__first, __last);   
    while (true)
    {
        if (__first == __last)
            return false;

        auto __mid = __length / 2;
        auto __middle = __XVI_STD_NS::next(__first, __mid);

        if (bool(__XVI_STD_NS::invoke(__comp, *__middle, __value)))
        {
            __first = __XVI_STD_NS::next(__middle);
            __length -= (__mid + 1);
        }
        else if (bool(__XVI_STD_NS::invoke(__comp, __value, *__middle)))
        {
            __last = __middle;
            __length = __mid;
        }
        else
        {
            return true;
        }
    }
}

template <class _ForwardIterator, class _T>
constexpr bool binary_search(_ForwardIterator __first, _ForwardIterator __last, const _T& __value)
{
    return __XVI_STD_ALGORITHM_NS::binary_search(__first, __last, __value, less{});
}

template <class _InputIterator, class _Predicate>
constexpr bool is_partitioned(_InputIterator __first, _InputIterator __last, _Predicate __pred)
{
    for (; __first != __last; ++__first)
        if (__pred(*__first) == false)
            break;

    for (; __first != __last; ++__first)
        if (__pred(*__first) != false)
            return false;

    return true;
}

template <class _ForwardIterator, class _Predicate>
constexpr _ForwardIterator partition(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred)
{
    if constexpr (is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<_ForwardIterator>::iterator_category>)
    {
        // Hoare partition.
        while (true)
        {
            for (; __first != __last && bool(__XVI_STD_NS::invoke(__pred, *__first)) != false; ++__first)
                ;

            for (; __last != __first && bool(__XVI_STD_NS::invoke(__pred, *__XVI_STD_NS::prev(__last))) == false; --__last)
                ;

            if (__first == __last)
                return __first;

            __XVI_STD_NS::iter_swap(__first, __XVI_STD_NS::prev(__last));
            ++__first;

            if (__first != __last)
                --__last;
        }
    }
    else
    {
        // Lomuto partition.
        _ForwardIterator __i = __first;
        _ForwardIterator __j = __first;

        for (; __j != __last; ++__j)
        {
            if (bool(__XVI_STD_NS::invoke(__pred, *__j)))
            {
                __XVI_STD_NS::iter_swap(__i, __j);
                ++__i;
            }
        }

        return __i;
    }
}

template <class _InputIterator, class _OutputIterator1, class _OutputIterator2, class _Predicate>
constexpr pair<_OutputIterator1, _OutputIterator2> partition_copy(_InputIterator __first, _InputIterator __last, _OutputIterator1 __out_true, _OutputIterator2 __out_false, _Predicate __pred)
{
    for (; __first != __last; ++__first)
    {
        if (bool(__XVI_STD_NS::invoke(__pred, *__first)))
        {
            *__out_true = *__first;
            ++__out_true;
        }
        else
        {
            *__out_false = *__first;
            ++__out_false;
        }
    }

    return {__out_true, __out_false};
}

template <class _BidirectionalIterator, class _Size, class _Predicate>
constexpr _BidirectionalIterator __stable_partition(_BidirectionalIterator __first, _BidirectionalIterator __last, _Size __n, _Predicate& __pred, pair<void*, size_t> __buffer)
{
    // Skip any elements already in the right place.
    for (; __first != __last; ++__first, (void)--__n)
    {
        if (!bool(__XVI_STD_NS::invoke(__pred, *__first)))
            break;
    }
    
    if (__n <= 1)
        return __first;

    if (__n == 2)
    {
        _BidirectionalIterator __second = __XVI_STD_NS::next(__first);
        if (bool(__XVI_STD_NS::invoke(__pred, *__second)))
        {
            __XVI_STD_NS::iter_swap(__first, __second);
            return __second;
        }
        else
        {
            return __first;
        }
    }

    // Is buffer big enough?
    using _T = typename iterator_traits<_BidirectionalIterator>::value_type;
    if (__buffer.second >= (__n * sizeof(_T)))
    {
        // Move false elements into the buffer, shift true elements to beginning.
        _T* __arr = static_cast<_T*>(__buffer.first);
        size_t __false_count = 0;
        _BidirectionalIterator __read = __first;
        _BidirectionalIterator __write = __first;
        for (; __read != __last; ++__read)
        {
            if (bool(__XVI_STD_NS::invoke(__pred, *__read)))
            {
                if (__read != __write)
                    __XVI_STD_NS::iter_swap(__read, __write);
                ++__write;
            }
            else
            {
                new (&__arr[__false_count]) _T(__XVI_STD_NS::move(*__read));
                ++__false_count;
            }
        }

        // Move the false elements back.
        _BidirectionalIterator __boundary = __write;
        for (size_t __i = 0; __write != __last; (void)++__write, ++__i)
        {
            *__write = __XVI_STD_NS::move(__arr[__i]);
            __arr[__i].~_T();
        }

        return __boundary;
    }

    // Temporary buffer not available; split into two and recurse.
    _Size __half = __n / 2;
    _BidirectionalIterator __middle = __XVI_STD_NS::next(__first, __half);
    _BidirectionalIterator __bound1 = __XVI_STD_ALGORITHM_NS::__stable_partition(__first, __middle, __half, __pred, __buffer);
    _BidirectionalIterator __bound2 = __XVI_STD_ALGORITHM_NS::__stable_partition(__middle, __last, __n - __half, __pred, __buffer);

    // We now have something that looks like:
    //  TTTTTTFFFFFFTTTTTTFFFFFF
    //  ^     ^     ^     ^     ^
    //  first |     mid   |     last
    //        bound1      bound2
    //
    // By rotating the section between bound1 and bound2 around mid we get:
    // TTTTTTTTTTTTFFFFFFFFFFFF
    //
    return __XVI_STD_ALGORITHM_NS::rotate(__bound1, __middle, __bound2);
}

template <class _BidirectionalIterator, class _Predicate>
constexpr _BidirectionalIterator stable_partition(_BidirectionalIterator __first, _BidirectionalIterator __last, _Predicate __pred)
{
    auto __n = __XVI_STD_NS::distance(__first, __last);

    // Attempt to allocate a temporary buffer as large as possible.
    using _T = typename iterator_traits<_BidirectionalIterator>::value_type;
    pair<void*, size_t> __buffer = {nullptr, size_t{0}};
    if (!is_constant_evaluated())
    {
        // Note: arbitrary minimum size
        size_t __size = static_cast<size_t>(__n) * sizeof(_T);
        while (__size >= 4 * sizeof(_T))
        {
            void* __ptr = new (std::nothrow) __XVI_STD_ALGORITHM_NS::byte[__size];
            if (__ptr)
            {
                __buffer.first = __ptr;
                __buffer.second = __size;
                break;
            }

            __size /= 2;
        }
    }

    return __XVI_STD_ALGORITHM_NS::__stable_partition(__first, __last, __n, __pred, __buffer);
}

template <class _ForwardIterator, class _Predicate>
constexpr _ForwardIterator partition_point(_ForwardIterator __first, _ForwardIterator __last, _Predicate __pred)
{
    auto __length = __XVI_STD_NS::distance(__first, __last);   
    while (true)
    {
        if (__first == __last)
            return __first;

        auto __mid = __length / 2;
        auto __middle = __XVI_STD_NS::next(__first, __mid);

        if (bool(__XVI_STD_NS::invoke(__pred, *__middle)))
        {
            __first = __XVI_STD_NS::next(__middle);
            __length -= (__mid + 1);
        }
        else
        {
            __last = __middle;
            __length = __mid;
        }
    }
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator, class _Compare>
constexpr _OutputIterator merge(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result, _Compare __comp)
{
    for (; __first1 != __last1 && __first2 != __last2; ++__result)
    {
        if (bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1)))
        {
            *__result = *__first2;
            ++__first2;
        }
        else
        {
            *__result = *__first1;
            ++__first1;
        }
    }

    for (; __first1 != __last1; ++__first1, (void)++__result)
        *__result = *__first1;

    for (; __first2 != __last2; ++__first2, (void)++__result)
        *__result = *__first2;

    return __result;
}

//! @TODO: implement.
template <class _BidirectionalIterator, class _Size, class _Compare>
constexpr void __inplace_merge(_BidirectionalIterator __first, _BidirectionalIterator __middle, _BidirectionalIterator __last, _Size __n1, _Size __n2, _Compare __comp, pair<void*, size_t> __buffer);

template <class _BidirectionalIterator, class _Compare>
constexpr void inplace_merge(_BidirectionalIterator __first, _BidirectionalIterator __middle, _BidirectionalIterator __last, _Compare __comp)
{
    auto __n1 = __XVI_STD_NS::distance(__first, __middle);
    auto __n2 = __XVI_STD_NS::distance(__middle, __last);
    auto __n = __n1 + __n2;

    // Attempt to allocate a temporary buffer as large as possible.
    using _T = typename iterator_traits<_BidirectionalIterator>::value_type;
    pair<void*, size_t> __buffer = {nullptr, size_t{0}};
    if (!is_constant_evaluated())
    {
        // Note: arbitrary minimum size
        size_t __size = static_cast<size_t>(__n) * sizeof(_T);
        while (__size >= 4 * sizeof(_T))
        {
            void* __ptr = new (std::nothrow) __XVI_STD_ALGORITHM_NS::byte[__size];
            if (__ptr)
            {
                __buffer.first = __ptr;
                __buffer.second = __size;
                break;
            }

            __size /= 2;
        }
    }

    __XVI_STD_ALGORITHM_NS::__inplace_merge(__first, __middle, __last, __n1, __n2, __comp, __buffer);
}

template <class _BidirectionalIterator>
constexpr void inplace_merge(_BidirectionalIterator __first, _BidirectionalIterator __middle, _BidirectionalIterator __last)
{
    __XVI_STD_ALGORITHM_NS::inplace_merge(__first, __middle, __last, less{});
}


template <class _InputIterator1, class _InputIterator2, class _Compare>
constexpr bool includes(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _Compare __comp)
{
    for (; __first2 != __last2; ++__first2)
    {
        while (__first1 != __last1 && bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2)))
            ++__first1;

        if (__first1 == __last1)
            return false;

        bool __equal = !bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1));
        if (!__equal)
            return false;

        ++__first1;
        while (__first1 != __last1 && !bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1)))
            ++__first1;
    }

    return true;
}

template <class _InputIterator1, class _InputIterator2>
constexpr bool includes(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2)
{
    return includes(__first1, __last1, __first2, __last2, equal_to{});
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator, class _Compare>
constexpr _OutputIterator set_union(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result, _Compare __comp)
{
    while (true)
    {
        while (__first1 != __last1 && (__first2 == __last2 || bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2))))
        {
            *__result = *__first1;
            ++__first1;
            ++__result;
        }

        while (__first1 != __last1
              && __first2 != __last2
              && !bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1))
              && !bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2)))
        {
            *__result = *__first1;
            ++__first1;
            ++__first2;
            ++__result;
        }

        while (__first2 != __last2 && (__first1 == __last1 || bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1))))
        {
            *__result = *__first2;
            ++__first2;
            ++__result;
        }

        if (__first1 == __last1 && __first2 == __last2)
            break;
    }

    return __result;
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator>
constexpr _OutputIterator set_union(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result)
{
    return __XVI_STD_ALGORITHM_NS::set_union(__first1, __last1, __first2, __last2, __result, less{});
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator, class _Compare>
constexpr _OutputIterator set_intersection(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result, _Compare __comp)
{
    while (true)
    {
        while (__first1 != __last1
               && __first2 != __last2
               && bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2)))
        {
            ++__first1;
        }

        while (__first1 != __last1
              && __first2 != __last2
              && !bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1))
              && !bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2)))
        {
            *__result = *__first1;
            ++__first1;
            ++__first2;
            ++__result;
        }

        while (__first2 != __last2
               && __first1 != __last1
               && bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1)))
        {
            ++__first2;
        }

        if (__first1 == __last1 || __first2 == __last2)
            break;
    }

    return __result;
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator>
constexpr _OutputIterator set_intersection(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result)
{
    return __XVI_STD_ALGORITHM_NS::set_intersection(__first1, __last1, __first2, __last2, __result, less{});
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator, class _Compare>
constexpr _OutputIterator set_difference(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result, _Compare __comp)
{
    while (true)
    {
        while (__first1 != __last1 && (__first2 == __last2 || bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2))))
        {
            *__result = *__first1;
            ++__first1;
            ++__result;
        }

        while (__first1 != __last1
              && __first2 != __last2
              && !bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1))
              && !bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2)))
        {
            ++__first1;
            ++__first2;
        }

        while (__first2 != __last2 && (__first1 == __last1 || bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1))))
        {
            ++__first2;
        }

        if (__first1 == __last1 && __first2 == __last2)
            break;
    }

    return __result;
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator>
constexpr _OutputIterator set_difference(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result)
{
    return __XVI_STD_ALGORITHM_NS::set_difference(__first1, __last1, __first2, __last2, __result, less{});
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator, class _Compare>
constexpr _OutputIterator set_symmetric_difference(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result, _Compare __comp)
{
    while (true)
    {
        while (__first1 != __last1 && (__first2 == __last2 || bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2))))
        {
            *__result = *__first1;
            ++__first1;
            ++__result;
        }

        while (__first1 != __last1
              && __first2 != __last2
              && !bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1))
              && !bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2)))
        {
            ++__first1;
            ++__first2;
        }

        while (__first2 != __last2 && (__first1 == __last1 || bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1))))
        {
            *__result = *__first2;
            ++__first2;
            ++__result;
        }

        if (__first1 == __last1 && __first2 == __last2)
            break;
    }

    return __result;
}

template <class _InputIterator1, class _InputIterator2, class _OutputIterator>
constexpr _OutputIterator set_symmetric_difference(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _OutputIterator __result)
{
    return __XVI_STD_ALGORITHM_NS::set_symmetric_difference(__first1, __last1, __first2, __last2, __result, less{});
}


template <class _T>
constexpr _T __heap_left(_T __i)
{
    return 2*__i + 1;
}

template <class _T>
constexpr _T __heap_right(_T __i)
{
    return 2*__i + 2;
}

template <class _T>
constexpr _T __heap_parent(_T __i)
{
    return (__i - 1) / 2;
}

template <class _RandomAccessIterator, class _Compare>
constexpr bool __heapify(_RandomAccessIterator __first, _RandomAccessIterator __last, typename iterator_traits<_RandomAccessIterator>::difference_type __index, _Compare __comp)
{
    auto __length = (__last - __first);
    
    auto __left = __heap_left(__index);
    auto __right = __heap_right(__index);
    auto __largest = __index;

    if (__left < __length && __XVI_STD_NS::invoke(__comp, __first[__largest], __first[__left]))
        __largest = __left;

    if (__right < __length && __XVI_STD_NS::invoke(__comp, __first[__largest], __first[__right]))
        __largest = __right;

    if (__largest != __index)
    {
        __XVI_STD_NS::iter_swap(__first + __index, __first + __largest);
        __XVI_STD_ALGORITHM_NS::__heapify(__first, __last, __largest, __comp);
        return true;
    }

    return false;
}

template <class _RandomAccessIterator, class _Compare>
constexpr size_t __bad_heap_index(_RandomAccessIterator __first, _RandomAccessIterator __last, typename iterator_traits<_RandomAccessIterator>::difference_type __index, _Compare __comp)
{
    auto __length = (__last - __first);
    
    auto __left = __heap_left(__index);
    auto __right = __heap_right(__index);
    auto __largest = __index;

    if (__left < __length && __XVI_STD_NS::invoke(__comp, __first[__largest], __first[__left]))
        return __left;

    if (__right < __length && __XVI_STD_NS::invoke(__comp, __first[__largest], __first[__right]))
        return __right;

    if (__left < __length)
        if (auto __bad = __bad_heap_index(__first, __last, __left, __comp); __bad != 0)
            return __bad;

    if (__right < __length)
        if (auto __bad = __bad_heap_index(__first, __last, __right, __comp); __bad != 0)
            return __bad;

    return 0;
}

template <class _RandomAccessIterator, class _Compare>
constexpr void push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    auto __i = (__last - __first) - 1;
    if (__i == 0)
        return;
    
    auto __parent = __XVI_STD_ALGORITHM_NS::__heap_parent(__i);
    while (true)
    {
        bool __continue = __XVI_STD_ALGORITHM_NS::__heapify(__first, __last, __parent, __comp);

        if (!__continue || __parent == 0)
            break;

        __parent = __XVI_STD_ALGORITHM_NS::__heap_parent(__parent);
    }
}

template <class _RandomAccessIterator>
constexpr void push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    __XVI_STD_ALGORITHM_NS::push_heap(__first, __last, less{});
}

template <class _RandomAccessIterator, class _Compare>
constexpr void pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    __XVI_STD_NS::iter_swap(__first, __last - 1);
    __XVI_STD_ALGORITHM_NS::__heapify(__first, __last - 1, 0, __comp);
}

template <class _RandomAccessIterator>
constexpr void pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    __XVI_STD_ALGORITHM_NS::pop_heap(__first, __last, less{});
}

template <class _RandomAccessIterator, class _Compare>
constexpr void make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    auto __len = (__last - __first);
    for (decltype(__len) __i = 0; __i < __len; ++__i)
        __XVI_STD_ALGORITHM_NS::__heapify(__first, __last, __len - __i - 1, __comp);
}

template <class _RandomAccessIterator>
constexpr void make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    __XVI_STD_ALGORITHM_NS::make_heap(__first, __last, less{});
}

template <class _RandomAccessIterator, class _Compare>
constexpr void sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    auto __len = (__last - __first);
    for (decltype(__len) __i = 0; __i < __len; ++__i)
    {
        __XVI_STD_ALGORITHM_NS::pop_heap(__first, __last, __comp);
        --__last;
    }

    __XVI_STD_ALGORITHM_NS::reverse(__first, __last);
}

template <class _RandomAccessIterator>
constexpr void sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    __XVI_STD_ALGORITHM_NS::sort_heap(__first, __last, less{});
}

template <class _RandomAccessIterator, class _Compare>
constexpr _RandomAccessIterator is_heap_until(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    auto __bad_index = __XVI_STD_ALGORITHM_NS::__bad_heap_index(__first, __last, 0, __comp);
    return (__bad_index == 0) ? __last : __first + __bad_index;
}

template <class _RandomAccessIterator>
constexpr _RandomAccessIterator is_heap_until(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    return __XVI_STD_ALGORITHM_NS::is_heap_until(__first, __last, less{});
}

template <class _RandomAccessIterator, class _Compare>
constexpr bool is_heap(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    return __XVI_STD_ALGORITHM_NS::is_heap_until(__first, __last, __comp) == __last;
}

template <class _RandomAccessIterator>
constexpr bool is_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    return __XVI_STD_ALGORITHM_NS::is_heap(__first, __last, less{});
}


template <class _T, class _Compare>
constexpr const _T& min(const _T& __a, const _T& __b, _Compare __comp)
{
    if (bool(__XVI_STD_NS::invoke(__comp, __b, __a)))
        return __b;
    return __a;
}

template <class _T>
constexpr const _T& min(const _T& __a, const _T& __b)
{
    return __XVI_STD_ALGORITHM_NS::min(__a, __b, less{});
}

template <class _T, class _Compare>
constexpr _T min(std::initializer_list<_T> __il, _Compare __comp)
{
    _T __min = *__il.begin();

    for (auto __i = __il.begin() + 1; __i != __il.end(); ++__i)
        __min = __XVI_STD_ALGORITHM_NS::min(__min, *__i, __comp);

    return __min;
}

template <class _T>
constexpr _T min(std::initializer_list<_T> __il)
{
    return __XVI_STD_ALGORITHM_NS::min(__il, less{});
}

template <class _T, class _Compare>
constexpr const _T& max(const _T& __a, const _T& __b, _Compare __comp)
{
    if (bool(__XVI_STD_NS::invoke(__comp, __a, __b)))
        return __b;
    return __a;
}

template <class _T>
constexpr const _T& max(const _T& __a, const _T& __b)
{
    return __XVI_STD_ALGORITHM_NS::max(__a, __b, less{});
}

template <class _T, class _Compare>
constexpr _T max(std::initializer_list<_T> __il, _Compare __comp)
{
    const _T& __max = *__il.begin();

    for (auto __i = __il.begin() + 1; __i != __il.end(); ++__i)
        __max = __XVI_STD_ALGORITHM_NS::max(__max, *__i, __comp);

    return __max;
}

template <class _T>
constexpr _T max(std::initializer_list<_T> __il)
{
    return __XVI_STD_ALGORITHM_NS::max(__il, less{});
}

template <class _T, class _Compare>
constexpr pair<const _T&, const _T&> minmax(const _T& __a, const _T& __b, _Compare __comp)
{
    if (bool(__XVI_STD_NS::invoke(__comp, __b, __a)))
        return {__b, __a};
    return {__a, __b};
}

template <class _T>
constexpr pair<const _T&, const _T&> minmax(const _T& __a, const _T& __b)
{
    return __XVI_STD_ALGORITHM_NS::minmax(__a, __b, less{});
}

template <class _T, class _Compare>
constexpr pair<_T, _T> minmax(std::initializer_list<_T> __il, _Compare __comp)
{
    const _T& __min = *__il.begin();
    const _T& __max = *__il.begin();

    for (auto __i = __il.begin() + 1; __i != __il.end(); ++__i)
        if (bool(__XVI_STD_NS::invoke(__comp, *__i, __min)))
            __min = *__i;
        else if (bool(__XVI_STD_NS::invoke(__comp, __max, *__i)))
            __max = *__i;

    return {__min, __max};
}

template <class _T>
constexpr pair<_T, _T> minmax(std::initializer_list<_T> __il)
{
    return __XVI_STD_ALGORITHM_NS::minmax(__il, less{});
}

template <class _ForwardIterator, class _Compare>
constexpr _ForwardIterator min_element(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
    if (__first == __last)
        return __first;

    _ForwardIterator __min = __first;
    ++__first;

    for (; __first != __last; ++__first)
        if (bool(__XVI_STD_NS::invoke(__comp, *__first, *__min)))
            __min = __first;

    return __min;
}

template <class _ForwardIterator>
constexpr _ForwardIterator min_element(_ForwardIterator __first, _ForwardIterator __last)
{
    return __XVI_STD_ALGORITHM_NS::min_element(__first, __last, less{});
}

template <class _ForwardIterator, class _Compare>
constexpr _ForwardIterator max_element(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
    if (__first == __last)
        return __first;

    _ForwardIterator __max = __first;
    ++__first;

    for (; __first != __last; ++__first)
        if (bool(__XVI_STD_NS::invoke(__comp, *__max, *__first)))
            __max = __first;

    return __max;
}

template <class _ForwardIterator>
constexpr _ForwardIterator max_element(_ForwardIterator __first, _ForwardIterator __last)
{
    return __XVI_STD_ALGORITHM_NS::max_element(__first, __last, less{});
}

template <class _ForwardIterator, class _Compare>
constexpr pair<_ForwardIterator, _ForwardIterator> minmax_element(_ForwardIterator __first, _ForwardIterator __last, _Compare __comp)
{
    if (__first == __last)
        return {__first, __first};

    _ForwardIterator __min = __first;
    _ForwardIterator __max = __first;
    ++__first;

    for (; __first != __last; ++__first)
        if (bool(__XVI_STD_NS::invoke(__comp, *__first, *__min)))
            __min = __first;
        else if (!bool(__XVI_STD_NS::invoke(__comp, *__first, *__max)))
            __max = __first;

    return {__min, __max};
}

template <class _ForwardIterator>
constexpr pair<_ForwardIterator, _ForwardIterator> minmax_element(_ForwardIterator __first, _ForwardIterator __last)
{
    return __XVI_STD_ALGORITHM_NS::minmax_element(__first, __last, less{});
}

template <class _T, class _Compare>
constexpr const _T& clamp(const _T& __value, const _T& __lo, const _T& __hi, _Compare __comp)
{
    if (bool(__XVI_STD_NS::invoke(__comp, __value, __lo)))
        return __lo;
    else if (bool(__XVI_STD_NS::invoke(__comp, __hi, __value)))
        return __hi;
    return __value;
}

template <class _T>
constexpr const _T& clamp(const _T& __value, const _T& __lo, const _T& __hi)
{
    return __XVI_STD_ALGORITHM_NS::clamp(__value, __lo, __hi, less{});
}

template <class _InputIterator1, class _InputIterator2, class _Compare>
constexpr bool lexicographical_compare(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _Compare __comp)
{
    for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
    {
        if (bool(__XVI_STD_NS::invoke(__comp, *__first1, *__first2)))
            return true;
        else if (bool(__XVI_STD_NS::invoke(__comp, *__first2, *__first1)))
            return false;
    }

    return __first1 == __last1 && __first2 != __last2;
}

template <class _InputIterator1, class _InputIterator2, class _Compare>
constexpr auto lexicographical_compare_three_way(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2, _Compare __comp)
    -> common_comparison_category_t<decltype(__comp(*__first1, *__first2)), strong_ordering>
{
    for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
        if (auto __c = __comp(*__first1, *__first2); __c != 0)
            return __c;

    return __first1 != __last1 ? strong_ordering::greater
         : __first2 != __last2 ? strong_ordering::less
         : strong_ordering::equal;
}

template <class _InputIterator1, class _InputIterator2>
constexpr auto lexicographical_compare_three_way(_InputIterator1 __first1, _InputIterator1 __last1, _InputIterator2 __first2, _InputIterator2 __last2)
{
    return lexicographical_compare_three_way(__first1, __last1, __first2, __last2, less{});
}


template <class _RandomAccessIterator, class _Compare>
constexpr void __insertion_sort(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    // Zero- and one-element arrays are (trivially) sorted.
    auto __n = (__last - __first);
    if (__n < 2)
        return;

    for (decltype(__n) __i = 1; __i < __n; ++__i)
    {
        using _T = typename iterator_traits<_RandomAccessIterator>::value_type;
        _T __tmp = __XVI_STD_NS::move(__first[__i]);

        auto __j = __i;
        for (; __j > 0; --__j)
        {
            if (!bool(__XVI_STD_NS::invoke(__comp, __tmp, __first[__j-1])))
                break;
            
            __first[__j] = __XVI_STD_NS::move(__first[__j - 1]);
        }

        __first[__j] = __XVI_STD_NS::move(__tmp);
    }
}

template <class _RandomAccessIterator, class _Compare>
constexpr _RandomAccessIterator __median_of_three(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    // Note: modifies the array!
    
    auto __n = (__last - __first);
    if (__n < 3)
        return __first;

    auto __mid = __n / 2;
    _RandomAccessIterator __middle = __first + __mid;
    --__last; // Point to an actual element for simplicity.

    if (bool(__XVI_STD_NS::invoke(__comp, *__last, *__first)))
        __XVI_STD_NS::iter_swap(__last, __first);

    if (bool(__XVI_STD_NS::invoke(__comp, *__middle, *__first)))
        __XVI_STD_NS::iter_swap(__middle, __first);

    if (bool(__XVI_STD_NS::invoke(__comp, *__last, *__middle)))
        __XVI_STD_NS::iter_swap(__last, __middle);

    return __middle;
}

template <class _RandomAccessIterator, class _Compare>
constexpr void __introsort(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp, size_t __depth)
{
    constexpr decltype(__last - __first) _InsertionSortThreshold = 16;
    
    auto __n = (__last - __first);
    
    // Check for trivially-sorted inputs.
    if (__n <= 1)
        return;

    if (__n <= _InsertionSortThreshold)
    {
        // Insertion sort for sufficiently-small arrays.
        __XVI_STD_ALGORITHM_NS::__insertion_sort(__first, __last, __comp);
        return;
    }

    if (__depth == 0)
    {
        // Fall back to heapsort for guaranteed O(n log n) worst-case behaviour.
        __XVI_STD_ALGORITHM_NS::make_heap(__first, __last, __comp);
        __XVI_STD_ALGORITHM_NS::sort_heap(__first, __last, __comp);
        return;
    }

    // Quicksort for everything else.

    // First step is to partition the array based on a pivot value.
    // Pivot value gets swapped to the end so we can keep track of it.
    _RandomAccessIterator __pivot = __XVI_STD_ALGORITHM_NS::__median_of_three(__first, __last, __comp);
    _RandomAccessIterator __pivot_stash = __last - 1;
    __XVI_STD_NS::iter_swap(__pivot, __pivot_stash);
    __pivot = __XVI_STD_ALGORITHM_NS::partition(__first, __pivot_stash, [&__comp, __pivot_stash](const auto& __x) -> bool
        {
            return bool(__XVI_STD_NS::invoke(__comp, __x, *__pivot_stash));
        });
    __XVI_STD_NS::iter_swap(__pivot, __pivot_stash);

    // Then, recurse into each half.
    __XVI_STD_ALGORITHM_NS::__introsort(__first, __pivot, __comp, __depth - 1);
    __XVI_STD_ALGORITHM_NS::__introsort(__pivot + 1, __last, __comp, __depth - 1);
}

template <class _RandomAccessIterator, class _Compare>
constexpr void __introsort(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    // Arbitrary depth threshold.
    size_t __n = static_cast<size_t>(__last - __first);
    size_t __max_depth = 2 * __XVI_STD_ALGORITHM_NS::bit_width(__n);
    __XVI_STD_ALGORITHM_NS::__introsort(__first, __last, __comp, __max_depth);
}

template <class _RandomAccessIterator, class _Compare>
constexpr _RandomAccessIterator __heapselect(_RandomAccessIterator __first, _RandomAccessIterator __last, typename iterator_traits<_RandomAccessIterator>::difference_type __k, _Compare __comp)
{
    // Create a heap and then extract the first k elements.
    __XVI_STD_ALGORITHM_NS::make_heap(__first, __last, __comp);
    for (decltype(__k) __i = 0; __i < __k; ++__i)
        __XVI_STD_ALGORITHM_NS::pop_heap(__first, __last - __i);

    // The elements are at the wrong end of the array; reverse it to ensure the k'th element is in the right place.
    __XVI_STD_ALGORITHM_NS::reverse(__first, __last);
    return __first + __k;
}

template <class _RandomAccessIterator, class _Compare>
constexpr void __partial_introsort(_RandomAccessIterator __first, _RandomAccessIterator __last, typename iterator_traits<_RandomAccessIterator>::difference_type __k, _Compare __comp, size_t __depth)
{
    // Note: __k might be > the length of the input array!
    //       (happens when we're called recursively).
    
    constexpr decltype(__k) _InsertionSortThreshold = 16;
    
    auto __n = (__last - __first);
    
    // Check for trivially-sorted inputs.
    if (__n <= 1)
        return;

    if (__n <= _InsertionSortThreshold)
    {
        // Insertion sort for sufficiently-small arrays.
        __XVI_STD_ALGORITHM_NS::__insertion_sort(__first, __last, __comp);
        return;
    }

    if (__depth == 0)
    {
        // Fall back to heapsort for guaranteed O(n log n) worst-case behaviour.
        //
        // For a partial sort, heapselect is faster as it only sorts the first k elements.
        __XVI_STD_ALGORITHM_NS::__heapselect(__first, __last, __k, __comp);
        return;
    }

    // Quicksort for everything else.

    // First step is to partition the array based on a pivot value.
    // Pivot value gets swapped to the end so we can keep track of it.
    _RandomAccessIterator __pivot = __XVI_STD_ALGORITHM_NS::__median_of_three(__first, __last, __comp);
    _RandomAccessIterator __pivot_stash = __last - 1;
    __XVI_STD_NS::iter_swap(__pivot, __pivot_stash);
    __pivot = __XVI_STD_ALGORITHM_NS::partition(__first, __pivot_stash, [&__comp, __pivot_stash](const auto& __x) -> bool
        {
            return bool(__XVI_STD_NS::invoke(__comp, __x, *__pivot_stash));
        });
    __XVI_STD_NS::iter_swap(__pivot, __pivot_stash);

    // Which of the partitions need to be sorted?
    auto __pivot_pos = (__pivot - __first);

    // The left-hand sub-array always needs to be sorted.
    __XVI_STD_ALGORITHM_NS::__partial_introsort(__first, __pivot, __k, __comp, __depth - 1);

    // The right-hand sub-array only needs to be sorted if the k'th value lies within it.
    if (__pivot_pos < __k)
        __XVI_STD_ALGORITHM_NS::__partial_introsort(__pivot + 1, __last, __k - __pivot_pos, __comp, __depth - 1);
}

template <class _RandomAccessIterator, class _Compare>
constexpr void __partial_introsort(_RandomAccessIterator __first, _RandomAccessIterator __last, typename iterator_traits<_RandomAccessIterator>::difference_type __k, _Compare __comp)
{
    // Arbitrary depth threshold.
    auto __n = (__last - __first);
    auto __max_depth = 2 * __XVI_STD_ALGORITHM_NS::bit_width(__n);
    __XVI_STD_ALGORITHM_NS::__partial_introsort(__first, __last, __k, __comp, __max_depth);
}

template <class _RandomAccessIterator, class _Compare>
constexpr _RandomAccessIterator __introselect(_RandomAccessIterator __first, _RandomAccessIterator __last, typename iterator_traits<_RandomAccessIterator>::difference_type __k, _Compare __comp, size_t __depth)
{
    constexpr decltype(__k) _InsertionSortThreshold = 16;
    
    auto __n = (__last - __first);
    
    // Check for trivially-sorted inputs.
    if (__n <= 1)
        return __first; // Assumed: __k == 0.

    if (__n <= _InsertionSortThreshold)
    {
        // Insertion sort for sufficiently-small arrays.
        __XVI_STD_ALGORITHM_NS::__insertion_sort(__first, __last, __comp);
        return __first + __k;
    }

    if (__depth == 0)
    {
        // Fall back to heapselect for guaranteed O(n log n) worst-case behaviour.
        //! @TODO: median-of-medians has better (i.e O(n)) worst-case behaviour; implement it.
        return __XVI_STD_ALGORITHM_NS::__heapselect(__first, __last, __k, __comp);
    }

    // Quickselect for everything else.

    // First step is to partition the array based on a pivot value.
    // Pivot value gets swapped to the end so we can keep track of it.
    _RandomAccessIterator __pivot = __XVI_STD_ALGORITHM_NS::__median_of_three(__first, __last, __comp);
    _RandomAccessIterator __pivot_stash = __last - 1;
    __XVI_STD_NS::iter_swap(__pivot, __pivot_stash);
    __pivot = __XVI_STD_ALGORITHM_NS::partition(__first, __pivot_stash, [&__comp, __pivot_stash](const auto& __x) -> bool
        {
            return bool(__XVI_STD_NS::invoke(__comp, __x, *__pivot_stash));
        });
    __XVI_STD_NS::iter_swap(__pivot, __pivot_stash);

    // Which partition do we need to recurse into?
    auto __pivot_pos = static_cast<size_t>(__pivot - __first);
    if (__k < __pivot_pos)
        return __XVI_STD_ALGORITHM_NS::__introselect(__first, __pivot, __k, __comp, __depth - 1);
    else if (__k > __pivot_pos)
        return __XVI_STD_ALGORITHM_NS::__introselect(__pivot + 1, __last, __k - __pivot_pos, __comp, __depth - 1);
    else
        return __pivot;
}

template <class _RandomAccessIterator, class _Compare>
constexpr _RandomAccessIterator __introselect(_RandomAccessIterator __first, _RandomAccessIterator __last, typename iterator_traits<_RandomAccessIterator>::difference_type __k, _Compare __comp)
{
    // Arbitrary depth threshold.
    auto __n = static_cast<size_t>(__last - __first);
    auto __max_depth = 2 * __XVI_STD_ALGORITHM_NS::bit_width(__n);
    return __XVI_STD_ALGORITHM_NS::__introselect(__first, __last, __k, __comp, __max_depth);
}

template <class _RandomAccessIterator, class _Compare>
constexpr void sort(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    __XVI_STD_ALGORITHM_NS::__introsort(__first, __last, __comp);
}

template <class _RandomAccessIterator>
constexpr void sort(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    __XVI_STD_ALGORITHM_NS::sort(__first, __last, less{});
}

template <class _RandomAccessIterator, class _Compare>
constexpr void nth_element(_RandomAccessIterator __first, _RandomAccessIterator __nth, _RandomAccessIterator __last, _Compare __comp)
{
    auto __k = (__nth - __first);
    __XVI_STD_ALGORITHM_NS::__introselect(__first, __last, __k, __comp);
}

template <class _RandomAccessIterator>
constexpr void nth_element(_RandomAccessIterator __first, _RandomAccessIterator __nth, _RandomAccessIterator __last)
{
    __XVI_STD_ALGORITHM_NS::nth_element(__first, __nth, __last, less{});
}

template <class _RandomAccessIterator, class _Compare>
constexpr void partial_sort(_RandomAccessIterator __first, _RandomAccessIterator __middle, _RandomAccessIterator __last, _Compare __comp)
{
    auto __k = (__middle - __first);
    __XVI_STD_ALGORITHM_NS::__partial_introsort(__first, __last, __k, __comp);
}

template <class _RandomAccessIterator>
constexpr void partial_sort(_RandomAccessIterator __first, _RandomAccessIterator __middle, _RandomAccessIterator __last)
{
    __XVI_STD_ALGORITHM_NS::partial_sort(__first, __middle, __last, less{});
}

//! @TODO: partial_sort_copy
//! @TODO: stable_sort

//! @TODO: compare_3way

template <class _BidirectionalIterator, class _Compare>
constexpr bool next_permutation(_BidirectionalIterator __first, _BidirectionalIterator __last, _Compare __comp)
{
    if (__first == __last || __XVI_STD_NS::next(__first) == __last)
        return false;
    
    // Highest index k such that A[k] < A[k + 1]
    _BidirectionalIterator __k = __last;
    for (_BidirectionalIterator __i = __first; __XVI_STD_NS::next(__i) != __last; ++__i)
        if (bool(__XVI_STD_NS::invoke(__comp, *__i, *__XVI_STD_NS::next(__i))))
            __k = __i;

    if (__k == __last)
    {
        // This is the last permutation and is in reverse-sorted order.
        __XVI_STD_ALGORITHM_NS::reverse(__first, __last);
        return false;
    }

    // Highest index > k such that A[k] < A[l] (always exists).
    _BidirectionalIterator __l = __last;
    for (_BidirectionalIterator __i = __XVI_STD_NS::next(__k); __i != __last; ++__i)
        if (bool(__XVI_STD_NS::invoke(__comp, *__k, *__i)))
            __l = __i;

    __XVI_STD_NS::iter_swap(__k, __l);
    __XVI_STD_ALGORITHM_NS::reverse(__XVI_STD_NS::next(__k), __last);
    return true;
}

template <class _BidirectionalIterator>
constexpr bool next_permutation(_BidirectionalIterator __first, _BidirectionalIterator __last)
{
    return __XVI_STD_NS::next_permutation(__first, __last, less{});
}

template <class _BidirectionalIterator, class _Compare>
constexpr bool prev_permutation(_BidirectionalIterator __first, _BidirectionalIterator __last, _Compare __comp)
{
    auto __rfirst = __XVI_STD_NS::make_reverse_iterator(__last);
    auto __rlast = __XVI_STD_NS::make_reverse_iterator(__first);
    return __XVI_STD_NS::next_permutation(__rfirst, __rlast, __comp);
}

template <class _BidirectionalIterator>
constexpr bool prev_permutation(_BidirectionalIterator __first, _BidirectionalIterator __last)
{
    return __XVI_STD_NS::prev_permutation(__first, __last, less{});
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ALGORITHM_H */
