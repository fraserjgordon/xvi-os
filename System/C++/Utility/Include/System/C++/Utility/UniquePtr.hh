#pragma once
#ifndef __SYSTEM_CXX_UTILITY_UNIQUEPTR_H
#define __SYSTEM_CXX_UTILITY_UNIQUEPTR_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/DefaultDelete.hh>
#include <System/C++/Utility/FunctionalUtils.hh>
#include <System/C++/Utility/IosFwd.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


namespace __detail
{

template <class _D>
using __remove_reference_pointer_detector = typename remove_reference_t<_D>::pointer;

template <class _T, class _D>
using __unique_ptr_pointer_type = detected_or_t<_T*, __remove_reference_pointer_detector, _D>;


} // namespace __detail


template <class _T, class _D = default_delete<_T>>
class unique_ptr
{
public:

    using pointer = __detail::__unique_ptr_pointer_type<_T, _D>;
    using element_type = _T;
    using deleter_type = _D;

    constexpr unique_ptr() noexcept
        requires (!std::is_pointer_v<_D> && std::is_default_constructible_v<_D>)
        = default;

    constexpr unique_ptr(nullptr_t) noexcept
        requires (!std::is_pointer_v<_D> && std::is_default_constructible_v<_D>)
        : unique_ptr()
    {
    }

    constexpr explicit unique_ptr(type_identity_t<pointer> __p) noexcept
        requires (!std::is_pointer_v<_D> && std::is_default_constructible_v<_D>)
        : _M_ptr(__p)
    {
    }

    constexpr unique_ptr(type_identity_t<pointer> __p, const _D& __d) noexcept
        requires std::is_constructible_v<_D, decltype(__d)>
        : _M_ptr(__p),
          _M_deleter(__d)
    {
    }

    constexpr unique_ptr(type_identity_t<pointer> __p, std::remove_reference_t<_D>&& __d) noexcept
        requires (std::is_constructible_v<_D, decltype(__d)> && !std::is_reference_v<_D>)
        : _M_ptr(__p),
          _M_deleter(std::forward<decltype(__d)>(__d))
    {
    }

    constexpr unique_ptr(type_identity_t<pointer>, std::remove_reference_t<_D>&&)
        requires std::is_reference_v<_D>
        = delete;

    constexpr unique_ptr(unique_ptr&& __u) noexcept
        requires std::is_move_constructible_v<_D>
        : _M_ptr(__u._M_ptr),
          _M_deleter(std::forward<_D>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    template <class _U, class _E>
        requires std::is_convertible_v<typename unique_ptr<_U, _E>::pointer, pointer>
            && (!std::is_array_v<_U>)
            && ((std::is_reference_v<_D> && std::is_same_v<_D, _E>) || (!std::is_reference_v<_D> && std::is_convertible_v<_E, _D>))
    constexpr unique_ptr(unique_ptr<_U, _E>&& __u) noexcept
        : _M_ptr(__u._M_ptr),
          _M_deleter(std::forward<_E>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    constexpr ~unique_ptr()
    {
        if (get())
            get_deleter()(get());
    }

    constexpr unique_ptr& operator=(unique_ptr&& __u) noexcept
        requires std::is_move_assignable_v<_D>
    {
        reset(__u.release());
        get_deleter() = __XVI_STD_NS::forward<_D>(__u.get_deleter());
        return *this;
    }

    template <class _U, class _E>
        requires std::is_convertible_v<typename unique_ptr<_U, _E>::pointer, pointer>
            && (!std::is_array_v<_U>)
            && std::is_assignable_v<_D&, _E&&>
    constexpr unique_ptr& operator=(unique_ptr<_U, _E>&& __u) noexcept
    {
        reset(__u.release());
        get_deleter() = std::forward<_E>(__u.get_deleter());
        return *this;
    }

    constexpr unique_ptr& operator=(nullptr_t) noexcept
    {
        reset();
    }

    constexpr add_lvalue_reference_t<_T> operator*() const noexcept(noexcept(*declval<pointer>()))
    {
        return *get();
    }

    constexpr pointer operator->() const noexcept
    {
        return get();
    }

    constexpr pointer get() const noexcept
    {
        return _M_ptr;
    }

    constexpr deleter_type& get_deleter() noexcept
    {
        return _M_deleter;
    }

    constexpr const deleter_type& get_deleter() const noexcept
    {
        return _M_deleter;
    }

    constexpr explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    constexpr pointer release() noexcept
    {
        pointer __p = _M_ptr;
        _M_ptr = nullptr;
        return __p;
    }

    constexpr void reset(pointer __p = pointer()) noexcept
    {
        pointer __old_p = _M_ptr;
        _M_ptr = __p;

        if (__old_p != nullptr)
            get_deleter()(__old_p);
    }

    constexpr void swap(unique_ptr& __u) noexcept
    {
        using std::swap;
        swap(_M_ptr, __u._M_ptr);
        swap(_M_deleter, __u._M_deleter);
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

private:

    pointer _M_ptr = {};
    [[no_unique_address]] deleter_type _M_deleter = {};
};


template <class _T, class _D>
class unique_ptr<_T[], _D>
{
public:

    using pointer = __detail::__unique_ptr_pointer_type<_T, _D>;
    using element_type = _T;
    using deleter_type = _D;

    constexpr unique_ptr() noexcept = default;

    constexpr unique_ptr(nullptr_t) noexcept
        : unique_ptr()
    {
    }

    template <class _U>
        requires std::is_default_constructible_v<_D>
            && (std::is_same_v<_U, pointer>
                || (std::is_same_v<pointer, element_type*>
                    && std::is_pointer_v<_U>
                    && std::is_convertible_v<std::remove_pointer_t<_U>(*)[], element_type(*)[]>))
    constexpr explicit unique_ptr(_U __p) noexcept
        : _M_ptr(__p)
    {
    }

    template <class _U>
        requires is_constructible_v<_D, const _D&>
            && (std::is_same_v<_U, pointer>
                || std::is_same_v<_U, std::nullptr_t>
                || (std::is_same_v<pointer, element_type*>
                    && std::is_pointer_v<_U>
                    && std::is_convertible_v<std::remove_pointer_t<_U>(*)[], element_type(*)[]>))
    constexpr unique_ptr(_U __p, const _D& __d) noexcept
        : _M_ptr(__p),
          _M_deleter(__d)
    {
    }

    template <class _U>
        requires is_constructible_v<_D, std::remove_reference_t<_D>&&>
            && (!std::is_reference_v<_D>)
            && (std::is_same_v<_U, pointer>
                || std::is_same_v<_U, std::nullptr_t>
                || (std::is_same_v<pointer, element_type*>
                    && std::is_pointer_v<_U>
                    && std::is_convertible_v<std::remove_pointer_t<_U>(*)[], element_type(*)[]>))
    constexpr unique_ptr(_U __p, std::remove_reference_t<_D>&& __d) noexcept
        : _M_ptr(__p),
          _M_deleter(std::forward<decltype(__d)>(__d))
    {
    }

    template <class _U>
        requires std::is_reference_v<_D>
    unique_ptr(_U, std::remove_reference_t<_D>&&) = delete;

    constexpr unique_ptr(unique_ptr&& __u) noexcept
        requires std::is_move_constructible_v<_D>
        : _M_ptr(__u._M_ptr),
          _M_deleter(std::forward<_D>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    template <class _U, class _E>
        requires std::is_array_v<_U>
            && std::is_same_v<pointer, element_type*>
            && std::is_same_v<typename unique_ptr<_U, _E>::pointer, typename unique_ptr<_U, _E>::element_type*>
            && std::is_convertible_v<typename unique_ptr<_U, _E>::element_type(*)[], element_type(*)[]>
            && ((std::is_reference_v<_D> && std::is_same_v<_D, _E>) || (!std::is_reference_v<_D> && std::is_convertible_v<_E, _D>))
    constexpr unique_ptr(unique_ptr<_U, _E>&& __u) noexcept
        : _M_ptr(__u._M_ptr),
          _M_deleter(std::forward<_E>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    constexpr ~unique_ptr()
    {
        if (_M_ptr != nullptr)
            get_deleter()(get());
    }

    constexpr unique_ptr& operator=(unique_ptr&& __u) noexcept
        requires std::is_move_assignable_v<_D>
    {
        reset(__u.release());
        get_deleter() = std::forward<_D>(__u.get_deleter());
        return *this;
    }

    template <class _U, class _E>
        requires std::is_array_v<_U>
            && std::is_same_v<pointer, element_type*>
            && std::is_same_v<typename unique_ptr<_U, _E>::pointer, typename unique_ptr<_U, _E>::element_type*>
            && std::is_convertible_v<typename unique_ptr<_U, _E>::element_type(*)[], element_type(*)[]>
            && std::is_assignable_v<_D&, _E&&>
    constexpr unique_ptr& operator=(unique_ptr<_U, _E>&& __u) noexcept
    {
        reset(__u.release());
        get_deleter() = std::forward<_E>(__u.get_deleter());
        return *this;
    }

    constexpr unique_ptr& operator=(nullptr_t) noexcept
    {
        reset();
    }

    constexpr _T& operator[](size_t __i) const
    {
        return get()[__i];
    }

    constexpr pointer get() const noexcept
    {
        return _M_ptr;
    }

    constexpr deleter_type& get_deleter() noexcept
    {
        return _M_deleter;
    }

    constexpr const deleter_type& get_deleter() const noexcept
    {
        return _M_deleter;
    }

    constexpr explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    constexpr pointer release() noexcept
    {
        pointer __p = _M_ptr;
        _M_ptr = nullptr;
        return __p;
    }

    constexpr void reset(std::nullptr_t = nullptr) noexcept
    {
        reset(pointer());
    }

    template <class _U>
        requires std::is_same_v<_U, pointer>
            || (std::is_same_v<pointer, element_type*>
                && std::is_pointer_v<_U>
                && std::is_convertible_v<std::remove_pointer_t<_U>(*)[], element_type(*)[]>)
    constexpr void reset(_U __p) noexcept
    {
        pointer __old_p = _M_ptr;
        _M_ptr = __p;

        if (__old_p != nullptr)
            get_deleter()(__old_p);
    }

    constexpr void swap(unique_ptr& __u) noexcept
    {
        using std::swap;
        swap(_M_ptr, __u._M_ptr);
        swap(_M_deleter, __u._M_deleter);
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

private:

    pointer _M_ptr = nullptr;
    [[no_unique_address]] deleter_type _M_deleter = {};
};


template <class _T, class... _Args>
    requires (!std::is_array_v<_T>)
constexpr unique_ptr<_T> make_unique(_Args&&... __args)
{
    return unique_ptr<_T>(new _T(std::forward<_Args>(__args)...));
}

template <class _T>
    requires std::is_array_v<_T> && (std::extent_v<_T> == 0)
constexpr unique_ptr<_T> make_unique(size_t __n)
{
    return unique_ptr<_T>(new remove_extent_t<_T>[__n]());
}

template <class _T, class... _Args>
    requires std::is_array_v<_T> && (std::extent_v<_T> != 0)
void make_unique(_Args&&...) = delete;


template <class _T>
    requires (!std::is_array_v<_T>)
constexpr unique_ptr<_T> make_unique_for_overwrite()
{
    return unique_ptr<_T>(new _T);
}

template <class _T>
    requires std::is_array_v<_T> && (std::extent_v<_T> == 0)
constexpr unique_ptr<_T> make_unique_for_overwrite(std::size_t __n)
{
    return unique_ptr<_T>(new std::remove_extent_t<_T>[__n]);
}

template <class _T, class... _Args>
    requires std::is_array_v<_T> && (std::extent_v<_T> != 0)
void make_unique_for_overwrite(_Args&&...) = delete;


template <class _T, class _D>
    requires std::is_swappable_v<_D>
constexpr void swap(unique_ptr<_T, _D>& __lhs, unique_ptr<_T, _D>& __rhs) noexcept
{
    __lhs.swap(__rhs);
}


template <class _T1, class _D1, class _T2, class _D2>
constexpr bool operator==(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) noexcept
{
    return __x.get() == __y.get();
}

template <class _T1, class _D1, class _T2, class _D2>
bool operator<(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) noexcept
{
    using _CT = common_type_t<typename unique_ptr<_T1, _D1>::pointer, typename unique_ptr<_T2, _D2>::pointer>;

    return less<_CT>()(__x.get(), __y.get());
}

template <class _T1, class _D1, class _T2, class _D2>
bool operator>(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) noexcept
{
    return __y < __x;
}

template <class _T1, class _D1, class _T2, class _D2>
bool operator<=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) noexcept
{
    return !(__y < __x);
}

template <class _T1, class _D1, class _T2, class _D2>
bool operator>=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) noexcept
{
    return !(__x < __y);
}

template <class _T1, class _D1, class _T2, class _D2>
    requires std::three_way_comparable_with<typename unique_ptr<_T1, _D1>::pointer, typename unique_ptr<_T2, _D2>::pointer>
std::compare_three_way_result_t<typename unique_ptr<_T1, _D1>::pointer, typename unique_ptr<_T2, _D2>::pointer>
operator<=>(const unique_ptr<_T1, _D1>& __rhs, const unique_ptr<_T2, _D2>& __lhs) noexcept
{
    return compare_three_way()(__rhs.get(), __lhs.get());
}

template <class _T, class _D>
constexpr bool operator==(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return !__x;
}

template <class _T, class _D>
constexpr bool operator<(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return less<typename unique_ptr<_T, _D>::pointer>()(__x.get(), nullptr);
}

template <class _T, class _D>
constexpr bool operator<(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return less<typename unique_ptr<_T, _D>::pointer>()(nullptr, __x.get());
}

template <class _T, class _D>
constexpr bool operator>(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return nullptr < __x;
}

template <class _T, class _D>
constexpr bool operator>(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return __x < nullptr;
}

template <class _T, class _D>
constexpr bool operator<=(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return !(nullptr < __x);
}

template <class _T, class _D>
constexpr bool operator<=(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return !(__x < nullptr);
}

template <class _T, class _D>
constexpr bool operator>=(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return !(__x < nullptr);
}

template <class _T, class _D>
constexpr bool operator>=(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return !(nullptr < __x);
}

template <class _T, class _D>
    requires std::three_way_comparable<typename unique_ptr<_T, _D>::pointer>
constexpr std::compare_three_way_result_t<typename unique_ptr<_T, _D>::pointer>
operator<=>(const unique_ptr<_T, _D>& __x, std::nullptr_t)
{
    return compare_three_way()(__x.get(), static_cast<typename unique_ptr<_T, _D>::pointer>(nullptr));
}


//! @TODO: implement.
template <class _CharT, class _Traits, class _T, class _D>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const unique_ptr<_T, _D>& __p);


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_UNIQUEPTR_H */
