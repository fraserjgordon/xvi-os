#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ABS_H
#define __SYSTEM_CXX_UTILITY_ABS_H


#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{

template <class _T>
constexpr _T __sign(_T __t)
{
    return (__t < 0) ? _T(-1) : _T(1);
}

constexpr int abs(int __j)
{
    return (__j < 0) ? -__j : __j;
}

constexpr long abs(long __j)
{
    return (__j < 0) ? -__j : __j;
}

constexpr long long abs(long long __j)
{
    return (__j < 0) ? -__j : __j;
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ABS_H */
