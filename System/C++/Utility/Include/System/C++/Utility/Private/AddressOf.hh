#pragma once
#ifndef __SYSTEM_CXX_UTILITY_PRIVATE_ADDRESSOF_H
#define __SYSTEM_CXX_UTILITY_PRIVATE_ADDRESSOF_H


#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _T> constexpr _T* addressof(_T& __t)
{
    return __builtin_addressof(__t);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_PRIVATE_ADDRESSOF_H */
