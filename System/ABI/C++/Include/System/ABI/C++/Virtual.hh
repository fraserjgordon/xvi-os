#pragma once
#ifndef __SYSTEM_ABI_CXX_VIRTUAL_H
#define __SYSTEM_ABI_CXX_VIRTUAL_H


#include <System/ABI/C++/Private/Config.hh>


namespace __SYSTEM_ABI_CXX_NS
{


extern "C" __SYSTEM_ABI_CXX_VIRTUAL_EXPORT [[noreturn]] void __cxa_deleted_virtual();
extern "C" __SYSTEM_ABI_CXX_VIRTUAL_EXPORT [[noreturn]] void __cxa_pure_virtual();


} // namespace __SYSTEM_ABI_CXX_NS


#endif /* ifndef __SYSTEM_ABI_CXX_VIRTUAL_H */
