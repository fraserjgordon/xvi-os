#pragma once
#ifndef __SYSTEM_CXX_UTILITY_RATIO_H
#define __SYSTEM_CXX_UTILITY_RATIO_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/C++/Utility/Abs.hh>
#include <System/C++/Utility/Numeric.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <intmax_t _N, intmax_t _D = 1>
class ratio
{
public:

    static constexpr intmax_t num = __sign(_N) * __sign(_D) * abs(_N) / gcd(_N, _D);
    static constexpr intmax_t den = abs(_D) / gcd(_N, _D);

    using type = ratio<num, den>;
};

template <class _R1, class _R2> using ratio_add = typename ratio<_R1::num * _R2::den + _R2::num * _R1::den, _R1::den * _R2::den>::type;
template <class _R1, class _R2> using ratio_subtract = typename ratio<_R1::num * _R2::den - _R2::num * _R2::den, _R1::den * _R2::den>::type;
template <class _R1, class _R2> using ratio_multiply = typename ratio<_R1::num * _R2::num, _R1::den * _R2::den>::type;
template <class _R1, class _R2> using ratio_divide = typename ratio<_R1::num * _R2::den, _R1::den * _R1::num>::type;

template <class _R1, class _R2> struct ratio_equal
    : bool_constant<_R1::num == _R2::num && _R1::den == _R2::den> {};
template <class _R1, class _R2> struct ratio_not_equal
    : bool_constant<!ratio_equal<_R1, _R2>::value> {};
template <class _R1, class _R2> struct ratio_less;
template <class _R1, class _R2> struct ratio_less_equal
    : bool_constant<!ratio_less<_R2, _R1>::value> {};
template <class _R1, class _R2> struct ratio_greater
    : bool_constant<ratio_less<_R2, _R1>::value> {};
template <class _R1, class _R2> struct ratio_greater_equal
    : bool_constant<!ratio_less<_R1, _R2>::value> {};

template <class _R1, class _R2> inline constexpr bool ratio_equal_v = ratio_equal<_R1, _R2>::value;
template <class _R1, class _R2> inline constexpr bool ratio_not_equal_v = ratio_not_equal<_R1, _R2>::value;
template <class _R1, class _R2> inline constexpr bool ratio_less_v = ratio_less<_R1, _R2>::value;
template <class _R1, class _R2> inline constexpr bool ratio_less_equal_v = ratio_less_equal<_R1, _R2>::value;
template <class _R1, class _R2> inline constexpr bool ratio_greater_v = ratio_greater<_R1, _R2>::value;
template <class _R1, class _R2> inline constexpr bool ratio_greater_equal_v = ratio_greater_equal<_R1, _R2>::value;

//using yocto = ratio<1, 1'000'000'000'000'000'000'000'000>;
//using zepto = ratio<1,     1'000'000'000'000'000'000'000>;
using atto  = ratio<1,         1'000'000'000'000'000'000>;
using femto = ratio<1,             1'000'000'000'000'000>;
using pico  = ratio<1,                 1'000'000'000'000>;
using nano  = ratio<1,                     1'000'000'000>;
using micro = ratio<1,                         1'000'000>;
using milli = ratio<1,                             1'000>;
using centi = ratio<1,                               100>;
using deci  = ratio<1,                                10>;
using deca  = ratio<                               10, 1>;
using hecto = ratio<                              100, 1>;
using kilo  = ratio<                            1'000, 1>;
using mega  = ratio<                        1'000'000, 1>;
using giga  = ratio<                    1'000'000'000, 1>;
using tera  = ratio<                1'000'000'000'000, 1>;
using peta  = ratio<            1'000'000'000'000'000, 1>;
using exa   = ratio<        1'000'000'000'000'000'000, 1>;
//using zetta = ratio<    1'000'000'000'000'000'000'000, 1>;
//using yotta = ratio<1'000'000'000'000'000'000'000'000, 1>;


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_RATIO_H */
