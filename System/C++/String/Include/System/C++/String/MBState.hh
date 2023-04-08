#ifndef __SYSTEM_CXX_STRING_MBSTATE_H
#define __SYSTEM_CXX_STRING_MBSTATE_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/C++/String/Private/Config.hh>


namespace __XVI_STD_STRING_NS_DECL
{


struct mbstate_t
{
    uint16_t __state;
    uint8_t  __counter;
    uint8_t  __buffer[5];
};


} // namespace __XVI_STD_STRING_NS_DECL


#endif /* ifndef __SYSTEM_CXX_STRING_MBSTATE_H */
