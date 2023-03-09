#ifndef __SYSTEM_CXX_CORE_INTEGERSEQUENCE_H
#define __SYSTEM_CXX_CORE_INTEGERSEQUENCE_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


template <class _T, _T... _I> struct integer_sequence
{
    using value_type = _T;
    static constexpr size_t size() noexcept { return sizeof...(_I); }
};
template <size_t... _I> using index_sequence = integer_sequence<size_t, _I...>;

#ifdef __llvm__
template <class _T, _T _Target, _T... _Nums> struct __make_integer_sequence
{
    static auto __calc()
    {
        if constexpr (sizeof...(_Nums) == _Target)
            return integer_sequence<_T, _Nums...>{};
        else
            return typename __make_integer_sequence<_T, _Target, _Nums..., sizeof...(_Nums)>::__type {};
    }

    using __type = decltype(__calc());
};

template <class _T, _T _N> using make_integer_sequence = typename __make_integer_sequence<_T, _N>::__type;
#else
// GCC has a special built-in for this.
template <class _T, _T _N> using make_integer_sequence = integer_sequence<_T, __integer_pack(_N)...>;
#endif

template <size_t _N> using make_index_sequence = make_integer_sequence<size_t, _N>;


namespace __detail
{

template <class _T, _T... _Seq1, _T... _Seq2>
integer_sequence<_T, _Seq1..., _Seq2...> __cat_integer_seq_helper_fn(integer_sequence<_T, _Seq1...>, integer_sequence<_T, _Seq2...>);

template <class _Seq1, class _Seq2> using __cat_two_integer_sequences = decltype(__cat_integer_seq_helper_fn(declval<_Seq1>(), declval<_Seq2>()));

template <class... _Seqs> struct __cat_integer_sequences_helper;

template <class _Seq1, class _Seq2, class... _Rest> struct __cat_integer_sequences_helper<_Seq1, _Seq2, _Rest...>
    { using type = typename __cat_integer_sequences_helper<__cat_two_integer_sequences<_Seq1, _Seq2>, _Rest...>::type; };
template <class _Seq> struct __cat_integer_sequences_helper<_Seq>
    { using type = _Seq; };

template <class... _Seqs> using __cat_integer_sequences = typename __cat_integer_sequences_helper<_Seqs...>::type;


template <class _T, _T _Value, size_t _N, _T... _Seq> struct __repeat_integer_helper
    { using type = typename __repeat_integer_helper<_T, _Value, _N-1, _Seq..., _Value>::type; };
template <class _T, _T _Value, _T... _Seq> struct __repeat_integer_helper<_T, _Value, 0, _Seq...>
    { using type = integer_sequence<_T, _Seq...>; };

template <class _T, _T _Value, size_t _N> using __repeated_integer_sequence = typename __repeat_integer_helper<_T, _Value, _N>::type;


template <class _T, size_t... _Counts, size_t... _Values>
__cat_integer_sequences<__repeated_integer_sequence<_T, _Values, _Counts>...>
__make_repeated_integer_sequence_helper_fn(index_sequence<_Values...>);

template <class _T, size_t... _Counts>
using __make_repeated_integer_sequence = decltype(__make_repeated_integer_sequence_helper_fn<_T, _Counts...>(make_index_sequence<sizeof...(_Counts)>()));

} // namespace __detail


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_INTEGERSEQUENCE_H */
