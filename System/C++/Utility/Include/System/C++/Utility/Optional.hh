#pragma once
#ifndef __SYSTEM_CXX_UTILITY_OPTIONAL_H
#define __SYSTEM_CXX_UTILITY_OPTIONAL_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/AddressOf.hh>
#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/InPlace.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{

// Defined further below.
template <class> struct __optional_storage;

template <class _T>
struct __optional_storage_base
{
    union __storage_t
    {
        aligned_storage_t<sizeof(_T), alignof(_T)> __raw;
        _T __val;

        constexpr __storage_t() : __raw() {}

        constexpr ~__storage_t() requires is_trivially_destructible_v<_T> = default;

        constexpr ~__storage_t()
            requires (!is_trivially_destructible_v<_T>)
        {
        }
    };

    __storage_t _M_storage;

    constexpr _T* __ptr() noexcept 
        { return __XVI_STD_NS::addressof(_M_storage.__val); }
    constexpr const _T* __ptr() const noexcept 
        { return __XVI_STD_NS::addressof(_M_storage.__val); }
};


template <class _T, bool = is_copy_constructible_v<_T>, bool = is_trivially_copy_constructible_v<_T>>
struct __optional_copy_ctor_base : __optional_storage_base<_T>
{
    // Default when trivially copy constructible - use the copy ctor of aligned_storage_t which will just memcpy the
    // underlying memory (as trivially-copyable allows).
};

template <class _T, bool _Ignored>
struct __optional_copy_ctor_base<_T, false, _Ignored> : __optional_storage_base<_T>
{
    // Non-copyable.
    constexpr __optional_copy_ctor_base() = default;
    constexpr __optional_copy_ctor_base(__optional_copy_ctor_base&&) = default;
    constexpr __optional_copy_ctor_base& operator=(const __optional_copy_ctor_base&) = default;
    constexpr __optional_copy_ctor_base& operator=(__optional_copy_ctor_base&&) = default;
    __optional_copy_ctor_base(const __optional_copy_ctor_base&) = delete;
};

template <class _T>
struct __optional_copy_ctor_base<_T, true, false> : __optional_storage_base<_T>
{
    // Copyable but not trivially.
    constexpr __optional_copy_ctor_base() = default;
    constexpr __optional_copy_ctor_base(__optional_copy_ctor_base&&) = default;
    constexpr __optional_copy_ctor_base& operator=(const __optional_copy_ctor_base&) = default;
    constexpr __optional_copy_ctor_base& operator=(__optional_copy_ctor_base&&) = default;
    constexpr __optional_copy_ctor_base(const __optional_copy_ctor_base& __rhs)
    {
        auto __this = static_cast<__optional_storage<_T>*>(this);
        auto __that = static_cast<const __optional_storage<_T>*>(&__rhs);
        if (__that->_M_armed)
            new (__this->__ptr()) _T(*__that->__ptr());
    }
};


template <class _T, bool = is_move_constructible_v<_T>, bool = is_trivially_move_constructible_v<_T>>
struct __optional_move_ctor_base : __optional_copy_ctor_base<_T>
{
    // Default when trivially move constructible - use the move ctor of aligned_storage_t which will just memcpy the
    // underlying memory (as trivially-copyable allows).
};

template <class _T, bool _Ignored>
struct __optional_move_ctor_base<_T, false, _Ignored> : __optional_copy_ctor_base<_T>
{
    // Non-moveable.
    constexpr __optional_move_ctor_base() = default;
    constexpr __optional_move_ctor_base(const __optional_move_ctor_base&) = default;
    constexpr __optional_move_ctor_base& operator=(const __optional_move_ctor_base&) = default;
    constexpr __optional_move_ctor_base& operator=(__optional_move_ctor_base&&) = delete;
    __optional_move_ctor_base(__optional_move_ctor_base&&) = delete;
};

template <class _T>
struct __optional_move_ctor_base<_T, true, false> : __optional_copy_ctor_base<_T>
{
    // Moveable but not trivially.
    constexpr __optional_move_ctor_base() = default;
    constexpr __optional_move_ctor_base(const __optional_move_ctor_base&) = default;
    constexpr __optional_move_ctor_base& operator=(const __optional_move_ctor_base&) = default;
    constexpr __optional_move_ctor_base& operator=(__optional_move_ctor_base&&) = delete;
    constexpr __optional_move_ctor_base(__optional_move_ctor_base&& __rhs)
    {
        auto __this = static_cast<__optional_storage<_T>*>(this);
        auto __that = static_cast<__optional_storage<_T>*>(&__rhs);
        if (__that->_M_armed)
            new (__this->__ptr()) _T(__XVI_STD_NS::move(*__that->__ptr()));
    }
};

template <class _T,
          bool = is_copy_constructible_v<_T> && is_copy_assignable_v<_T>,
          bool = is_trivially_copy_constructible_v<_T> && is_trivially_copy_assignable_v<_T>>
struct __optional_copy_assign_base : __optional_move_ctor_base<_T>
{
    // Default when trivially copy constructible and assignable - rely on a memcpy of the underlying bytes.
};

template <class _T, bool _Ignored>
struct __optional_copy_assign_base<_T, false, _Ignored> : __optional_move_ctor_base<_T>
{
    // Not copy assignable.
    constexpr __optional_copy_assign_base() = default;
    constexpr __optional_copy_assign_base(const __optional_copy_assign_base&) = default;
    constexpr __optional_copy_assign_base(__optional_copy_assign_base&&) = default;
    constexpr __optional_copy_assign_base& operator=(__optional_copy_assign_base&&) = default;
    __optional_copy_assign_base& operator=(const __optional_copy_assign_base&) = delete;
};

template <class _T>
struct __optional_copy_assign_base<_T, true, false> : __optional_move_ctor_base<_T>
{
    // Copy assignable but not trivially.
    constexpr __optional_copy_assign_base() = default;
    constexpr __optional_copy_assign_base(const __optional_copy_assign_base&) = default;
    constexpr __optional_copy_assign_base(__optional_copy_assign_base&&) = default;
    constexpr __optional_copy_assign_base& operator=(__optional_copy_assign_base&&) = default;
    constexpr __optional_copy_assign_base& operator=(const __optional_copy_assign_base& __rhs)
    {
        auto __this = static_cast<__optional_storage<_T>*>(this);
        auto __that = static_cast<const __optional_storage<_T>*>(&__rhs);

        if (__that->_M_armed)
        {
            if (__this->_M_armed)
                *__this->__ptr() = *__that->__ptr();
            else
                new (__this->__ptr()) _T(*__that->__ptr());
            __this->_M_armed = true;
        }
        else
        {
            if (__this->_M_armed)
                __this->__ptr()->~_T();
            else
                {} // Nothing to do.
            __this->_M_armed = false;
        }

        return *this;
    }
};


template <class _T,
          bool = is_move_constructible_v<_T> && is_move_assignable_v<_T>,
          bool = is_trivially_move_constructible_v<_T> && is_trivially_move_assignable_v<_T>>
struct __optional_move_assign_base : __optional_copy_assign_base<_T>
{
    // Default when trivially move constructible and assignable - rely on a memcpy of the underlying bytes.
};

template <class _T, bool _Ignored>
struct __optional_move_assign_base<_T, false, _Ignored> : __optional_copy_assign_base<_T>
{
    // Not move assignable.
    constexpr __optional_move_assign_base() = default;
    constexpr __optional_move_assign_base(const __optional_move_assign_base&) = default;
    constexpr __optional_move_assign_base(__optional_move_assign_base&&) = default;
    constexpr __optional_move_assign_base& operator=(const __optional_move_assign_base&) = default;
    __optional_move_assign_base& operator=(__optional_move_assign_base&&) = delete;
};

template <class _T>
struct __optional_move_assign_base<_T, true, false> : __optional_copy_assign_base<_T>
{
    // Move assignable but not trivially.
    constexpr __optional_move_assign_base() = default;
    constexpr __optional_move_assign_base(const __optional_move_assign_base&) = default;
    constexpr __optional_move_assign_base(__optional_move_assign_base&&) = default;
    constexpr __optional_move_assign_base& operator=(const __optional_move_assign_base&) = default;
    constexpr __optional_move_assign_base& operator=(__optional_move_assign_base&& __rhs)
    {
        auto __this = static_cast<__optional_storage<_T>*>(this);
        auto __that = static_cast<__optional_storage<_T>*>(&__rhs);

        if (__that->_M_armed)
        {
            if (__this->_M_armed)
                *__this->__ptr() = __XVI_STD_NS::move(*__that->__ptr());
            else
                new (__this->__ptr()) _T(__XVI_STD_NS::move(*__that->__ptr()));
            __this->_M_armed = true;
        }
        else
        {
            if (__this->_M_armed)
                __this->__ptr()->~_T();
            else
                {} // Do nothing.
            __this->_M_armed = false;
        }

        return *this;
    }
};


template <class _T, bool = is_trivially_destructible_v<_T>>
struct __optional_dtor_base : __optional_move_assign_base<_T>
{
    // Destructor is trivial.
};

template <class _T>
struct __optional_dtor_base<_T, false> : __optional_move_assign_base<_T>
{
    // Non-trivial destructor.
    ~__optional_dtor_base()
    {
        auto __this = static_cast<__optional_storage<_T>*>(this);
        __this->__ptr()->~_T();
    }
};


template <class _T>
struct __optional_storage
    : public __optional_move_assign_base<_T>
{
    constexpr __optional_storage() = default;
    constexpr __optional_storage(const __optional_storage&) = default;
    constexpr __optional_storage(__optional_storage&&) = default;

    constexpr __optional_storage& operator=(const __optional_storage&) = default;
    constexpr __optional_storage& operator=(__optional_storage&&) = default;

    ~__optional_storage() = default;

    bool _M_armed = false;
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
    : private __detail::__optional_storage<_T>
{
public:

    constexpr optional() noexcept : __storage_t() {}
    constexpr optional(nullopt_t) noexcept : optional() {}

    constexpr optional(const optional&) = default;
    constexpr optional(optional&&) noexcept(is_nothrow_move_constructible_v<_T>) = default;

    template <class... _Args,
              class = enable_if_t<is_constructible_v<_T, _Args...>, void>>
    constexpr explicit optional(in_place_t, _Args&&... __args)
        : optional()
    {
        new (__storage_t::__ptr()) _T(__XVI_STD_NS::forward<_Args>(__args)...);
        __storage_t::_M_armed = true;
    }

    template <class _U, class... _Args,
              class = enable_if_t<is_constructible_v<_T, initializer_list<_U>&, _Args&&...>, void>>
    constexpr explicit optional(in_place_t, initializer_list<_U> __il, _Args&&... __args)
        : optional()
    {
        new (__storage_t::__ptr()) _T(__il, __XVI_STD_NS::forward<_Args>(__args)...);
        __storage_t::_M_armed = true;
    }

    template <class _U = _T,
              class = enable_if_t<is_constructible_v<_T, _U&&>
                                  && !is_same_v<remove_cvref_t<_U>, in_place_t>
                                  && !is_same_v<remove_cvref_t<_U>, optional>, void>>
    explicit(!is_convertible_v<_U, _T>) constexpr optional(_U&& __v)
        : optional()
    {
        new (__storage_t::__ptr()) _T(__XVI_STD_NS::forward<_U>(__v));
        __storage_t::_M_armed = true;
    }

    template <class _U,
              class = enable_if_t<is_constructible_v<_T, const _U&>
                                  && !is_constructible_v<_T, optional<_U>&>
                                  && !is_constructible_v<_T, optional<_U>&&>
                                  && !is_constructible_v<_T, const optional<_U>&>
                                  && !is_constructible_v<_T, const optional<_U>&&>
                                  && !is_convertible_v<optional<_U>&, _T>
                                  && !is_convertible_v<optional<_U>&&, _T>
                                  && !is_convertible_v<const optional<_U>&, _T>
                                  && !is_convertible_v<const optional<_U>&&, _T>, void>>
    explicit(!is_convertible_v<const _U&, _T>) optional(const optional<_U>& __u)
        : optional()
    {
        if (__u._M_armed)
            new (__storage_t::__ptr()) _T(*__u);
        __storage_t::_M_armed = __u._M_armed;
    }

    template <class _U,
              class = enable_if_t<is_constructible_v<_T, const _U&>
                                  && !is_constructible_v<_T, optional<_U>&>
                                  && !is_constructible_v<_T, optional<_U>&&>
                                  && !is_constructible_v<_T, const optional<_U>&>
                                  && !is_constructible_v<_T, const optional<_U>&&>
                                  && !is_convertible_v<optional<_U>&, _T>
                                  && !is_convertible_v<optional<_U>&&, _T>
                                  && !is_convertible_v<const optional<_U>&, _T>
                                  && !is_convertible_v<const optional<_U>&&, _T>, void>>
    explicit(!is_convertible_v<_U&&, _T>) optional(optional<_U>&& __u)
        : optional()
    {
        if (__u._M_armed)
            new (__storage_t::__ptr()) _T(__XVI_STD_NS::move(*__u));
        __storage_t::_M_armed = __u._M_armed;
    }


    optional<_T>& operator=(nullopt_t) noexcept
    {
        reset();
        return *this;
    }

    constexpr optional& operator=(const optional&) = default;
    constexpr optional& operator=(optional&&) noexcept (is_nothrow_move_constructible_v<_T> && is_nothrow_move_assignable_v<_T>)= default;

    template <class _U = _T,
              class = enable_if_t<!is_same_v<remove_cvref_t<_U>, optional>
                                  && !conjunction_v<is_scalar_v<_T>, is_same_v<_T, decay_t<_U>>>
                                  && is_constructible_v<_T, _U>
                                  && is_assignable_v<_T&, _U>, void>>
    optional<_T>& operator=(_U&& __v)
    {
        if (__storage_t::_M_armed)
            *__storage_t::__ptr() = __XVI_STD_NS::forward<_U>(__v);
        else
            new (__storage_t::__ptr()) _T(__XVI_STD_NS::forward<_U>(__v));
        __storage_t::_M_armed = true;

        return *this;
    }

    template <class _U,
              class = enable_if_t<is_constructible_v<_T, const _U&>
                                  && is_assignable_v<_T&, const _U&>
                                  && !is_constructible_v<_T, optional<_U>&>
                                  && !is_constructible_v<_T, optional<_U>&&>
                                  && !is_constructible_v<_T, const optional<_U>&>
                                  && !is_constructible_v<_T, const optional<_U>&&>
                                  && !is_convertible_v<optional<_U>&, _T>
                                  && !is_convertible_v<optional<_U>&&, _T>
                                  && !is_convertible_v<const optional<_U>&, _T>
                                  && !is_convertible_v<const optional<_U>&&, _T>
                                  && !is_assignable_v<_T&, optional<_U>&>
                                  && !is_assignable_v<_T&, optional<_U>&&>
                                  && !is_assignable_v<_T&, const optional<_U>&>
                                  && !is_assignable_v<_T&, const optional<_U>&&>, void>>
    optional<_T>& operator=(const optional<_U>& __rhs)
    {
        if (__rhs._M_armed)
        {
            if (__storage_t::_M_armed)
                *__storage_t::__ptr() = *__rhs.__ptr();
            else
                new (__storage_t::__ptr()) _T(*__rhs.__ptr());
            __storage_t::_M_armed = true;
        }
        else
        {
            if (__storage_t::_M_armed)
                __storage_t::__ptr()->~_T();
            else
                {} // Do nothing.
            __storage_t::_M_armed = false;
        }

        return *this;
    }

    template <class _U,
              class = enable_if_t<is_constructible_v<_T, _U>
                                  && is_assignable_v<_T&, _U>
                                  && !is_constructible_v<_T, optional<_U>&>
                                  && !is_constructible_v<_T, optional<_U>&&>
                                  && !is_constructible_v<_T, const optional<_U>&>
                                  && !is_constructible_v<_T, const optional<_U>&&>
                                  && !is_convertible_v<optional<_U>&, _T>
                                  && !is_convertible_v<optional<_U>&&, _T>
                                  && !is_convertible_v<const optional<_U>&, _T>
                                  && !is_convertible_v<const optional<_U>&&, _T>
                                  && !is_assignable_v<_T&, optional<_U>&>
                                  && !is_assignable_v<_T&, optional<_U>&&>
                                  && !is_assignable_v<_T&, const optional<_U>&>
                                  && !is_assignable_v<_T&, const optional<_U>&&>, void>>
    optional<_T>& operator=(optional<_U>&& __rhs)
    {
        if (__rhs._M_armed)
        {
            if (__storage_t::_M_armed)
                *__storage_t::__ptr() = __XVI_STD_NS::move(*__rhs.__ptr());
            else
                new (__storage_t::__ptr()) _T(__XVI_STD_NS::move(*__rhs.__ptr()));
            __storage_t::_M_armed = true;
        }
        else
        {
            if (__storage_t::_M_armed)
                __storage_t::__ptr()->~_T();
            else
                {} // Do nothing.
            __storage_t::_M_armed = false;
        }

        return *this;
    }


    template <class... _Args>
    _T& emplace(_Args&&... __args)
    {
        reset();
        new (__storage_t::__ptr()) _T(__XVI_STD_NS::forward<_Args>(__args)...);
        __storage_t::_M_armed = true;
        return *this;
    }

    template <class _U, class... _Args>
    _T& emplace(initializer_list<_U> __il, _Args&&... __args)
    {
        reset();
        new (__storage_t::__ptr()) _T(__il, __XVI_STD_NS::forward<_Args>(__args)...);
        __storage_t::_M_armed = true;
        return *this;
    }


    void swap(optional& __rhs) noexcept(is_nothrow_move_constructible_v<_T> && is_nothrow_swappable_v<_T>)
    {
        if (__rhs._M_armed)
        {
            if (__storage_t::_M_armed)
            {
                swap(**this, *__rhs);
            }
            else
            {
                new (__storage_t::__ptr()) _T(__XVI_STD_NS::move(*__rhs.__ptr()));
                __rhs.reset();
            }
        }
        else
        {
            if (__storage_t::_M_armed)
            {
                new (__rhs.__ptr()) _T(__XVI_STD_NS::move(*__storage_t::__ptr()));
                reset();
            }
            else
            {
                // Do nothing.
            }
        }
    }


    constexpr const _T* operator->() const
    {
        return __storage_t::__ptr();
    }

    constexpr _T* operator->()
    {
        return __storage_t::__ptr();
    }

    constexpr const _T& operator*() const &
    {
        return *__storage_t::__ptr();
    }

    constexpr _T& operator*() &
    {
        return *__storage_t::__ptr();
    }

    constexpr const _T&& operator*() const &&
    {
        return __XVI_STD_NS::move(*__storage_t::__ptr());
    }

    constexpr _T&& operator*() &&
    {
        return __XVI_STD_NS::move(*__storage_t::__ptr());
    }

    constexpr explicit operator bool() const noexcept
    {
        return __storage_t::_M_armed;
    }

    constexpr bool has_value() const noexcept
    {
        return __storage_t::_M_armed;
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

    void reset() noexcept
    {
        if (__storage_t::_M_armed)
            __storage_t::__ptr()->~_T();
        __storage_t::_M_armed = false;
    }

private:

    template <class _U> friend class optional;
    using __storage_t = __detail::__optional_storage<_T>;

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

template <class _T>
constexpr bool operator==(const optional<_T>& __x, nullopt_t) noexcept
{
    return !__x;
}

template <class _T>
constexpr bool operator==(nullopt_t, const optional<_T>& __x) noexcept
{
    return !__x;
}

template <class _T>
constexpr bool operator<(const optional<_T>&, nullopt_t) noexcept
{
    return false;
}

template <class _T>
constexpr bool operator<(nullopt_t, const optional<_T>& __x) noexcept
{
    return bool(__x);
}

template <class _T>
constexpr bool operator>(const optional<_T>& __x, nullopt_t) noexcept
{
    return bool(__x);
}

template <class _T>
constexpr bool operator>(nullopt_t, const optional<_T>&) noexcept
{
    return false;
}

template <class _T>
constexpr bool operator<=(const optional<_T>& __x, nullopt_t) noexcept
{
    return !__x;
}

template <class _T>
constexpr bool operator<=(nullopt_t, const optional<_T>&) noexcept
{
    return true;
}

template <class _T>
constexpr bool operator>=(const optional<_T>&, nullopt_t) noexcept
{
    return true;
}

template <class _T>
constexpr bool operator>=(nullopt_t, const optional<_T>& __x) noexcept
{
    return !__x;
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


template <class _T, class = enable_if_t<is_move_constructible_v<_T> && is_swappable_v<_T>, void>>
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


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_OPTIONAL_H */
