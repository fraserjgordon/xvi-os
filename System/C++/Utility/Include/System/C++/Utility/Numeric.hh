#pragma once
#ifndef __SYSTEM_CXX_UTILITY_NUMERIC_H
#define __SYSTEM_CXX_UTILITY_NUMERIC_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Abs.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _M, class _N>
constexpr common_type_t<_M, _N> gcd(_M __m, _N __n)
{
    __m = __XVI_STD_UTILITY_NS::abs(__m);
    __n = __XVI_STD_UTILITY_NS::abs(__n);    

    unsigned int __shift = 0;
    while (__m % 2 == 0 && __n % 2 == 0)
    {
        __m /= 2;
        __n /= 2;
        __shift += 1;
    }

    while (__m != __n)
    {
        if (__m % 2 == 0)
            __m /= 2;
        else if (__n % 2 == 0)
            __n /= 2;
        else if (__m > __n)
            __m = (__m - __n) / 2;
        else
            __n = (__n - __m) / 2;
    }

    return (__m << __shift);
}

template <class _M, class _N>
constexpr common_type_t<_M, _N> lcm(_M __m, _N __n)
{
    __m = __XVI_STD_UTILITY_NS::abs(__m);
    __n = __XVI_STD_UTILITY_NS::abs(__n);
    
    common_type_t<_M, _N> __gcd = __XVI_STD_UTILITY_NS::gcd(__m, __n);
    return (__m / __gcd) * __n;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_NUMERIC_H */
