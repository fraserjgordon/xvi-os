#ifndef __SYSTEM_CXX_EXCEPTION_NESTEDEXCEPTION_H
#define __SYSTEM_CXX_EXCEPTION_NESTEDEXCEPTION_H


#include <System/C++/TypeTraits/Concepts.hh>

#include <System/C++/Exception/Private/Config.hh>
#include <System/C++/Exception/ExceptionPtr.hh>
#include <System/C++/Exception/Terminate.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


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


} // namespace __XVI_STD_EXCEPTION_NS_DECL


#endif /* ifndef __SYSTEM_CXX_EXCEPTION_NESTEDEXCEPTION_H */
