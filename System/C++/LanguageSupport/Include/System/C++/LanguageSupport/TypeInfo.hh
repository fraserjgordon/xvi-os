#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_TYPEINFO_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_TYPEINFO_H


#if defined(__XVI_HOSTED)
#  define __XVI_USING_HOST_TYPEINFO
#  include <typeinfo>
#endif

#include <System/ABI/C++/TypeInfo.hh>

#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS_DECL
{


#if defined(__XVI_USING_HOST_TYPEINFO)
using type_info = std::type_info;
#endif


class bad_cast : public exception
{
public:

    bad_cast() noexcept = default;
    bad_cast(const bad_cast&) noexcept = default;
    bad_cast& operator=(const bad_cast&) noexcept = default;
    const char* what() const noexcept override;
};

class bad_typeid : public exception
{
public:

    bad_typeid() noexcept = default;
    bad_typeid(const bad_typeid&) noexcept = default;
    bad_typeid& operator=(const bad_typeid&) noexcept = default;
    const char* what() const noexcept override;
};


} // namespace __XVI_STD_LANGSUPPORT_NS_DECL



#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_TYPEINFO_H */
