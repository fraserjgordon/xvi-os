#ifndef __SYSTEM_CXX_EXCEPTION_TERMINATE_H
#define __SYSTEM_CXX_EXCEPTION_TERMINATE_H


#include <System/C++/Exception/Private/Config.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


using terminate_handler = void (*)();
__SYSTEM_CXX_EXCEPTION_EXPORT terminate_handler set_terminate(terminate_handler) noexcept;
__SYSTEM_CXX_EXCEPTION_EXPORT terminate_handler get_terminate() noexcept;

__SYSTEM_CXX_EXCEPTION_EXPORT [[noreturn]] void terminate();


} // namespace __XVI_STD_EXCEPTION_NS_DECL


#endif /* ifndef __SYSTEM_CXX_EXCEPTION_TERMINATE_H */
