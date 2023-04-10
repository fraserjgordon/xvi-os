#ifndef __SYSTEM_CXX_CORE_EXPECTED_H
#define __SYSTEM_CXX_CORE_EXPECTED_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/AddressOf.hh>
#include <System/C++/Core/CtorTags.hh>
#include <System/C++/Core/Invoke.hh>
#include <System/C++/Core/MemoryAlgorithms.hh>
#include <System/C++/Core/Swap.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class, class> class expected;


namespace __detail
{


struct __expected_void_value_type {};


template <class> struct __is_expected_specialization : false_type {};

template <class _T, class _E> struct __is_expected_specialization<expected<_T, _E>> : true_type {};

template <class _T> concept __expected_specialization = __is_expected_specialization<_T>::value;


} // namespace __detail


template <class _E>
class unexpected
{
public:

    constexpr unexpected(const unexpected&) = default;
    constexpr unexpected(unexpected&&) = default;

    template <class... _Args>
        requires is_constructible_v<_E, _Args...>
    constexpr explicit unexpected(in_place_t, _Args&&... __args) :
        _M_val(__XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
        requires is_constructible_v<_E, std::initializer_list<_U>&, _Args...>
    constexpr explicit unexpected(in_place_t, std::initializer_list<_U> __il, _Args&&... __args) :
        _M_val(__il, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class _Err = _E>
        requires (!is_same_v<remove_cvref_t<_Err>, unexpected>)
            && (!is_same_v<remove_cvref_t<_Err>, in_place_t>)
            && is_constructible_v<_E, _Err>
    constexpr explicit unexpected(_Err&& __err) :
        _M_val(__XVI_STD_NS::forward<_Err>(__err))
    {
    }

    constexpr unexpected& operator=(const unexpected&) = default;
    constexpr unexpected& operator=(unexpected&&) = default;

    constexpr const _E& error() const & noexcept
    {
        return _M_val;
    }

    constexpr _E& error() & noexcept
    {
        return _M_val;
    }

    constexpr const _E&& error() const && noexcept
    {
        return __XVI_STD_NS::move(_M_val);
    }

    constexpr _E&& error() && noexcept
    {
        return __XVI_STD_NS::move(_M_val);
    }

    constexpr void swap(unexpected& __other) noexcept(is_nothrow_swappable_v<_E>)
    {
        using __XVI_STD_NS::swap;
        swap(_M_val, __other._M_val);
    }

    template <class _E2>
    friend constexpr bool operator==(const unexpected& __x, const unexpected<_E2>& __y)
    {
        return __x.error() == __y.error();
    }

    friend constexpr void swap(unexpected& __x, unexpected& __y) noexcept(noexcept(__x.swap(__y)))
        requires (is_swappable_v<_E>)
    {
        __x.swap(__y);
    }

private:

    _E  _M_val;
};

template <class _E>
unexpected(_E) -> unexpected<_E>;


template <class _E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public exception
{
public:

    const char* what() const noexcept override;

protected:

    bad_expected_access() noexcept = default;

    bad_expected_access(const bad_expected_access&) = default;

    bad_expected_access(bad_expected_access&&) = default;

    bad_expected_access& operator=(const bad_expected_access&) = default;

    bad_expected_access& operator=(bad_expected_access&&) = default;

    ~bad_expected_access() = default;
};


template <class _E>
struct bad_expected_access : public bad_expected_access<void>
{
public:

    explicit bad_expected_access(_E __e) :
        _M_val(__XVI_STD_NS::move(__e))
    {
    }

    _E& error() & noexcept
    {
        return _M_val;
    }

    const _E& error() const & noexcept
    {
        return _M_val;
    }

    _E&& error() && noexcept
    {
        return __XVI_STD_NS::move(_M_val);
    }

    const _E&& error() const && noexcept
    {
        return __XVI_STD_NS::move(_M_val);
    }

private:

    _E _M_val;
};


struct unexpect_t
{
    explicit unexpect_t() = default;
};

inline constexpr unexpect_t unexpect {};


template <class _T, class _E> class expected;


namespace __detail
{


template <class _T, class _E>
class __expected_storage
{
public:

    template <class _V, class _U, class... _Args>
    static constexpr void __reinit_expected(_V& __newval, _U& __oldval, _Args&&... __args)
    {
        if constexpr (is_nothrow_constructible_v<_T, _Args...>)
        {
            __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(__oldval));
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(__newval), __XVI_STD_NS::forward<_Args>(__args)...);
        }
        else if constexpr (is_nothrow_move_constructible_v<_T>)
        {
            _V __tmp(__XVI_STD_NS::forward<_Args>(__args)...);
            __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(__oldval));
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(__newval), __XVI_STD_NS::move(__tmp));
        }
        else
        {
            _U __tmp(__XVI_STD_NS::move(__oldval));
            __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(__oldval));
            try
            {
                __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(__newval), __XVI_STD_NS::forward<_Args>(__args)...);
            }
            catch (...)
            {
                __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(__oldval), __XVI_STD_NS::move(__tmp));
                throw;
            }
        }
    }


    union __storage_t
    {
        _T      _M_U_val;
        _E      _M_U_unex;
        char    _M_U_placeholder = {};

        template <class... _Args>
        constexpr __storage_t(in_place_t, _Args&&... __args) :
            _M_U_val(__XVI_STD_NS::forward<_Args>(__args)...) {}

        template <class... _Args>
        constexpr __storage_t(unexpect_t, _Args&&... __args) :
            _M_U_unex(__XVI_STD_NS::forward<_Args>(__args)...) {}

        constexpr __storage_t()
            requires is_default_constructible_v<_T> = default;

        constexpr __storage_t()
            requires (!(is_default_constructible_v<_T>)) {}

        constexpr __storage_t(const __storage_t&)
            requires is_trivially_copy_constructible_v<_T> && is_trivially_copy_constructible_v<_E> = default;

        constexpr __storage_t(const __storage_t&)
            requires (!(is_trivially_copy_constructible_v<_T> && is_trivially_copy_constructible_v<_E>)) {}

        constexpr __storage_t(__storage_t&&)
            requires is_trivially_move_constructible_v<_T> && is_trivially_move_constructible_v<_E> = default;

        constexpr __storage_t(__storage_t&&)
            requires (!(is_trivially_move_constructible_v<_T> && is_trivially_move_constructible_v<_E>)) {}

        constexpr ~__storage_t()
            requires is_trivially_destructible_v<_T> && is_trivially_destructible_v<_E> = default;

        constexpr ~__storage_t()
            requires (!(is_trivially_destructible_v<_T> && is_trivially_destructible_v<_E>)) {}

        constexpr __storage_t& operator=(const __storage_t&)
            requires is_trivially_copy_assignable_v<_T> && is_trivially_move_assignable_v<_E> = default;

        constexpr __storage_t& operator=(const __storage_t&)
            requires (!(is_trivially_copy_assignable_v<_T> && is_trivially_move_assignable_v<_T>)) { return *this; }

        constexpr __storage_t& operator=(__storage_t&&)
            requires is_trivially_move_assignable_v<_T> && is_trivially_move_assignable_v<_E> = default;

        constexpr __storage_t& operator=(__storage_t&&)
            requires (!(is_trivially_move_assignable_v<_T> && is_trivially_move_assignable_v<_E>)) { return *this; }
    };

    __storage_t _M_storage;
    bool        _M_has_value = true;

    constexpr __expected_storage() = default;

    constexpr __expected_storage(const __expected_storage&)
        requires is_trivially_copy_constructible_v<__storage_t> = default;

    constexpr __expected_storage(const __expected_storage& __other)
        requires (!(is_trivially_copy_constructible_v<__storage_t>))
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_val), __other._M_storage._M_U_val);
        else
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex), __other._M_storage._M_U_unex);
    }

    constexpr __expected_storage(__expected_storage&&)
        requires is_trivially_move_constructible_v<__storage_t> = default;

    constexpr __expected_storage(__expected_storage&& __other)
        requires (!(is_trivially_move_constructible_v<__storage_t>))
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_val), __XVI_STD_NS::move(__other._M_storage._M_U_val));
        else
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex), __XVI_STD_NS::move(__other._M_storage._M_U_unex));
    }

    template <class _U, class _G>
    constexpr __expected_storage(const __expected_storage<_U, _G>& __other)
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_val), __other._M_storage._M_U_val);
        else
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex), __other._M_storage._M_U_unex);
    }

    template <class _U, class _G>
    constexpr __expected_storage(__expected_storage<_U, _G>&& __other)
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_val), __XVI_STD_NS::move(__other._M_storage._M_U_val));
        else
            __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex), __XVI_STD_NS::move(__other._M_storage._M_U_unex));
    }

    template <class... _Args>
    constexpr explicit __expected_storage(in_place_t, _Args&&... __args) :
        _M_storage(in_place, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class... _Args>
    constexpr explicit __expected_storage(unexpect_t, _Args&&... __args) :
        _M_storage(unexpect, __XVI_STD_NS::forward<_Args>(__args)...),
        _M_has_value(false)
    {
    }

    constexpr ~__expected_storage()
        requires is_trivially_destructible_v<__storage_t> = default;

    constexpr ~__expected_storage()
        requires (!(is_trivially_destructible_v<__storage_t>))
    {
        if (_M_has_value)
            __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(_M_storage._M_U_val));
        else
            __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex));
    }

    constexpr __expected_storage& operator=(const __expected_storage& __other)
        requires is_trivially_copy_constructible_v<_T>
            && is_trivially_copy_constructible_v<_E>
            && is_trivially_copy_assignable_v<_T>
            && is_trivially_copy_assignable_v<_E>
            && is_trivially_destructible_v<_T>
            && is_trivially_destructible_v<_E>
        = default;

    constexpr __expected_storage& operator=(const __expected_storage& __other)
        requires (!(is_trivially_copy_constructible_v<_T>
            && is_trivially_copy_constructible_v<_E>
            && is_trivially_copy_assignable_v<_T>
            && is_trivially_copy_assignable_v<_E>
            && is_trivially_destructible_v<_T>
            && is_trivially_destructible_v<_E>))
    {
        if (_M_has_value && __other._M_has_value)
        {
            _M_storage._M_U_val = __other._M_storage._M_U_val;
        }
        else if (_M_has_value)
        {
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, __other._M_storage._M_U_unex);
        }
        else if (__other._M_has_value)
        {
            __reinit_expected(_M_storage._M_U_val, _M_storage._M_U_unex, __other._M_storage._M_U_val);
        }
        else
        {
            _M_storage._M_U_unex = __other._M_storage._M_U_unex;
        }

        _M_has_value = __other._M_has_value;

        return *this;
    }

    constexpr __expected_storage& operator=(__expected_storage&& __other)
        requires is_trivially_move_constructible_v<_T>
            && is_trivially_move_constructible_v<_E>
            && is_trivially_move_assignable_v<_T>
            && is_trivially_move_assignable_v<_E>
            && is_trivially_destructible_v<_T>
            && is_trivially_destructible_v<_E>
        = default;

    constexpr __expected_storage& operator=(__expected_storage&& __other)
        requires (!(is_trivially_move_constructible_v<_T>
            && is_trivially_move_constructible_v<_E>
            && is_trivially_move_assignable_v<_T>
            && is_trivially_move_assignable_v<_E>
            && is_trivially_destructible_v<_T>
            && is_trivially_destructible_v<_E>))
    {
        if (_M_has_value && __other._M_has_value)
        {
            _M_storage._M_U_val = __XVI_STD_NS::move(__other._M_storage._M_U_val);
        }
        else if (_M_has_value)
        {
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, __XVI_STD_NS::move(__other._M_storage._M_U_unex));
        }
        else if (__other._M_has_value)
        {
            __reinit_expected(_M_storage._M_U_val, _M_storage._M_U_unex, __XVI_STD_NS::move(__other._M_storage._M_U_val));
        }
        else
        {
            _M_storage._M_U_unex = __other._M_storage._M_U_unex;
        }

        _M_has_value = __other._M_has_value;

        return *this;
    }

    template <class _U = _T>
    constexpr __expected_storage& operator=(_U&& __t)
    {
        if (_M_has_value)
            _M_storage._M_U_val = __XVI_STD_NS::forward<_U>(__t);
        else
            __reinit_expected(_M_storage._M_U_val, _M_storage._M_U_unex, __XVI_STD_NS::forward<_U>(__t));

        _M_has_value = true;

        return *this;
    }

    template <class _G>
    constexpr __expected_storage& operator=(const unexpected<_G>& __u)
    {
        if (_M_has_value)
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, __u.error());
        else
            _M_storage._M_U_unex = __u.error();

        _M_has_value = false;

        return *this;
    }

    template <class _G>
    constexpr __expected_storage& operator=(unexpected<_G>&& __u)
    {
        if (_M_has_value)
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, __XVI_STD_NS::move(__u.error()));
        else
            _M_storage._M_U_unex = __XVI_STD_NS::move(__u.error());

        _M_has_value = false;

        return *this;
    }

    template <class... _Args>
    constexpr _T& emplace(_Args&&... __args) noexcept
    {
        if (_M_has_value)
            __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(_M_storage._M_U_val));
        else
            __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex));

        __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_val), __XVI_STD_NS::forward<_Args>(__args)...);
        _M_has_value = true;

        return _M_storage._M_U_val;
    }

    constexpr void swap(__expected_storage& __other)
    {
        if (_M_has_value && __other._M_has_value)
        {
            using __XVI_STD_NS::swap;
            swap(_M_storage._M_U_val, __other._M_storage._M_U_val);
        }
        else if (_M_has_value)
        {
            if constexpr (is_nothrow_move_constructible_v<_E>)
            {
                _E __tmp(__XVI_STD_NS::move(__other._M_storage._M_U_unex));
                __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(__other._M_storage._M_U_unex));

                try
                {
                    __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(__other._M_storage._M_U_val), __XVI_STD_NS::move(_M_storage._M_U_val));
                    _M_storage._M_U_val.~_T();
                    __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex), __XVI_STD_NS::move(__tmp));
                    
                    using __XVI_STD_NS::swap;
                    swap(_M_has_value, __other._M_has_value);
                }
                catch (...)
                {
                    __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(__other._M_storage._M_U_unex), __XVI_STD_NS::move(__tmp));
                    throw;
                }
            }
            else
            {
                _T __tmp(__XVI_STD_NS::move(_M_storage._M_U_val));
                __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(_M_storage._M_U_val));

                try
                {
                    __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_unex), __XVI_STD_NS::move(__other._M_storage._M_U_unex));
                    __XVI_STD_NS::destroy_at(__XVI_STD_NS::addressof(__other._M_storage._M_U_unex));
                    __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(__other._M_storage._M_U_val), __XVI_STD_NS::move(__tmp));
                    
                    using __XVI_STD_NS::swap;
                    swap(_M_has_value, __other._M_has_value);
                }
                catch (...)
                {
                    __XVI_STD_NS::construct_at(__XVI_STD_NS::addressof(_M_storage._M_U_val), __XVI_STD_NS::move(__tmp));
                    throw;
                }
            }
        }
        else if (__other._M_has_value)
        {
            // Reverse the swap operands to avoid duplicating the complex behaviour in the case above.
            __other.swap(*this);
        }
        else
        {
            using __XVI_STD_NS::swap;
            swap(_M_storage._M_U_unex, __other._M_storage._M_U_unex);
        }
    }
};


template <class> struct __is_unexpected_specialization : false_type {};

template <class _E> struct __is_unexpected_specialization<unexpected<_E>> : true_type {};

template <class _T>
inline constexpr bool __is_unexpected_specialization_v = __is_unexpected_specialization<_T>::value;


} // namespace __detail


template <class _T, class _E>
class expected
{
    template <class _U, class _G> friend class expected;

public:

    using value_type        = _T;
    using error_type        = _E;
    using unexpected_type   = unexpected<_E>;

    template <class _U>
    using rebind = expected<_U, error_type>;

    constexpr expected()
        requires is_default_constructible_v<_T> = default;

    constexpr expected(const expected&)
        requires is_copy_constructible_v<_T> && is_copy_constructible_v<_E> = default;

    constexpr expected(const expected &)
        requires (!(is_copy_constructible_v<_T> && is_copy_constructible_v<_E>)) = delete;

    constexpr expected(expected&&) noexcept(is_nothrow_move_constructible_v<_T> && is_nothrow_move_constructible_v<_E>)
        requires is_move_constructible_v<_T> && is_move_constructible_v<_E> = default;

    template <class _U, class _G>
    constexpr explicit(!is_convertible_v<const _U&, _T> || !is_convertible_v<const _G&, _E>) expected(const expected<_U, _G>& __e)
        requires (is_constructible_v<_T, const _U&>
            && is_constructible_v<_E, const _G&>
            && !is_constructible_v<_T, expected<_U, _G>&>
            && !is_constructible_v<_T, expected<_U, _G>>
            && !is_constructible_v<_T, const expected<_U, _G>&>
            && !is_constructible_v<_T, const expected<_U, _G>>
            && !is_convertible_v<expected<_U, _G>&, _T>
            && !is_convertible_v<expected<_U, _G>&&, _T>
            && !is_convertible_v<const expected<_U, _G>&, _T>
            && !is_convertible_v<const expected<_U, _G>&&, _T>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>>) :
        _M_storage(__e._M_storage)
    {
    }

    template <class _U, class _G>
    constexpr explicit(!is_convertible_v<_U, _T> || !is_convertible_v<_G, _E>) expected(expected<_U, _G>&& __e)
        requires (is_constructible_v<_T, _U>
            && is_constructible_v<_E, _G>
            && !is_constructible_v<_T, expected<_U, _G>&>
            && !is_constructible_v<_T, expected<_U, _G>>
            && !is_constructible_v<_T, const expected<_U, _G>&>
            && !is_constructible_v<_T, const expected<_U, _G>>
            && !is_convertible_v<expected<_U, _G>&, _T>
            && !is_convertible_v<expected<_U, _G>&&, _T>
            && !is_convertible_v<const expected<_U, _G>&, _T>
            && !is_convertible_v<const expected<_U, _G>&&, _T>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>>) :
        _M_storage(__XVI_STD_NS::move(__e._M_storage))
    {
    }

    template <class _U = _T>
    constexpr explicit(!is_convertible_v<_U, _T>) expected(_U&& __v)
        requires (!is_same_v<remove_cvref_t<_U>, in_place_t>
            && !is_same_v<expected<_T, _E>, remove_cvref_t<_U>>
            && !__detail::__is_unexpected_specialization_v<remove_cvref_t<_U>>
            && is_constructible_v<_T, _U>) :
        _M_storage(in_place, __XVI_STD_NS::forward<_U>(__v))
    {
    }

    template <class _G>
    constexpr explicit(!is_convertible_v<const _G&, _E>) expected(const unexpected<_G>& __e)
        requires is_constructible_v<_E, const _G&> :
        _M_storage(unexpect, __e.error())
    {
    }

    template <class _G>
    constexpr explicit (!is_convertible_v<_G, _E>) expected(unexpected<_G>&& __e)
        requires is_constructible_v<_E, _G> :
        _M_storage(unexpect, __XVI_STD_NS::move(__e.error()))
    {
    }

    template <class... _Args>
    constexpr explicit expected(in_place_t, _Args&&... __args)
        requires is_constructible_v<_T, _Args...> :
        _M_storage(in_place, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
    constexpr explicit expected(in_place_t, std::initializer_list<_U> __il, _Args&&... __args)
        requires is_constructible_v<_T, std::initializer_list<_U>&, _Args...> :
        _M_storage(in_place, __il, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class... _Args>
    constexpr explicit expected(unexpect_t, _Args&&... __args)
        requires is_constructible_v<_E, _Args...> :
        _M_storage(unexpect, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
    constexpr explicit expected(unexpect_t, std::initializer_list<_U> __il, _Args&&... __args)
        requires is_constructible_v<_E, std::initializer_list<_U>&, _Args...> :
        _M_storage(unexpect, __il, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    constexpr ~expected() = default;

    constexpr expected& operator=(const expected&)
        requires is_copy_assignable_v<_T>
            && is_copy_constructible_v<_T>
            && is_copy_assignable_v<_E>
            && is_copy_constructible_v<_E>
            && (is_nothrow_move_constructible_v<_T> || is_nothrow_move_constructible_v<_E>)
    = default;

    constexpr expected& operator=(const expected&)
        requires (!(is_copy_assignable_v<_T>
            && is_copy_constructible_v<_T>
            && is_copy_assignable_v<_E>
            && is_copy_constructible_v<_E>
            && (is_nothrow_move_constructible_v<_T> || is_nothrow_move_constructible_v<_E>)))
    = delete;

    constexpr expected& operator=(expected&&) 
        noexcept(is_nothrow_move_assignable_v<_T> && is_nothrow_move_constructible_v<_T> && is_nothrow_move_assignable_v<_E> && is_nothrow_move_constructible_v<_E>)
        requires is_move_constructible_v<_T>
            && is_move_assignable_v<_T>
            && is_move_constructible_v<_E>
            && is_move_assignable_v<_E>
            && (is_nothrow_move_constructible_v<_T> || is_nothrow_move_constructible_v<_E>)
    = default;

    template <class _U = _T>
        requires (!is_same_v<expected, remove_cvref_t<_U>>
            && !__detail::__is_unexpected_specialization_v<remove_cvref_t<_U>>
            && is_constructible_v<_T, _U>
            && is_assignable_v<_T&, _U>
            && (is_nothrow_constructible_v<_T, _U> || is_nothrow_move_constructible_v<_T> || is_nothrow_move_constructible_v<_E>))
    constexpr expected& operator=(_U&& __t)
    {
        _M_storage = __XVI_STD_NS::forward<_U>(__t);
        return *this;
    }

    template <class _G>
        requires is_constructible_v<_E, const _G&>
            && is_assignable_v<_E&, const _G&>
            && (is_nothrow_constructible_v<_E, const _G&> || is_nothrow_move_constructible_v<_T> || is_nothrow_move_constructible_v<_E>)
    constexpr expected& operator=(const unexpected<_G>& __u)
    {
        _M_storage = __u;
        return *this;
    }

    template <class _G>
        requires is_constructible_v<_E, _G>
            && is_assignable_v<_E&, _G>
            && (is_nothrow_constructible_v<_E, _G> || is_nothrow_move_constructible_v<_T> || is_nothrow_move_constructible_v<_E>)
    constexpr expected& operator=(unexpected<_G>&& __u)
    {
        _M_storage = __XVI_STD_NS::move(__u);
        return *this;
    }

    template <class... _Args>
        requires is_nothrow_constructible_v<_T, _Args...>
    constexpr _T& emplace(_Args&&... __args) noexcept
    {
        return _M_storage.emplace(__XVI_STD_NS::forward<_Args>(__args)...);
    }

    template <class _U, class... _Args>
        requires is_nothrow_constructible_v<_T, std::initializer_list<_U>&, _Args...>
    constexpr _T& emplace(std::initializer_list<_U> __il, _Args&&... __args) noexcept
    {
        return _M_storage.emplace(__il, __XVI_STD_NS::forward<_Args>(__args)...);
    }

    constexpr void swap(expected& __e)
        noexcept(is_nothrow_move_constructible_v<_T> && is_nothrow_swappable_v<_T> && is_nothrow_move_constructible_v<_E> && is_nothrow_swappable_v<_E>)
        requires is_swappable_v<_T>
            && is_swappable_v<_E>
            && is_move_constructible_v<_T>
            && is_move_constructible_v<_E>
            && (is_nothrow_move_constructible_v<_T> || is_nothrow_move_constructible_v<_E>)
    {
        _M_storage.swap(__e._M_storage);
    }

    constexpr const _T* operator->() const noexcept
    {
        return __XVI_STD_NS::addressof(_M_storage._M_storage._M_U_val);
    }

    constexpr _T* operator->() noexcept
    {
        return __XVI_STD_NS::addressof(_M_storage._M_storage._M_U_val);
    }

    constexpr const _T& operator*() const & noexcept
    {
        return _M_storage._M_storage._M_U_val;
    }

    constexpr _T& operator*() & noexcept
    {
        return _M_storage._M_storage._M_U_val;
    }

    constexpr const _T&& operator*() const && noexcept
    {
        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_val);
    }

    constexpr _T&& operator*() && noexcept
    {
        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_val);
    }

    constexpr explicit operator bool() const noexcept
    {
        return _M_storage._M_has_value;
    }

    constexpr bool has_value() const noexcept
    {
        return _M_storage._M_has_value;
    }

    constexpr const _T& value() const &
    {
        if (!has_value())
            throw (bad_expected_access(error()));

        return _M_storage._M_storage._M_U_val;
    }

    constexpr _T& value() &
    {
        if (!has_value())
            throw (bad_expected_access(error()));

        return _M_storage._M_storage._M_U_val;
    }

    constexpr const _T&& value() const &&
    {
        if (!has_value())
            throw (bad_expected_access(__XVI_STD_NS::move(error())));

        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_val);
    }

    constexpr _T&& value() &&
    {
        if (!has_value())
            throw (bad_expected_access(__XVI_STD_NS::move(error())));

        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_val);
    }

    constexpr const _E& error() const &
    {
        return _M_storage._M_storage._M_U_unex;
    }

    constexpr _E& error() &
    {
        return _M_storage._M_storage._M_U_unex;
    }

    constexpr const _E&& error() const &&
    {
        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_unex);
    }

    constexpr _E&& error() &&
    {
        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_unex);
    }

    template <class _U>
    constexpr _T value_or(_U&& __u) const &
    {
        return has_value() ? **this : static_cast<_T>(__XVI_STD_NS::forward<_U>(__u));
    }

    template <class _U>
    constexpr _T value_or(_U&& __u) &&
    {
        return has_value() ? __XVI_STD_NS::move(**this) : static_cast<_T>(__XVI_STD_NS::forward<_U>(__u));
    }

    template <class _G = _E>
    constexpr _E error_or(_G&& __e) const &
    {
        return has_value() ? __XVI_STD_NS::forward<_G>(__e) : error();
    }

    template <class _G = _E>
    constexpr _E error_or(_G&& __e) &&
    {
        return has_value() ? __XVI_STD_NS::forward<_G>(__e) : __XVI_STD_NS::move(error());
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F, decltype(value())>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);

        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value());
        else
            return _U(unexpect, error());
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) const &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F, decltype(value())>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);

        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value());
        else
            return _U(unexpect, error());
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);

        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value()));
        else
            return _U(unexpect, __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) const &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);

        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value()));
        else
            return _U(unexpect, __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) &
        requires is_copy_constructible_v<_T>
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(error())>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G(in_place, value());
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error());
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) const &
        requires is_copy_constructible_v<_T>
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(error())>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G(in_place, value());
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error());
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) &&
        requires is_move_constructible_v<_T>
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G(in_place, __XVI_STD_NS::move(value()));
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) const &&
        requires is_move_constructible_v<_T>
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G(in_place, __XVI_STD_NS::move(value()));
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto transform(_F&& __f) &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F, decltype(value())>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, error());

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value()));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value());
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform(_F&& __f) const &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F, decltype(value())>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, error());

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value()));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value());
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform(_F&& __f) &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, __XVI_STD_NS::move(error()));

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value())));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value()));
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform(_F&& __f) const &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, __XVI_STD_NS::move(error()));

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value())));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value()));
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) &
        requires is_copy_constructible_v<_T>
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(error())>>;

        if (has_value())
            return expected<_T, _G>(in_place, value());
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error()));
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) const &
        requires is_copy_constructible_v<_T>
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(error())>>;

        if (has_value())
            return expected<_T, _G>(in_place, value());
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error()));
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) &&
        requires is_move_constructible_v<_T>
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;

        if (has_value())
            return expected<_T, _G>(in_place, value());
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error())));
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) const &&
        requires is_move_constructible_v<_T>
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;

        if (has_value())
            return expected<_T, _G>(in_place, value());
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error())));
    }


    friend constexpr void swap(expected& __x, expected& __y) noexcept(noexcept(__x.swap(__y)))
    {
        __x.swap(__y);
    }

    template <class _T2, class _E2>
        requires (!is_void_v<_T2>)
    friend constexpr bool operator==(const expected& __x, const expected<_T2, _E2>& __y)
    {
        if (__x.has_value() != __y.has_value())
            return false;

        if (__x.has_value())
            return *__x == *__y;
        else
            return __x.error() == __y.error();
    }

    template <class _T2>
    friend constexpr bool operator==(const expected& __x, const _T2& __y)
    {
        return __x.has_value() && static_cast<bool>(*__x == __y);
    }

    template <class _E2>
    friend constexpr bool operator==(const expected& __x, const unexpected<_E2>& __y)
    {
        return !__x.has_value() && static_cast<bool>(__x.error() == __y.value());
    }

private:

    __detail::__expected_storage<_T, _E>    _M_storage = {};
};


template <class _T, class _E>
    requires is_void_v<_T>
class expected<_T, _E>
{
    template <class _U, class _G> friend class expected;

public:

    using value_type        = _T;
    using error_type        = _E;
    using unexpected_type   = unexpected<_E>;

    template <class _U> using rebind = expected<_U, error_type>;


    constexpr expected() noexcept = default;

    constexpr expected(const expected&)
        requires is_copy_constructible_v<_E> = default;

    constexpr expected(const expected&)
        requires (!is_copy_constructible_v<_E>) = delete;

    constexpr expected(expected&&) noexcept(is_nothrow_move_constructible_v<_E>)
        requires is_move_constructible_v<_E> = default;

    template <class _U, class _G>
        requires (is_void_v<_U>
            && is_constructible_v<_E, const _G&>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>>)
    constexpr explicit(!is_convertible_v<const _G&, _E>) expected(const expected<_U, _G>& __rhs) :
        _M_storage(__rhs._M_storage)
    {
    }

    template <class _U, class _G>
        requires (is_void_v<_U>
            && is_constructible_v<_E, _G>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !is_constructible_v<unexpected<_E>, const expected<_U, _G>>)
    constexpr explicit(!is_convertible_v<_G, _E>) expected(expected<_U, _G>&& __rhs) :
        _M_storage(__XVI_STD_NS::move(__rhs._M_storage))
    {
    }

    template <class _G>
        requires is_constructible_v<_E, const _G&>
    constexpr explicit(!is_convertible_v<const _G&, _E>) expected(const unexpected<_G>& __u) :
        _M_storage(unexpect, __u.error())
    {
    }

    template <class _G>
        requires is_constructible_v<_E, _G>
    constexpr explicit(!is_convertible_v<_G, _E>) expected(unexpected<_G>&& __u) :
        _M_storage(unexpect, __XVI_STD_NS::move(__u.error()))
    {
    }

    constexpr explicit expected(in_place_t) noexcept :
        expected()
    {
    }

    template <class... _Args>
        requires is_constructible_v<_E, _Args...>
    constexpr explicit expected(unexpect_t, _Args&&... __args) :
        _M_storage(unexpect, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
        requires is_constructible_v<_E, std::initializer_list<_U>&, _Args...>
    constexpr explicit expected(unexpect_t, std::initializer_list<_U> __il, _Args&&... __args) :
        _M_storage(unexpect, __il, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    constexpr ~expected() = default;

    constexpr expected& operator=(const expected&)
        requires is_copy_assignable_v<_E> && is_copy_constructible_v<_E> = default;

    constexpr expected& operator=(const expected&)
        requires (!(is_copy_assignable_v<_E> && is_copy_constructible_v<_E>)) = delete;

    constexpr expected& operator=(expected&&) noexcept(is_nothrow_move_assignable_v<_E> && is_nothrow_move_constructible_v<_E>)
        requires is_move_assignable_v<_E> && is_move_constructible_v<_E> = default;

    template <class _G>
        requires is_constructible_v<_E, const _G&> && is_assignable_v<_E&, const _G&>
    constexpr expected& operator=(const unexpected<_G>& __u)
    {
        _M_storage = __u;
        return *this;
    }

    template <class _G>
        requires is_constructible_v<_E, _G> && is_assignable_v<_E&, _G>
    constexpr expected& operator=(unexpected<_G>&& __u)
    {
        _M_storage = __XVI_STD_NS::move(__u);
        return *this;
    }

    constexpr void emplace() noexcept
    {
        operator=(expected{});
    }

    constexpr explicit operator bool() const noexcept
    {
        return _M_storage._M_has_value;
    }

    constexpr bool has_value() const noexcept
    {
        return _M_storage._M_has_value;
    }

    constexpr void operator*() const noexcept
    {
    }

    constexpr void value() const &
    {
        if (!has_value())
            throw (bad_expected_access(error()));
    }

    constexpr void value() &&
    {
        if (!has_value())
            throw (bad_expected_access(__XVI_STD_NS::move(error())));
    }

    constexpr const _E& error() const &
    {
        return _M_storage._M_storage._M_U_unex;
    }

    constexpr _E& error() &
    {
        return _M_storage._M_storage._M_U_unex;
    }

    constexpr const _E&& error() const &&
    {
        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_unex);
    }

    constexpr _E&& error() &&
    {
        return __XVI_STD_NS::move(_M_storage._M_storage._M_U_unex);
    }

    constexpr void swap(expected& __rhs) noexcept(is_nothrow_move_constructible_v<_E> && is_nothrow_swappable_v<_E>)
        requires is_swappable_v<_E> && is_move_constructible_v<_E>
    {
        _M_storage.swap(__rhs._M_storage);
    }

    template <class _G = _E>
    constexpr _E error_or(_G&& __e) const &
    {
        if (has_value())
            return __XVI_STD_NS::forward<_G>(__e);
        else
            return error();
    }

    template <class _G = _E>
    constexpr _E error_or(_G&& __e) &&
    {
        if (has_value())
            return __XVI_STD_NS::forward<_G>(__e);
        else
            return __XVI_STD_NS::move(error());
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);
    
        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
        else
            return _U(unexpect, error());
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) const &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);

        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
        else
            return _U(unexpect, error());
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);

        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
        else
            return _U(unexpect, __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) const &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cvref_t<invoke_result_t<_F>>;
        static_assert(__detail::__expected_specialization<_U>);
        static_assert(is_same_v<typename _U::error_type, _E>);

        if (has_value())
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
        else
            return _U(unexpect, __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) &
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(error())>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G();
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error());
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) const &
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(error())>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G();
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error());
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) &&
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G();
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto or_else(_F&& __f) const &&
    {
        using _G = remove_cvref_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;
        static_assert(__detail::__expected_specialization<_G>);
        static_assert(is_same_v<typename _G::value_type, _T>);

        if (has_value())
            return _G();
        else
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error()));
    }

    template <class _F>
    constexpr auto transform(_F&& __f) &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, error());

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f)));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform(_F&& __f) const &
        requires is_copy_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, error());

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f)));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform(_F&& __f) &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, __XVI_STD_NS::move(error()));

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f)));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform(_F&& __f) const &&
        requires is_move_constructible_v<_E>
    {
        using _U = remove_cv_t<invoke_result_t<_F>>;

        if (!has_value())
            return expected<_U, _E>(unexpect, __XVI_STD_NS::move(error()));

        if constexpr (!is_void_v<_U>)
        {
            return expected<_U, _E>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f)));
        }
        else
        {
            __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f));
            return expected<_U, _E>();
        }
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) &
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(error())>>;

        if (has_value())
            return expected<_T, _G>();
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error()));
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) const &
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(error())>>;

        if (has_value())
            return expected<_T, _G>();
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), error()));
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) &&
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;

        if (has_value())
            return expected<_T, _G>();
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error())));
    }

    template <class _F>
    constexpr auto transform_error(_F&& __f) const &&
    {
        using _G = remove_cv_t<invoke_result_t<_F, decltype(__XVI_STD_NS::move(error()))>>;

        if (has_value())
            return expected<_T, _G>();
        else
            return expected<_T, _G>(unexpect, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(error())));
    }


    friend constexpr void swap(expected& __x, expected& __y) noexcept(noexcept(__x.swap(__y)))
    {
        __x.swap(__y);
    }

    template <class _T2, class _E2>
        requires is_void_v<_T2>
    friend constexpr bool operator==(const expected& __x, const expected<_T2, _E2>& __y)
    {
        if (__x.has_value() != __y.has_value())
            return false;

        return __x.has_value() || static_cast<bool>(__x.error() == __y.error());
    }

    template <class _E2>
    friend constexpr bool operator==(const expected& __x, const unexpected<_E2>& __y)
    {
        return !__x.has_value() && static_cast<bool>(__x.error() == __y.value());
    }


private:

    using __no_value = __detail::__expected_void_value_type;

    __detail::__expected_storage<__no_value, _E>    _M_storage = {};
};


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_EXPECTED_H */
