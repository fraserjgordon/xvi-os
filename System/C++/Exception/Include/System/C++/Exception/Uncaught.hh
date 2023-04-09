#ifndef __SYSTEM_CXX_EXCEPTION_UNCAUGHT_H
#define __SYSTEM_CXX_EXCEPTION_UNCAUGHT_H


#include <System/C++/Exception/Private/Config.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


__SYSTEM_CXX_EXCEPTION_EXPORT int uncaught_exceptions() noexcept;

[[deprecated("use std::uncaught_exceptions")]]
inline bool uncaught_exception() noexcept
{
    return uncaught_exceptions() > 0;
}


} // namespace __XVI_STD_EXCEPTION_NS_DECL


#endif /* ifndef __SYSTEM_CXX_EXCEPTION_UNCAUGHT_H */
