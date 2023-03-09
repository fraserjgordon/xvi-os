#ifndef __SYSTEM_CXX_CORE_PRIVATE_ADDRESSOF_H
#define __SYSTEM_CXX_CORE_PRIVATE_ADDRESSOF_H


#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


template <class _T> constexpr _T* addressof(_T& __t)
{
    return __builtin_addressof(__t);
}


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_PRIVATE_ADDRESSOF_H */
