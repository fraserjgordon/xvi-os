#pragma once
#ifndef __SYSTEM_ABI_CXX_DYNAMICCAST_H
#define __SYSTEM_ABI_CXX_DYNAMICCAST_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/ABI/C++/Private/Config.hh>
#include <System/ABI/C++/TypeInfo.hh>


namespace __SYSTEM_ABI_CXX_NS
{


extern "C"
__SYSTEM_ABI_CXX_EXPORT
void* __dynamic_cast(const void* __sub, const __class_type_info* src, const __class_type_info* dest, std::ptrdiff_t src2dest_offset);


} // namespace __SYSTEM_ABI_CXX_NS


#endif /* ifndef __SYSTEM_ABI_CXX_DYNAMICCAST_H */
