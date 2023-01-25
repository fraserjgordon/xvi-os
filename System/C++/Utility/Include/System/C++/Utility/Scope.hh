#ifndef __SYSTEM_CXX_UTILITY_SCOPE_H
#define __SYSTEM_CXX_UTILITY_SCOPE_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _EF, bool _OnGood = true, bool _OnBad = true>
class scope_exit
{
public:

    template <class _EFP>
        requires (!is_base_of_v<scope_exit, remove_cvref_t<_EFP>>)
                 && is_constructible_v<_EF, _EFP>
                 && (!is_lvalue_reference_v<_EFP> && is_nothrow_constructible_v<_EF, _EFP>)
    explicit scope_exit(_EFP&& __f)
        noexcept(is_nothrow_constructible_v<_EF, _EFP> || is_nothrow_constructible_v<_EF, _EFP&>)
    try
        : _M_exit_fn(std::forward<_EFP>(__f))
    {
    }
    catch (...)
    {
        if constexpr (_OnBad)
            __f();

        // This 'if' statement is to prevent a compiler warning but is meaningless.
        if constexpr (!is_nothrow_constructible_v<_EF, _EFP> && !is_nothrow_constructible_v<_EF, _EFP&>)
            throw;
    }

    template <class _EFP>
        requires (!is_base_of_v<scope_exit, remove_cvref_t<_EFP>>)
                 && is_constructible_v<_EF, _EFP>
                 && (!(!is_lvalue_reference_v<_EFP> && is_nothrow_constructible_v<_EF, _EFP>))
    explicit scope_exit(_EFP&& __f)
        noexcept(is_nothrow_constructible_v<_EF, _EFP> || is_nothrow_constructible_v<_EF, _EFP&>)
    try :
        _M_exit_fn(__f)
    {
    }
    catch (...)
    {
        if constexpr (_OnBad)
            __f();
        throw;
    }

    scope_exit(scope_exit&& __rhs) 
        noexcept(is_nothrow_move_constructible_v<_EF>)
        requires (is_nothrow_move_constructible_v<_EF>) :
        _M_exit_fn(std::forward<_EF>(__rhs.M_exit_fn)),
        _M_armed(__rhs._M_armed),
        _M_uncaught(__rhs._M_uncaught)
    {
        __rhs.release();
    }

    scope_exit(scope_exit&& __rhs) 
        noexcept(is_nothrow_copy_constructible_v<_EF>)
        requires (!is_nothrow_move_constructible_v<_EF> && is_nothrow_copy_constructible_v<_EF>) :
        _M_exit_fn(__rhs.M_exit_fn),
        _M_armed(__rhs._M_armed),
        _M_uncaught(__rhs._M_uncaught)
    {
        __rhs.release();
    }
    

    scope_exit(const scope_exit&) = delete;
    scope_exit& operator=(const scope_exit&) = delete;
    scope_exit& operator=(scope_exit&&) = delete;

    ~scope_exit() noexcept(_OnBad || noexcept(_M_exit_fn()))
    {
        if (!_M_armed)
            return;

        bool __run = true;
        if constexpr (_OnBad)
            __run = __run && (uncaught_exceptions() > _M_uncaught);
        if constexpr (_OnGood)
            __run = __run && (uncaught_exceptions() <= _M_uncaught);

        if (__run)
            _M_exit_fn();
    }

    void release() noexcept
    {
        _M_armed = false;
    }

private:

    _EF     _M_exit_fn;
    bool    _M_armed{true};
    int     _M_uncaught{uncaught_exceptions()};
};


template <class _EF>
class scope_fail : public scope_exit<_EF, false, true>
{
public:

    using scope_exit<_EF, false, true>::scope_exit;
};

template <class _EF>
class scope_success : public scope_exit<_EF, true, false>
{
public:

    using scope_exit<_EF, true, false>::scope_exit;
};

template <class _EF>
scope_exit(_EF) -> scope_exit<_EF>;

template <class _EF>
scope_fail(_EF) -> scope_fail<_EF>;

template <class _EF>
scope_success(_EF) -> scope_success<_EF>;


namespace __detail
{

struct __disarmed_unique_resource_t {};

}


template <class _R, class _D>
class unique_resource
{
    using _R1 = conditional_t<is_reference_v<_R>, reference_wrapper<remove_reference_t<_R>>, _R>;
    using __disarmed_t = __detail::__disarmed_unique_resource_t;

public:

    unique_resource()
        requires is_default_constructible_v<_R> && is_default_constructible_v<_D> :
        _M_resource{},
        _M_deleter{},
        _M_armed{false}
    {
    }

    template <class _RR, class _DD>
        requires is_constructible_v<_R1, _RR>
                 && is_constructible_v<_D, _DD>
                 && (is_nothrow_constructible_v<_R1, _RR> || is_constructible_v<_R1, _RR&>)
                 && (is_nothrow_constructible_v<_D, _DD> || is_constructible_v<_D, _DD&>)
    unique_resource(_RR&& __r, _DD&& __d)
        noexcept((is_nothrow_constructible_v<_R1, _RR> || is_nothrow_constructible_v<_R1, _RR&>)
                 && (is_nothrow_constructible_v<_D, _DD> || is_nothrow_constructible_v<_D, _DD&>)) 
    try :
        _M_resource{__construct_r(std::forward<_RR>(__r))},
        _M_deleter{__construct_d(std::forward<_DD>(__d))},
        _M_armed{true}
    {
    }
    catch (...)
    {
        if (_M_armed)
            __d(__resource());
        else
            __d(__r);
    }

    template <class _RR, class _DD>
        requires is_constructible_v<_R1, _RR>
                 && is_constructible_v<_D, _DD>
                 && (is_nothrow_constructible_v<_R1, _RR> || is_constructible_v<_R1, _RR&>)
                 && (is_nothrow_constructible_v<_D, _DD> || is_constructible_v<_D, _DD&>)
    unique_resource(__disarmed_t, _RR&& __r, _DD&& __d)
        noexcept((is_nothrow_constructible_v<_R1, _RR> || is_nothrow_constructible_v<_R1, _RR&>)
                 && (is_nothrow_constructible_v<_D, _DD> || is_nothrow_constructible_v<_D, _DD&>)) 
    :
        _M_resource{__construct_r(std::forward<_RR>(__r))},
        _M_deleter{__construct_d(std::forward<_DD>(__d), true)},
        _M_armed{false}
    {
    }

    unique_resource(unique_resource&& __rhs)
        noexcept(is_nothrow_move_constructible_v<_R> && is_nothrow_move_constructible_v<_D>)
    try :
        _M_resource{__move_r(std::move(__rhs))},
        _M_deleter{__move_d(std::move(__rhs))},
        _M_armed{exchange(__rhs._M_armed, false)}
    {
    }
    catch (...)
    {
        if constexpr (is_nothrow_move_constructible_v<_R1>)
        {
            if (__rhs._M_armed)
            {
                __rhs._M_deleter(__resource());
                __rhs.release();
            }
        }
    }

    ~unique_resource()
    {
        reset();
    }

    unique_resource& operator=(const unique_resource&& __rhs)
        noexcept(is_nothrow_move_assignable_v<_R> && is_nothrow_move_assignable_v<_D>)
    {
        reset();
        if constexpr (is_nothrow_move_assignable_v<_R1>)
        {
            if constexpr (is_nothrow_move_assignable_v<_D>)
            {
                _M_resource = std::move(__rhs._M_resource);
                _M_deleter = std::move(__rhs._M_deleter);
            }
            else
            {
                _M_deleter = __rhs._M_deleter;
                _M_resource = std::move(__rhs._M_resource);
            }
        }
        else
        {
            if constexpr (is_nothrow_move_assignable_v<_D>)
            {
                _M_resource = __rhs._M_resource;
                _M_deleter = std::move(__rhs._M_deleter);
            }
            else
            {
                _M_resource = __rhs._M_resource;
                _M_deleter = __rhs._M_deleter;
            }
        }

        _M_armed = exchange(__rhs._M_armed, false);

        return *this;
    }

    void reset() noexcept
    {
        if (_M_armed)
        {
            _M_armed = false;
            _M_deleter(__resource());
        }
    }

    template <class _RR>
    void reset(_RR&& __r)
    {
        reset();

        if constexpr (is_nothrow_assignable_v<_R1, _RR>)
            _M_resource = std::forward<_RR>(__r);
        else
            _M_resource = as_const(__r);

        _M_armed = true;
    }

    void release() noexcept
    {
        _M_armed = false;
    }

    const _R& get() const noexcept
    {
        return __resource();
    }

    auto operator*() const noexcept
        requires is_pointer_v<_R> && (!is_void_v<remove_pointer_t<_R>>)
    {
        return *get();
    }

    _R operator->() const noexcept
        requires is_pointer_v<_R>
    {
        return get();
    }

    const _D& get_deleter() const noexcept
    {
        return _M_deleter;
    }

private:

    _R1                         _M_resource;
    [[no_unique_address]] _D    _M_deleter;
    bool                        _M_armed{true};


    decltype(auto) __resource() const noexcept
    {
        if constexpr (is_reference_v<_R>)
            return _M_resource.get();
        else
            return _M_resource;
    }

    template <class _RR>
    decltype(auto) __construct_r(_RR&& __r) noexcept
    {
        _M_armed = false;
        if constexpr (is_nothrow_constructible_v<_R1, _RR>)
            return std::forward<_RR>(__r);
        else
            return __r;
    }

    template <class _DD>
    decltype(auto) __construct_d(_DD&& __d, bool __disarmed = false) noexcept
    {
        _M_armed = !__disarmed;
        if constexpr (is_nothrow_constructible_v<_D, _DD>)
            return std::forward<_DD>(__d);
        else
            return __d;
    }

    decltype(auto) __move_r(unique_resource&& __other) noexcept
    {
        _M_armed = false;
        if constexpr (is_nothrow_move_constructible_v<_R1>)
            return std::move(__other._M_resource);
        else
            return __other._M_resource;
    }

    decltype(auto) __move_d(unique_resource&& __other) noexcept
    {
        _M_armed = true;
        if constexpr (is_nothrow_move_constructible_v<_D>)
            return std::move(__other._M_deleter);
        else
            return __other._M_deleter;
    }
};


template <class _R, class _D>
unique_resource(_R, _D) -> unique_resource<_R, _D>;


template <class _R, class _D, class _S = decay_t<_R>>
unique_resource<decay_t<_R>, decay_t<_D>> make_unique_resource_checked(_R&& __resource, const _S& __invalid, _D&& __d)
    noexcept(is_nothrow_constructible_v<decay_t<_R>, _R> && is_nothrow_constructible_v<decay_t<_D>, _D>)
{
    bool __failed = bool(__resource == __invalid);
    if (!__failed)
        return unique_resource(std::forward<_R>(__resource), std::forward<_D>(__d));
    else
        return unique_resource(__detail::__disarmed_unique_resource_t{}, std::forward<_R>(__resource), std::forward<_D>(__resource));
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SCOPE_H */
