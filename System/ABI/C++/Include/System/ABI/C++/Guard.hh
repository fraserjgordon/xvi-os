#pragma once
#ifndef __SYSTEM_ABI_CXX_GUARD_H
#define __SYSTEM_ABI_CXX_GUARD_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/C++/Private/Config.hh>


namespace __cxxabiv1
{


extern "C" __SYSTEM_ABI_CXX_GUARD_EXPORT int __cxa_guard_acquire(std::int64_t*);
extern "C" __SYSTEM_ABI_CXX_GUARD_EXPORT void __cxa_guard_release(std::int64_t*);
extern "C" __SYSTEM_ABI_CXX_GUARD_EXPORT void __cxa_guard_abort(std::int64_t*);


} // namespace __cxxabiv1


#endif /* ifndef __SYSTEM_ABI_CXX_GUARD_H */
