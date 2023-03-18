#ifndef __SYSTEM_CXX_CORE_OPTIONAL_H
#define __SYSTEM_CXX_CORE_OPTIONAL_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/AddressOf.hh>
#include <System/C++/Core/CtorTags.hh>
#include <System/C++/Core/Invoke.hh>
#include <System/C++/Core/Swap.hh>


namespace __XVI_STD_CORE_NS_DECL
{


// Forward declarations.
template <class> class optional;


namespace __detail
{


template <class _T>
concept __is_derived_from_optional = requires (const _T& __t)
    { []<class _U>(const optional<_U>&){}(__t); };


template <class _T, class _W>
constexpr bool __converts_from_any_cvref = disjunction_v<
    is_constructible<_T, _W&>, is_convertible<_W&, _T>,
    is_constructible<_T, _W>, is_convertible<_W, _T>,
    is_constructible<_T, const _W&>, is_convertible<const _W&, _T>,
    is_constructible<_T, const _W&&>, is_convertible<const _W, _T>>;


template <class _T>
struct __optional_storage
{
    union __storage_t
    {
        aligned_storage_t<sizeof(_T), alignof(_T)> __raw;
        _T __val;

        // Always constexpr constructible, regardless of type.
        constexpr __storage_t() : __raw() {}

        template <class... _Args>
        constexpr __storage_t(in_place_t, _Args&&... __args) : __val(__XVI_STD_NS::forward<_Args>(__args)...) {};

        constexpr __storage_t(const __storage_t&) requires (__XVI_STD_NS::is_trivially_copy_constructible_v<_T>) = default;

        constexpr __storage_t(const __storage_t&) requires (!__XVI_STD_NS::is_trivially_copy_constructible_v<_T>) {};

        constexpr __storage_t(__storage_t&&) requires (__XVI_STD_NS::is_trivially_move_constructible_v<_T>) = default;

        constexpr __storage_t(__storage_t&&) requires (!__XVI_STD_NS::is_trivially_move_constructible_v<_T>) {};

        constexpr __storage_t& operator=(const __storage_t&) requires (__XVI_STD_NS::is_trivially_copy_assignable_v<_T>) = default;

        constexpr __storage_t& operator=(const __storage_t&) requires (!__XVI_STD_NS::is_trivially_copy_assignable_v<_T>) { return *this; }

        constexpr __storage_t& operator=(__storage_t&&) requires (__XVI_STD_NS::is_trivially_move_assignable_v<_T>) = default;

        constexpr __storage_t& operator=(__storage_t&&) requires (!__XVI_STD_NS::is_trivially_move_assignable_v<_T>) { return *this; }

        constexpr ~__storage_t() requires (__XVI_STD_NS::is_trivially_destructible_v<_T>) = default;

        constexpr ~__storage_t() requires (!__XVI_STD_NS::is_trivially_destructible_v<_T>) {}
    };


    __storage_t _M_storage = {};
    bool _M_armed = false;


    constexpr __optional_storage() = default;

    template <class... _Args>
    constexpr __optional_storage(in_place_t, _Args&&... __args) :
        _M_storage(in_place, __XVI_STD_NS::forward<_Args>(__args)...),
        _M_armed(true)
    {
    }

    constexpr __optional_storage(const __optional_storage&)
        requires (__XVI_STD_NS::is_trivially_copy_constructible_v<_T>) = default;

    constexpr __optional_storage(const __optional_storage& __o)
        requires (!__XVI_STD_NS::is_trivially_copy_constructible_v<_T>) :
        _M_armed(__o._M_armed)
    {
        if (_M_armed)
            ::new (__XVI_STD_NS::addressof(_M_storage.__val)) _T(__o._M_storage.__val);
    }

    constexpr __optional_storage(__optional_storage&&)
        requires (__XVI_STD_NS::is_trivially_move_constructible_v<_T>) = default;

    constexpr __optional_storage(__optional_storage&& __o)
        requires (!__XVI_STD_NS::is_trivially_move_constructible_v<_T>) :
        _M_armed(__o._M_armed)
    {
        if (_M_armed)
            ::new (__XVI_STD_NS::addressof(_M_storage.__val)) _T(__XVI_STD_NS::move(__o._M_storage.__val));
    }

    template <class _Arg>
    constexpr __optional_storage& operator=(_Arg&& __arg)
    {
        if (_M_armed)
            _M_storage.__val = __XVI_STD_NS::forward<_Arg>(__arg);
        else
            new (__XVI_STD_NS::addressof(_M_storage.__val)) _T(__XVI_STD_NS::forward<_Arg>(__arg));

        _M_armed = true;

        return *this;
    }

    constexpr __optional_storage& operator=(const __optional_storage&)
        requires (__XVI_STD_NS::is_trivially_copy_assignable_v<_T> && __XVI_STD_NS::is_trivially_copy_constructible_v<_T> && __XVI_STD_NS::is_trivially_destructible_v<_T>) = default;

    constexpr __optional_storage& operator=(const __optional_storage& __o)
        requires (!(__XVI_STD_NS::is_trivially_copy_assignable_v<_T> && __XVI_STD_NS::is_trivially_copy_constructible_v<_T> && __XVI_STD_NS::is_trivially_destructible_v<_T>))
    {
        if (&__o == this) [[unlikely]]
            return *this;

        if (__o._M_armed)
        {
            if (_M_armed)
                _M_storage.__val = __o._M_storage.__val;
            else
                ::new (__XVI_STD_NS::addressof(_M_storage.__val)) _T(__o._M_storage.__val);

            _M_armed = true;
        }
        else
        {
            if (_M_armed)
                _M_storage.__val.~_T();

            _M_armed = false;
        }

        return *this;
    }

    constexpr __optional_storage& operator=(__optional_storage&&)
        requires (__XVI_STD_NS::is_trivially_move_assignable_v<_T> && __XVI_STD_NS::is_trivially_move_constructible_v<_T> && __XVI_STD_NS::is_trivially_destructible_v<_T>) = default;

    constexpr __optional_storage& operator=(__optional_storage&& __o)
        requires (!(__XVI_STD_NS::is_trivially_move_assignable_v<_T> && __XVI_STD_NS::is_trivially_move_constructible_v<_T> && __XVI_STD_NS::is_trivially_destructible_v<_T>))
    {
        if (&__o == this) [[unlikely]]
            return *this;

        if (__o._M_armed)
        {
            if (_M_armed)
                _M_storage.__val = __XVI_STD_NS::move(__o._M_storage.__val);
            else
                new (__XVI_STD_NS::addressof(_M_storage.__val)) _T(__XVI_STD_NS::move(__o._M_storage.__val));

            _M_armed = true;
        }
        else
        {
            if (_M_armed)
                _M_storage.__val.~_T();

            _M_armed = false;
        }

        return *this;
    }

    constexpr ~__optional_storage()
        requires (__XVI_STD_NS::is_trivially_destructible_v<_T>) = default;

    constexpr ~__optional_storage()
        requires (!__XVI_STD_NS::is_trivially_destructible_v<_T>)
    {
        if (_M_armed)
            _M_storage.__val.~_T();
    }

    constexpr void swap(__optional_storage& __rhs)
    {
        using __XVI_STD_NS::swap;

        if (_M_armed)
        {
            if (__rhs._M_armed)
            {
                swap(_M_storage.__val, __rhs._M_storage.__val);
            }
            else
            {
                ::new (__XVI_STD_NS::addressof(__rhs._M_storage.__val)) _T(__XVI_STD_NS::move(_M_storage.__val));
                _M_storage.__val.~_T();
                __rhs._M_armed = true;
                _M_armed = false;
            }
        }
        else
        {
            if (__rhs._M_armed)
            {
                ::new (__XVI_STD_NS::addressof(_M_storage.__val)) _T(__XVI_STD_NS::move(__rhs._M_storage.__val));
                __rhs._M_storage.__val.~_T();
                _M_armed = true;
                __rhs._M_armed = false;
            }
        }
    }

    constexpr void reset() noexcept
    {
        if (_M_armed)
            _M_storage.__val.~_T();

        _M_armed = false;
    }

    template <class... _Args>
    constexpr void emplace(_Args&&... __args)
    {
        if (_M_armed)
            _M_storage.__val.~_T();

        _M_armed = false;

        ::new (__XVI_STD_NS::addressof(_M_storage.__val)) _T(__XVI_STD_NS::forward<_Args>(__args)...);

        _M_armed = true;
    }

    constexpr const _T* __ptr() const noexcept
    {
        return __XVI_STD_NS::addressof(_M_storage.__val);
    }

    constexpr _T* __ptr() noexcept
    {
        return __XVI_STD_NS::addressof(_M_storage.__val);
    }
};


} // namespace __detail


struct nullopt_t
{
    nullopt_t() = delete;
    constexpr explicit nullopt_t(int) {}
};

inline constexpr nullopt_t nullopt {0};


class bad_optional_access : public exception
{
public:

    constexpr bad_optional_access() = default;

    const char* what() const noexcept override
    {
        return "bad optional access";
    }
};


template <class _T>
class optional
{
public:

    using value_type = _T;

    constexpr optional() noexcept = default;
    constexpr optional(nullopt_t) noexcept : optional() {}

    constexpr optional(const optional&)
        requires __XVI_STD_NS::is_copy_constructible_v<_T> = default;

    constexpr optional(const optional&)
        requires (!__XVI_STD_NS::is_copy_constructible_v<_T>) = delete;

    constexpr optional(optional&&) noexcept(is_nothrow_move_constructible_v<_T>)
        requires __XVI_STD_NS::is_move_constructible_v<_T> = default;

    template <class... _Args>
        requires __XVI_STD_NS::is_constructible_v<_T, _Args...>
    constexpr explicit optional(in_place_t, _Args&&... __args)
        : _M_storage(in_place, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class _U, class... _Args>
        requires __XVI_STD_NS::is_constructible_v<_T, std::initializer_list<_U>&, _Args...>
    constexpr explicit optional(in_place_t, std::initializer_list<_U> __il, _Args&&... __args)
        : _M_storage(in_place, __il, __XVI_STD_NS::forward<_Args>(__args)...)
    {
    }

    template <class _U = _T>
        requires __XVI_STD_NS::is_constructible_v<_T, _U>
            && (!__XVI_STD_NS::is_same_v<__XVI_STD_NS::remove_cvref_t<_U>, in_place_t>)
            && (!__XVI_STD_NS::is_same_v<__XVI_STD_NS::remove_cvref_t<_U>, optional>)
    explicit(!is_convertible_v<_U, _T>) constexpr optional(_U&& __v)
        : _M_storage(in_place, __XVI_STD_NS::forward<_U>(__v))
    {
    }

    template <class _U>
        requires is_constructible_v<_T, const _U&>
            && (!__detail::__converts_from_any_cvref<_T, optional<_U>>)
    explicit(!is_convertible_v<const _U&, _T>) constexpr optional(const optional<_U>& __u)
        : optional()
    {
        // Will perform a copy-construct despite looking like assignment.
        if (__u._M_storage._M_armed)
            _M_storage = __u.value();
    }

    template <class _U>
        requires is_constructible_v<_T, _U>
            && (!__detail::__converts_from_any_cvref<_T, optional<_U>>)
    explicit(!is_convertible_v<_U, _T>) constexpr optional(optional<_U>&& __u)
        : optional()
    {
        // Will perform a move-construct despite looking like assignment.
        if (__u._M_storage._M_armed)
            _M_storage = __XVI_STD_NS::move(__u).value();
    }

    constexpr ~optional() = default;

    constexpr optional& operator=(nullopt_t) noexcept
    {
        reset();
        return *this;
    }

    constexpr optional& operator=(const optional&)
        requires __XVI_STD_NS::is_copy_constructible_v<_T> && __XVI_STD_NS::is_copy_assignable_v<_T> = default;

    constexpr optional& operator=(const optional&)
        requires (!(__XVI_STD_NS::is_copy_constructible_v<_T> && __XVI_STD_NS::is_copy_assignable_v<_T>)) = delete;

    constexpr optional& operator=(optional&&) noexcept(is_nothrow_move_constructible_v<_T> && is_nothrow_move_assignable_v<_T>)
        requires __XVI_STD_NS::is_move_constructible_v<_T> && __XVI_STD_NS::is_move_assignable_v<_T> = default;

    template <class _U = _T>
        requires (!is_same_v<remove_cvref_t<_U>, optional>)
            && (!conjunction_v<is_scalar<_T>, is_same<_T, decay_t<_U>>>)
            && is_constructible_v<_T, _U>
            && is_assignable_v<_T&, _U>
    constexpr optional& operator=(_U&& __v)
    {
        _M_storage = __XVI_STD_NS::forward<_U>(__v);

        return *this;
    }

    template <class _U>
        requires is_constructible_v<_T, const _U&>
            && is_assignable_v<_T, const _U&>
            && (!__detail::__converts_from_any_cvref<_T, optional<_U>>)
            && (!is_assignable_v<_T&, optional<_U>&>)
            && (!is_assignable_v<_T&, optional<_U>&&>)
            && (!is_assignable_v<_T&, const optional<_U>&>)
            && (!is_assignable_v<_T&, const optional<_U>&&>)
    constexpr optional& operator=(const optional<_U>& __rhs)
    {
        if (__rhs._M_storage._M_armed)
            _M_storage = __rhs._M_storage.__val;
        else
            reset();

        return *this;
    }

    template <class _U>
        requires is_constructible_v<_T, _U>
            && is_assignable_v<_T, _U>
            && (!__detail::__converts_from_any_cvref<_T, optional<_U>>)
            && (!is_assignable_v<_T&, optional<_U>&>)
            && (!is_assignable_v<_T&, optional<_U>&&>)
            && (!is_assignable_v<_T&, const optional<_U>&>)
            && (!is_assignable_v<_T&, const optional<_U>&&>)
    constexpr optional& operator=(optional<_U>&& __rhs)
    {
        if (__rhs._M_storage._M_armed)
            _M_storage = __XVI_STD_NS::move(__rhs).value();
        else
            reset();

        return *this;
    }


    template <class... _Args>
    constexpr _T& emplace(_Args&&... __args)
    {
        _M_storage.emplace(__XVI_STD_NS::forward<_Args>(__args)...);

        return value();
    }

    template <class _U, class... _Args>
        requires __XVI_STD_NS::is_constructible_v<_T, __XVI_STD_NS::initializer_list<_U>&, _Args...>
    constexpr _T& emplace(initializer_list<_U> __il, _Args&&... __args)
    {
        _M_storage.emplace(__il, __XVI_STD_NS::forward<_Args>(__args)...);

        return value();
    }


    constexpr void swap(optional& __rhs) noexcept(is_nothrow_move_constructible_v<_T> && is_nothrow_swappable_v<_T>)
    {
        _M_storage.swap(__rhs._M_storage);
    }


    constexpr const _T* operator->() const
    {
        return _M_storage.__ptr();
    }

    constexpr _T* operator->()
    {
        return _M_storage.__ptr();
    }

    constexpr const _T& operator*() const &
    {
        return *_M_storage.__ptr();
    }

    constexpr _T& operator*() &
    {
        return *_M_storage.__ptr();
    }

    constexpr const _T&& operator*() const &&
    {
        return __XVI_STD_NS::move(*_M_storage.__ptr());
    }

    constexpr _T&& operator*() &&
    {
        return __XVI_STD_NS::move(*_M_storage.__ptr());
    }

    constexpr explicit operator bool() const noexcept
    {
        return _M_storage._M_armed;
    }

    constexpr bool has_value() const noexcept
    {
        return _M_storage._M_armed;
    }

    constexpr const _T& value() const &
    {
        if (!has_value())
            __invalid_access();
        return **this;
    }

    constexpr _T& value() &
    {
        if (!has_value())
            __invalid_access();
        return **this;
    }

    constexpr const _T&& value() const &&
    {
        if (!has_value())
            __invalid_access();
        return __XVI_STD_NS::move(**this);
    }

    constexpr _T&& value() &&
    {
        if (!has_value())
            __invalid_access();
        return __XVI_STD_NS::move(**this);
    }

    template <class _U>
    constexpr _T value_or(_U&& __v) const &
    {
        return has_value() ? **this : static_cast<_T>(__XVI_STD_NS::forward<_U>(__v));
    }

    template <class _U>
    constexpr _T value_or(_U&& __v) &&
    {
        return has_value() ? __XVI_STD_NS::move(**this) : static_cast<_T>(__XVI_STD_NS::forward<_U>(__v));
    }

    constexpr void reset() noexcept
    {
        _M_storage.reset();
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) &
    {
        if (*this)
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value());
        else
            return __XVI_STD_NS::remove_cvref_t<__XVI_STD_NS::invoke_result_t<_F, decltype(value())>>();
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) const &
    {
        if (*this)
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value());
        else
            return __XVI_STD_NS::remove_cvref_t<__XVI_STD_NS::invoke_result_t<_F, decltype(value())>>();
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) &&
    {
        if (*this)
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value()));
        else
            return __XVI_STD_NS::remove_cvref_t<__XVI_STD_NS::invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>();
    }

    template <class _F>
    constexpr auto and_then(_F&& __f) const &&
    {
        if (*this)
            return __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value()));
        else
            return __XVI_STD_NS::remove_cvref_t<__XVI_STD_NS::invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>();
    }

    template <class _F>
    constexpr auto transform(_F&& __f) &
    {
        using _U = __XVI_STD_NS::remove_cv_t<__XVI_STD_NS::invoke_result_t<_F, decltype(value())>>;

        if (*this)
            return optional<_U>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value()));
        else
            return optional<_U>();
    }

    template <class _F>
    constexpr auto transform(_F&& __f) const &
    {
        using _U = __XVI_STD_NS::remove_cv_t<__XVI_STD_NS::invoke_result_t<_F, decltype(value())>>;

        if (*this)
            return optional<_U>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), value()));
        else
            return optional<_U>();
    }

    template <class _F>
    constexpr auto transform(_F&& __f) &&
    {
        using _U = __XVI_STD_NS::remove_cv_t<__XVI_STD_NS::invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>;

        if (*this)
            return optional<_U>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value())));
        else
            return optional<_U>();
    }

    template <class _F>
    constexpr auto transform(_F&& __f) const &&
    {
        using _U = __XVI_STD_NS::remove_cv_t<__XVI_STD_NS::invoke_result_t<_F, decltype(__XVI_STD_NS::move(value()))>>;

        if (*this)
            return optional<_U>(in_place, __XVI_STD_NS::invoke(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::move(value())));
        else
            return optional<_U>();
    }

    template <class _F>
        requires __XVI_STD_NS::invocable<_F>
            && __XVI_STD_NS::copy_constructible<_T>
    constexpr auto or_else(_F&& __f) const &
    {
        if (*this)
            return *this;
        else
            return __XVI_STD_NS::forward<_F>(__f)();
    }

    template <class _F>
        requires __XVI_STD_NS::invocable<_F>
            && __XVI_STD_NS::move_constructible<_T>
    constexpr auto or_else(_F&& __f) &&
    {
        if (*this)
            return __XVI_STD_NS::move(*this);
        else
            return __XVI_STD_NS::forward<_F>(__f)();
    }

private:

    template <class _U> friend class optional;
    using __storage_t = __detail::__optional_storage<_T>;

    __storage_t _M_storage = {};

#ifdef __XVI_CXX_UTILITY_NO_EXCEPTIONS
    [[noreturn]] void __invalid_access() const noexcept
    {
        terminate();
    }
#else
    [[noreturn]] void __invalid_access() const
    {
        throw bad_optional_access();
    }
#endif
};

template <class _T> optional(_T) -> optional<_T>;


template <class _T, class _U>
constexpr bool operator==(const optional<_T>& __x, const optional<_U>& __y)
{
    if (bool(__x) != bool(__y))
        return false;
    else if (!bool(__x))
        return true;
    else
        return *__x == *__y;
}

template <class _T, class _U>
constexpr bool operator!=(const optional<_T>& __x, const optional<_U>& __y)
{
    if (bool(__x) != bool(__y))
        return true;
    else if (!bool(__x))
        return false;
    else
        return *__x != *__y;
}

template <class _T, class _U>
constexpr bool operator<(const optional<_T>& __x, const optional<_U>& __y)
{
    if (!__y)
        return false;
    else if (!__x)
        return true;
    else
        return *__x < *__y;
}

template <class _T, class _U>
constexpr bool operator>(const optional<_T>& __x, const optional<_U>& __y)
{
    if (!__x)
        return false;
    else if (!__y)
        return true;
    else
        return *__x > *__y;
}

template <class _T, class _U>
constexpr bool operator<=(const optional<_T>& __x, const optional<_U>& __y)
{
    if (!__x)
        return true;
    else if (!__y)
        return false;
    else
        return *__x <= *__y;
}

template <class _T, class _U>
constexpr bool operator>=(const optional<_T>& __x, const optional<_U>& __y)
{
    if (!__y)
        return true;
    else if (!__x)
        return false;
    else
        return *__x >= *__y;
}

template <class _T, __XVI_STD_NS::three_way_comparable_with<_T> _U>
constexpr __XVI_STD_NS::compare_three_way_result_t<_T, _U>
operator<=>(const optional<_T>& __x, const optional<_T>& __y)
{
    if (__x && __y)
        return *__x <=> *__y;
    else
        return __x.has_value() <=> __y.has_value();
}

template <class _T>
constexpr bool operator==(const optional<_T>& __x, nullopt_t) noexcept
{
    return !__x;
}

template <class _T>
constexpr __XVI_STD_NS::strong_ordering operator<=>(const optional<_T>& __x, nullopt_t) noexcept
{
    return __x.has_value <=> false;
}

template <class _T, class _U>
constexpr bool operator==(const optional<_T>& __x, const _U& __v)
{
    return bool(__x) ? *__x == __v : false;
}

template <class _T, class _U>
constexpr bool operator==(const _T& __v, const optional<_U>& __x)
{
    return bool(__x) ? __v == *__x : false;
}

template <class _T, class _U>
constexpr bool operator!=(const optional<_T>& __x, const _U& __v)
{
    return bool(__x) ? *__x == __v : true;
}

template <class _T, class _U>
constexpr bool operator!=(const _T& __v, const optional<_U>& __x)
{
    return bool(__x) ? __v == *__x : true;
}

template <class _T, class _U>
constexpr bool operator<(const optional<_T>& __x, const _U& __v)
{
    return bool(__x) ? *__x < __v : true;
}

template <class _T, class _U>
constexpr bool operator<(const _T& __v, const optional<_U>& __x)
{
    return bool(__x) ? __v < *__x : false;
}

template <class _T, class _U>
constexpr bool operator>(const optional<_T>& __x, const _U& __v)
{
    return bool(__x) ? *__x > __v : false;
}

template <class _T, class _U>
constexpr bool operator>(const _T& __v, const optional<_U>& __x)
{
    return bool(__x) ? __v > *__x : true;
}

template <class _T, class _U>
constexpr bool operator<=(const optional<_T>& __x, const _U& __v)
{
    return bool(__x) ? *__x <= __v : true;
}

template <class _T, class _U>
constexpr bool operator<=(const _T& __v, const optional<_U>& __x)
{
    return bool(__x) ? __v <= *__x : false;
}

template <class _T, class _U>
constexpr bool operator>=(const optional<_T>& __x, const _U& __v)
{
    return bool(__x) ? *__x >= __v : false;
}

template <class _T, class _U>
constexpr bool operator>=(const _U& __v, const optional<_T>& __x)
{
    return bool(__x) ? __v >= *__x : true;
}

template <class _T, class _U>
    requires (!__detail::__is_derived_from_optional<_U>)
        && __XVI_STD_NS::three_way_comparable_with<_T, _U>
constexpr __XVI_STD_NS::compare_three_way_result_t<_T, _U>
operator<=>(const optional<_T>& __x, const _U& __v)
{
    return __x.has_value() ? *__x <=> __v : __XVI_STD_NS::strong_ordering::less;
}


template <class _T>
    requires __XVI_STD_NS::is_move_constructible_v<_T> && __XVI_STD_NS::is_swappable_v<_T>
void swap(optional<_T>& __x, optional<_T>& __y) noexcept(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}


template <class _T>
constexpr optional<decay_t<_T>> make_optional(_T&& __t)
{
    return optional<decay_t<_T>>(__XVI_STD_NS::forward<_T>(__t));
}

template <class _T, class... _Args>
constexpr optional<_T> make_optional(_Args&&... __args)
{
    return optional<_T>(in_place, __XVI_STD_NS::forward<_Args>(__args)...);
}

template <class _T, class _U, class... _Args>
constexpr optional<_T> make_optional(initializer_list<_U> __il, _Args&&... __args)
{
    return optional<_T>(in_place, __il, __XVI_STD_NS::forward<_Args>(__args)...);
}


//! @TODO: hash support.


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_OPTIONAL_H */
