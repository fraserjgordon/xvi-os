#ifndef __SYSTEM_CXX_CORE_STDEXCEPT_H
#define __SYSTEM_CXX_CORE_STDEXCEPT_H


#include <System/C++/Atomic/Atomic.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/New.hh>

#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class> class allocator;
template <class> struct char_traits;
template <class, class, class> class basic_string;

using string = basic_string<char, char_traits<char>, allocator<char>>;


namespace __detail
{


class __exception_string
{
public:

    class __ptr;

    void __ref()
    {
        ++_M_refcount;
    }

    void __unref()
    {
        if (--_M_refcount == 0)
            delete[] reinterpret_cast<byte*>(this);
    }

    char* __string()
    {
        return _M_chars;
    }

    // Defined in the System.C++.Exception library.
    static __ptr __make(const char*, size_t, size_t);

private:

    atomic<size_t>  _M_refcount = 1;
    char            _M_chars[0];
};


class __exception_string::__ptr
{
public:

    constexpr __ptr() = default;

    __ptr(const __ptr& __other)
        : _M_ptr(__other._M_ptr)
    {
        if (_M_ptr)
            _M_ptr->__ref();
    }

    __ptr(__ptr&& __other)
        : _M_ptr(__other._M_ptr)
    {
        __other._M_ptr = nullptr;
    }

    __ptr& operator=(const __ptr& __other)
    {
        auto __p = __other._M_ptr;
        if (__p)
            __p->__ref();
        if (_M_ptr)
            _M_ptr->__unref();
        _M_ptr = __p;

        return *this;
    }

    __ptr& operator=(__ptr&& __other)
    {
        auto __p = __other._M_ptr;
        __other._M_ptr = nullptr;
        if (_M_ptr)
            _M_ptr->__unref();
        _M_ptr = __p;

        return *this;
    }

    ~__ptr()
    {
        if (_M_ptr)
            _M_ptr->__unref();
    }

    constexpr explicit __ptr(__exception_string* __p)
        : _M_ptr(__p)
    {
    }

    operator const char*() const
    {
        if (!_M_ptr)
            return nullptr;

        return _M_ptr->__string();
    }

    char* __raw()
    {
        return _M_ptr->__string();
    }

private:

    __exception_string*  _M_ptr = nullptr;
};


// Defined in the System.C++.Exception library.
template <class... _Args> __exception_string::__ptr __make_exception_string(_Args&&... __args);


} // namespace __detail


class logic_error : public exception
{
public:

    explicit logic_error(const string& __what);

    explicit logic_error(const char* __what)
        : _M_what(__detail::__make_exception_string(__what))
    {
    }

    const char* what() const noexcept override
    {
        return _M_what;
    }

    ~logic_error() = default;

protected:

    const __detail::__exception_string::__ptr _M_what;
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

    explicit runtime_error(const string& __what);

    explicit runtime_error(const char* __what)
        : _M_what(__detail::__make_exception_string(__what))
    {
    }

    const char* what() const noexcept override
    {
        return _M_what;
    }

private:

    const __detail::__exception_string::__ptr _M_what;
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


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_STDEXCEPT_H */
