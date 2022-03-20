#ifndef __SYSTEM_CXX_UTILITY_CSTRING_H
#define __SYSTEM_CXX_UTILITY_CSTRING_H


#include <System/C++/Utility/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>


namespace __XVI_STD_UTILITY_NS
{


//! @TODO: finish this.


void* memcpy(void* __restrict, const void* __restrict, std::size_t) asm("memcpy");
void* memmove(void*, const void*, std::size_t) asm("memmove");


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_CSTRING_H */
