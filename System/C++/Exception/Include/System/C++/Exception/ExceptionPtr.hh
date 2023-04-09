#ifndef __SYSTEM_CXX_EXCEPTION_EXCEPTIONPTR_H
#define __SYSTEM_CXX_EXCEPTION_EXCEPTIONPTR_H


#if defined(__XVI_HOSTED)
#  include <new>
#else
#  include <System/C++/LanguageSupport/New.hh>
#endif

#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/TypeInfo.hh>

#include <System/C++/Exception/Private/Config.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


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

    __SYSTEM_CXX_EXCEPTION_EXPORT void __ref() const noexcept;
    __SYSTEM_CXX_EXCEPTION_EXPORT void __unref() const noexcept;


    // Creates an exception by copying the given object.
    __SYSTEM_CXX_EXCEPTION_EXPORT static exception_ptr __make(void* obj, size_t obj_size, const std::type_info*, void (*destructor)(void*), void (*copy)(void* dest, const void* src));

    // Creates from an existing exception object.
    __SYSTEM_CXX_EXCEPTION_EXPORT static exception_ptr __wrap(void* obj);

private:

    void* _M_ptr = nullptr;
};


__SYSTEM_CXX_EXCEPTION_EXPORT exception_ptr current_exception() noexcept;
__SYSTEM_CXX_EXCEPTION_EXPORT [[noreturn]] void rethrow_exception(exception_ptr);

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


} // namespace __XVI_STD_EXCEPTION_NS_DECL


#endif /* ifndef __SYSTEM_CXX_EXCEPTION_EXCEPTIONPTR_H */
