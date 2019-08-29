#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_CONTRACT_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_CONTRACT_H


#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


// Forward declarations.
class string_view;


class contract_violation
{
public:

    uint_least32_t line_number() const noexcept;
    string_view file_name() const noexcept;
    string_view function_name() const noexcept;
    string_view comment() const noexcept;
    string_view assertion_level() const noexcept;
};


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_CONTRACT_H */
