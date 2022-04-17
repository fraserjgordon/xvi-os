#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_SOURCELOCATION_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_SOURCELOCATION_H


#include <System/C++/LanguageSupport/Private/Namespace.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


struct source_location
{
    static consteval source_location current() noexcept
    {
        source_location __loc;
#if __has_builtin(__builtin_LINE)
        __loc._M_line = __builtin_LINE();
        __loc._M_column = __builtin_COLUMN();
        __loc._M_file_name = __builtin_FILE();
        __loc._M_function_name = __builtin_FUNCTION();
#endif
        return __loc;
    }

    constexpr source_location() noexcept = default;

    constexpr uint_least32_t line() const noexcept
    {
        return _M_line;
    }

    constexpr uint_least32_t column() const noexcept
    {
        return _M_column;
    }

    constexpr const char* file_name() const noexcept
    {
        return _M_file_name;
    }

    constexpr const char* function_name() const noexcept
    {
        return _M_function_name;
    }

private:

    uint_least32_t _M_line = 0;
    uint_least32_t _M_column = 0;
    const char* _M_file_name = "";
    const char* _M_function_name = "";
};


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_SOURCELOCATION_H */
