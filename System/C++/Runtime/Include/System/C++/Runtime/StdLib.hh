#pragma once
#ifndef __SYSTEM_CXX_RUNTIME_STDLIB_H
#define __SYSTEM_CXX_RUNTIME_STDLIB_H


#include <System/C++/Runtime/Private/Config.hh>


namespace __XVI_STD_RUNTIME_NS
{


namespace __detail
{

extern "C" using __c_atexit_handler = void();
extern "C++" using __cxx_atexit_handler = void();

} // namespace __detail


[[noreturn]] void abort() noexcept;
int atexit(__detail::__c_atexit_handler*) noexcept;
int at_quick_exit(__detail::__c_atexit_handler*) noexcept;
[[noreturn]] void exit(int __status);
[[noreturn]] void _Exit(int __status);
[[noreturn]] void quick_exit(int __status);

char* getenv(const char* __name);
int system(const char* __string);


} // namespace __XVI_STD_RUNTIME_NS


#endif /* ifndef __SYSTEM_CXX_RUNTIME_STDLIB_H */
