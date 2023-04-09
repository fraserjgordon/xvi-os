#ifndef __SYSTEM_CXX_EXCEPTION_UNEXPECTED_H
#define __SYSTEM_CXX_EXCEPTION_UNEXPECTED_H


#include <System/C++/Exception/Private/Config.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


// The following are deprecated (along with exception specifications).
using unexpected_handler = void (*)();
__SYSTEM_CXX_EXCEPTION_EXPORT [[deprecated]] unexpected_handler set_unexpected(unexpected_handler) noexcept;
__SYSTEM_CXX_EXCEPTION_EXPORT [[deprecated]] unexpected_handler get_unexpected() noexcept;


} // namespace __XVI_STD_EXCEPTION_NS_DECL


#endif /* ifndef __SYSTEM_CXX_EXCEPTION_UNEXPECTED_H */
