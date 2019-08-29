#pragma once
#ifndef __SYSTEM_CXX_UTILITY_ANY_H
#define __SYSTEM_CXX_UTILITY_ANY_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/TypeInfo.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/InPlace.hh>


namespace __XVI_STD_UTILITY_NS
{


class bad_any_cast
    : public exception
{
public:

    constexpr bad_any_cast() = default;
    const char* what() const noexcept override
        { return "bad any_cast"; }
};


namespace __detail
{

// Forward declarations.
template <class> class __any_utils;

inline constexpr size_t __any_obj_size = 16;
inline constexpr size_t __any_inline_size = __any_obj_size - sizeof(void*);

// Dummy type used to represent the empty state.
struct __empty_any {};

struct __any_storage_t
{
    union
    {
        byte    __raw[__any_inline_size];
        void*   __external;
    };
};

class __any_utils_base
{
public:

    virtual void __duplicate_utils_to(void*) const = 0;

    virtual void __copy_construct_to(__any_storage_t&, const __any_storage_t&) const = 0;
    virtual void __move_construct_to(__any_storage_t&, __any_storage_t&) const = 0;
    virtual void __destroy(__any_storage_t&) const = 0;
    virtual void __copy_assign_to(__any_storage_t&, const __any_storage_t&) const = 0;
    virtual void __move_assign_to(__any_storage_t&, __any_storage_t&) const = 0;
    virtual const type_info& __get_type() const = 0;
    virtual void* __ptr(__any_storage_t&) const = 0;
    virtual const void* __ptr(const __any_storage_t&) const = 0;

    ~__any_utils_base() = default;

    template <class _T, class... _Args>
    void __emplace(__any_storage_t& __s, _Args&&... __args) const
    {
        // Cast allows avoidance of virtual method calls.
        auto __this = static_cast<const __any_utils<_T>*>(this);
        
        __this->__prepare(__s);
        new (__this->__ptr(__s)) _T(__XVI_STD_NS::forward<_Args>(__args)...);
    }

protected:

    virtual void __prepare(__any_storage_t&) const;
    virtual void __cleanup(__any_storage_t&) const;
};

template <class _T, 
          bool _StoreExternally = (sizeof(_T) > __any_inline_size)
                                  || (alignof(_T) > alignof(void*))
                                  || !is_nothrow_move_constructible_v<_T>>
class __any_utils_ptr : public __any_utils_base
{
public:

    void* __ptr(__any_storage_t& __s) const final
    {
        return __s.__raw;
    }

    const void* __ptr(const __any_storage_t& __s) const final
    {
        return __s.__raw;
    }

protected:

    void __prepare(__any_storage_t&) const final
    {
    }

    void __cleanup(__any_storage_t&) const final
    {
    }
};

template <class _T>
class __any_utils_ptr<_T, true> : public __any_utils_base
{
public:

    void* __ptr(__any_storage_t& __s) const final
    {
        return __s.__external;
    }

    const void* __ptr(const __any_storage_t& __s) const final
    {
        return __s.__external;
    }

protected:

    void __prepare(__any_storage_t& __s) const final
    {
        __s.__external = operator new(sizeof(_T), align_val_t{alignof(_T)});
    }

    void __cleanup(__any_storage_t& __s) const final
    {
        operator delete(__s.__external, align_val_t{alignof(_T)});
    }
};

template <class _T>
class __any_utils final : public __any_utils_ptr<_T>
{
public:

    using _Super = __any_utils_ptr<_T>;
    using _Super::__cleanup;
    using _Super::__prepare;
    using _Super::__ptr;

    void __duplicate_utils_to(void* __to) const final
    {
        reinterpret_cast<__any_utils_base*>(__to)->~__any_utils_base();
        new (__to) __any_utils<_T>();
    }

    void __copy_construct_to(__any_storage_t& __to, const __any_storage_t& __from) const final
    {
        __prepare(__to);
        new (__ptr(__to)) _T(*reinterpret_cast<const _T*>(__ptr(__from)));
    }

    void __move_construct_to(__any_storage_t& __to, __any_storage_t& __from) const final
    {
        __prepare(__to);
        new (__ptr(__to)) _T(__XVI_STD_NS::move(*reinterpret_cast<_T*>(__ptr(__from))));
    }

    void __destroy(__any_storage_t& __s) const final
    {
        reinterpret_cast<_T*>(__ptr(__s))->~_T();
        __cleanup(__s);
    }

    void __copy_assign_to(__any_storage_t& __to, const __any_storage_t& __from) const final
    {
        *reinterpret_cast<_T*>(__ptr(__to)) = *reinterpret_cast<const _T*>(__ptr(__from));
    }

    void __move_assign_to(__any_storage_t& __to, __any_storage_t& __from) const final
    {
        *reinterpret_cast<_T*>(__ptr(__to)) = __XVI_STD_NS::move(*reinterpret_cast<_T*>(__ptr(__from)));
    }

    const type_info& __get_type() const final
    {
        if constexpr (is_same_v<_T, __empty_any>)
            return typeid(void);
        else
            return typeid(_T);
    }
};


#ifdef __XVI_CXX_UTILITY_NO_EXCEPTIONS
[[noreturn]] inline void __invalid_any_cast() noexcept
{
    terminate();
}
#else
inline void __invalid_any_cast()
{
    throw bad_any_cast();
}
#endif

} // namespace __detail


class any
{
public:

    constexpr any() noexcept
        : _M_utils_storage {},
          _M_data_storage {}
    {
        __construct_utils<__detail::__empty_any>();
    }

    any(const any& __rhs)
        : any()
    {
        __rhs.__get_utils()->__copy_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
    }

    any(any&& __rhs) noexcept
        : any()
    {
        __rhs.__get_utils()->__move_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
    }

    template <class _T,
              class _VT = decay_t<_T>,
              class = enable_if_t<!is_same_v<_VT, any>
                                  && !__detail::__is_in_place_type_specialization_v<_VT>
                                  && is_copy_constructible_v<_VT>, void>>
    any(_T&& __t)
        : any()
    {
        __detail::__any_utils<_VT> __utils;
        __utils.template __emplace<_VT>(_M_data_storage, __XVI_STD_NS::forward<_T>(__t));
        __construct_utils<_VT>();
    }

    template <class _T, class... _Args,
              class _VT = decay_t<_T>,
              class = enable_if_t<is_copy_constructible_v<_VT>
                                  && is_constructible_v<_VT, _Args...>, void>>
    explicit any(in_place_type_t<_T>, _Args&&... __args)
        : any()
    {
        __detail::__any_utils<_VT> __utils;
        __utils.template __emplace<_VT>(_M_data_storage, __XVI_STD_NS::forward<_Args>(__args)...);
        __construct_utils<_VT>();
    }

    template <class _T, class _U, class... _Args,
              class _VT = decay_t<_T>,
              class = enable_if_t<is_copy_constructible_v<_VT>
                                 && is_constructible_v<_VT, initializer_list<_U>&, _Args...>, void>>
    explicit any(in_place_type_t<_T>, initializer_list<_U> __il, _Args&&... __args)
        : any()
    {
        __detail::__any_utils<_T> __utils;
        __utils.template __emplace<_VT>(_M_data_storage, __il, __XVI_STD_NS::forward<_Args>(__args)...);
        __construct_utils<_VT>();
    }

    ~any()
    {
        reset();
    }

    any& operator=(const any& __rhs)
    {
        if (&__rhs == this)
            return *this;

        reset();
        __rhs.__get_utils()->__copy_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
        return *this;
    }

    any& operator=(any&& __rhs) noexcept
    {
        if (&__rhs == this)
            return *this;

        reset();
        __rhs.__get_utils()->__move_construct_to(_M_data_storage, __rhs._M_data_storage);
        __rhs.__get_utils()->__duplicate_utils_to(&_M_utils_storage);
        return *this;
    }

    template <class _T,
              class _VT = decay_t<_T>,
              class = enable_if_t<!is_same_v<_VT, any>
                                  && is_copy_constructible_v<_VT>, void>>
    any& operator=(_T&& __t)
    {
        return operator=(any(in_place_type<_VT>, __XVI_STD_NS::forward<_T>(__t)));
    }

    template <class _T, class... _Args,
              class _VT = decay_t<_T>,
              class = enable_if_t<is_copy_constructible_v<_VT>
                                  && is_constructible_v<_T, _Args...>, void>>
    _VT& emplace(_Args&&... __args)
    {
        reset();
        __detail::__any_utils<_VT> __utils;
        __utils.template __emplace<_VT>(_M_data_storage, __XVI_STD_NS::forward<_Args>(__args)...);
        __set_utils<_VT>(_M_utils_storage);
        return *reinterpret_cast<_VT*>(__get_utils()->__ptr(_M_data_storage));
    }

    template <class _T, class _U, class... _Args,
              class _VT = decay_t<_T>,
              class = enable_if_t<is_copy_constructible_v<_VT>
                                  && is_constructible_v<_T, initializer_list<_U>&, _Args...>, void>>
    decay_t<_T>& emplace(initializer_list<_U> __il, _Args&&... __args)
    {
        reset();
        __detail::__any_utils<_VT> __utils;
        __utils.template __emplace<_VT>(_M_data_storage, __il, __XVI_STD_NS::forward<_Args>(__args)...);
        __set_utils<_VT>(_M_utils_storage);
        return *reinterpret_cast<_VT*>(__get_utils()->__ptr(_M_data_storage));
    }

    void reset() noexcept
    {
        __get_utils()->__destroy(_M_data_storage);
        __set_utils<__detail::__empty_any>();
    }

    void swap(any& __rhs) noexcept
    {
        any __temp(__XVI_STD_NS::move(__rhs));
        __rhs = __XVI_STD_NS::move(*this);
        *this = __XVI_STD_NS::move(__temp);
    }

    bool has_value() const noexcept
    {
        return type() == typeid(void);
    }

    const type_info& type() const noexcept
    {
        return __get_utils()->__get_type();
    }

    template <class _T>
    _T* __ptr()
    {
        return reinterpret_cast<_T*>(__get_utils()->__ptr(_M_data_storage));
    }

    template <class _T>
    const _T* __ptr() const
    {
        return reinterpret_cast<const _T*>(__get_utils()->__ptr(_M_data_storage));
    }

private:

    using __utils_t = aligned_storage_t<sizeof(__detail::__any_utils<int>), alignof(__detail::__any_utils<int>)>;
    using __storage_t = __detail::__any_storage_t;
    
    __utils_t   _M_utils_storage;
    __storage_t _M_data_storage;

    const __detail::__any_utils_base* __get_utils() const
    {
        return reinterpret_cast<const __detail::__any_utils_base*>(&_M_utils_storage);
    }

    template <class _T> void __set_utils()
    {
        auto __utils = reinterpret_cast<__detail::__any_utils_base*>(&_M_utils_storage);
        __utils->~__any_utils_base();
        new (__utils) __detail::__any_utils<_T>();
    }

    template <class _T> constexpr void __construct_utils()
    {
        static_assert(sizeof(__detail::__any_utils<_T>) == sizeof(void*));

        new(&_M_utils_storage) __detail::__any_utils<_T>();
    }
};

static_assert(sizeof(any) == __detail::__any_obj_size);


void swap(any& __l, any& __r) noexcept
{
    __l.swap(__r);
}


template <class _T, class... _Args>
any make_any(_Args&&... __args)
{
    return any(in_place_type<_T>, __XVI_STD_NS::forward<_Args>(__args)...);
}

template <class _T, class _U, class... _Args>
any make_any(initializer_list<_U> __il, _Args&&... __args)
{
    return any(in_place_type<_T>, __il, __XVI_STD_NS::forward<_Args>(__args)...);
}


template <class _T>
_T any_cast(const any& __a)
{
    using _U = remove_cvref_t<_T>;
    if (__a.type() != typeid(remove_reference_t<_T>))
        __detail::__invalid_any_cast();

    return static_cast<_T>(*any_cast<_U>(&__a));
}

template <class _T>
_T any_cast(any& __a)
{
    using _U = remove_cvref_t<_T>;
    if (__a.type() != typeid(remove_reference_t<_T>))
        __detail::__invalid_any_cast();

    return static_cast<_T>(*any_cast<_U>(&__a));
}

template <class _T>
_T any_cast(any&& __a)
{
    using _U = remove_cvref_t<_T>;
    if (__a.type() != typeid(remove_reference_t<_T>))
        __detail::__invalid_any_cast();

    return static_cast<_T>(__XVI_STD_NS::move(*any_cast<_U>(&__a)));
}


template <class _T>
const _T* any_cast(const any* __a) noexcept
{
    if (__a == nullptr || __a->type() != typeid(_T))
        return nullptr;

    return __a->template __ptr<_T>();
}

template <class _T>
_T* any_cast(any* __a) noexcept
{
    if (__a == nullptr || __a->type() != typeid(_T))
        return nullptr;

    return __a->template __ptr<_T>();
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_ANY_H */
