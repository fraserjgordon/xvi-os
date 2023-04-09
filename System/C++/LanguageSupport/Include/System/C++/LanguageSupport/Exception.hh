#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H


#include <System/C++/LanguageSupport/Private/Config.hh>


namespace __XVI_STD_LANGSUPPORT_NS_DECL
{


__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT class exception
{
public:

    exception() noexcept = default;
    exception(const exception&) noexcept = default;
    exception& operator= (const exception&) noexcept = default;
    virtual ~exception() = default;

    virtual const char* what() const noexcept;
};

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT class bad_exception : public exception
{
public:

    bad_exception() noexcept = default;
    bad_exception(const bad_exception&) = default;
    bad_exception& operator= (const bad_exception&) noexcept = default;
    
    const char* what() const noexcept override;
};

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT class bad_alloc : public exception
{
public:

    bad_alloc() noexcept = default;
    bad_alloc(const bad_alloc&) = default;
    bad_alloc& operator=(const bad_alloc&) noexcept = default;

    const char* what() const noexcept override;
};

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT class bad_array_new_length : public exception
{
public:

    bad_array_new_length() noexcept = default;
    bad_array_new_length(const bad_array_new_length&) = default;
    bad_array_new_length& operator=(const bad_array_new_length&) noexcept = default;

    const char* what() const noexcept override;
};


} // namespace __XVI_STD_LANGSUPPORT_NS_DECL


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H */
