#ifndef __SYSTEM_CXX_UTILITY_EXPECTED_H
#define __SYSTEM_CXX_UTILITY_EXPECTED_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Swap.hh>
#include <System/C++/Utility/Private/InPlace.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _E>
class unexpected
{
public:

    constexpr unexpected(const unexpected&) = default;
    constexpr unexpected(unexpected&&) = default;

    template <class... _Args>
        requires std::is_constructible_v<_E, _Args...>
    constexpr explicit unexpected(in_place_t, _Args&&... __args) :
        _M_val(std::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
        requires std::is_constructible_v<_E, std::initializer_list<_U>&, _Args...>
    constexpr explicit unexpected(in_place_t, std::initializer_list<_U> __il, _Args&&... __args) :
        _M_val(__il, std::forward<_Args>(__args)...)
    {
    }

    template <class _Err = _E>
        requires (!std::is_same_v<std::remove_cvref_t<_Err>, unexpected>)
            && (!std::is_same_v<std::remove_cvref_t<_Err>, in_place_t>)
            && std::is_constructible_v<_E, _Err>
    constexpr explicit unexpected(_Err&& __err) :
        _M_val(std::forward<_Err>(__err))
    {
    }

    constexpr unexpected& operator=(const unexpected&) = default;
    constexpr unexpected& operator=(unexpected&&) = default;

    constexpr const _E& value() const & noexcept
    {
        return _M_val;
    }

    constexpr _E& value() & noexcept
    {
        return _M_val;
    }

    constexpr const _E&& value() const && noexcept
    {
        return std::move(_M_val);
    }

    constexpr _E&& value() && noexcept
    {
        return std::move(_M_val);
    }

    constexpr void swap(unexpected& __other) noexcept(std::is_nothrow_swappable_v<_E>)
    {
        using std::swap;
        swap(_M_val, __other._M_val);
    }

    template <class _E2>
    friend constexpr bool operator==(const unexpected& __x, const unexpected<_E2>& __y)
    {
        return __x.value() == __y.value();
    }

    friend constexpr void swap(unexpected& __x, unexpected& __y) noexcept(noexcept(__x.swap(__y)))
        requires (std::is_swappable_v<_E>)
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
class bad_expected_access<void> : public std::exception
{
public:

    const char* what() const noexcept override
    {
        return "bad expected access";
    }

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
        _M_val(std::move(__e))
    {
    }

    const char* what() const noexcept override
    {
        return "bad expected access: contained error";
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
        return std::move(_M_val);
    }

    const _E&& error() const && noexcept
    {
        return std::move(_M_val);
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
        if constexpr (std::is_nothrow_constructible_v<_T, _Args...>)
        {
            __oldval.~_U();
            ::new (std::addressof(__newval)) _V(std::forward<_Args>(__args)...);
        }
        else if constexpr (std::is_nothrow_move_constructible_v<_T>)
        {
            _V __tmp(std::forward<_Args>(__args)...);
            __oldval.~_U();
            ::new (std::addressof(__newval)) _V(std::move(__tmp));
        }
        else
        {
            _U __tmp(std::move(__oldval));
            __oldval.~_U();
            __XVI_CXX_UTILITY_TRY
            {
                ::new (std::addressof(__newval)) _V(std::forward<_Args>(__args)...);
            }
            __XVI_CXX_UTILITY_CATCH(...)
            {
                ::new (std::addressof(__oldval)) _U(std::move(__tmp));
                __XVI_CXX_UTILITY_RETHROW;
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
            _M_U_val(std::forward<_Args>(__args)...) {}

        template <class... _Args>
        constexpr __storage_t(unexpect_t, _Args&&... __args) :
            _M_U_unex(std::forward<_Args>(__args)...) {}

        constexpr __storage_t()
            requires std::is_default_constructible_v<_T> = default;

        constexpr __storage_t()
            requires (!(std::is_default_constructible_v<_T>)) {}

        constexpr __storage_t(const __storage_t&)
            requires std::is_trivially_copy_constructible_v<_T> && std::is_trivially_copy_constructible_v<_E> = default;

        constexpr __storage_t(const __storage_t&)
            requires (!(std::is_trivially_copy_constructible_v<_T> && std::is_trivially_copy_constructible_v<_E>)) {}

        constexpr __storage_t(__storage_t&&)
            requires std::is_trivially_move_constructible_v<_T> && std::is_trivially_move_constructible_v<_E> = default;

        constexpr __storage_t(__storage_t&&)
            requires (!(std::is_trivially_move_constructible_v<_T> && std::is_trivially_move_constructible_v<_E>)) {}

        constexpr ~__storage_t()
            requires std::is_trivially_destructible_v<_T> && std::is_trivially_destructible_v<_E> = default;

        constexpr ~__storage_t()
            requires (!(std::is_trivially_destructible_v<_T> && std::is_trivially_destructible_v<_E>)) {}

        constexpr __storage_t& operator=(const __storage_t&)
            requires std::is_trivially_copy_assignable_v<_T> && std::is_trivially_move_assignable_v<_E> = default;

        constexpr __storage_t& operator=(const __storage_t&)
            requires (!(std::is_trivially_copy_assignable_v<_T> && std::is_trivially_move_assignable_v<_T>)) { return *this; }

        constexpr __storage_t& operator=(__storage_t&&)
            requires std::is_trivially_move_assignable_v<_T> && std::is_trivially_move_assignable_v<_E> = default;

        constexpr __storage_t& operator=(__storage_t&&)
            requires (!(std::is_trivially_move_assignable_v<_T> && std::is_trivially_move_assignable_v<_E>)) { return *this; }
    };

    __storage_t _M_storage;
    bool        _M_has_value = true;

    constexpr __expected_storage() = default;

    constexpr __expected_storage(const __expected_storage&)
        requires std::is_trivially_copy_constructible_v<__storage_t> = default;

    constexpr __expected_storage(const __expected_storage& __other)
        requires (!(std::is_trivially_copy_constructible_v<__storage_t>))
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            ::new (std::addressof(_M_storage._M_U_val)) _T(__other._M_storage._M_U_val);
        else
            ::new (std::addressof(_M_storage._M_U_unex)) _E(__other._M_storage._M_U_unex);
    }

    constexpr __expected_storage(__expected_storage&&)
        requires std::is_trivially_move_constructible_v<__storage_t> = default;

    constexpr __expected_storage(__expected_storage&& __other)
        requires (!(std::is_trivially_move_constructible_v<__storage_t>))
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            ::new (std::addressof(_M_storage._M_U_val)) _T(std::move(__other._M_storage._M_U_val));
        else
            ::new (std::addressof(_M_storage._M_U_unex)) _E(std::move(__other._M_storage._M_U_unex));
    }

    template <class _U, class _G>
    constexpr __expected_storage(const __expected_storage<_U, _G>& __other)
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            ::new (std::addressof(_M_storage._M_U_val)) _T(__other._M_storage._M_U_val);
        else
            ::new (std::addressof(_M_storage._M_U_unex)) _E(__other._M_storage._M_U_unex);
    }

    template <class _U, class _G>
    constexpr __expected_storage(__expected_storage<_U, _G>&& __other)
    {
        _M_has_value = __other._M_has_value;
        if (_M_has_value)
            ::new (std::addressof(_M_storage._M_U_val)) _T(std::move(__other._M_storage._M_U_val));
        else
            ::new (std::addressof(_M_storage._M_U_unex)) _E(std::move(__other._M_storage._M_U_unex));
    }

    template <class... _Args>
    constexpr explicit __expected_storage(in_place_t, _Args&&... __args) :
        _M_storage(in_place, std::forward<_Args>(__args)...)
    {
    }

    template <class... _Args>
    constexpr explicit __expected_storage(unexpect_t, _Args&&... __args) :
        _M_storage(unexpect, std::forward<_Args>(__args)...),
        _M_has_value(false)
    {
    }

    constexpr ~__expected_storage()
        requires std::is_trivially_destructible_v<__storage_t> = default;

    constexpr ~__expected_storage()
        requires (!(std::is_trivially_destructible_v<__storage_t>))
    {
        if (_M_has_value)
            _M_storage._M_U_val.~_T();
        else
            _M_storage._M_U_unex.~_E();
    }

    constexpr __expected_storage& operator=(const __expected_storage& __other)
    {
        if (_M_has_value && __other._M_has_value)
        {
            _M_storage._M_U_val = __other._M_storage._M_U_val;
        }
        else if (_M_has_value)
        {
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, __other._M_U_unex);
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
    {
        if (_M_has_value && __other._M_has_value)
        {
            _M_storage._M_U_val = std::move(__other._M_storage._M_U_val);
        }
        else if (_M_has_value)
        {
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, std::move(__other._M_storage._M_U_unex));
        }
        else if (__other._M_has_value)
        {
            __reinit_expected(_M_storage._M_U_val, _M_storage._M_U_unex, std::move(__other._M_storage._M_U_val));
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
            _M_storage._M_U_val = std::forward<_U>(__t);
        else
            __reinit_expected(_M_storage._M_U_val, _M_storage._M_U_unex, std::forward<_U>(__t));

        _M_has_value = true;

        return *this;
    }

    template <class _G>
    constexpr __expected_storage& operator=(const unexpected<_G>& __u)
    {
        if (_M_has_value)
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, __u.value());
        else
            _M_storage._M_U_unex = __u.value();

        _M_has_value = false;

        return *this;
    }

    template <class _G>
    constexpr __expected_storage& operator=(unexpected<_G>&& __u)
    {
        if (_M_has_value)
            __reinit_expected(_M_storage._M_U_unex, _M_storage._M_U_val, std::move(__u.value()));
        else
            _M_storage._M_U_unex = std::move(__u.value());

        _M_has_value = false;

        return *this;
    }

    template <class... _Args>
    constexpr _T& emplace(_Args&&... __args) noexcept
    {
        if (_M_has_value)
            _M_storage._M_U_val.~_T();
        else
            _M_storage._M_U_unex.~_E();

        ::new (std::addressof(_M_storage._M_U_val)) _T(std::forward<_Args>(__args)...);

        return _M_storage._M_U_val;
    }

    constexpr void swap(__expected_storage& __other)
    {
        if (_M_has_value && __other._M_has_value)
        {
            using std::swap;
            swap(_M_storage._M_U_val, __other._M_storage._M_U_val);
        }
        else if (_M_has_value)
        {
            if constexpr (std::is_nothrow_move_constructible_v<_E>)
            {
                _E __tmp(std::move(__other._M_storage._M_U_unex));
                __other._M_storage._M_U_unex.~_E();

                __XVI_CXX_UTILITY_TRY
                {
                    ::new (std::addressof(__other._M_storage._M_U_val)) _T(std::move(_M_storage._M_U_val));
                    _M_storage._M_U_val.~_T();
                    ::new (std::addressof(_M_storage._M_U_unex)) _E(std::move(__tmp));
                }
                __XVI_CXX_UTILITY_CATCH(...)
                {
                    ::new (std::addressof(__other._M_storage._M_U_unex)) _E(std::move(__tmp));
                    __XVI_CXX_UTILITY_RETHROW;
                }
            }
            else
            {
                _T __tmp(std::move(_M_storage._M_U_val));
                _M_storage._M_U_val.~_T();

                __XVI_CXX_UTILITY_TRY
                {
                    ::new (std::addressof(_M_storage._M_U_unex)) _E(std::move(__other._M_storage._M_U_unex));
                    __other._M_storage._M_U_unex.~_E();
                    ::new (std::addressof(__other._M_storage._M_U_val)) _T(std::move(__tmp));
                }
                __XVI_CXX_UTILITY_CATCH(...)
                {
                    ::new (std::addressof(_M_storage._M_U_val)) _T(std::move(__tmp));
                    __XVI_CXX_UTILITY_RETHROW;
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
            using std::swap;
            swap(_M_storage._M_U_unex, __other._M_storage._M_U_unex);
        }
    }
};


template <class> struct __is_unexpected_specialization : std::false_type {};

template <class _E> struct __is_unexpected_specialization<unexpected<_E>> : std::true_type {};

template <class _T>
inline constexpr bool __is_unexpected_specialization_v = __is_unexpected_specialization<_T>::value;


} // namespace __detail


template <class _T, class _E>
class expected
{
public:

    using value_type        = _T;
    using error_type        = _E;
    using unexpected_type   = unexpected<_E>;

    template <class _U>
    using rebind = expected<_U, error_type>;

    constexpr expected()
        requires std::is_default_constructible_v<_T> = default;

    constexpr expected(const expected&)
        requires std::is_copy_constructible_v<_T> && std::is_copy_constructible_v<_E> = default;

    constexpr expected(const expected &)
        requires (!(std::is_copy_constructible_v<_T> && std::is_copy_constructible_v<_E>)) = delete;

    constexpr expected(expected&&) noexcept(std::is_nothrow_move_constructible_v<_T> && std::is_nothrow_move_constructible_v<_E>)
        requires std::is_move_constructible_v<_T> && std::is_move_constructible_v<_E> = default;

    template <class _U, class _G>
    constexpr explicit(!std::is_convertible_v<const _U&, _T> || !std::is_convertible_v<const _G&, _E>) expected(const expected<_U, _G>& __e)
        requires (std::is_constructible_v<_T, const _U&>
            && std::is_constructible_v<_E, const _G&>
            && !std::is_constructible_v<_T, expected<_U, _G>&>
            && !std::is_constructible_v<_T, expected<_U, _G>>
            && !std::is_constructible_v<_T, const expected<_U, _G>&>
            && !std::is_constructible_v<_T, const expected<_U, _G>>
            && !std::is_convertible_v<expected<_U, _G>&, _T>
            && !std::is_convertible_v<expected<_U, _G>&&, _T>
            && !std::is_convertible_v<const expected<_U, _G>&, _T>
            && !std::is_convertible_v<const expected<_U, _G>&&, _T>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>>) :
        _M_storage(__e._M_storage)
    {
    }

    template <class _U, class _G>
    constexpr explicit(!std::is_convertible_v<_U, _T> || !std::is_convertible_v<_G, _E>) expected(expected<_U, _G>&& __e)
        requires (std::is_constructible_v<_T, _U>
            && std::is_constructible_v<_E, _G>
            && !std::is_constructible_v<_T, expected<_U, _G>&>
            && !std::is_constructible_v<_T, expected<_U, _G>>
            && !std::is_constructible_v<_T, const expected<_U, _G>&>
            && !std::is_constructible_v<_T, const expected<_U, _G>>
            && !std::is_convertible_v<expected<_U, _G>&, _T>
            && !std::is_convertible_v<expected<_U, _G>&&, _T>
            && !std::is_convertible_v<const expected<_U, _G>&, _T>
            && !std::is_convertible_v<const expected<_U, _G>&&, _T>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>>) :
        _M_storage(std::move(__e._M_storage))
    {
    }

    template <class _U = _T>
    constexpr explicit(!std::is_convertible_v<_U, _T>) expected(_U&& __v)
        requires (!std::is_same_v<std::remove_cvref_t<_U>, in_place_t>
            && !std::is_same_v<expected<_T, _E>, std::remove_cvref_t<_U>>
            && !__detail::__is_unexpected_specialization_v<std::remove_cvref_t<_U>>
            && std::is_constructible_v<_T, _U>) :
        _M_storage(in_place, std::forward<_U>(__v))
    {
    }

    template <class _G>
    constexpr explicit(!std::is_convertible_v<const _G&, _E>) expected(const unexpected<_G>& __e)
        requires std::is_constructible_v<_E, const _G&> :
        _M_storage(unexpect, __e.value())
    {
    }

    template <class _G>
    constexpr explicit (!std::is_convertible_v<_G, _E>) expected(unexpected<_G>&& __e)
        requires std::is_constructible_v<_E, _G> :
        _M_storage(unexpect, std::move(__e.value()))
    {
    }

    template <class... _Args>
    constexpr explicit expected(in_place_t, _Args&&... __args)
        requires std::is_constructible_v<_T, _Args...> :
        _M_storage(in_place, std::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
    constexpr explicit expected(in_place_t, std::initializer_list<_U> __il, _Args&&... __args)
        requires std::is_constructible_v<_T, std::initializer_list<_U>&, _Args...> :
        _M_storage(in_place, __il, std::forward<_Args>(__args)...)
    {
    }

    template <class... _Args>
    constexpr explicit expected(unexpect_t, _Args&&... __args)
        requires std::is_constructible_v<_E, _Args...> :
        _M_storage(unexpect, std::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
    constexpr explicit expected(unexpect_t, std::initializer_list<_U> __il, _Args&&... __args)
        requires std::is_constructible_v<_E, std::initializer_list<_U>&, _Args...> :
        _M_storage(unexpect, __il, std::forward<_Args>(__args)...)
    {
    }

    constexpr ~expected() = default;

    constexpr expected& operator=(const expected&)
        requires std::is_copy_assignable_v<_T>
            && std::is_copy_constructible_v<_T>
            && std::is_copy_assignable_v<_E>
            && std::is_copy_constructible_v<_E>
            && (std::is_nothrow_move_constructible_v<_T> || std::is_nothrow_move_constructible_v<_E>)
    = default;

    constexpr expected& operator=(const expected&)
        requires (!(std::is_copy_assignable_v<_T>
            && std::is_copy_constructible_v<_T>
            && std::is_copy_assignable_v<_E>
            && std::is_copy_constructible_v<_E>
            && (std::is_nothrow_move_constructible_v<_T> || std::is_nothrow_move_constructible_v<_E>)))
    = delete;

    constexpr expected& operator=(expected&&) 
        noexcept(std::is_nothrow_move_assignable_v<_T> && std::is_nothrow_move_constructible_v<_T> && std::is_nothrow_move_assignable_v<_E> && std::is_nothrow_move_constructible_v<_E>)
        requires std::is_move_constructible_v<_T>
            && std::is_move_assignable_v<_T>
            && std::is_move_constructible_v<_E>
            && std::is_move_assignable_v<_E>
            && (std::is_nothrow_move_constructible_v<_T> || std::is_nothrow_move_constructible_v<_E>)
    = default;

    template <class _U = _T>
        requires (!std::is_same_v<expected, std::remove_cvref_t<_U>>
            && !__detail::__is_unexpected_specialization_v<std::remove_cvref_t<_U>>
            && std::is_constructible_v<_T, _U>
            && std::is_assignable_v<_T&, _U>
            && (std::is_nothrow_constructible_v<_T, _U> || std::is_nothrow_move_constructible_v<_T> || std::is_nothrow_move_constructible_v<_E>))
    constexpr expected& operator=(_U&& __t)
    {
        _M_storage = std::forward<_U>(__t);
        return *this;
    }

    template <class _G>
        requires std::is_constructible_v<_E, const _G&>
            && std::is_assignable_v<_E&, const _G&>
            && (std::is_nothrow_constructible_v<_E, const _G&> || std::is_nothrow_move_constructible_v<_T> || std::is_nothrow_move_constructible_v<_E>)
    constexpr expected& operator=(const unexpected<_G>& __u)
    {
        _M_storage = __u;
        return *this;
    }

    template <class _G>
        requires std::is_constructible_v<_E, _G>
            && std::is_assignable_v<_E&, _G>
            && (std::is_nothrow_constructible_v<_E, _G> || std::is_nothrow_move_constructible_v<_T> || std::is_nothrow_move_constructible_v<_E>)
    constexpr expected& operator=(unexpected<_G>&& __u)
    {
        _M_storage = std::move(__u);
        return *this;
    }

    template <class... _Args>
        requires std::is_nothrow_constructible_v<_T, _Args...>
    constexpr _T& emplace(_Args&&... __args) noexcept
    {
        return _M_storage.emplace(std::forward<_Args>(__args)...);
    }

    template <class _U, class... _Args>
        requires std::is_nothrow_constructible_v<_T, std::initializer_list<_U>&, _Args...>
    constexpr _T& emplace(std::initializer_list<_U> __il, _Args&&... __args) noexcept
    {
        return _M_storage.emplace(__il, std::forward<_Args>(__args)...);
    }

    constexpr void swap(expected& __e)
        noexcept(std::is_nothrow_move_constructible_v<_T> && std::is_nothrow_swappable_v<_T> && std::is_nothrow_move_constructible_v<_E> && std::is_nothrow_swappable_v<_E>)
        requires std::is_swappable_v<_T>
            && std::is_swappable_v<_E>
            && std::is_move_constructible_v<_T>
            && std::is_move_constructible_v<_E>
            && (std::is_nothrow_move_constructible_v<_T> || std::is_nothrow_move_constructible_v<_E>)
    {
        _M_storage.swap(__e._M_storage);
    }

    constexpr const _T* operator->() const noexcept
    {
        return std::addressof(_M_storage._M_storage._M_U_val);
    }

    constexpr _T* operator->() noexcept
    {
        return std::addressof(_M_storage._M_storage._M_U_val);
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
        return std::move(_M_storage._M_storage._M_U_val);
    }

    constexpr _T&& operator*() && noexcept
    {
        return std::move(_M_storage._M_storage._M_U_val);
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
            __XVI_CXX_UTILITY_THROW(bad_expected_access(error()));

        return _M_storage._M_storage._M_U_val;
    }

    constexpr _T& value() &
    {
        if (!has_value())
            __XVI_CXX_UTILITY_THROW(bad_expected_access(error()));

        return _M_storage._M_storage._M_U_val;
    }

    constexpr const _T&& value() const &&
    {
        if (!has_value())
            __XVI_CXX_UTILITY_THROW(bad_expected_access(std::move(error())));

        return std::move(_M_storage._M_storage._M_U_val);
    }

    constexpr _T&& value() &&
    {
        if (!has_value())
            __XVI_CXX_UTILITY_THROW(bad_expected_access(std::move(error())));

        return std::move(_M_storage._M_storage._M_U_val);
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
        return std::move(_M_storage._M_storage._M_U_unex);
    }

    constexpr _E&& error() &&
    {
        return std::move(_M_storage._M_storage._M_U_unex);
    }

    template <class _U>
    constexpr _T value_or(_U&& __u) const &
    {
        return has_value() ? **this : static_cast<_T>(std::forward<_U>(__u));
    }

    template <class _U>
    constexpr _T value_or(_U&& __u) &&
    {
        return has_value() ? std::move(**this) : static_cast<_T>(std::forward<_U>(__u));
    }


    friend constexpr void swap(expected& __x, expected& __y) noexcept(noexcept(__x.swap(__y)))
    {
        __x.swap(__y);
    }

    template <class _T2, class _E2>
        requires (!std::is_void_v<_T2>)
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
    requires std::is_void_v<_T>
class expected<_T, _E>
{
public:

    using value_type        = _T;
    using error_type        = _E;
    using unexpected_type   = unexpected<_E>;

    template <class _U> using rebind = expected<_U, error_type>;


    constexpr expected() noexcept = default;

    constexpr expected(const expected&)
        requires std::is_copy_constructible_v<_E> = default;

    constexpr expected(const expected&)
        requires (!std::is_copy_constructible_v<_E>) = delete;

    constexpr expected(expected&&) noexcept(std::is_nothrow_move_constructible_v<_E>)
        requires std::is_move_constructible_v<_E> = default;

    template <class _U, class _G>
        requires (std::is_void_v<_U>
            && std::is_constructible_v<_E, const _G&>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>>)
    constexpr explicit(!std::is_convertible_v<const _G&, _E>) expected(const expected<_U, _G>& __rhs) :
        _M_storage(__rhs._M_storage)
    {
    }

    template <class _U, class _G>
        requires (std::is_void_v<_U>
            && std::is_constructible_v<_E, _G>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>&>
            && !std::is_constructible_v<unexpected<_E>, expected<_U, _G>>
            && !std::is_constructible_v<unexpected<_E>, const expected<_U, _G>>)
    constexpr explicit(!std::is_convertible_v<_G, _E>) expected(expected<_U, _G>&& __rhs) :
        _M_storage(std::move(__rhs._M_storage))
    {
    }

    template <class _G>
        requires std::is_constructible_v<_E, const _G&>
    constexpr explicit(!std::is_convertible_v<const _G&, _E>) expected(const unexpected<_G>& __u) :
        _M_storage(unexpect, __u.value())
    {
    }

    template <class _G>
        requires std::is_constructible_v<_E, _G>
    constexpr explicit(!std::is_convertible_v<_G, _E>) expected(unexpected<_G>&& __u) :
        _M_storage(unexpect, std::move(__u.value()))
    {
    }

    constexpr explicit expected(in_place_t) noexcept :
        expected()
    {
    }

    template <class... _Args>
        requires std::is_constructible_v<_E, _Args...>
    constexpr explicit expected(unexpect_t, _Args&&... __args) :
        _M_storage(unexpect, std::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
        requires std::is_constructible_v<_E, std::initializer_list<_U>&, _Args...>
    constexpr explicit expected(unexpect_t, std::initializer_list<_U> __il, _Args&&... __args) :
        _M_storage(unexpect, __il, std::forward<_Args>(__args)...)
    {
    }

    constexpr ~expected() = default;

    constexpr expected& operator=(const expected&)
        requires std::is_copy_assignable_v<_E> && std::is_copy_constructible_v<_E> = default;

    constexpr expected& operator=(const expected&)
        requires (!(std::is_copy_assignable_v<_E> && std::is_copy_constructible_v<_E>)) = delete;

    constexpr expected& operator=(expected&&) noexcept(std::is_nothrow_move_assignable_v<_E> && std::is_nothrow_move_constructible_v<_E>)
        requires std::is_move_assignable_v<_E> && std::is_move_constructible_v<_E> = default;

    template <class _G>
        requires std::is_constructible_v<_E, const _G&> && std::is_assignable_v<_E&, const _G&>
    constexpr expected& operator=(const unexpected<_G>& __u)
    {
        _M_storage = __u;
        return *this;
    }

    template <class _G>
        requires std::is_constructible_v<_E, _G> && std::is_assignable_v<_E&, _G>
    constexpr expected& operator=(unexpected<_G>&& __u)
    {
        _M_storage = std::move(__u);
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
            __XVI_CXX_UTILITY_THROW(bad_expected_access(error()));
    }

    constexpr void value() &&
    {
        if (!has_value())
            __XVI_CXX_UTILITY_THROW(bad_expected_access(std::move(error())));
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
        return std::move(_M_storage._M_storage._M_U_unex);
    }

    constexpr _E&& error() &&
    {
        return std::move(_M_storage._M_storage._M_U_unex);
    }

    constexpr void swap(expected& __rhs) noexcept(std::is_nothrow_move_constructible_v<_E> && std::is_nothrow_swappable_v<_E>)
        requires std::is_swappable_v<_E> && std::is_move_constructible_v<_E>
    {
        _M_storage.swap(__rhs._M_storage);
    }

    friend constexpr void swap(expected& __x, expected& __y) noexcept(noexcept(__x.swap(__y)))
    {
        __x.swap(__y);
    }

    template <class _T2, class _E2>
        requires std::is_void_v<_T2>
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

    struct __no_value {};

    __detail::__expected_storage<__no_value, _E>    _M_storage = {};
};


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_EXPECTED_H */
