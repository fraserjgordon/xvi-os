#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_LIMITS_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_LIMITS_H


#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


enum float_round_style
{
    round_indeterminate         = -1,
    round_toward_zero           = 0,
    round_to_nearest            = 1,
    round_toward_infinity       = 2,
    round_toward_neg_infinity   = 3,
};

enum float_denorm_style
{
    denorm_indeterminate        = -1,
    denorm_absent               = 0,
    denorm_present              = 1,
};


namespace __detail
{

template <class _T> struct __bool_limits
{
    static constexpr bool is_specialized = true;
    static constexpr _T min() noexcept { return false; }
    static constexpr _T max() noexcept { return true; }
    static constexpr _T lowest() noexcept { return false; }

    static constexpr int digits = 1;
    static constexpr int digits10 = 0;
    static constexpr int max_digits10 = 0;

    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static constexpr _T epsilon() noexcept { return 0; }
    static constexpr _T round_error() noexcept { return 0; }

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;

    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr _T infinity() noexcept { return 0; }
    static constexpr _T quiet_NaN() noexcept { return 0; }
    static constexpr _T signaling_NaN() noexcept { return 0; }
    static constexpr _T denorm_min() noexcept { return 0; }

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
};

template <class _T> struct __integer_limits
{
    static constexpr bool is_specialized = true;

    static constexpr bool is_signed = _T(-1) < _T(0);
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static constexpr int digits = is_signed ? (8 * sizeof(_T) - 1) : (8 * sizeof(_T));
    static constexpr int digits10 = (digits * 100) / 333;   // Good-enough approximation.
    static constexpr int max_digits10 = 0;

    static constexpr _T min() noexcept { if constexpr (is_signed) return (_T(1) << digits); else return _T(0); }
    static constexpr _T max() noexcept { return ~min(); }
    static constexpr _T lowest() noexcept { return min(); }

    static constexpr _T epsilon() noexcept { return _T(0); }
    static constexpr _T round_error() noexcept { return _T(0); }

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;

    static constexpr _T infinity() noexcept { return _T(0); }
    static constexpr _T quiet_NaN() noexcept { return _T(0); }
    static constexpr _T signaling_NaN() noexcept { return _T(0); }
    static constexpr _T denorm_min() noexcept { return _T(0); }

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = !is_signed;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_zero;
};

template <class _T> struct __float_limits
{

};

}


template <class _T> struct numeric_limits
{
    static constexpr bool is_specialized = false;
    static constexpr _T min() noexcept { return _T(); }
    static constexpr _T max() noexcept { return _T(); }
    static constexpr _T lowest() noexcept { return _T(); }

    static constexpr int digits = 0;
    static constexpr int digits10 = 0;
    static constexpr int max_digits10 = 0;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr int radix = 0;
    static constexpr _T epsilon() noexcept { return _T(); }
    static constexpr _T round_error() noexcept { return _T(); }

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;

    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr _T infinity() noexcept { return _T(); }
    static constexpr _T quiet_Nan() noexcept { return _T(); }
    static constexpr _T signaling_NaN() noexcept { return _T(); }
    static constexpr _T denorm_min() noexcept { return _T(); }

    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = false;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_toward_infinity;
};


template <> struct numeric_limits<bool> : __detail::__bool_limits<bool> {};

template <> struct numeric_limits<char> : __detail::__integer_limits<char> {};
template <> struct numeric_limits<signed char> : __detail::__integer_limits<signed char> {};
template <> struct numeric_limits<unsigned char> : __detail::__integer_limits<unsigned char> {};

template <> struct numeric_limits<char8_t> : __detail::__integer_limits<char8_t> {};
template <> struct numeric_limits<char16_t> : __detail::__integer_limits<char16_t> {};
template <> struct numeric_limits<char32_t> : __detail::__integer_limits<char32_t> {};
template <> struct numeric_limits<wchar_t> : __detail::__integer_limits<wchar_t> {};

template <> struct numeric_limits<short> : __detail::__integer_limits<short> {};
template <> struct numeric_limits<int> : __detail::__integer_limits<int> {};
template <> struct numeric_limits<long> : __detail::__integer_limits<long> {};
template <> struct numeric_limits<long long> : __detail::__integer_limits<long long> {};
template <> struct numeric_limits<unsigned short> : __detail::__integer_limits<unsigned short> {};
template <> struct numeric_limits<unsigned int> : __detail::__integer_limits<unsigned int> {};
template <> struct numeric_limits<unsigned long> : __detail::__integer_limits<unsigned long> {};
template <> struct numeric_limits<unsigned long long> : __detail::__integer_limits<unsigned long long> {};

template <> struct numeric_limits<float> : __detail::__float_limits<float> {};
template <> struct numeric_limits<double> : __detail::__float_limits<double> {};
template <> struct numeric_limits<long double> : __detail::__float_limits<long double> {};

template <class _T> struct numeric_limits<const _T> : numeric_limits<_T> {};
template <class _T> struct numeric_limits<volatile _T> : numeric_limits<_T> {};
template <class _T> struct numeric_limits<const volatile _T> : numeric_limits<_T> {};


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_LIMITS_H */
