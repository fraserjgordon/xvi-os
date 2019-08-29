#pragma once
#ifndef __SYSTEM_CXX_UTILITY_STDEXCEPT_H
#define __SYSTEM_CXX_UTILITY_STDEXCEPT_H


#include <System/C++/LanguageSupport/Exception.hh>

#include <System/C++/Utility/SharedPtr.hh>
#include <System/C++/Utility/StringView.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{


template <class... _Args>
shared_ptr<const char[]> __make_exception_string(_Args&&... __args)
{
    auto __get_len = []<class _T>(const _T& __t)
    {
        return string_view(__t).length();
    };

    auto __get_ptr = []<class _T>(const _T& __t)
    {
        return string_view(__t).data();
    };

    size_t __len = (1 + ... + __get_len(__args));

    auto __ptr = make_shared<char[]>(__len);
    __ptr[0] = '\0';

    (__builtin_strcat(__ptr.get(), __get_ptr(__args)), ...);

    return const_pointer_cast<const char[]>(__ptr);
}


} // namespace __detail


class logic_error : public exception
{
public:

    explicit logic_error(const __XVI_STD_STRING_NS::string& __what);

    explicit logic_error(const char* __what)
        : _M_what(__detail::__make_exception_string(__what))
    {
    }

    const char* what() const noexcept override
    {
        return _M_what.get();
    }

protected:

    const shared_ptr<const char[]>  _M_what;
};

class domain_error : public logic_error
{
public:

    using logic_error::logic_error;
};

class invalid_argument : public logic_error
{
public:

    using logic_error::logic_error;
};

class length_error : public logic_error
{
public:

    using logic_error::logic_error;
};

class out_of_range : public logic_error
{
public:

    using logic_error::logic_error;
};


class runtime_error : public exception
{
public:

    explicit runtime_error(const __XVI_STD_STRING_NS::string& __what);

    explicit runtime_error(const char* __what)
        : _M_what(__detail::__make_exception_string(__what))
    {
    }

    const char* what() const noexcept override
    {
        return _M_what.get();
    }

protected:

    explicit runtime_error(shared_ptr<const char[]> __what)
        : _M_what(__XVI_STD_NS::move(__what))
    {
    }

private:

    const shared_ptr<const char[]>  _M_what;
};

class range_error : public runtime_error
{
public:

    using runtime_error::runtime_error;
};

class overflow_error : public runtime_error
{
public:

    using runtime_error::runtime_error;
};

class underflow_error : public runtime_error
{
public:

    using runtime_error::runtime_error;
};


} // namespace _XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_STDEXCEPT_H */
