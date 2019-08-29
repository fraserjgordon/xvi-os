#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/LanguageSupport/Private/Namespace.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


// Forward declarations.
[[noreturn]] void terminate();
template <class _T> constexpr _T* addressof(_T&);


using terminate_handler = void (*)();
terminate_handler set_terminate(terminate_handler) noexcept;
terminate_handler get_terminate() noexcept;

int uncaught_exceptions() noexcept;


class exception
{
public:

    exception() noexcept = default;
    exception(const exception&) noexcept = default;
    exception& operator= (const exception&) noexcept = default;
    virtual ~exception() = default;

    virtual const char* what() const noexcept
        { return "unknown exception"; }
};

class bad_exception : public exception
{
public:

    bad_exception() noexcept = default;
    bad_exception(const bad_exception&) = default;
    bad_exception& operator= (const bad_exception&) noexcept = default;
    
    const char* what() const noexcept override
        { return "bad exception"; }
};

//! @TODO: implement
class bad_alloc : public exception
{
};


namespace __detail
{

class __exception_ptr
{
public:

    constexpr __exception_ptr() noexcept = default;

    constexpr __exception_ptr(nullptr_t) noexcept
        : __exception_ptr()
    {
    }

    __exception_ptr(const __exception_ptr& __p) noexcept
        : _M_ptr(__p._M_ptr)
    {
        __ref();
    }

    __exception_ptr(__exception_ptr&& __p) noexcept
        : _M_ptr(__p._M_ptr)
    {
        __p._M_ptr = nullptr;
    }

    ~__exception_ptr()
    {
        __unref();
    }

    __exception_ptr& operator=(const __exception_ptr& __p) noexcept
    {
        if (&__p == this)
            return *this;

        __unref();
        _M_ptr = __p._M_ptr;
        __ref();

        return *this;
    }

    __exception_ptr& operator=(__exception_ptr&& __p) noexcept
    {
        if (&__p == this)
            return *this;

        __unref();
        _M_ptr = __p._M_ptr;
        __p._M_ptr = nullptr;

        return *this;
    }

    __exception_ptr& operator=(nullptr_t) noexcept
    {
        return operator=(__exception_ptr{});
    }

    constexpr operator bool() const noexcept
    {
        return _M_ptr != nullptr;
    }

    friend constexpr bool operator==(const __exception_ptr& __l, const __exception_ptr& __r) noexcept
    {
        return __l._M_ptr == __r._M_ptr;
    }

    friend constexpr bool operator!=(const __exception_ptr& __l, const __exception_ptr& __r) noexcept
    {
        return __l._M_ptr != __r._M_ptr;
    }

    friend constexpr bool operator==(const __exception_ptr& __l, nullptr_t) noexcept
    {
        return !__l;
    }

    friend constexpr bool operator==(nullptr_t, const __exception_ptr& __r) noexcept
    {
        return !__r;
    }

    friend constexpr bool operator!=(const __exception_ptr& __l, nullptr_t) noexcept
    {
        return !!__l;
    }

    friend constexpr bool operator!=(nullptr_t, const __exception_ptr& __r) noexcept
    {
        return !!__r;
    }

private:

    void __ref() const noexcept;
    void __unref() const noexcept;

    const void* _M_ptr = nullptr;
};

__exception_ptr __make_exception_ptr(void*, size_t, void (*)(void*, const void*));

} // namespace __detail


using exception_ptr = __detail::__exception_ptr;

exception_ptr current_exception() noexcept;
[[noreturn]] void rethrow_exception(exception_ptr);

template <class _E> exception_ptr make_exception_ptr(_E __e) noexcept
{
    return __detail::__make_exception_ptr(addressof(__e), sizeof(__e), [](void* __dest, const void* __src)
    {
        new (__dest) _E(*static_cast<const _E*>(__src));
    });
}


class nested_exception
{
public:

    nested_exception() noexcept
        : _M_exception_ptr(current_exception())
    {
    }

    nested_exception(const nested_exception&) noexcept = default;
    nested_exception(nested_exception&&) noexcept = default;
    virtual ~nested_exception() = default;
    nested_exception& operator=(const nested_exception&) noexcept = default;
    nested_exception& operator=(nested_exception&&) noexcept = default;

    [[noreturn]] void rethrow_nested() const
    {
        if (!_M_exception_ptr)
            terminate();
        
        rethrow_exception(_M_exception_ptr);
    }

    exception_ptr nested_ptr() const noexcept
    {
        return _M_exception_ptr;
    }

private:

    exception_ptr _M_exception_ptr = {};
};


template <class _T> [[noreturn]] void throw_with_nested(_T&& __t)
{
    using _U = decay_t<_T>;
    if constexpr (is_class_v<_U> && !is_final_v<_U> && !is_base_of_v<nested_exception, _U>)
    {
        class _E : public _U, public nested_exception
        {
        public:
            _E() = delete;
            _E(const _E&) = default;
            _E(_E&&) = default;
            constexpr _E(const _U& __u) : _U(__u) {}
            constexpr _E(_U&& __u) : _U(forward<_U&&>(__u)) {}
            ~_E() override = default;
            _E& operator=(const _E&) = default;
            _E& operator=(_E&&) = default;
        };
        throw _E(forward<_T>(__t));
    }
    else
    {
        throw forward<_T>(__t);
    }
}

template <class _E> void rethrow_if_nested(const _E& __e)
{
    if constexpr (!is_class_v<_E> || !is_polymorphic_v<_E>)
        return;

    if (auto __p = dynamic_cast<nested_exception*>(addressof(__e)))
        __p->rethrow_nested();
}


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H */
