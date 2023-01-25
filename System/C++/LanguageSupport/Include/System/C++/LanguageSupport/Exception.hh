#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H


#include <System/C++/TypeTraits/Concepts.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/LanguageSupport/Private/Config.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/Utility.hh>

#include <System/ABI/C++/TypeInfo.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


// Forward declarations.
template <class _T> constexpr _T* addressof(_T&);


using terminate_handler = void (*)();
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT terminate_handler set_terminate(terminate_handler) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT terminate_handler get_terminate() noexcept;

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT [[noreturn]] void terminate();


// The following are deprecated (along with exception specifications).
using unexpected_handler = void (*)();
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT [[deprecated]] unexpected_handler set_unexpected(unexpected_handler) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT [[deprecated]] unexpected_handler get_unexpected() noexcept;

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT int uncaught_exceptions() noexcept;

[[deprecated]]
inline bool uncaught_exception() noexcept
{
    return uncaught_exceptions() > 0;
}


__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT class exception
{
public:

    exception() noexcept = default;
    exception(const exception&) noexcept = default;
    exception& operator= (const exception&) noexcept = default;
    virtual ~exception();

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


class exception_ptr
{
public:

    constexpr exception_ptr() noexcept = default;

    constexpr exception_ptr(nullptr_t) noexcept
        : exception_ptr()
    {
    }

    exception_ptr(const exception_ptr& __p) noexcept
        : _M_ptr(__p._M_ptr)
    {
        __ref();
    }

    exception_ptr(exception_ptr&& __p) noexcept
        : _M_ptr(__p._M_ptr)
    {
        __p._M_ptr = nullptr;
    }

    ~exception_ptr()
    {
        __unref();
    }

    exception_ptr& operator=(const exception_ptr& __p) noexcept
    {
        if (&__p == this)
            return *this;

        __unref();
        _M_ptr = __p._M_ptr;
        __ref();

        return *this;
    }

    exception_ptr& operator=(exception_ptr&& __p) noexcept
    {
        if (&__p == this)
            return *this;

        __unref();
        _M_ptr = __p._M_ptr;
        __p._M_ptr = nullptr;

        return *this;
    }

    exception_ptr& operator=(nullptr_t) noexcept
    {
        return operator=(exception_ptr{});
    }

    void* __get() const noexcept
    {
        return _M_ptr;
    }

    constexpr operator bool() const noexcept
    {
        return _M_ptr != nullptr;
    }

    friend constexpr bool operator==(const exception_ptr& __l, const exception_ptr& __r) noexcept = default;

    friend constexpr bool operator==(const exception_ptr& __l, nullptr_t) noexcept
    {
        return !__l;
    }

    __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT void __ref() const noexcept;
    __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT void __unref() const noexcept;


    // Creates an exception by copying the given object.
    __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT static exception_ptr __make(void* obj, size_t obj_size, const std::type_info*, void (*destructor)(void*), void (*copy)(void* dest, const void* src));

    // Creates from an existing exception object.
    __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT static exception_ptr __wrap(void* obj);

private:

    void* _M_ptr = nullptr;
};


__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT exception_ptr current_exception() noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT [[noreturn]] void rethrow_exception(exception_ptr);

template <class _E> exception_ptr make_exception_ptr(_E __e) noexcept
{
    return exception_ptr::__make(addressof(__e), sizeof(__e), typeid(_E),
        [](void* __obj)
        {
            static_cast<_E*>(__obj)->~_E();
        },
        [](void* __dest, const void* __src)
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
    if constexpr (__XVI_STD_NS::is_class_v<_U> && !__XVI_STD_NS::is_final_v<_U> && !__XVI_STD_NS::is_base_of_v<nested_exception, _U>)
    {
        class _E : public _U, public nested_exception
        {
        public:
            _E() = delete;
            _E(const _E&) = default;
            _E(_E&&) = default;
            constexpr _E(const _U& __u) : _U(__u) {}
            constexpr _E(_U&& __u) : _U(__XVI_STD_NS::forward<_U&&>(__u)) {}
            ~_E() override = default;
            _E& operator=(const _E&) = default;
            _E& operator=(_E&&) = default;
        };
        throw _E(__XVI_STD_NS::forward<_T>(__t));
    }
    else
    {
        throw __XVI_STD_NS::forward<_T>(__t);
    }
}

template <class _E> void rethrow_if_nested(const _E& __e)
{
    if constexpr (!__XVI_STD_NS::is_class_v<_E> || !__XVI_STD_NS::is_polymorphic_v<_E> || !__XVI_STD_NS::derived_from<_E, nested_exception>)
        return;

    if (auto __p = dynamic_cast<nested_exception*>(addressof(__e)))
        __p->rethrow_nested();
}


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_EXCEPTION_H */
