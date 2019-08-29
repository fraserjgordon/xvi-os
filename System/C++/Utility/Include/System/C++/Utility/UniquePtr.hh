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

    constexpr unique_ptr() noexcept = default;

    constexpr unique_ptr(nullptr_t) noexcept
        : unique_ptr()
    {
    }

    template <class = enable_if_t<!is_pointer_v<deleter_type> && is_default_constructible_v<deleter_type>, void>>
    explicit unique_ptr(pointer __p) noexcept
        : _M_ptr(__p)
    {
    }

    template <class = enable_if_t<is_constructible_v<_D, const _D&>, void>>
    unique_ptr(pointer __p, const _D& __d) noexcept
        : _M_ptr(__p),
          _M_deleter(__d)
    {
    }

    template <class _DD = remove_reference_t<_D>&&, class = enable_if_t<is_constructible_v<_D, _DD>
                                  && !is_reference_v<_D>, void>>
    unique_ptr(pointer __p, type_identity<_DD> __d) noexcept
        : _M_ptr(__p),
          _M_deleter(__XVI_STD_NS::forward<decltype(__d)>(__d))
    {
    }

    template <class = void, class _DD = remove_reference_t<_D>&&, class = enable_if_t<is_constructible_v<_D, _DD>
                                  && is_reference_v<_D>, void>>
    unique_ptr(pointer, type_identity<_DD>) = delete;

    unique_ptr(unique_ptr&& __u) noexcept
        : _M_ptr(__u._M_ptr),
          _M_deleter(__XVI_STD_NS::forward<_D>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    template <class _U, class _E,
              class = enable_if_t<is_convertible_v<pointer, typename unique_ptr<_U, _E>::pointer>
                                  && !is_array_v<_U>
                                  && ((is_reference_v<_D> && is_same_v<_D, _E>)
                                      || (!is_reference_v<_D> && is_convertible_v<_D, _E>)), void>>
    unique_ptr(unique_ptr<_U, _E>&& __u) noexcept
        : _M_ptr(__u._M_ptr),
          _M_deleter(__XVI_STD_NS::forward<_E>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    ~unique_ptr()
    {
        if (_M_ptr != nullptr)
            get_deleter()(get());
    }

    unique_ptr& operator=(unique_ptr&& __u) noexcept
    {
        reset(__u.release());
        get_deleter() = __XVI_STD_NS::forward<_D>(__u.get_deleter());
        return *this;
    }

    template <class _U, class _E,
              class = enable_if_t<is_convertible_v<pointer, typename unique_ptr<_U, _E>::pointer>
                                  && !is_array_v<_U>
                                  && is_assignable_v<_D&, _E&&>, void>>
    unique_ptr& operator=(unique_ptr<_U, _E>&& __u) noexcept
    {
        reset(__u.release());
        get_deleter() = __XVI_STD_NS::forward<_E>(__u.get_deleter());
        return *this;
    }

    unique_ptr& operator=(nullptr_t) noexcept
    {
        reset();
    }

    add_lvalue_reference_t<_T> operator*() const
    {
        return *get();
    }

    pointer operator->() const noexcept
    {
        return get();
    }

    pointer get() const noexcept
    {
        return _M_ptr;
    }

    deleter_type& get_deleter() noexcept
    {
        return _M_deleter;
    }

    const deleter_type& get_deleter() const noexcept
    {
        return _M_deleter;
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    pointer release() noexcept
    {
        pointer __p = _M_ptr;
        _M_ptr = nullptr;
        return __p;
    }

    void reset(pointer __p = pointer()) noexcept
    {
        pointer __old_p = _M_ptr;
        _M_ptr = __p;

        if (__old_p != nullptr)
            get_deleter()(__old_p);
    }

    void swap(unique_ptr& __u) noexcept
    {
        using __XVI_STD_NS::swap;
        swap(_M_ptr, __u._M_ptr);
        swap(_M_deleter, __u._M_deleter);
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

private:

    pointer _M_ptr = nullptr;
    [[no_unique_address]] deleter_type _M_deleter = {};
};


template <class _T, class _D>
class unique_ptr<_T[], _D>
{
    using pointer = __detail::__unique_ptr_pointer_type<_T, _D>;
    using element_type = _T;
    using deleter_type = _D;

    constexpr unique_ptr() noexcept = default;

    constexpr unique_ptr(nullptr_t) noexcept
        : unique_ptr()
    {
    }

    template <class _U,
              class = enable_if_t<!is_pointer_v<deleter_type>
                                  && is_default_constructible_v<deleter_type>
                                  && (is_same_v<_U, pointer>
                                      || is_same_v<_U, nullptr_t>
                                      || (is_same_v<pointer, element_type*>
                                          && is_pointer_v<_U>
                                          && is_convertible_v<element_type(*)[], remove_pointer_t<_U>(*)[]>)), void>>
    explicit unique_ptr(_U __p) noexcept
        : _M_ptr(__p)
    {
    }

    template <class _U,
              class = enable_if_t<is_constructible_v<_D, const _D&>
                                  && (is_same_v<_U, pointer>
                                      || is_same_v<_U, nullptr_t>
                                      || (is_same_v<pointer, element_type*>
                                          && is_pointer_v<_U>
                                          && is_convertible_v<element_type(*)[], remove_pointer_t<_U>(*)[]>)), void>>
    unique_ptr(_U __p, const _D& __d) noexcept
        : _M_ptr(__p),
          _M_deleter(__d)
    {
    }

    template <class _U, class _DD = remove_reference_t<_D>&&,
              class = enable_if_t<is_constructible_v<_D, _DD>
                                  && !is_reference_v<_D>
                                  && (is_same_v<_U, pointer>
                                      || is_same_v<_U, nullptr_t>
                                      || (is_same_v<pointer, element_type*>
                                          && is_pointer_v<_U>
                                          && is_convertible_v<element_type(*)[], remove_pointer_t<_U>(*)[]>)), void>>
    unique_ptr(_U __p, type_identity<_DD> __d) noexcept
        : _M_ptr(__p),
          _M_deleter(__XVI_STD_NS::forward<decltype(__d)>(__d))
    {
    }

    template <class _U, class = void, class _DD = remove_reference_t<_D>&&,
              class = enable_if_t<is_constructible_v<_D, _DD>
                                  && is_reference_v<_D>
                                  && (is_same_v<_U, pointer>
                                      || is_same_v<_U, nullptr_t>
                                      || (is_same_v<pointer, element_type*>
                                          && is_pointer_v<_U>
                                          && is_convertible_v<element_type(*)[], remove_pointer_t<_U>(*)[]>)), void>>
    unique_ptr(_U, type_identity<_DD>) = delete;

    unique_ptr(unique_ptr&& __u) noexcept
        : _M_ptr(__u._M_ptr),
          _M_deleter(__XVI_STD_NS::forward<_D>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    template <class _U, class _E,
              class = enable_if_t<is_array_v<_U>
                                  && is_same_v<pointer, element_type*>
                                  && is_same_v<typename unique_ptr<_U, _E>::pointer, typename unique_ptr<_U, _E>::element_type*>
                                  && is_convertible_v<element_type(*)[], typename unique_ptr<_U, _E>::element_type(*)[]>
                                  && ((is_reference_v<_D> && is_same_v<_D, _E>
                                      || !is_reference_v<_D> && is_convertible_v<_D, _E>)), void>>
    unique_ptr(unique_ptr<_U, _E>&& __u) noexcept
        : _M_ptr(__u._M_ptr),
          _M_deleter(__XVI_STD_NS::forward<_E>(__u._M_deleter))
    {
        __u._M_ptr = nullptr;
    }

    ~unique_ptr()
    {
        if (_M_ptr != nullptr)
            get_deleter()(get());
    }

    unique_ptr& operator=(unique_ptr&& __u) noexcept
    {
        reset(__u.release());
        get_deleter() = __XVI_STD_NS::forward<_D>(__u.get_deleter());
        return *this;
    }

    template <class _U, class _E,
              class = enable_if_t<is_array_v<_U>
                                  && is_same_v<pointer, element_type*>
                                  && is_same_v<typename unique_ptr<_U, _E>::pointer, typename unique_ptr<_U, _E>::element_type*>
                                  && is_convertible_v<element_type(*)[], typename unique_ptr<_U, _E>::element_type(*)[]>
                                  && is_assignable_v<_D&, _E&>, void>>
    unique_ptr& operator=(unique_ptr<_U, _E>&& __u) noexcept
    {
        reset(__u.release());
        get_deleter() = __XVI_STD_NS::forward<_E>(__u.get_deleter());
        return *this;
    }

    unique_ptr& operator=(nullptr_t) noexcept
    {
        reset();
    }

    _T& operator[](size_t __i) const
    {
        return get()[__i];
    }

    pointer get() const noexcept
    {
        return _M_ptr;
    }

    deleter_type& get_deleter() noexcept
    {
        return _M_deleter;
    }

    const deleter_type& get_deleter() const noexcept
    {
        return _M_deleter;
    }

    explicit operator bool() const noexcept
    {
        return get() != nullptr;
    }

    pointer release() noexcept
    {
        pointer __p = _M_ptr;
        _M_ptr = nullptr;
        return __p;
    }

    void reset(nullptr_t __p = pointer()) noexcept
    {
        reset(pointer());
    }

    template <class _U,
              class = enable_if_t<is_same_v<pointer, _U>
                                  || (is_same_v<pointer, element_type*>
                                      && is_pointer_v<_U>
                                      && is_convertible_v<element_type(*)[], remove_pointer_t<_U>(*)[]>), void>>
    void reset(_U __p) noexcept
    {
        pointer __old_p = _M_ptr;
        _M_ptr = __p;

        if (__old_p != nullptr)
            get_deleter()(__old_p);
    }

    void swap(unique_ptr& __u) noexcept
    {
        using __XVI_STD_NS::swap;
        swap(_M_ptr, __u._M_ptr);
        swap(_M_deleter, __u._M_deleter);
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

private:

    pointer _M_ptr = nullptr;
    [[no_unique_address]] deleter_type _M_deleter = {};
};


template <class _T, class... _Args,
          class = enable_if_t<!is_array_v<_T>, void>>
unique_ptr<_T> make_unique(_Args&&... __args)
{
    return unique_ptr<_T>(new _T(__XVI_STD_NS::forward<_Args>(__args)...));
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
unique_ptr<_T> make_unique(size_t __n)
{
    return unique_ptr<_T>(new remove_extent_t<_T>[__n]());
}

template <class _T, class... _Args,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
void make_unique(_Args&&...) = delete;

template <class _T,
          class = enable_if_t<!is_array_v<_T>, void>>
unique_ptr<_T> make_unique_default_init()
{
    return unique_ptr<_T>(new _T);
}

template <class _T,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> == 0, void>>
unique_ptr<_T> make_unique_default_init(size_t __n)
{
    return unique_ptr<_T>(new _T[__n]);
}

template <class _T, class... _Args,
          class = enable_if_t<is_array_v<_T> && extent_v<_T> != 0, void>>
void make_unique_default_init(_Args&&...) = delete;


template <class _T, class _D>
void swap(unique_ptr<_T, _D>& __lhs, unique_ptr<_T, _D>& __rhs) noexcept
{
    __lhs.swap(__rhs);
}


template <class _T1, class _D1, class _T2, class _D2>
bool operator==(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) noexcept
{
    return __x.get() == __y.get();
}

template <class _T1, class _D1, class _T2, class _D2>
bool operator!=(const unique_ptr<_T1, _D1>& __x, const unique_ptr<_T2, _D2>& __y) noexcept
{
    return __x.get() != __y.get();
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

template <class _T, class _D>
bool operator==(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return !__x;
}

template <class _T, class _D>
bool operator==(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return !__x;
}

template <class _T, class _D>
bool operator!=(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return !!__x;
}

template <class _T, class _D>
bool operator!=(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return !!__x;
}

template <class _T, class _D>
bool operator<(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return less<typename unique_ptr<_T, _D>::pointer>()(__x.get(), nullptr);
}

template <class _T, class _D>
bool operator<(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return less<typename unique_ptr<_T, _D>::pointer>()(nullptr, __x.get());
}

template <class _T, class _D>
bool operator>(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return nullptr < __x;
}

template <class _T, class _D>
bool operator>(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return __x < nullptr;
}

template <class _T, class _D>
bool operator<=(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return !(nullptr < __x);
}

template <class _T, class _D>
bool operator<=(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return !(__x < nullptr);
}

template <class _T, class _D>
bool operator>=(const unique_ptr<_T, _D>& __x, nullptr_t) noexcept
{
    return !(__x < nullptr);
}

template <class _T, class _D>
bool operator>=(nullptr_t, class unique_ptr<_T, _D>& __x) noexcept
{
    return !(nullptr < __x);
}


//! @TODO: implement.
template <class _CharT, class _Traits, class _T, class _D>
basic_ostream<_CharT, _Traits>& operator<<(basic_ostream<_CharT, _Traits>& __os, const unique_ptr<_T, _D>& __p);


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_UNIQUEPTR_H */
