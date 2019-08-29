#pragma once
#ifndef __SYSTEM_CXX_UTILITY_MBSTATE_H
#define __SYSTEM_CXX_UTILITY_MBSTATE_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


struct mbstate_t
{
    uint16_t __state;
    uint8_t  __counter;
    uint8_t  __buffer[5];
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_MBSTATE_H */
