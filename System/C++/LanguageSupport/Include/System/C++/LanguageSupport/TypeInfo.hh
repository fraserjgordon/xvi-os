#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_TYPEINFO_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_TYPEINFO_H


#include <System/ABI/C++/TypeInfo.hh>

#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


#if !__XVI_FOREIGN_HOSTED
using std::type_info;
#endif


class bad_cast : public exception
{
public:

    bad_cast() noexcept = default;
    bad_cast(const bad_cast&) noexcept = default;
    bad_cast& operator=(const bad_cast&) noexcept = default;
    const char* what() const noexcept override
        { return "bad cast"; }
};

class bad_typeid : public exception
{
public:

    bad_typeid() noexcept = default;
    bad_typeid(const bad_typeid&) noexcept = default;
    bad_typeid& operator=(const bad_typeid&) noexcept = default;
    const char* what() const noexcept override
        { return "bad typeid"; }
};


} // namespace __XVI_STD_LANGSUPPORT_NS



#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_TYPEINFO_H */
