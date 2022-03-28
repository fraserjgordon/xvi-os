#pragma once
#ifndef __SYSTEM_CXX_UTILITY_RANGESALGORITHM_H
#define __SYSTEM_CXX_UTILITY_RANGESALGORITHM_H


#include <System/C++/Utility/Private/Config.hh>

#include <System/C++/Utility/Ranges.hh>


//! @TODO: remove std::forward/std::move from range operator() methods.


namespace __XVI_STD_UTILITY_NS
{

#if __cpp_concepts
namespace ranges
{


template <class _I, class _F>
struct for_each_result
{
    [[no_unique_address]] _I in;
    [[no_unique_address]] _F fun;

    template <class _I2, class _F2>
        requires convertible_to<const _I&, _I2> && convertible_to<const _F&, _F2>
    operator for_each_result<_I2, _F2>() const &
    {
        return {in, fun};
    }

    template <class _I2, class _F2>
        requires convertible_to<_I, _I2> && convertible_to<_F, _F2>
    operator for_each_result<_I2, _F2>() &&
    {
        return {std::move(in), std::move(fun)};
    }
};

template <class _I1, class _I2>
struct mismatch_result
{
    [[no_unique_address]] _I1 in1;
    [[no_unique_address]] _I2 in2;

    template <class _II1, class _II2>
        requires convertible_to<const _I1, _II1> && convertible_to<const _I2&, _II2>
    operator mismatch_result<_II1, _II2>() const &
    {
        return {in1, in2};
    }

    template <class _II1, class _II2>
        requires convertible_to<_I1, _II1> && convertible_to<_I2, _II2>
    operator mismatch_result<_II1, _II2>() &&
    {
        return {std::move(in1), std::move(in2)};
    }
};

template <class _I, class _O>
struct copy_result
{
    [[no_unique_address]] _I in;
    [[no_unique_address]] _O out;

    template <class _I2, class _O2>
        requires convertible_to<const _I&, _I2> && convertible_to<const _O&, _O2>
    operator copy_result<_I2, _O2>() const &
    {
        return {in, out};
    }

    template <class _I2, class _O2>
        requires convertible_to<_I, _I2> && convertible_to<_O, _O2>
    operator copy_result<_I2, _O2>() &&
    {
        return {std::move(in), std::move(out)};
    }
};

template <class _I, class _O>
using copy_n_result = copy_result<_I, _O>;

template <class _I, class _O>
using copy_if_result = copy_result<_I, _O>;

template <class _I, class _O>
using copy_backward_result = copy_result<_I, _O>;

template <class _I, class _O>
using move_result = copy_result<_I, _O>;

template <class _I, class _O>
using move_backward_result = copy_result<_I, _O>;

template <class _I1, class _I2>
using swap_ranges_result = mismatch_result<_I1, _I2>;

template <class _I, class _O>
using unary_transform_result = copy_result<_I, _O>;

template <class _I1, class _I2, class _O>
struct binary_transform_result
{
    [[no_unique_address]] _I1 in1;
    [[no_unique_address]] _I2 in2;
    [[no_unique_address]] _O  out;

    template <class _II1, class _II2, class _OO>
        requires convertible_to<const _I1&, _II1> && convertible_to<const _I2&, _II2> && convertible_to<const _O&, _OO>
    operator binary_transform_result<_II1, _II2, _OO>() const &
    {
        return {in1, in2, out};
    }

    template <class _II1, class _II2, class _OO>
        requires convertible_to<_I1, _II1> && convertible_to<_I2, _II2> && convertible_to<_O, _OO>
    operator binary_transform_result<_II1, _II2, _OO>() &&
    {
        return {std::move(in1), std::move(in2), std::move(out)};
    }
};

template <class _I, class _O>
using replace_copy_result = copy_result<_I, _O>;

template <class _I, class _O>
using replace_copy_if_result = copy_result<_I, _O>;

template <class _I, class _O>
using remove_copy_result = copy_result<_I, _O>;

template <class _I, class _O>
using remove_copy_if_result = copy_result<_I, _O>;

template <class _I, class _O>
using unique_copy_result = copy_result<_I, _O>;

template <class _I, class _O>
using reverse_copy_result = copy_result<_I, _O>;

template <class _I, class _O>
using rotate_copy_result = copy_result<_I, _O>;

template <class _I, class _O1, class _O2>
struct partition_copy_result
{
    [[no_unique_address]] _I    in;
    [[no_unique_address]] _O1   out1;
    [[no_unique_address]] _O2   out2;

    template <class _II, class _OO1, class _OO2>
        requires convertible_to<const _I&, _II> && convertible_to<const _O1&, _OO1> && convertible_to<const _O2&, _OO2>
    operator partition_copy_result<_II, _OO1, _OO2>() const &
    {
        return {in, out1, out2};
    }

    template <class _II, class _OO1, class _OO2>
        requires convertible_to<_I, _II> && convertible_to<_O1, _OO1> && convertible_to<_O2, _OO2>
    operator partition_copy_result<_II, _OO1, _OO2>() &&
    {
        return {std::move(in), std::move(out1), std::move(out2)};
    }
};

template <class _I1, class _I2, class _O>
using merge_result = binary_transform_result<_I1, _I2, _O>;

template <class _I1, class _I2, class _O>
using set_union_result = binary_transform_result<_I1, _I2, _O>;

template <class _I1, class _I2, class _O>
using set_intersection_result = binary_transform_result<_I1, _I2, _O>;

template <class _I1, class _I2, class _O>
using set_difference_result = binary_transform_result<_I1, _I2, _O>;

template <class _I1, class _I2, class _O>
using set_symmetric_difference_result = binary_transform_result<_I1, _I2, _O>;

template <class _T>
struct minmax_result
{
    [[no_unique_address]] _T min;
    [[no_unique_address]] _T max;

    template <class _T2>
        requires convertible_to<const _T&, _T2>
    operator minmax_result<_T2>() const &
    {
        return {min, max};
    }

    template <class _T2>
        requires convertible_to<_T, _T2>
    operator minmax_result<_T2>() &&
    {
        return {std::move(min), std::move(max)};
    }
};

template <class _I>
using minmax_element_result = minmax_result<_I>;

template <class _I>
struct next_permutation_result
{
    bool found;
    _I   in;
};

template <class _I>
using prev_permutation_result = next_permutation_result<_I>;


namespace __detail
{

struct __all_of
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr bool operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (invoke(__pred, invoke(__proj, *__first)) == false)
                return false;
        return true;
    }

    template <input_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
    constexpr bool operator()(_R&& __range, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__range)), ranges::end(std::forward<_R>(__range)), std::move(__pred), std::move(__proj));
    }
};

struct __any_of
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr bool operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (invoke(__pred, invoke(__proj, *__first)) != false)
                return true;
        return false;
    }

    template <input_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
    constexpr bool operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), std::move(__proj));
    }
};

struct __none_of
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr bool operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (invoke(__pred, invoke(__proj, *__first)) != false)
                return false;
        return true;
    }

    template <input_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
    constexpr bool operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), std::move(__proj));
    }
};

struct __for_each
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirectly_unary_invocable<projected<_I, _Proj>> _Fun>
    constexpr ranges::for_each_result<_I, _Fun> operator()(_I __first, _S __last, _Fun __f, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            invoke(__f, invoke(__proj, *__first));
        return {__last, std::move(__f)};
    }

    template <input_range _R, class _Proj = identity, indirectly_unary_invocable<projected<iterator_t<_R>, _Proj>> _Fun>
    constexpr ranges::for_each_result<safe_iterator_t<_R>, _Fun> operator()(_R&& __r, _Fun __f, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__f), std::move(__proj));
    }
};

struct __find
{
    template <input_iterator _I, sentinel_for<_I> _S, class _T, class _Proj = identity>
        requires indirect_binary_predicate<ranges::equal_to, projected<_I, _Proj>, const _T*>
    constexpr _I operator()(_I __first, _S __last, const _T& __value, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (invoke(__proj, *__first) == __value)
                return __first;
        return __first; // == __last
    }

    template <input_range _R, class _T, class _Proj = identity>
        requires indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_R>, _Proj>, const _T*>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, const _T& __value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __value, std::move(__proj));
    }
};

struct __find_if
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr _I operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (invoke(__pred, invoke(__proj, *__first)) != false)
                return __first;
        return __first; // == __last
    }

    template <input_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), std::move(__proj));
    }
};

struct __find_if_not
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr _I operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (invoke(__pred, invoke(__proj, *__first)) == false)
                return __first;
        return __first; // == __last
    }

    template <input_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), std::move(__proj));
    }
};

struct __find_first_of
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, forward_iterator _I2, sentinel_for<_I2> _S2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<_I1, _I2, _Pred, _Proj1, _Proj2>
    constexpr _I1 operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        for (; __first1 != __last1; ++__first1)
        {
            for (auto __i = __first2; __i != __last2; ++__i)
                if (invoke(__pred, invoke(__proj1, *__first1), invoke(__proj2, *__i)) != false)
                    return __first1;
        }

        return __first1; // == __last1
    }

    template <input_range _R1, forward_range _R2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<iterator_t<_R1>, iterator_t<_R2>, _Pred, _Proj1, _Proj2>
    constexpr safe_iterator_t<_R1> operator()(_R1&& __r1, _R2&& __r2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)), std::move(__pred), std::move(__proj1), std::move(__proj2));
    }
};

struct __adjacent_find
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_binary_predicate<projected<_I, _Proj>, projected<_I, _Proj>> _Pred = ranges::equal_to>
    constexpr _I operator()(_I __first, _S __last, _Pred __pred = {}, _Proj __proj = {}) const
    {
        if (__first == __last)
            return __first;

        for (auto __second = ranges::next(__first); __second != __last; ++__first, (void)++__second)
            if (invoke(__pred, invoke(__proj, *__first), invoke(__proj, *__second)) != false)
                return __first;

        return ranges::next(__first, __last);
    }

    template <forward_range _R, class _Proj = identity, indirect_binary_predicate<projected<iterator_t<_R>, _Proj>, projected<iterator_t<_R>, _Proj>> _Pred = ranges::equal_to>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Pred __pred = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), std::move(__proj));
    }
};

struct __count
{
    template <input_iterator _I, sentinel_for<_I> _S, class _T, class _Proj = identity>
        requires indirect_binary_predicate<ranges::equal_to, projected<_I, _Proj>, const _T*>
    constexpr iter_difference_t<_I> operator()(_I __first, _S __last, const _T& __value, _Proj __proj = {}) const
    {
        iter_difference_t<_I> __n = 0;
        for (; __first != __last; ++__first)
            if (invoke(__proj, *__first) == __value)
                ++__n;

        return __n;
    }

    template <input_range _R, class _T, class _Proj = identity>
        requires indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_R>, _Proj>, const _T*>
    constexpr range_difference_t<_R> operator()(_R&& __r, const _T& __value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __value, std::move(__proj));
    }
};

struct __count_if
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr iter_difference_t<_I> operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        iter_difference_t<_I> __n = 0;
        for (; __first != __last; ++__first)
            if (invoke(__pred, invoke(__proj, *__first)) != false)
                ++__n;

        return __n;
    }

    template <input_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
    constexpr range_difference_t<_R> operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), std::move(__proj));
    }
};

struct __mismatch
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<_I1, _I2, _Pred, _Proj1, _Proj2>
    constexpr mismatch_result<_I1, _I2> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        for (; __first1 != __last1; ++__first1, (void)++__first2)
            if (!invoke(__pred, invoke(__proj1, *__first1), invoke(__proj2, *__first2)))
                return {__first1, __first2};

        return {ranges::next(__first1, __last1), ranges::next(__first2, __last2)};  // == {__last1, __last2}
    }

    template <input_range _R1, input_range _R2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<iterator_t<_R1>, iterator_t<_R2>, _Pred, _Proj1, _Proj2>
    constexpr mismatch_result<iterator_t<_R1>, iterator_t<_R2>> operator()(_R1&& __r1, _R2&& __r2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)), std::move(__pred), std::move(__proj1), std::move(__proj2));
    }
};

struct __equal
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<_I1, _I2, _Pred, _Proj1, _Proj2>
    constexpr bool operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        if constexpr (sized_sentinel_for<_S1, _I1> && sized_sentinel_for<_S2, _I2>)
            if (ranges::distance(__first1, __last1) != ranges::distance(__first2, __last2))
                return false;
        
        for (; __first1 != __last1; ++__first1, (void)++__first2)
            if (invoke(__pred, invoke(__proj1, *__first1), invoke(__proj2, *__first2)) == false)
                return false;

        return true;
    }

    template <input_range _R1, input_range _R2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<iterator_t<_R1>, iterator_t<_R2>, _Pred, _Proj1, _Proj2>
    constexpr bool operator()(_R1&& __r1, _R2&& __r2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)), std::move(__pred), std::move(__proj1), std::move(__proj2));
    }
};

struct __find_end
{
    template <forward_iterator _I1, sentinel_for<_I1> _S1, forward_iterator _I2, sentinel_for<_I2> _S2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<_I1, _I2, _Pred, _Proj1, _Proj2>
    constexpr subrange<_I1> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        if (__first2 == __last2)
        {
            auto __end = ranges::next(__first1, __last1);
            return {__end, __end};
        }

        _I1 __result = ranges::next(__first1, __last1);

        for (; __first1 != __last1; ++__first1)
        {
            auto [__i, __j] = __mismatch{}(__first1, __last1, __first2, __last2, __pred, __proj1, __proj2);
            if (__j == __last2)
                __result = __first1;
            if (__i == __last1)
                break;
        }

        return {__result, ranges::next(__result, (__result == __last1) ? 0 : ranges::distance(__first2, __last2))};
    }

    template <forward_range _R1, forward_range _R2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<iterator_t<_R1>, iterator_t<_R2>, _Pred, _Proj1, _Proj2>
    constexpr safe_subrange_t<_R1> operator()(_R1&& __r1, _R2&& __r2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)), std::move(__pred), std::move(__proj1), std::move(__proj2));
    }
};

struct __is_permutation
{
    template <forward_iterator _I1, sentinel_for<_I1> _S1, forward_iterator _I2, sentinel_for<_I2> _S2, class _Proj1 = identity, class _Proj2 = identity, indirect_equivalence_relation<projected<_I1, _Proj1>, projected<_I2, _Proj2>> _Pred = ranges::equal_to>
    constexpr bool operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        if constexpr (sized_sentinel_for<_S1, _I1> && sized_sentinel_for<_S2, _I2>)
            if (ranges::distance(__first1, __last1) != ranges::distance(__first2, __last2))
                return false;

        auto [__start1, __start2] = __mismatch{}(__first1, __last1, __first2, __last2, __pred, __proj1, __proj2);
        if (__start1 == __last1 && __start2 == __last2)
            return true;

        for (; __start1 != __last1; ++__start1)
        {
            auto __n1 = __count_if{}(__start1, __last1, [__pred, __proj1, __start1 = __start1]<class _T>(_T&& __t)
            {
                return invoke(__pred, invoke(__proj1, *__start1), std::forward<_T>(__t));
            }, __proj1);

            auto __n2 = __count_if{}(__start2, __last2, [__pred, __proj1, __start1 = __start1]<class _T>(_T&& __t)
            {
                return invoke(__pred, invoke(__proj1, *__start1), std::forward<_T>(__t));
            }, __proj2);

            if (__n1 != __n2)
                return false;
        }

        return true;
    }

    template <forward_range _R1, forward_range _R2, class _Proj1 = identity, class _Proj2 = identity, indirect_equivalence_relation<projected<iterator_t<_R1>, _Proj1>, projected<iterator_t<_R2>, _Proj2>> _Pred = ranges::equal_to>
    constexpr bool operator()(_R1&& __r1, _R2&& __r2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)), std::move(__pred), std::move(__proj1), std::move(__proj2));
    }
};

struct __search
{
    template <forward_iterator _I1, sentinel_for<_I1> _S1, forward_iterator _I2, sentinel_for<_I2> _S2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<_I1, _I2, _Pred, _Proj1, _Proj2>
    constexpr subrange<_I1> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        if (__first2 == __last2)
            return {__first1, __first1};

        for (; __first1 != __last1; ++__first1)
        {
            auto __i = __first1;
            auto __j = __first2;
            while (true)
            {
                if (__j == __last2)
                    return {__first1, __i};

                if (__i == __last1)
                    return {__i, __i};

                if (!bool(invoke(__pred, invoke(__proj1, *__i), invoke(__proj2, *__j))))
                    break;

                ++__i;
                ++__j;
            }

            return {__first1, __first1};    // == {__last1, __last1}
        }    
    }

    template <forward_range _R1, forward_range _R2, class _Pred = ranges::equal_to, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_comparable<iterator_t<_R1>, iterator_t<_R2>, _Pred, _Proj1, _Proj2>
    constexpr safe_subrange_t<_R1> operator()(_R1&& __r1, _R2&& __r2, _Pred __pred = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)), std::move(__pred), std::move(__proj1), std::move(__proj2));
    }
};

struct __search_n
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _T, class _Pred = ranges::equal_to, class _Proj = identity>
        requires indirectly_comparable<_I, const _T*, _Pred, _Proj>
    constexpr subrange<_I> operator()(_I __first, _S __last, iter_difference_t<_I> __count, const _T& __value, _Pred __pred = {}, _Proj __proj = {}) const
    {
        while (__first != __last)
        {
            auto __i = __first;
            iter_difference_t<_I> __n = 0;
            for (; __n < __count && __i != __last; ++__i, (void)++__n)
                if (invoke(__pred, invoke(__proj, *__i), __value) == false)
                    break;

            if (__n == __count)
                return {__first, __i};

            __first = __i;
        }

        return {__first, __first};  // == {__last, __last}
    }

    template <forward_range _R, class _T, class _Pred = ranges::equal_to, class _Proj = identity>
        requires indirectly_comparable<iterator_t<_R>, const _T*, _Pred, _Proj>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, range_difference_t<_R> __count, const _T& __value, _Pred __pred = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __count, __value, std::move(__pred), std::move(__proj));
    }
};

struct __copy
{
    template <input_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O>
        requires indirectly_copyable<_I, _O>
    constexpr copy_result<_I, _O> operator()(_I __first, _S __last, _O __result) const
    {
        for (; __first != __last; ++__first, (void)++__result)
            *__result = *__first;

        return {__first, __result}; // == {__last, __result}
    }

    template <input_range _R, weakly_incrementable _O>
        requires indirectly_copyable<iterator_t<_R>, _O>
    constexpr copy_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __result);
    }
};

struct __copy_n
{
    template <input_iterator _I, weakly_incrementable _O>
        requires indirectly_copyable<_I, _O>
    constexpr copy_n_result<_I, _O> operator()(_I __first, iter_difference_t<_I> __n, _O __result) const
    {
        for (; __n > 0; --__n, (void)++__first, ++__result)
            *__result = *__first;

        return {__first, __result};
    }
};

struct __copy_if
{
    template <input_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
        requires indirectly_copyable<_I, _O>
    constexpr copy_if_result<_I, _O> operator()(_I __first, _S __last, _O __result, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
        {
            if (bool(invoke(__pred, invoke(__proj, *__first))))
            {
                *__result = *__first;
                ++__result;
            }
        }

        return {__first, __result}; // == {__last, __result}
    }

    template <input_range _R, weakly_incrementable _O, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires indirectly_copyable<iterator_t<_R>, _O>
    constexpr copy_if_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__result), std::move(__pred), std::move(__proj));
    }
};

struct __copy_backward
{
    template <bidirectional_iterator _I1, sentinel_for<_I1> _S1, bidirectional_iterator _I2>
        requires indirectly_copyable<_I1, _I2>
    constexpr copy_backward_result<_I1, _I2> operator()(_I1 __first, _S1 __last, _I2 __result) const
    {
        auto __iter_last = ranges::next(__first, __last);
        for (auto __end = __iter_last; __first != __end; --__end, (void)--__result)
            *ranges::prev(__result) = *ranges::prev(__end);

        return {__iter_last, __result};
    }

    template <bidirectional_range _R, bidirectional_iterator _I>
        requires indirectly_copyable<iterator_t<_R>, _I>
    constexpr copy_backward_result<safe_iterator_t<_R>, _I> operator()(_R&& __r, _I __result) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __result);
    }
};

struct __move
{
    template <input_iterator _I, sentinel_for <_I> _S, weakly_incrementable _O>
        requires indirectly_movable<_I, _O>
    constexpr move_result<_I, _O> operator()(_I __first, _S __last, _O __result) const
    {
        for (; __first != __last; ++__first)
            *__result = std::move(*__first);

        return {__first, __result}; // == {__last, __result}
    }

    template <input_range _R, weakly_incrementable _O>
        requires indirectly_movable<iterator_t<_R>, _O>
    constexpr move_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __result);
    }
};

struct __move_backward
{
    template <bidirectional_iterator _I1, sentinel_for<_I1> _S1, bidirectional_iterator _I2>
        requires indirectly_movable<_I1, _I2>
    constexpr move_backward_result<_I1, _I2> operator()(_I1 __first, _S1 __last, _I2 __result) const
    {
        auto __iter_last = ranges::next(__first, __last);
        for (auto __end = __iter_last; __first != __end; --__end, (void)--__result)
            *ranges::prev(__result) = std::move(*ranges::prev(__end));

        return {__iter_last, __result};
    }

    template <bidirectional_range _R, bidirectional_range _I>
        requires indirectly_movable<iterator_t<_R>, _I>
    constexpr move_backward_result<safe_iterator_t<_R>, _I> operator()(_R&& __r, _I __result)
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __result);
    }
};

struct __swap_ranges
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2>
        requires indirectly_swappable<_I1, _I2>
    constexpr swap_ranges_result<_I1, _I2> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2) const
    {
        for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
            ranges::iter_swap(__first1, __first2);

        return {__first1, __first2};
    }

    template <input_range _R1, input_range _R2>
        requires indirectly_swappable<iterator_t<_R1>, iterator_t<_R2>>
    constexpr swap_ranges_result<safe_iterator_t<_R1>, safe_iterator_t<_R2>> operator()(_R1&& __r1, _R2&& __r2) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)));
    }
};

struct __transform
{
    template <input_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O, copy_constructible _F, class _Proj = identity>
        requires indirectly_writable<_O, indirect_result_t<_F&, projected<_I, _Proj>>>
    constexpr unary_transform_result<_I, _O> operator()(_I __first, _S __last, _O __result, _F __op, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first, (void)++__result)
            *__result = invoke(__op, invoke(__proj, *__first));

        return {__first, __result};
    }

    template <input_range _R, weakly_incrementable _O, copy_constructible _F, class _Proj = identity>
        requires indirectly_writable<_O, projected<iterator_t<_R>, indirect_result_t<_F&, _Proj>>>
    constexpr unary_transform_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _F __op, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__op), std::move(__proj));
    }

    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, weakly_incrementable _O, copy_constructible _F, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_writable<_O, indirect_result_t<_F&, projected<_I1, _Proj1>, projected<_I2, _Proj2>>>
    constexpr binary_transform_result<_I1, _I2, _O> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _O __result, _F __op, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2, ++__result)
            *__result = invoke(__op, invoke(__proj1, *__first1), invoke(__proj2, *__first2));

        return {__first1, __first2, __result};
    }

    template <input_range _R1, input_range _R2, weakly_incrementable _O, copy_constructible _F, class _Proj1 = identity, class _Proj2 = identity>
        requires indirectly_writable<_O, indirect_result_t<_F&, projected<iterator_t<_R1>, _Proj1>, projected<iterator_t<_R2>, _Proj2>>>
    constexpr binary_transform_result<safe_iterator_t<_R1>, safe_iterator_t<_R2>, _O> operator()(_R1&& __r1, _R2&& __r2, _O __result, _F __op, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(std::forward<_R1>(__r1)), ranges::end(std::forward<_R1>(__r1)), ranges::begin(std::forward<_R2>(__r2)), ranges::end(std::forward<_R2>(__r2)), std::move(__result), std::move(__op), std::move(__proj1), std::move(__proj2));
    }
};

struct __replace
{
    template <input_iterator _I, sentinel_for<_I> _S, class _T1, class _T2, class _Proj = identity>
        requires indirectly_writable<_I, const _T2&> && indirect_binary_predicate<ranges::equal_to, projected<_I, _Proj>, const _T1*>
    constexpr _I operator()(_I __first, _S __last, const _T1 __old_value, const _T2& __new_value, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (bool(invoke(__proj, *__first) == __old_value))
                *__first = __new_value;

        return __first; // == __last
    }

    template <input_range _R, class _T1, class _T2, class _Proj = identity>
        requires indirectly_writable<iterator_t<_R>, const _T2&> && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_R>, _Proj>, const _T1*>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, const _T1& __old_value, const _T2& __new_value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __old_value, __new_value, std::move(__proj));
    }
};

struct __replace_if
{
    template <input_iterator _I, sentinel_for<_I> _S, class _T, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
        requires indirectly_writable<_I, const _T&>
    constexpr _I operator()(_I __first, _S __last, _Pred __pred, const _T& __new_value, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
            if (bool(invoke(__pred, invoke(__proj, *__first))))
                *__first = __new_value;

        return __first; // == __last
    }

    template <input_range _R, class _T, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires indirectly_writable<iterator_t<_R>, const _T&>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Pred __pred, const _T& __new_value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), __new_value, std::move(__proj));
    }
};

struct __replace_copy
{
    template <input_iterator _I, sentinel_for<_I> _S, class _T1, class _T2, output_iterator<const _T2&> _O, class _Proj = identity>
        requires indirectly_copyable<_I, _O> && indirect_binary_predicate<ranges::equal_to, projected<_I, _Proj>, const _T1*>
    constexpr replace_copy_result<_I, _O> operator()(_I __first, _S __last, _O __result, const _T1& __old_value, const _T2& __new_value, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first, (void)++__result)
            if (bool(invoke(__proj, *__first) == __old_value))
                *__result = __new_value;
            else
                *__result = *__first;

        return {__first, __result};
    }

    template <input_range _R, class _T1, class _T2, output_iterator<const _T2&> _O, class _Proj = identity>
        requires indirectly_copyable<iterator_t<_R>, _O> && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_R>, _Proj>, const _T1*>
    constexpr replace_copy_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result, const _T1& __old_value, const _T2& __new_value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__result), __old_value, __new_value, std::move(__proj));
    }
};

struct __replace_copy_if
{
    template <input_iterator _I, sentinel_for<_I> _S, class _T, output_iterator<const _T&> _O, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
        requires indirectly_copyable<_I, _O>
    constexpr replace_copy_if_result<_I, _O> operator()(_I __first, _S __last, _O __result, _Pred __pred, const _T& __new_value, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first, (void)++__result)
            if (bool(invoke(__pred, invoke(__proj, *__first))))
                *__result = __new_value;
            else
                *__result = *__first;

        return {__first, __result};
    }

    template <input_range _R, class _T, output_iterator<const _T&> _O, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires indirectly_copyable<iterator_t<_R>, _O>
    constexpr replace_copy_if_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result, _Pred __pred, const _T& __new_value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__result), std::move(__pred), __new_value, std::move(__proj));
    }
};

struct __fill
{
    template <class _T, output_iterator<const _T&> _O, sentinel_for<_O> _S>
    constexpr _O operator()(_O __first, _S __last, const _T& __value) const
    {
        for (; __first != __last; ++__first)
            *__first = __value;

        return __first; // == __last
    }

    template <class _T, output_range<const _T&> _R>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, const _T& __value) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __value);
    }
};

struct __fill_n
{
    template <class _T, output_iterator<const _T&> _O>
    constexpr _O operator()(_O __first, iter_difference_t<_O> __n, const _T& __value) const
    {
        for (; __n > 0; --__n, (void)++__first)
            *__first = __value;

        return __first;
    }
};

struct __generate
{
    template <input_or_output_iterator _O, sentinel_for<_O> _S, copy_constructible _F>
        requires invocable<_F&> && indirectly_writable<_O, invoke_result_t<_F&>>
    constexpr _O operator()(_O __first, _S __last, _F __gen) const
    {
        for (; __first != __last; ++__first)
            *__first = __gen();

        return __first; // == __last
    }

    template <class _R, copy_constructible _F>
        requires invocable<_F&> && output_range<_R, invoke_result_t<_F&>>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _F __gen) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__gen));
    }
};

struct __generate_n
{
    template <input_or_output_iterator _O, copy_constructible _F>
        requires invocable<_F&> && indirectly_writable<_O, invoke_result_t<_F&>>
    constexpr _O operator()(_O __first, iter_difference_t<_O> __n, _F __gen) const
    {
        for (; __n > 0; --__n, (void)++__first)
            *__first = __gen();

        return __first;
    }
};

struct __remove
{
    template <permutable _I, sentinel_for<_I> _S, class _T, class _Proj = identity>
        requires indirect_binary_predicate<ranges::equal_to, projected<_I, _Proj>, const _T*>
    constexpr subrange<_I> operator()(_I __first, _S __last, const _T& __value, _Proj __proj = {}) const
    {
        _I __result = __first;
        for (; __first != __last; ++__first)
        {
            if (bool(invoke(__proj, *__first) == __value))
                continue;

            if (__first != __result)
                *__result = ranges::iter_move(__first);

            ++__result;
        }

        return {__result, ranges::next(__result, __last)};
    }

    template <forward_range _R, class _T, class _Proj = identity>
        requires permutable<iterator_t<_R>> && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_R>, _Proj>, const _T*>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, const _T& __value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), __value, std::move(__proj));
    }
};

struct __remove_if
{
    template <permutable _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr subrange<_I> operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        _I __result = __first;
        for (; __first != __last; ++__first)
        {
            if (bool(invoke(__pred, invoke(__proj, *__first))))
                continue;

            if (__first != __result)
                *__result = ranges::iter_move(__first);

            ++__result;
        }

        return {__result, ranges::next(__result, __last)};
    }

    template <forward_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires permutable<iterator_t<_R>>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__pred), std::move(__proj));
    }
};

struct __remove_copy
{
    template <input_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O, class _T, class _Proj = identity>
        requires indirectly_copyable<_I, _O> && indirect_binary_predicate<ranges::equal_to, projected<_I, _Proj>, const _T*>
    constexpr remove_copy_result<_I, _O> operator()(_I __first, _S __last, _O __result, const _T& __value, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
        {
            if (bool(invoke(__proj, *__first) == __value))
                continue;

            *__result = *__first;
            ++__result;
        }

        return {__first, __result};
    }

    template <input_range _R, weakly_incrementable _O, class _T, class _Proj = identity>
        requires indirectly_copyable<iterator_t<_R>, _O> && indirect_binary_predicate<ranges::equal_to, projected<iterator_t<_R>, _Proj>, const _T*>
    constexpr remove_copy_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result, const _T& __value, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__result), __value, std::move(__proj));
    }
};

struct __remove_copy_if
{
    template <input_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
        requires indirectly_copyable<_I, _O>
    constexpr remove_copy_if_result<_I, _O> operator()(_I __first, _S __last, _O __result, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
        {
            if (bool(invoke(__pred, invoke(__proj, *__first))))
                continue;

            *__result = *__first;
            ++__result;
        }

        return {__first, __result};
    }

    template <input_range _R, weakly_incrementable _O, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires indirectly_copyable<iterator_t<_R>, _O>
    constexpr remove_copy_if_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__result), std::move(__pred), std::move(__proj));
    }
};

struct __unique
{
    template <permutable _I, sentinel_for<_I> _S, class _Proj = identity, indirect_equivalence_relation<projected<_I, _Proj>> _C = ranges::equal_to>
    constexpr subrange<_I> operator()(_I __first, _S __last, _C __comp = {}, _Proj __proj = {}) const
    {
        _I __cmp = __first;
        _I __result = __first;

        for (; __first != __last; ++__first)
        {
            if (__first == __cmp || bool(invoke(__comp, invoke(__proj, *__first), invoke(__proj, *__cmp))))
                continue;

            if (__result != __first)
                *__result = ranges::iter_move(__first);

            __cmp = __result;
            ++__result;
        }

        return {__result, ranges::next(__result, __last)};
    }

    template <forward_range _R, class _Proj = identity, indirect_equivalence_relation<projected<iterator_t<_R>, _Proj>> _C = ranges::equal_to>
        requires permutable<iterator_t<_R>>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, _C __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__comp), std::move(__proj));
    }
};

struct __unique_copy
{
    template <input_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O, class _Proj = identity, indirect_equivalence_relation<projected<_I, _Proj>> _Comp = ranges::equal_to>
        requires indirectly_copyable<_I, _O>
            && (forward_iterator<_I>
                || (input_iterator<_O> && same_as<iter_value_t<_I>, iter_value_t<_O>>)
                || indirectly_copyable_storable<_I, _O>)
    constexpr unique_copy_result<_I, _O> operator()(_I __first, _S __last, _O __result, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (__first == __last)
            return {__first, __result};

        if constexpr (forward_iterator<_I>)
        {
            _I __cmp = __first;
            for (; __first != __last; ++__first)
            {
                if (__first == __cmp || bool(invoke(__comp, invoke(__proj, *__first), invoke(__proj, *__cmp))))
                    continue;

                *__result = *__first;
                __cmp = __first;
                ++__result;
            }
        }
        else if constexpr (input_iterator<_O> && same_as<iter_value_t<_I>, iter_value_t<_O>>)
        {
            *__result = *__first;
            ++__first;

            for (; __first != __last; ++__first)
            {
                if (bool(invoke(__comp, invoke(__proj, *__first), invoke(__proj, *__result))))
                    continue;

                ++__result;
                *__result = *__first;
            }

            ++__result;
        }
        else /* if constexpr indirectly_copyable_storable<_I, _O> */
        {
            auto __t = *__first;
            *__result = *__first;
            ++__result;
            ++__first;

            for (; __first != __last; ++__first)
            {
                if (bool(invoke(__comp, invoke(__proj, *__first), invoke(__proj, __t))))
                    continue;

                __t = *__first;
                *__result = __t;
                ++__result;
            }
        }

        return {__first, __result};
    }

    template <input_range _R, weakly_incrementable _O, class _Proj = identity, indirect_equivalence_relation<projected<iterator_t<_R>, _Proj>> _Comp = ranges::equal_to>
        requires indirectly_copyable<iterator_t<_R>, _O>
            && (forward_iterator<iterator_t<_R>>
                || (input_iterator<_O> && same_as<range_value_t<_R>, iter_value_t<_O>>)
                || indirectly_copyable_storable<iterator_t<_R>, _O>)
    constexpr unique_copy_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__result), std::move(__comp), std::move(__proj));
    }
};

struct __reverse
{
    template <bidirectional_iterator _I, sentinel_for<_I> _S>
        requires permutable<_I>
    constexpr _I operator()(_I __first, _S __last) const
    {
        auto __end = ranges::next(__first, __last);
        for (auto __target = ranges::prev(__end); __first != __target; ++__first, (void)--__target)
        {
            ranges::iter_swap(__first, __target);
            if (ranges::next(__first) == __target)
                break;
        }

        return __end;
    }

    template <bidirectional_range _R>
        requires permutable<iterator_t<_R>>
    constexpr safe_iterator_t<_R> operator()(_R&& __r) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)));
    }
};

struct __reverse_copy
{
    template <bidirectional_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O>
        requires indirectly_copyable<_I, _O>
    constexpr reverse_copy_result<_I, _O> operator()(_I __first, _S __last, _O __result) const
    {
        auto __end = ranges::next(__first, __last);
        for (auto __from = ranges::prev(__end); __first != __last; --__from, (void)++__result, ++__first)
            *__result = *ranges::prev(__end);

        return {__end, __result};
    }

    template <bidirectional_range _R, weakly_incrementable _O>
        requires indirectly_copyable<iterator_t<_R>, _O>
    constexpr reverse_copy_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, _O __result) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__result));
    }
};

struct __rotate
{
    template <permutable _I, sentinel_for<_I> _S>
    constexpr subrange<_I> operator()(_I __first, _I __middle, _S __last) const
    {
        if (__first == __middle)
            return {__last, __last};
        if (__middle == __last)
            return {__first, __last};

        _I __read = __middle;
        _I __write = __first;
        _I __victim_ptr = __first;

        for (; __read != __last; ++__read, (void)++__write)
        {
            ranges::iter_swap(__read, __write);

            if (__write == __victim_ptr)
                __victim_ptr = __read;
        }

        operator()(__write, __victim_ptr, __read);

        return {__write, __read};
    }

    template <forward_range _R>
        requires permutable<iterator_t<_R>>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, iterator_t<_R> __middle) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), __middle, ranges::end(std::forward<_R>(__r)));
    }
};

struct __rotate_copy
{
    template <forward_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O>
        requires indirectly_copyable<_I, _O>
    constexpr rotate_copy_result<_I, _O> operator()(_I __first, _I __middle, _S __last, _O __result) const
    {
        for (auto __i = __middle; __i != __last; ++__i, (void)++__result)
            *__result = *__i;
        for (auto __i = __first; __i != __middle; ++__i, (void)++__result)
            *__result = *__i;

        return {ranges::next(__middle, __last), __result};
    }

    template <forward_range _R, weakly_incrementable _O>
        requires indirectly_copyable<iterator_t<_R>, _O>
    constexpr rotate_copy_result<safe_iterator_t<_R>, _O> operator()(_R&& __r, iterator_t<_R> __middle, _O __result) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), __middle, ranges::end(std::forward<_R>(__r)), __result);
    }
};

struct __is_sorted_until
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_strict_weak_order<projected<_I, _Proj>> _Comp = ranges::less>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (__first == __last)
            return __first; // == __last

        _I __second = ranges::next(__first);
        for (; __second != __last; ++__first, (void)++__second)
            if (!bool(invoke(__comp, invoke(__proj, *__first), invoke(__proj, *__second))))
                return __second;

        return __second; // == __last
    }

    template <forward_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__comp), std::move(__proj));
    }
};

struct __is_sorted
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_strict_weak_order<projected<_I, _Proj>> _Comp = ranges::less>
    constexpr bool operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return __is_sorted_until{}(std::move(__first), std::move(__last), std::move(__comp), std::move(__proj));
    }

    template <forward_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr bool operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(std::forward<_R>(__r)), ranges::end(std::forward<_R>(__r)), std::move(__comp), std::move(__proj));
    }
};

struct __lower_bound
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _T, class _Proj = identity, indirect_strict_weak_order<const _T*, projected<_I, _Proj>> _Comp = ranges::less>
    constexpr _I operator()(_I __first, _S __last, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __length = ranges::distance(__first, __last);
        while (true)
        {
            if (__first == __last)
                return __first;

            auto __mid = __length / 2;
            auto __middle = ranges::next(__first, __mid);

            if (bool(invoke(__comp, invoke(__proj, *__middle), __value)))
            {
                __first = ranges::next(__middle);
                __length -= (__mid + 1);
            }
            else
            {
                __last = __middle;
                __length = __mid;
            }
        }
    }

    template <forward_range _R, class _T, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __value, __comp, __proj);
    }
};

struct __upper_bound
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _T, class _Proj = identity, indirect_strict_weak_order<const _T*, projected<_I, _Proj>> _Comp = ranges::less>
    constexpr _I operator()(_I __first, _S __last, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __length = ranges::distance(__first, __last);
        while (true)
        {
            if (__first == __last)
                return __first;

            auto __mid = __length / 2;
            auto __middle = ranges::next(__first, __mid);

            if (!bool(invoke(__comp, __value, invoke(__proj, *__middle))))
            {
                __first = ranges::next(__middle);
                __length -= (__mid + 1);
            }
            else
            {
                __last = __middle;
                __length = __mid;
            }
        }
    }

    template <forward_range _R, class _T, class _Proj = identity, indirect_strict_weak_order<const _T*, projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __value, __comp, __proj);
    }
};

struct __equal_range
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _T, class _Proj = identity, indirect_strict_weak_order<const _T*, projected<_I, _Proj>> _Comp = ranges::less>
    constexpr subrange<_I> operator()(_I __first, _S __last, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return {__lower_bound{}(__first, __last, __value, __comp, __proj), __upper_bound{}(__first, __last, __value, __comp, __proj)};
    }

    template <forward_range _R, class _T, class _Proj = identity, indirect_strict_weak_order<const _T*, projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __value, __comp, __proj);
    }
};

struct __binary_search
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _T, class _Proj = identity, indirect_strict_weak_order<const _T*, projected<_I, _Proj>> _Comp = ranges::less>
    constexpr bool operator()(_I __first, _S __last, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __length = ranges::distance(__first, __last);
        auto __end = ranges::next(__first, __last);
        while (true)
        {
            if (__first == __end)
                return false;

            auto __mid = __length / 2;
            auto __middle = ranges::next(__first, __mid);

            if (bool(invoke(__comp, invoke(__proj, *__middle), __value)))
            {
                __first = ranges::next(__middle);
                __length -= (__mid + 1);
            }
            else if (bool(invoke(__comp, __value, invoke(__proj, *__middle))))
            {
                __end = __middle;
                __length = __mid;
            }
            else
            {
                return true;
            }
        }
    }

    template <forward_range _R, class _T, class _Proj = identity, indirect_strict_weak_order<const _T*, projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr bool operator()(_R&& __r, const _T& __value, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __value, __comp, __proj);
    }
};

struct __is_partitioned
{
    template <input_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr bool operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        for(; __first != __last; ++__first)
            if (!bool(invoke(__pred, invoke(__proj, *__first))))
                break;

        for(; __first != __last; ++__first)
            if(bool(invoke(__pred, invoke(__proj, *__first))))
                return false;

        return true;
    }

    template <input_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
    constexpr bool operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __pred, __proj);
    }
};

struct __partition
{
    template <permutable _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
    constexpr subrange<_I> operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        if constexpr (bidirectional_iterator<_I>)
        {
            // Hoare partition.
            auto __end = ranges::next(__first, __last);
            auto __original_end = __end;
            while (true)
            {
                for (; __first != __last && bool(invoke(__pred, invoke(__proj, *__first))); ++__first)
                    ;

                for (; __last != __first && !bool(invoke(__pred, invoke(__proj, *ranges::prev(__end)))); --__end)
                    ;

                if (__first == __end)
                    return {__end, __original_end};

                ranges::iter_swap(__first, ranges::prev(__end));
                ++__first;

                if (__first != __end)
                    --__end;
            }
        }
        else
        {
            // Lomutu partition.
            auto __i = __first;
            auto __j = __first;

            for(; __j != __last; ++__j)
            {
                if (bool(invoke(__pred, invoke(__proj, *__j))))
                {
                    ranges::iter_swap(__i, __j);
                    ++__i;
                }
            }

            return {__i, __j};
        }   
    }

    template <forward_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires permutable<iterator_t<_R>>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __pred, __proj);
    }
};

struct __partition_copy
{
    template <input_iterator _I, sentinel_for<_I> _S, weakly_incrementable _O1, weakly_incrementable _O2, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
        requires indirectly_copyable<_I, _O1> && indirectly_copyable<_I, _O2>
    constexpr partition_copy_result<_I, _O1, _O2> operator()(_I __first, _S __last, _O1 __out_true, _O2 __out_false, _Pred __pred, _Proj __proj = {}) const
    {
        for (; __first != __last; ++__first)
        {
            if (bool(invoke(__pred, invoke(__proj, *__first))))
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

        return {__first, __out_true, __out_false};
    }

    template <input_range _R, weakly_incrementable _O1, weakly_incrementable _O2, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires indirectly_copyable<iterator_t<_R>, _O1> && indirectly_copyable<iterator_t<_R>, _O2>
    constexpr partition_copy_result<safe_iterator_t<_R>, _O1, _O2> operator()(_R&& __r, _O1 __out_true, _O2 __out_false, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __out_true, __out_false, __pred, __proj);
    }
};

struct __stable_partition
{
    template <bidirectional_iterator _I, class _Proj, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
        requires permutable<_I>
    static constexpr subrange<_I> __do(_I __first, _I __last, _Pred __pred, _Proj __proj, iter_difference_t<_I> __n, pair<void*, size_t> __buffer)
    {
        // Skip any elements already in the right place.
        for (; __first != __last; ++__first, (void)--__n)
        {
            if (!bool(invoke(__pred, invoke(__proj, *__first))))
                break;
        }

        if (__n <= 1)
            return {__first, __last};

        if (__n == 2)
        {
            auto __second = ranges::next(__first);
            if (bool(invoke(__pred, invoke(__proj, *__second))))
            {
                ranges::iter_swap(__first, __second);
                    return {__second, __last};
            }
            else
            {
                return {__first, __last};
            }
        }

        // Is the buffer big enough?
        using _T = iter_value_t<_I>;
        if (__buffer.second >= (__n * sizeof(_T)))
        {
            // Move false elements into the buffer, shift true elements to beginning.
            _T* __arr = static_cast<_T*>(__buffer.first);
            size_t __false_count = 0;
            auto __read = __first;
            auto __write = __first;
            for (; __read != __last; ++__read)
            {
                if (bool(invoke(__pred, invoke(__proj, *__read))))
                {
                    if (__read != __write)
                        ranges::iter_swap(__read, __write);
                    ++__write;
                }
                else
                {
                    new (&__arr[__false_count]) _T(ranges::iter_move(__read));
                    ++__false_count;
                }
            }

            // Move the false elements back.
            _I __boundary = __write;
            for (size_t __i = 0; __write != __last; ++__write, (void)++__i)
            {
                *__write = std::move(__arr[__i]);
                __arr[__i].~_T();
            }

            return {__boundary, __last};
        }

        // Temporary buffer not available; split into two and recurse.
        auto __half = __n / 2;
        auto __middle = ranges::next(__first, __half);
        auto __bound1 = __do(__first, __middle, __pred, __proj, __half, __buffer).begin();
        auto __bound2 = __do(__middle, __last, __pred, __proj, __n - __half, __buffer).begin();

        // We now have something that looks like:
        //  TTTTTTFFFFFFTTTTTTFFFFFF
        //  ^     ^     ^     ^     ^
        //  first |     mid   |     last
        //        bound1      bound2
        //
        // By rotating the section between bound1 and bound2 around mid we get:
        // TTTTTTTTTTTTFFFFFFFFFFFF
        //
        return {__rotate{}(__bound1, __middle, __bound2).begin(), __last};
    }

    template <bidirectional_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Pred>
        requires permutable<_I>
    constexpr subrange<_I> operator()(_I __first, _S __last, _Pred __pred, _Proj __proj = {}) const
    {
        auto __n = ranges::distance(__first, __last);
        auto __end = ranges::next(__first, __last);

        // Attempt to allocate a temporary buffer as large as possible.
        using _T = iter_value_t<_I>;
        pair<void*, size_t> __buffer = {nullptr, size_t{0}};
        if (!is_constant_evaluated())
        {
            // Note: arbitrary minimum size.
            auto __size = static_cast<size_t>(__n) * sizeof(_T);
            while (__size >= 4 * sizeof(_T))
            {
                void* __ptr = new (std::nothrow) byte[__size];
                if (__ptr)
                {
                    __buffer.first = __ptr;
                    __buffer.second = __size;
                    break;
                }

                __size /= 2;
            }
        }

        return __do(__first, __end, __pred, __proj, __n, __buffer);
    }

    template <bidirectional_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Pred>
        requires permutable<iterator_t<_R>>
    constexpr safe_subrange_t<_R> operator()(_R&& __r, _Pred __pred, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __pred, __proj);
    }
};

struct __partition_point
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_unary_predicate<projected<_I, _Proj>> _Comp>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp, _Proj __proj = {}) const
    {
        auto __length = ranges::distance(__first, __last);
        auto __end = ranges::next(__first, __last);

        while (true)
        {
            if (__first == __end)
                return __first;

            auto __mid = __length / 2;
            auto __middle = ranges::next(__first, __mid);

            if (bool(invoke(__comp, invoke(__proj, *__middle))))
            {
                __first = ranges::next(__middle);
                __length -= (__mid + 1);
            }
            else
            {
                __end = __middle;
                __length = __mid;
            }
        }
    }

    template <forward_range _R, class _Proj = identity, indirect_unary_predicate<projected<iterator_t<_R>, _Proj>> _Comp>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __merge
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<_I1, _I2, _O, _Comp, _Proj1, _Proj2>
    constexpr merge_result<_I1, _I2, _O> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        for (; __first1 != __last1 && __first2 != __last2; ++__result)
        {
            if (bool(invoke(__comp, invoke(__proj1, *__first2), invoke(__proj2, *__first1))))
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

        return {__first1, __first2, __result};
    }

    template <input_range _R1, input_range _R2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<iterator_t<_R1>, iterator_t<_R2>, _O, _Comp, _Proj1, _Proj2>
    constexpr merge_result<safe_iterator_t<_R1>, safe_iterator_t<_R2>, _O> operator()(_R1&& __r1, _R2&& __r2, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(__r1), ranges::end(__r1), ranges::begin(__r2), ranges::end(__r2), __comp, __proj1, __proj2);
    }
};

struct __inplace_merge
{
    //! @TODO: implement.
    template <bidirectional_iterator _I, class _Comp, class _Proj>
    static constexpr _I __do(_I __first, _I __middle, _I __last, _Comp __comp, _Proj __proj, iter_difference_t<_I> __n1, iter_difference_t<_I> __n2, pair<void*, size_t> __buffer);
    
    // Note: not constexpr.
    template <bidirectional_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _I __middle, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __n1 = ranges::distance(__first, __middle);
        auto __n2 = ranges::distance(__middle, __last);
        auto __n = __n1 + __n2;
        auto __end = ranges::next(__middle, __last);

        // Attempt to allocate a temporary buffer as large as possible.
        using _T = iter_value_t<_I>;
        pair<void*, size_t> __buffer = {nullptr, size_t{0}};
        if (!is_constant_evaluated())
        {
            // Note: arbitrary minimum size.
            auto __size = static_cast<size_t>(__n) * sizeof(_T);
            while (__size >= 4 * sizeof(_T))
            {
                void* __ptr = new (std::nothrow) byte[__size];
                if (__ptr)
                {
                    __buffer.first = __ptr;
                    __buffer.second = __size;
                    break;
                }

                __size /= 2;
            }
        }

        return __do(__first, __middle, __end, __comp, __proj, __n1, __n2, __buffer);
    }

    // Note: not constexpr.
    template <bidirectional_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    safe_iterator_t<_R> operator()(_R&& __r, iterator_t<_R> __middle, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), __middle, ranges::end(__r), __comp, __proj);
    }
};

struct __includes
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, class _Proj1 = identity, class _Proj2 = identity, indirect_strict_weak_order<projected<_I1, _Proj1>, projected<_I2, _Proj2>> _Comp = ranges::less>
    constexpr bool operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        for (; __first2 != __last2; ++__first2)
        {
            while (__first1 != __last1 && bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))))
                ++__first1;

            if (__first1 != __last1)
                return false;

            bool __equal = !bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)));
            if (!__equal)
                return false;

            ++__first1;
            while (__first1 != __last1 && invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)))
                ++__first1;
        }

        return true;
    }

    template <input_range _R1, input_range _R2, class _Proj1 = identity, class _Proj2 = identity, indirect_strict_weak_order<projected<iterator_t<_R1>, _Proj1>, projected<iterator_t<_R2>, _Proj2>> _Comp = ranges::less>
    constexpr bool operator()(_R1&& __r1, _R2&& __r2, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(__r1), ranges::end(__r1), ranges::begin(__r2), ranges::end(__r2), __comp, __proj1, __proj2);
    }
};

struct __set_union
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<_I1, _I2, _Comp, _Proj1, _Proj2>
    constexpr set_union_result<_I1, _I2, _O> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        while (true)
        {
            while (__first1 != __last1 && (__first2 == __last2 || bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2)))))
            {
                *__result = *__first1;
                ++__first1;
                ++__result;
            }

            while (__first1 != __last1
                   && __first2 != __last2
                   && !bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)))
                   && !bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))))
            {
                *__result = *__first1;
                ++__first1;
                ++__first2;
                ++__result;
            }

            while (__first2 != __last2 && (__first1 == __last1 || bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first2)))))
            {
                *__result = *__first2;
                ++__first2;
                ++__result;
            }

            if (__first1 == __last1 && __first2 == __last2)
                break;
        }

        return {__first1, __first2, __result};
    }
};

struct __set_intersection
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<_I1, _I2, _O, _Proj1, _Proj2>
    constexpr set_intersection_result<_I1, _I2, _O> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        while (true)
        {
            while (__first1 != __last1
                   && __first2 != __last2
                   && bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))))
            {
                ++__first1;
            }

            while (__first1 != __last1
                   && __first2 != __last2
                   && !bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)))
                   && !bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))))
            {
                *__result = *__first1;
                ++__first1;
                ++__first2;
                ++__result;
            }

            while (__first2 != __last2
                   && __first1 != __last1
                   && bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1))))
            {
                ++__first2;
            }

            if (__first1 == __last1 || __first2 == __last2)
                break;
        }

        return {ranges::next(__first1, __last1), ranges::next(__first2, __last2), __result};
    }

    template <input_range _R1, input_range _R2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<iterator_t<_R1>, iterator_t<_R2>, _Comp, _Proj1, _Proj2>
    constexpr set_intersection_result<safe_iterator_t<_R1>, safe_iterator_t<_R2>, _O> operator()(_R1&& __r1, _R2&& __r2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(__r1), ranges::end(__r1), ranges::begin(__r2), ranges::end(__r2), __result, __comp, __proj1, __proj2);
    }
};

struct __set_difference
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<_I1, _I2, _Comp, _Proj1, _Proj2>
    constexpr set_difference_result<_I1, _I2, _O> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        while (true)
        {
            while (__first1 != __last1 && (__first2 == __last2 || bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2)))))
            {
                *__result = *__first1;
                ++__first1;
                ++__result;
            }

            while (__first1 != __last1
                   && __first2 != __last2
                   && !bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)))
                   && !bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))))
            {
                ++__first1;
                ++__first2;
            }

            while (__first2 != __last2 && (__first1 == __last1 || bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)))))
            {
                ++__first2;
            }

            if (__first1 == __last1 && __first2 == __last2)
                break;
        }

        return {__first1, __first2, __result};
    }

    template <input_range _R1, input_range _R2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<iterator_t<_R1>, iterator_t<_R2>, _Comp, _Proj1, _Proj2>
    constexpr set_difference_result<safe_iterator_t<_R1>, safe_iterator_t<_R2>, _O> operator()(_R1&& __r1, _R2&& __r2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(__r1), ranges::end(__r1), ranges::begin(__r2), ranges::end(__r2), __result, __comp, __proj1, __proj2);
    }
};

struct __set_symmetric_difference
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<_I1, _I2, _Comp, _Proj1, _Proj2>
    constexpr set_symmetric_difference_result<_I1, _I2, _O> operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        while (true)
        {
            while (__first1 != __last1 && (__first2 == __last2 || bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2)))))
            {
                *__result = *__first1;
                ++__first1;
                ++__result;
            }

            while (__first1 != __last1
                   && __first2 != __last2
                   && !bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)))
                   && !bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))))
            {
                ++__first1;
                ++__first2;
            }

            while (__first2 != __last2 && (__first1 == __last1 || bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1)))))
            {
                *__result = *__first2;
                ++__first2;
                ++__result;
            }

            if (__first1 == __last1 && __first2 == __last2)
                break;
        }

        return {__first1, __first2, __result};
    }

    template <input_range _R1, input_range _R2, weakly_incrementable _O, class _Comp = ranges::less, class _Proj1 = identity, class _Proj2 = identity>
        requires mergeable<iterator_t<_R1>, iterator_t<_R2>, _Comp, _Proj1, _Proj2>
    constexpr set_symmetric_difference_result<safe_iterator_t<_R1>, safe_iterator_t<_R2>, _O> operator()(_R1&& __r1, _R2&& __r2, _O __result, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(__r1), ranges::end(__r1), ranges::begin(__r2), ranges::end(__r2), __result, __comp, __proj1, __proj2);
    }
};

struct __heap_utils
{
protected:

    static constexpr size_t __heap_left(size_t __i)
    {
        return 2*__i + 1;
    }

    static constexpr size_t __heap_right(size_t __i)
    {
        return 2*__i + 2;
    }

    static constexpr size_t __heap_parent(size_t __i)
    {
        return (__i - 1) / 2;
    }

    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp, class _Proj>
    static constexpr bool __heapify(_I __first, _S __last, size_t __index, _Comp __comp, _Proj __proj)
    {
        auto __length = static_cast<size_t>(ranges::distance(__first, __last));

        auto __left = __heap_left(__index);
        auto __right = __heap_right(__index);
        auto __largest = __index;

        if (__left < __length && invoke(__comp, invoke(__proj, __first[__largest]), invoke(__proj, __first[__left])))
            __largest = __left;

        if (__right < __length && invoke(__comp, invoke(__proj, __first[__largest]), invoke(__proj, __first[__right])))
            __largest = __right;

        if (__largest != __index)
        {
            ranges::iter_swap(__first + __index, __first + __largest);
            __heapify(__first, __last, __largest, __comp, __proj);
            return true;
        }

        return false;
    }

    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp, class _Proj>
    static constexpr size_t __bad_heap_index(_I __first, _S __last, size_t __index, _Comp __comp, _Proj __proj)
    {
        auto __length = static_cast<size_t>(ranges::distance(__first, __last));

        auto __left = __heap_left(__index);
        auto __right = __heap_right(__index);
        auto __largest = __index;

        if (__left < __length && invoke(__comp, invoke(__proj, __first[__largest]), invoke(__proj, __first[__left])))
            return __left;

        if (__right < __length && invoke(__comp, invoke(__proj, __first[__largest]), invoke(__proj, __first[__right])))
            return __right;

        if (__left < __length)
            if (size_t __bad = __bad_heap_index(__first, __last, __left, __comp, __proj); __bad != 0)
                return __bad;

        if (__right < __length)
            if (size_t __bad = __bad_heap_index(__first, __last, __right, __comp, __proj); __bad != 0)
                return __bad;

        return 0;
    }
};

struct __push_heap : __heap_utils
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __i = static_cast<size_t>(ranges::distance(__first, __last));
        if (__i == 0)
            return __first;

        auto __parent = __heap_parent(__i);
        while (true)
        {
            bool __continue = __heapify(__first, __last, __parent, __comp, __proj);

            if (!__continue || __parent == 0)
                return ranges::next(__first, __last);

            __parent = __heap_parent(__parent);
        }
    }

    template <random_access_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __pop_heap : __heap_utils
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __end = ranges::prev(ranges::next(__first, __last));
        ranges::iter_swap(__first, __end);
        __heapify(__first, __last, 0, __comp, __proj);
    }

    template <random_access_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __make_heap : __heap_utils
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __len = static_cast<size_t>(ranges::distance(__first, __last));
        for (size_t __i = 0; __i < __len; ++__i)
            __heapify(__first, __last, __len - __i, __comp, __proj);
        return ranges::next(__first, __last);
    }

    template <random_access_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __sort_heap
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __len = static_cast<size_t>(ranges::distance(__first, __last));
        auto __end = ranges::next(__first, __last);
        auto __original_end = __end;
        for (size_t __i = 0; __i < __len; ++__i)
        {
            __pop_heap{}(__first, __end, __comp, __proj);
            --__end;
        }

        __reverse{}(__first, __last);

        return __original_end;
    }

    template <random_access_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __is_heap_until : __heap_utils
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_strict_weak_order<projected<_I, _Proj>> _Comp = ranges::less>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __bad_index = __bad_heap_index(__first, __last, 0, __comp, __proj);
        return (__bad_index == 0) ? ranges::next(__first, __last) : __first + __bad_index;
    }

    template <random_access_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __is_heap
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_strict_weak_order<projected<_I, _Proj>> _Comp = ranges::less>
    constexpr bool operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return __is_heap_until{}(__first, __last, __comp, __proj) == __last;
    }

    template <random_access_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr bool operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __min
{
    template <class _T, class _Proj = identity, indirect_strict_weak_order<projected<const _T*, _Proj>> _Comp = ranges::less>
    constexpr const _T& operator()(const _T& __a, const _T& __b, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (bool(invoke(__comp, invoke(__proj, __b), invoke(__proj, __a))))
            return __b;
        return __a;
    }

    template <copyable _T, class _Proj = identity, indirect_strict_weak_order<projected<const _T*, _Proj>> _Comp = ranges::less>
    constexpr _T operator()(initializer_list<_T> __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        const _T& __m = *__r.begin();

        for (auto __i = __r.begin() + 1; __i != __r.end(); ++__i)
            __m = operator()(__m, *__i, __comp, __proj);

        return __m;
    }

    template <input_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
        requires indirectly_copyable_storable<iterator_t<_R>, range_value_t<_R>*>
    constexpr range_value_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        const range_value_t<_R>& __m = *ranges::begin(__r);

        for (auto __i = ranges::next(ranges::begin(__r)); __i != ranges::end(__r); ++__i)
            __m = operator()(__m, *__i, __comp, __proj);

        return __m;
    }
};

struct __max
{
    template <class _T, class _Proj = identity, indirect_strict_weak_order<projected<const _T*, _Proj>> _Comp = ranges::less>
    constexpr const _T& operator()(const _T& __a, const _T& __b, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (bool(invoke(__comp, invoke(__proj, __a), invoke(__proj, __b))))
            return __b;
        return __a;
    }

    template <copyable _T, class _Proj = identity, indirect_strict_weak_order<projected<const _T*, _Proj>> _Comp = ranges::less>
    constexpr _T operator()(initializer_list<_T> __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        const _T& __m = *__r.begin();

        for (auto __i = __r.begin() + 1; __i != __r.end(); ++__i)
            __m = operator()(__m, *__i, __comp, __proj);

        return __m;
    }

    template <input_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
        requires indirectly_copyable_storable<iterator_t<_R>, range_value_t<_R>*>
    constexpr range_value_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        const range_value_t<_R>& __m = *ranges::begin(__r);

        for (auto __i = ranges::next(ranges::begin(__r)); __i != ranges::end(__r); ++__i)
            __m = operator()(__m, *__i, __comp, __proj);

        return __m;
    }
};

struct __minmax
{
    template <class _T, class _Proj = identity, indirect_strict_weak_order<projected<const _T*, _Proj>> _Comp = ranges::less>
    constexpr minmax_result<_T> operator()(const _T& __a, const _T& __b, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (bool(invoke(__comp, invoke(__proj, __b), invoke(__proj, __a))))
            return {__b, __a};
        return {__a, __b};
    }

    template <copyable _T, class _Proj = identity, indirect_strict_weak_order<projected<const _T*, _Proj>> _Comp = ranges::less>
    constexpr minmax_result<_T> operator()(initializer_list<_T> __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        const _T& __min = *__r.begin();
        const _T& __max = *__r.begin();

        for (auto __i = __r.begin() + 1; __i != __r.end(); ++__i)
            if (bool(invoke(__comp, invoke(__proj, *__i), invoke(__proj, __min))))
                __min = *__i;
            else if (bool(invoke(__comp, invoke(__proj, __max), invoke(__proj, *__i))))
                __max = *__i;

        return {__min, __max};
    }

    template <input_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
        requires indirectly_copyable_storable<iterator_t<_R>, range_value_t<_R>*>
    constexpr minmax_result<range_value_t<_R>> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        const range_value_t<_R>& __min = *ranges::begin(__r);
        const range_value_t<_R>& __max = *ranges::begin(__r);

        for (auto __i = ranges::next(ranges::begin(__r)); __i != ranges::end(__r); ++__i)
            if (bool(invoke(__comp, invoke(__proj, *__i), invoke(__proj, __min))))
                __min = *__i;
            else if (bool(invoke(__comp, invoke(__proj, __max), invoke(__proj, *__i))))
                __max = *__i;

        return {__min, __max};
    }
};

struct __min_element
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_strict_weak_order<projected<_I, _Proj>> _Comp = ranges::less>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (__first == __last)
            return __first;

        auto __min = __first;
        ++__first;

        for (; __first != __last; ++__first)
            if (bool(invoke(__comp, invoke(__proj, *__first), invoke(__proj, *__min))))
                __min = __first;

        return __min;
    }

    template <forward_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __max_element
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_strict_weak_order<projected<_I, _Proj>> _Comp = ranges::less>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (__first == __last)
            return __first;

        auto __max = __first;
        ++__first;

        for(; __first != __last; ++__first)
            if (bool(invoke(__comp, invoke(__proj, *__max), invoke(__proj, *__first))))
                __max = __first;

        return __max;
    }

    template <forward_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __minmax_element
{
    template <forward_iterator _I, sentinel_for<_I> _S, class _Proj = identity, indirect_strict_weak_order<projected<_I, _Proj>> _Comp = ranges::less>
    constexpr minmax_element_result<_I> operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (__first == __last)
            return {__first, __first};

        auto __min = __first;
        auto __max = __first;
        ++__first;

        for (; __first != __last; ++__first)
            if (bool(invoke(__comp, invoke(__proj, *__first), invoke(__proj, *__min))))
                __min = __first;
            else if (bool(invoke(__comp, invoke(__proj, *__max), invoke(__proj, *__first))))
                __max = __first;

        return {__min, __max};
    }

    template <forward_range _R, class _Proj = identity, indirect_strict_weak_order<projected<iterator_t<_R>, _Proj>> _Comp = ranges::less>
    constexpr minmax_element_result<safe_iterator_t<_R>> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __lexicographical_compare
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, class _Proj1 = identity, class _Proj2 = identity, indirect_strict_weak_order<projected<_I1, _Proj1>, projected<_I2, _Proj2>> _Comp = ranges::less>
    constexpr bool operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
        {
            if (bool(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))))
                return true;
            else if (bool(invoke(__comp, invoke(__proj2, *__first2), invoke(__proj1, *__first1))))
                return false;
        }

        return __first1 == __last1 && __first2 != __last2;
    }

    template <input_range _R1, input_range _R2, class _Proj1 = identity, class _Proj2 = identity, indirect_strict_weak_order<projected<iterator_t<_R1>, _Proj1>, projected<iterator_t<_R2>, _Proj2>> _Comp = ranges::less>
    constexpr bool operator()(_R1&& __r1, _R2&& __r2, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
    {
        return operator()(ranges::begin(__r1), ranges::end(__r1), ranges::begin(__r2), ranges::end(__r2), __comp, __proj1, __proj2);
    }
};

//! @TODO: not specified in the standard.
struct __lexicographical_compare_three_way
{
    template <input_iterator _I1, sentinel_for<_I1> _S1, input_iterator _I2, sentinel_for<_I2> _S2, class _Proj1 = identity, class _Proj2 = identity, predicate<projected<_I1, _Proj1>, projected<_I2, _Proj2>> _Comp = ranges::__compare_3way>
    constexpr auto operator()(_I1 __first1, _S1 __last1, _I2 __first2, _S2 __last2, _Comp __comp = {}, _Proj1 __proj1 = {}, _Proj2 __proj2 = {}) const
        -> common_comparison_category_t<decltype(invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2))), strong_ordering>
    {
        for (; __first1 != __last1 && __first2 != __last2; ++__first1, (void)++__first2)
            if (auto __c = invoke(__comp, invoke(__proj1, *__first1), invoke(__proj2, *__first2)); __c != 0)
                return __c;

        return __first1 != __last1 ? strong_ordering::greater
             : __first2 != __last2 ? strong_ordering::less
             : strong_ordering::equal;
    }
};

struct __sort_utils
{
protected:

    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp, class _Proj>
    static constexpr void __insertion_sort(_I __first, _S __last, _Comp __comp, _Proj __proj)
    {
        // Zero- and one-element arrays are (trivially) sorted.
        auto __n = static_cast<size_t>(ranges::distance(__first, __last));
        if (__n < 2)
            return;

        for (size_t __i = 1; __i < __n; ++__i)
        {
            using _T = iter_value_t<_I>;
            _T __tmp = ranges::iter_move(__first + __i);

            auto __j = __i;
            for (; __j > 0; --__j)
            {
                if (!bool(invoke(__comp, invoke(__proj, __tmp), invoke(__proj, __first[__j-1]))))
                    break;

                __first[__j] = std::move(__first[__j-1]);
            }

            __first[__j] = std::move(__tmp);
        }
    }

    template <random_access_iterator _I, class _Comp, class _Proj>
    static constexpr _I __median_of_three(_I __first, _I __last, _Comp __comp, _Proj __proj)
    {
        // Note: modifies the array!

        auto __n = static_cast<size_t>(ranges::distance(__first, __last));
        if (__n < 3)
            return __first;

        auto __mid = __n / 2;
        auto __middle = __first + __mid;
        --__last; // Point to an actual element for simplicity.

        if (bool(invoke(__comp, invoke(__proj, *__last), invoke(__proj, *__first))))
            ranges::iter_swap(__last, __first);

        if (bool(invoke(__comp, invoke(__proj, *__middle), invoke(__proj, *__first))))
            ranges::iter_swap(__middle, __first);

        if (bool(invoke(__comp, invoke(__proj, *__last), invoke(__proj, *__middle))))
            ranges::iter_swap(__last, __middle);

        return __middle;
    }

    template <random_access_iterator _I, class _Comp, class _Proj>
    static constexpr void __introsort(_I __first, _I __last, _Comp __comp, _Proj __proj, size_t __depth)
    {
        // Theshold number of elements for switching to an insertion sort.
        constexpr size_t _InsertionSortThreshold = 16;

        auto __n = static_cast<size_t>(ranges::distance(__last, __first));

        // Check for trivially-sorted inputs.
        if (__n <= 1)
            return;

        if (__n <= _InsertionSortThreshold)
        {
            // Insertion sort for sufficiently-small arrays.
            __insertion_sort(__first, __last, __comp, __proj);
            return;
        }

        if (__depth == 0)
        {
            // Fall back to heapsort for guaranteed O(n log n) worst-case behaviour.
            __make_heap{}(__first, __last, __comp, __proj);
            __sort_heap{}(__first, __last, __comp, __proj);
            return;
        }

        // Quicksort for everything else.

        // First step is to partition the array based on a pivot value.
        // The pivot value gets swapped to the end so we can keep track of it.
        auto __pivot = __median_of_three(__first, __last, __comp, __proj);
        auto __pivot_stash = __last - 1;
        ranges::iter_swap(__pivot, __pivot_stash);
        __pivot = __partition{}(__first, __pivot_stash, [&__comp, &__proj, __pivot_stash](const auto& __x) -> bool
        {
            return bool(invoke(__comp, invoke(__proj, __x), invoke(__proj, *__pivot_stash)));
        });
        ranges::iter_swap(__pivot, __pivot_stash);

        // Then, recurse into each half.
        __introsort(__first, __pivot, __comp, __proj, __depth - 1);
        __introsort(__pivot + 1, __last, __comp, __proj, __depth - 1);
    }

    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp, class _Proj>
    static constexpr void __introsort(_I __first, _S __last, _Comp __comp, _Proj __proj)
    {
        // Arbitrary depth threshold.
        auto __n = static_cast<size_t>(__last - __first);
        auto __max_depth = 2 * bit_width(__n);
        __introsort(__first, __first + __n, __comp, __proj, __max_depth);
    }

    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp, class _Proj>
    static constexpr _I __heapselect(_I __first, _S __last, iter_difference_t<_I> __k, _Comp __comp, _Proj __proj)
    {
        // Create a heap and then extract the first k elements.
        __make_heap{}(__first, __last, __comp, __proj);
        for (size_t __i = 0; __i < __k; ++__i)
            __pop_heap{}(__first, __last - __i);

        // The elements are at the wrong end of the array; reverse it to ensure the k'th element is in the right place.
        __reverse{}(__first, __last);
        return __first + __k;
    }

    template <random_access_iterator _I, class _Comp, class _Proj>
    static constexpr void __partial_introsort(_I __first, _I __last, iter_difference_t<_I> __k, _Comp __comp, _Proj __proj, size_t __depth)
    {
        // Note: __k might be > the length of the input array!
        //       (happens when we're called recursively).

        constexpr size_t _InsertionSortThreshold = 16;

        auto __n = static_cast<size_t>(__last - __first);

        // Check for trivially-sorted inputs.
        if (__n <= 1)
            return;

        if (__n <= _InsertionSortThreshold)
        {
            // Insertion sort for sufficiently-small arrays.
            __insertion_sort(__first, __last, __comp, __proj);
            return;
        }

        if (__depth == 0)
        {
            // Fall back to heapsort for guaranteed O(n log n) worst-case behaviour.
            //
            // For a partial sort, heapselect is faster as it only sorts the first k elements.
            __heapselect(__first, __last, __k, __comp, __proj);
            return;
        }

        // Quicksort for everything else.

        // First step is to partition the array based on a pivot value.
        // The pivot value gets swapped to the end so we can keep track of it.
        auto __pivot = __median_of_three(__first, __last, __comp, __proj);
        auto __pivot_stash = __last - 1;
        ranges::iter_swap(__pivot, __pivot_stash);
        __pivot = __partition{}(__first, __pivot_stash, [&__comp, &__proj, __pivot_stash](const auto& __x) -> bool
        {
            return bool(invoke(__comp, invoke(__proj, __x), invoke(__proj, *__pivot_stash)));
        });
        ranges::iter_swap(__pivot, __pivot_stash);

        // Which of the partitions need to be sorted?
        auto __pivot_pos = static_cast<size_t>(__pivot - __first);

        // The left-hand sub-array always needs to be sorted.
        __partial_introsort(__first, __pivot, __k, __comp, __proj, __depth - 1);

        // The right-hand sub-array only needs to be sorted if the k'th value lies within it.
        if (__pivot_pos < __k)
            __partial_introsort(__pivot  + 1, __last, __k - __pivot_pos, __comp, __proj, __depth - 1);
    }

    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp, class _Proj>
    static constexpr void __partial_introsort(_I __first, _S __last, iter_difference_t<_I> __k, _Comp __comp, _Proj __proj)
    {
        // Arbitrary depth threshold.
        auto __n = static_cast<size_t>(__last - __first);
        auto __max_depth = 2 * bit_width(__n);
        __partial_introsort(__first, __first + __n, __k, __comp, __proj, __max_depth);
    }

    template <random_access_iterator _I, class _Comp, class _Proj>
    static constexpr _I __median_of_medians(_I __first, _I __last, _Comp __comp, _Proj __proj)
    {
        // Five elements is the theoretically optimal group size for median-of-medians.
        constexpr size_t _GroupSize = 5;

        if (__last - __first <= _GroupSize)
        {
            // Just do an insertion sort.
            __insertion_sort(__first, __last, __comp, __proj);
            return __first + (__last - __first)/2;
        }

        // Scan the array in groups and get the median of each.
        for (auto __i = 0; __i < __last - __first; __i += _GroupSize)
        {
            auto __f = __first + __i;
            auto __l = (__f + _GroupSize > __last) ? __last : __f + _GroupSize;

            // Do an insertion sort on this group so we can take the median from it.
            __insertion_sort(__f, __l, __comp, __proj);
            
            // Group all the medians together at the beginning of the array.
            ranges::iter_swap(__f + 2, __first + (__i / _GroupSize));
        }

        // Recursively compute the median of the medians that we've grouped.
        auto __num_medians = (__last - __first) / _GroupSize;
        auto __mid = (__num_medians / 2) + 1;
        return __introselect(__first, __first + __num_medians, __mid, __comp, __proj);
    }

    template <random_access_iterator _I, class _Comp, class _Proj>
    static constexpr _I __introselect(_I __first, _I __last, iter_difference_t<_I> __k, _Comp __comp, _Proj __proj, size_t __depth)
    {
        constexpr size_t _InsertionSortThreshold = 16;

        auto __n = static_cast<size_t>(__last - __first);

        // Check for trivially-sorted inputs.
        if (__n <= 1)
            return __first; // Assumed: __k == 0.

        if (__n <= _InsertionSortThreshold)
        {
            // Insertion sort for sufficiently-small arrays.
            __insertion_sort(__first, __last, __comp, __proj);
            return __first + __k;
        }

        // First step is to partition the array based on a pivot value.
        auto __pivot = __first;
        if (__depth == 0)
        {
            // Fall back to median-of-medians for guaranteed O(n) worst-case behaviour.
            __pivot = __median_of_medians(__first, __last, __comp, __proj);

            // Bump the depth so that it doesn't overflow when we recurse below.
            __depth = 1;
        }
        else
        {
            // Median-of-three: may not be a good choice of pivot but often is.
            __pivot = __median_of_three(__first, __last, __comp, __proj);
        }

        // The pivot value gets swapped to the end so we can keep track of it.
        auto __pivot_stash = __last - 1;
        ranges::iter_swap(__pivot, __pivot_stash);
        __pivot = __partition{}(__first, __pivot_stash, [&__comp, &__proj, __pivot_stash](const auto& __x) -> bool
        {
            return bool(invoke(__comp, invoke(__proj, __x), invoke(__proj, *__pivot_stash)));
        });
        ranges::iter_swap(__pivot, __pivot_stash);

        // Which partition do we need to recurse into?
        auto __pivot_pos = static_cast<size_t>(__pivot - __first);
        if (__k < __pivot_pos)
            return __introselect(__first, __pivot, __k, __comp, __proj, __depth - 1);
        else if (__k > __pivot_pos)
            return __introselect(__pivot + 1, __last, __k - __pivot_pos, __comp, __proj, __depth - 1);
        else
            return __pivot;
    }

    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp, class _Proj>
    static constexpr _I __introselect(_I __first, _S __last, iter_difference_t<_I> __k, _Comp __comp, _Proj __proj)
    {
        // Arbitrary depth threshold.
        auto __n = static_cast<size_t>(__last - __first);
        auto __max_depth = 2 * bit_width(__n);
        return __introselect(__first, __first + __n, __k, __comp, __proj, __max_depth);
    }
};

struct __sort : __sort_utils
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        __introsort(__first, __last, __comp, __proj);
        return ranges::next(__first, __last);
    }

    template <random_access_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

//! @TODO: __stable_sort

struct __partial_sort : __sort_utils
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _I __middle, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        __partial_introsort(__first, __last, __middle - __last, __comp, __proj);
        return ranges::next(__first, __last);
    }

    template <random_access_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

//! @TODO: __partial_sort_copy

struct __nth_element : __sort_utils
{
    template <random_access_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr _I operator()(_I __first, _I __nth, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        __introselect(__first, __last, __nth - __first, __comp, __proj);
        return ranges::next(__first, __last);
    }

    template <random_access_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr safe_iterator_t<_R> operator()(_R&& __r, iterator_t<_R> __nth, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), __nth, ranges::end(__r), __comp, __proj);
    }
};

struct __next_permutation
{
    template <bidirectional_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr next_permutation_result<_I> operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        if (__first == __last || ranges::next(__first) == __last)
            return {false, __first};

        // Highest index k such that A[k] < A[k + 1].
        auto __k = __last;
        for (auto __i = __first; ranges::next(__i) != __last; ++__i)
            if (bool(invoke(__comp, invoke(__proj, *__i), invoke(__proj, *ranges::next(__i)))))
                __k = __i;

        if (__k == __last)
        {
            // This is the last permutation and is in reverse-sorted order.
            __reverse{}(__first, __last);
            return {false, __k};
        }

        // Highest index > k such that A[k] < A[l] (always exists).
        auto __l = __last;
        for (auto __i = ranges::next(__k); __i != __last; ++__i)
            if (bool(invoke(__comp, invoke(__proj, *__k), invoke(__proj, *__i))))
                __l = __i;

        ranges::iter_swap(__k, __l);
        __reverse{}(ranges::next(__k), __last);
        return {true, ranges::next(__k, __last)};
    }

    template <bidirectional_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr next_permutation_result<safe_iterator_t<_R>> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

struct __prev_permutation
{
    template <bidirectional_iterator _I, sentinel_for<_I> _S, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<_I, _Comp, _Proj>
    constexpr prev_permutation_result<_I> operator()(_I __first, _S __last, _Comp __comp = {}, _Proj __proj = {}) const
    {
        auto __end = ranges::next(__first, __last);
        auto __rfirst = make_reverse_iterator(__end);
        auto __rlast = make_reverse_iterator(__first);
        auto [__found, in] = __next_permutation{}(__rfirst, __rlast, __comp, __proj);
        return {__found, __end};
    }

    template <bidirectional_range _R, class _Comp = ranges::less, class _Proj = identity>
        requires sortable<iterator_t<_R>, _Comp, _Proj>
    constexpr prev_permutation_result<iterator_t<_R>> operator()(_R&& __r, _Comp __comp = {}, _Proj __proj = {}) const
    {
        return operator()(ranges::begin(__r), ranges::end(__r), __comp, __proj);
    }
};

} // namespace __detail

inline namespace __algorithm
{

inline constexpr __detail::__all_of                     all_of = {};
inline constexpr __detail::__any_of                     any_of = {};
inline constexpr __detail::__none_of                    none_of = {};
inline constexpr __detail::__for_each                   for_each = {};
inline constexpr __detail::__find                       find = {};
inline constexpr __detail::__find_if                    find_if = {};
inline constexpr __detail::__find_if_not                find_if_not = {};
inline constexpr __detail::__find_first_of              find_first_of = {};
inline constexpr __detail::__adjacent_find              adjacent_find = {};
inline constexpr __detail::__count                      count = {};
inline constexpr __detail::__count_if                   count_if = {};
inline constexpr __detail::__mismatch                   mismatch = {};
inline constexpr __detail::__equal                      equal = {};
inline constexpr __detail::__find_end                   find_end = {};
inline constexpr __detail::__is_permutation             is_permutation = {};
inline constexpr __detail::__search                     search = {};
inline constexpr __detail::__search_n                   search_n = {};
inline constexpr __detail::__copy                       copy = {};
inline constexpr __detail::__copy_n                     copy_n = {};
inline constexpr __detail::__copy_if                    copy_if = {};
inline constexpr __detail::__copy_backward              copy_backward = {};
inline constexpr __detail::__move                       move = {};
inline constexpr __detail::__move_backward              move_backward = {};
inline constexpr __detail::__swap_ranges                swap_ranges = {};
inline constexpr __detail::__transform                  transform = {};
inline constexpr __detail::__replace                    replace = {};
inline constexpr __detail::__replace_if                 replace_if = {};
inline constexpr __detail::__replace_copy               replace_copy = {};
inline constexpr __detail::__replace_copy_if            replace_copy_if = {};
inline constexpr __detail::__fill                       fill = {};
inline constexpr __detail::__fill_n                     fill_n = {};
inline constexpr __detail::__generate                   generate = {};
inline constexpr __detail::__generate_n                 generate_n = {};
inline constexpr __detail::__remove                     remove = {};
inline constexpr __detail::__remove_if                  remove_if = {};
inline constexpr __detail::__remove_copy                remove_copy = {};
inline constexpr __detail::__remove_copy_if             remove_copy_if = {};
inline constexpr __detail::__unique                     unique = {};
inline constexpr __detail::__unique_copy                unique_copy = {};
inline constexpr __detail::__reverse                    reverse = {};
inline constexpr __detail::__reverse_copy               reverse_copy = {};
inline constexpr __detail::__rotate                     rotate = {};
inline constexpr __detail::__rotate_copy                rotate_copy = {};
inline constexpr __detail::__is_sorted_until            is_sorted_until = {};
inline constexpr __detail::__is_sorted                  is_sorted = {};
inline constexpr __detail::__lower_bound                lower_bound = {};
inline constexpr __detail::__upper_bound                upper_bound = {};
inline constexpr __detail::__equal_range                equal_range = {};
inline constexpr __detail::__binary_search              binary_search = {};
inline constexpr __detail::__is_partitioned             is_partitioned = {};
inline constexpr __detail::__partition                  partition = {};
inline constexpr __detail::__partition_copy             partition_copy = {};
inline constexpr __detail::__stable_partition           stable_partition = {};
inline constexpr __detail::__partition_point            partition_point = {};
inline constexpr __detail::__merge                      merge = {};
inline constexpr __detail::__inplace_merge              inplace_merge = {};
inline constexpr __detail::__includes                   includes = {};
inline constexpr __detail::__set_union                  set_union = {};
inline constexpr __detail::__set_intersection           set_intersection = {};
inline constexpr __detail::__set_difference             set_difference = {};
inline constexpr __detail::__set_symmetric_difference   set_symmetric_difference = {};
inline constexpr __detail::__push_heap                  push_heap = {};
inline constexpr __detail::__pop_heap                   pop_heap = {};
inline constexpr __detail::__make_heap                  make_heap = {};
inline constexpr __detail::__sort_heap                  sort_heap = {};
inline constexpr __detail::__is_heap_until              is_heap_until = {};
inline constexpr __detail::__is_heap                    is_heap = {};
inline constexpr __detail::__min                        min = {};
inline constexpr __detail::__max                        max = {};
inline constexpr __detail::__minmax                     minmax = {};
inline constexpr __detail::__min_element                min_element = {};
inline constexpr __detail::__max_element                max_element = {};
inline constexpr __detail::__minmax_element             minmax_element = {};
inline constexpr __detail::__lexicographical_compare    lexicographical_compare = {};
inline constexpr __detail::__sort                       sort = {};
inline constexpr __detail::__partial_sort               partial_sort = {};
inline constexpr __detail::__nth_element                nth_element = {};
inline constexpr __detail::__next_permutation           next_permutation = {};
inline constexpr __detail::__prev_permutation           prev_permutation = {};

} // namespace __algorithm


// Needs to be defined after ranges::mismatch.
template <input_range _V, forward_range _Pattern>
    requires view<_V>
        && view<_Pattern>
        && indirectly_comparable<iterator_t<_V>, iterator_t<_Pattern>, __detail::__equal_to>
        && (forward_range<_V> || __detail::__tiny_range<_Pattern>)
template <bool _Const>
constexpr typename split_view<_V, _Pattern>::template __outer_iterator<_Const>& split_view<_V, _Pattern>::__outer_iterator<_Const>::operator++()
{
    const auto __end = ranges::end(_M_parent->_M_base);

    if (_M_current == __end)
        return *this;

    const auto [__pbegin, __pend] = subrange{_M_parent->_M_pattern};

    if (__pbegin == __pend)
        ++__current();
    else
    {
        do
        {
            auto [__p, __b] = ranges::mismatch(std::move(__current()), __end, __pbegin, __pend);
            __current() = std::move(__b);

            if (__p == __end)
                break;
        }
        while (++__current() != __end);
    }

    return *this;
}


} // namespace ranges
#endif

} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_RANGESALGORITHM_H */
