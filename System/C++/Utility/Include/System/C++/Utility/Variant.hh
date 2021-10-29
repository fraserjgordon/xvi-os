#pragma once
#ifndef __SYSTEM_CXX_UTILITY_VARIANT_H
#define __SYSTEM_CXX_UTILITY_VARIANT_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/IntegerSequence.hh>
#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/Private/InPlace.hh>
#include <System/C++/Utility/Private/TupleTraits.hh>


namespace __XVI_STD_UTILITY_NS
{


inline constexpr size_t variant_npos = static_cast<size_t>(-1);


namespace __detail
{

template <class... _Types> union __variant_storage_union_base;

template <class _T, class... _Types>
union __variant_storage_union_base<_T, _Types...>
{
    using __value_t = _T;
    using __inner_t = __variant_storage_union_base<_Types...>;
    
    __inner_t __inner;
    __value_t __val;
};

template <class _T>
union __variant_storage_union_base<_T>
{
    using __value_t = _T;
    
    __value_t __val;
};

template <>
union __variant_storage_union_base<>
{

};

template <size_t _I, class... _Types>
constexpr auto& __get(__variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (_I == 0)
        return __u.__val;
    else
        return __get<_I - 1>(__u.__inner);
}

template <size_t _I, class... _Types>
constexpr const auto& __get(const __variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (_I == 0)
        return __u.__val;
    else
        return __get<_I - 1>(__u.__inner);
}

template <class _T, class... _Types>
constexpr _T& __get(__variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (is_same_v<_T, typename __variant_storage_union_base<_Types...>::__value_t>)
        return __u.__val;
    else
        return __get<_T>(__u.__inner);
}

template <class _T, class... _Types>
constexpr const _T& __get(const __variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (is_same_v<_T, typename __variant_storage_union_base<_Types...>::__value_t>)
        return __u.__val;
    else
        return __get<_T>(__u.__inner);
}


template <size_t _I, class... _Types>
constexpr auto __ptr(__variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (_I == 0)
        return __XVI_STD_NS::addressof(__u.__val);
    else
        return __ptr<_I - 1>(__u.__inner);
}

template <size_t _I, class... _Types>
constexpr auto __ptr(const __variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (_I == 0)
        return __XVI_STD_NS::addressof(__u.__val);
    else
        return __ptr<_I - 1>(__u.__inner);
}

template <class _T, class... _Types>
constexpr _T* __ptr(__variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (is_same_v<_T, typename __variant_storage_union_base<_Types...>::__value_t>)
        return __XVI_STD_NS::addressof(__u.__val);
    else
        return __ptr<_T>(__u.__inner);
}

template <class _T, class... _Types>
constexpr const _T* __ptr(const __variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (is_same_v<_T, typename __variant_storage_union_base<_Types...>::__value_t>)
        return __XVI_STD_NS::addressof(__u.__val);
    else
        return __ptr<_T>(__u.__inner);
}

template <class _T, class... _Types> struct __index_of;
template <class _T, class... _Types> struct __index_of<_T, _T, _Types...> : integral_constant<size_t, 0> {};
template <class _T, class _U, class... _Types> struct __index_of<_T, _U, _Types...> : integral_constant<size_t, __index_of<_T, _Types...>::value + 1> {};
template <class _T, class... _Types> inline constexpr size_t __index_of_v = __index_of<_T, _Types...>::value;


template <class _T, class... _Types>
constexpr void __copy_construct(__variant_storage_union_base<_T, _Types...>& __dest,
                                const __variant_storage_union_base<_T, _Types...>& __src,
                                size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __copy_construct(__dest.__inner, __src.__inner, __index - 1);
        return;
    }

    new (__ptr<0>(__dest)) _T(__get<0>(__src));
}

template <class _T, class... _Types>
constexpr void __move_construct(__variant_storage_union_base<_T, _Types...>& __dest,
                                __variant_storage_union_base<_T, _Types...>& __src,
                                size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __move_construct(__dest.__inner, __src.__inner, __index - 1);
        return;
    }

    new (__ptr<0>(__dest)) _T(__XVI_STD_NS::move(__get<0>(__src)));
}

template <class _T, class... _Types>
constexpr void __destruct(__variant_storage_union_base<_T, _Types...>& __v,
                          size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __destruct(__v.__inner, __index - 1);
        return;
    }

    __get<0>(__v).~_T();
}

template <class _T, class... _Types>
constexpr void __copy_assign(__variant_storage_union_base<_T, _Types...>& __dest,
                             const __variant_storage_union_base<_T, _Types...>& __src,
                             size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __copy_assign(__dest.__inner, __src.__inner, __index - 1);
        return;
    }

    __get<0>(__dest)  = __get<0>(__src);
}

template <class _T, class... _Types>
constexpr void __move_assign(__variant_storage_union_base<_T, _Types...>& __dest,
                             __variant_storage_union_base<_T, _Types...>& __src,
                             size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __move_assign(__dest.__inner, __src.__inner, __index - 1);
        return;
    }

    __get<0>(__dest) = __XVI_STD_NS::move(__get<0>(__src));
}

template <class _T, class... _Types, class... _Args>
constexpr void __emplace(__variant_storage_union_base<_T, _Types...>& __dest,
                         size_t __index,
                         _Args&&... __args)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __emplace(__dest.__inner, __index - 1, __XVI_STD_NS::forward<_Args>(__args)...);
        return;
    }

    new (__ptr<0>(__dest)) _T(__XVI_STD_NS::forward<_Args>(__args)...);
}

template <class _T, class... _Types>
constexpr void __swap_elem(__variant_storage_union_base<_T, _Types...>& __lhs,
                           __variant_storage_union_base<_T, _Types...>& __rhs,
                           size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __swap_elem(__lhs.__inner, __rhs.__inner, __index - 1);
        return;
    }

    swap(__get<0>(__lhs), __get<0>(__rhs));
}

template <class _T, class... _Types>
constexpr bool __variant_eq(const __variant_storage_union_base<_T, _Types...>& __lhs,
                    const __variant_storage_union_base<_T, _Types...>& __rhs,
                    size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            return __variant_eq(__lhs.__inner, __rhs.__inner, __index - 1);
        return true;
    }

    return __get<0>(__lhs) == __get<0>(__rhs);
}

template <class _T, class... _Types>
constexpr bool __neq(const __variant_storage_union_base<_T, _Types...>& __lhs,
                     const __variant_storage_union_base<_T, _Types...>& __rhs,
                     size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            return __neq(__lhs.__inner, __rhs.__inner, __index - 1);
        return false;
    }

    return __get<0>(__lhs) != __get<0>(__rhs);
}

template <class _T, class... _Types>
constexpr bool __lt(const __variant_storage_union_base<_T, _Types...>& __lhs,
                    const __variant_storage_union_base<_T, _Types...>& __rhs,
                    size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            return __lt(__lhs.__inner, __rhs.__inner, __index - 1);
        return false;
    }

    return __get<0>(__lhs) < __get<0>(__rhs);
}

template <class _T, class... _Types>
constexpr bool __gt(const __variant_storage_union_base<_T, _Types...>& __lhs,
                    const __variant_storage_union_base<_T, _Types...>& __rhs,
                    size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            return __gt(__lhs.__inner, __rhs.__inner, __index - 1);
        return false;
    }

    return __get<0>(__lhs) > __get<0>(__rhs);
}

template <class _T, class... _Types>
constexpr bool __le(const __variant_storage_union_base<_T, _Types...>& __lhs,
                    const __variant_storage_union_base<_T, _Types...>& __rhs,
                    size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            return __le(__lhs.__inner, __rhs.__inner, __index - 1);
        return true;
    }

    return __get<0>(__lhs) <= __get<0>(__rhs);
}

template <class _T, class... _Types>
constexpr bool __ge(const __variant_storage_union_base<_T, _Types...>& __lhs,
                    const __variant_storage_union_base<_T, _Types...>& __rhs,
                    size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            return __ge(__lhs.__inner, __rhs.__inner, __index - 1);
        return true;
    }

    return __get<0>(__lhs) >= __get<0>(__rhs);
}


template <class... _Types>
union __variant_storage_union
{
    using __storage_t = aligned_union_t<0, _Types...>;
    using __inner_t = __variant_storage_union_base<_Types...>;
    
    __storage_t __raw;
    __inner_t __inner;

    // Note: does not initialise as any of the alternatives, only as raw storage.
    constexpr __variant_storage_union() : __raw() {}

#ifndef __llvm__
    //! @todo clang bug? - doesn't seem to handle the requires clause properly.
    ~__variant_storage_union() = default;
#endif

    // Non-trivial but empty destructor for unions with non-trivial members.
    ~__variant_storage_union()
#ifndef __llvm__
        requires (!(is_trivially_destructible_v<_Types> && ...))
#endif
    {
    }
};


template <class... _Types>
struct __variant_storage_base
{
    using __union_t = __variant_storage_union<_Types...>;

    __union_t _M_storage;
    size_t    _M_index = 0;

    constexpr __variant_storage_base() = default;
    constexpr __variant_storage_base(const __variant_storage_base&) = default;
    constexpr __variant_storage_base(__variant_storage_base&&) = default;

    constexpr ~__variant_storage_base() = default;

    constexpr __variant_storage_base& operator=(const __variant_storage_base&) = default;
    constexpr __variant_storage_base& operator=(__variant_storage_base&&) = default;

    template <size_t _I> constexpr auto& __get() noexcept
        { return __detail::__get<_I>(_M_storage.__inner); }
    template <size_t _I> constexpr const auto& __get() const noexcept
        { return __detail::__get<_I>(_M_storage.__inner); }

    template <class _T> constexpr _T& __get() noexcept
        { return __detail::__get<_T>(_M_storage.__inner); }
    template <class _T> constexpr const _T& __get() const noexcept
        { return __detail::__get<_T>(_M_storage.__inner); }

    template <size_t _I> constexpr auto __ptr() noexcept
        { return __detail::__ptr<_I>(_M_storage.__inner); }
    template <size_t _I> constexpr auto __ptr() const noexcept
        { return __detail::__ptr<_I>(_M_storage.__inner); }

    template <class _T> constexpr _T* __ptr() noexcept
        { return __detail::__ptr<_T>(_M_storage.__inner); }
    template <class _T> constexpr const _T* __ptr() const noexcept
        { return __detail::__ptr<_T>(_M_storage.__inner); }

    constexpr bool operator==(const __variant_storage_base& __v) const noexcept
    {
        if (_M_index != __v._M_index)
            return false;

        if (_M_index == variant_npos)
            return true;
        
        if (true || is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __variant_eq(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __eq_fns[_M_index](*this, __v);
    }

    constexpr bool operator!=(const __variant_storage_base& __v) const noexcept
    {
        if (_M_index != __v._M_index)
            return true;

        if (_M_index == variant_npos)
            return false;
        
        if (true || is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __neq(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __neq_fns[_M_index](*this, __v);
    }

    constexpr bool operator<(const __variant_storage_base& __v) const noexcept
    {
        if (__v._M_index == variant_npos)
            return false;

        if (_M_index == variant_npos)
            return true;

        if (_M_index < __v._M_index)
            return true;

        if (_M_index > __v._M_index)
            return false;
        
        if (true || is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __lt(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __lt_fns[_M_index](*this, __v);
    }

    constexpr bool operator>(const __variant_storage_base& __v) const noexcept
    {
        if (_M_index == variant_npos)
            return false;

        if (__v._M_index == variant_npos)
            return true;

        if (_M_index > __v._M_index)
            return true;

        if (_M_index < __v._M_index)
            return false;
        
        if (true || is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __gt(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __gt_fns[_M_index](*this, __v);
    }

    constexpr bool operator<=(const __variant_storage_base& __v) const noexcept
    {
        if (_M_index == variant_npos)
            return true;

        if (__v._M_index == variant_npos)
            return false;

        if (_M_index < __v._M_index)
            return true;

        if (_M_index > __v._M_index)
            return false;
        
        if (true || is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __le(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __le_fns[_M_index](*this, __v);
    }

    constexpr bool operator>=(const __variant_storage_base& __v) const noexcept
    {
        if (__v._M_index == variant_npos)
            return true;

        if (_M_index == variant_npos)
            return false;

        if (_M_index > __v._M_index)
            return true;

        if (_M_index < __v._M_index)
            return false;
        
        if (true || is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __ge(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __ge_fns[_M_index](*this, __v);
    }

    // Copy constructor methods.
    template <class _T> static constexpr void __copy_ctor(__variant_storage_base& __to, const __variant_storage_base& __from)
        { new (__to.template __ptr<_T>()) _T(__from.template __get<_T>()); }
    static inline constexpr void (*__copy_ctors[])(__variant_storage_base&, const __variant_storage_base&) =
    {
        &__copy_ctor<_Types>...
    };

    // Move constructor methods.
    template <class _T> static constexpr void __move_ctor(__variant_storage_base& __to, __variant_storage_base& __from)
        { new (__to.template __ptr<_T>()) _T(__XVI_STD_NS::move(__from.template __get<_T>())); }
    static inline constexpr void (*__move_ctors[])(__variant_storage_base&, __variant_storage_base&) =
    {
        &__move_ctor<_Types>...
    };

    // Destructor methods.
    template <class _T> static constexpr void __dtor(__variant_storage_base& __v)
        { __v.template __get<_T>().~_T(); }
    static inline constexpr void (*__dtors[])(__variant_storage_base&) =
    {
        &__dtor<_Types>...
    };

    // Copy assignment methods.
    template <class _T> static constexpr void __copy_assign(__variant_storage_base& __to, const __variant_storage_base& __from)
        { __to.template __get<_T>() = __from.template __get<_T>(); }
    static inline constexpr void (*__copy_assigns[])(__variant_storage_base&, const __variant_storage_base&) =
    {
        &__copy_assign<_Types>...
    };

    // Move assignment methods.
    template <class _T> static void __move_assign(__variant_storage_base& __to, __variant_storage_base& __from)
        { __to.template __get<_T>() = __XVI_STD_NS::move(__from.template __get<_T>()); }
    static inline constexpr void (*__move_assigns[])(__variant_storage_base&, __variant_storage_base&) =
    {
        &__move_assign<_Types>...
    };

    // Swap methods.
    template <class _T> static void __swap_fn(__variant_storage_base& __l, __variant_storage_base& __r)
        { swap(__l.template __get<_T>(), __r.template __get<_T>()); }
    static inline constexpr void (*__swap_fns[])(__variant_storage_base&, __variant_storage_base&)
    {
        &__swap_fn<_Types>...
    };

    // Equality methods.
    template <class _T> static bool __eq_fn(const __variant_storage_base& __l, const __variant_storage_base& __r)
        { return __l.template __get<_T>() == __r.template __get<_T>(); }
    static inline constexpr bool (*__eq_fns[])(const __variant_storage_base&, const __variant_storage_base&)
    {
        &__eq_fn<_Types>...
    };

    // Inequality methods.
    template <class _T> static bool __neq_fn(const __variant_storage_base& __l, const __variant_storage_base& __r)
        { return __l.template __get<_T>() != __r.template __get<_T>(); }
    static inline constexpr bool (*__neq_fns[])(const __variant_storage_base&, const __variant_storage_base&)
    {
        &__neq_fn<_Types>...
    };

    // Less-than methods.
    template <class _T> static bool __lt_fn(const __variant_storage_base& __l, const __variant_storage_base& __r)
        { return __l.template __get<_T>() < __r.template __get<_T>(); }
    static inline constexpr bool (*__lt_fns[])(const __variant_storage_base&, const __variant_storage_base&)
    {
        &__lt_fn<_Types>...
    };

    // Greater-than methods.
    template <class _T> static bool __gt_fn(const __variant_storage_base& __l, const __variant_storage_base& __r)
        { return __l.template __get<_T>() > __r.template __get<_T>(); }
    static inline constexpr bool (*__gt_fns[])(const __variant_storage_base&, const __variant_storage_base&)
    {
        &__gt_fn<_Types>...
    };

    // Less-or-equal methods.
    template <class _T> static bool __le_fn(const __variant_storage_base& __l, const __variant_storage_base& __r)
        { return __l.template __get<_T>() <= __r.template __get<_T>(); }
    static inline constexpr bool (*__le_fns[])(const __variant_storage_base&, const __variant_storage_base&)
    {
        &__le_fn<_Types>...
    };

    // Greater-or-equal methods.
    template <class _T> static bool __ge_fn(const __variant_storage_base& __l, const __variant_storage_base& __r)
        { return __l.template __get<_T>() >= __r.template __get<_T>(); }
    static inline constexpr bool (*__ge_fns[])(const __variant_storage_base&, const __variant_storage_base&)
    {
        &__ge_fn<_Types>...
    };

    // Truth table for nothrow copy constructibility.
    static inline constexpr bool __nothrow_copy_constructible[] =
    {
        is_nothrow_copy_constructible_v<_Types>...
    };

    // Truth table for nothrow move constructibility.
    static inline constexpr bool __nothrow_move_constructible[] =
    {
        is_nothrow_move_constructible_v<_Types>...
    };
};


template <class... _Types>
struct __variant_storage
    : public __variant_storage_base<_Types...>
{
    using _Base = __variant_storage_base<_Types...>;
    using _T0 = typename __variant_storage_union<_Types...>::__inner_t::__value_t;    

    constexpr __variant_storage()
        requires is_default_constructible_v<_T0>
    {
        // Explicitly default-construct using the default constructor of the first alternative in the variant.
        new (__ptr<0>(_Base::_M_storage.__inner)) _T0();
    }

    constexpr __variant_storage()
    requires (!is_default_constructible_v<_T0>)
        = delete;

    constexpr __variant_storage(const __variant_storage&)
    requires (is_trivially_copy_constructible_v<_Types> && ...)
        = default;

    constexpr __variant_storage(const __variant_storage& __rhs)
    requires (!(is_trivially_copy_constructible_v<_Types> && ...) && (is_copy_constructible_v<_Types> && ...))
    {
        auto __this = static_cast<_Base*>(this);
        auto __that = static_cast<const _Base*>(&__rhs);

        //! @TODO: evaluate recursive vs jump table.
        if (true || is_constant_evaluated())
        {
            if (__that->_M_index != variant_npos)
                __copy_construct(__this->_M_storage.__inner, __that->_M_storage.__inner, __that->_M_index);
            __this->_M_index = __that->_M_index;
            return;
        }

        if (__that->_M_index != variant_npos)
            _Base::__copy_ctors[__that->_M_index](*__this, *__that);
        __this->_M_index = __that->_M_index;
    }

    constexpr __variant_storage(const __variant_storage&)
    requires (!(is_copy_constructible_v<_Types> && ...))
        = delete;

    constexpr __variant_storage(__variant_storage&&)
    requires (is_trivially_move_constructible_v<_Types> && ...)
        = default;

    constexpr __variant_storage(__variant_storage&& __rhs)
    requires (!(is_trivially_move_constructible_v<_Types> && ...) && (is_move_constructible_v<_Types> && ...))
    {
        auto __this = static_cast<_Base*>(this);
        auto __that = static_cast<_Base*>(&__rhs);

        //! @TODO: evaluate recursive vs jump table.
        if (true || is_constant_evaluated())
        {
            if (__that->_M_index != variant_npos)
                __move_construct(__this->_M_storage.__inner, __that->_M_storage.__inner, __that->_M_index);
            __this->_M_index = __that->_M_index;
            return;
        }

        if (__that->_M_index != variant_npos)
            _Base::__move_ctors[__that->_M_index](*__this, *__that);
        __this->_M_index = __that->_M_index;
    }

    constexpr __variant_storage(__variant_storage&&) 
    requires (!(is_move_constructible_v<_Types> && ...))
        = delete;

#ifndef __llvm__
    //! @todo clang bug? - doesn't seem to handle the requires clause properly.
    ~__variant_storage()
    requires (is_trivially_destructible_v<_Types> && ...)
        = default;
#endif

    ~__variant_storage()
#ifndef __llvm__
        requires (!(is_trivially_destructible_v<_Types> && ...))
#endif
    {
        auto __this = static_cast<_Base*>(this);
        
        if (__this->_M_index == variant_npos)
            return;
        
        //! @TODO: evaluate recursive vs jump table.
        if (true || is_constant_evaluated())
        {
            __destruct(__this->_M_storage.__inner, __this->_M_index);
            __this->_M_index = variant_npos;
            return;
        }

        _Base::__dtors[__this->_M_index](*__this);
        __this->_M_index = variant_npos;
    }

    constexpr __variant_storage& operator=(__variant_storage&&)
    requires ((is_trivially_move_constructible_v<_Types> && is_trivially_move_assignable_v<_Types> && is_trivially_destructible_v<_Types>) && ...)
        = default;

    constexpr __variant_storage& operator=(__variant_storage&& __rhs)
    requires (((is_move_constructible_v<_Types> && is_move_assignable_v<_Types>) && ...)
        && !((is_trivially_move_constructible_v<_Types> && is_trivially_move_assignable_v<_Types> && is_trivially_destructible_v<_Types>) && ...))
    {
        auto __this = static_cast<_Base*>(this);
        auto __that = static_cast<_Base*>(&__rhs);

        // Note: safe for self-assignment if the underlying types are safe.

        bool __this_empty = __this->_M_index == variant_npos;
        bool __that_empty = __that->_M_index == variant_npos;

        size_t __j = __that->_M_index;

        if (__this_empty && __that_empty)
        {
            return *this;
        }
        else if (!__this_empty && __that_empty)
        {
            // Note: requires __variant_destroy_base to be an ancestor of this class.
            __this->~_Base();
            __this->_M_index = variant_npos;
            return *this;
        }
        else if (__this->_M_index == __that->_M_index)
        {
            //! @TODO: evaluate recursive vs jump table.
            if (true || is_constant_evaluated())
            {
                __move_assign(__this->_M_storage.__inner, __that->_M_storage.__inner, __that->_M_index);
                return *this;
            }

            _Base::__move_assigns[__that->_M_index](*__this, *__that);
            return *this;
        }
        else
        {
            // Note: requires __variant_destroy_base to be an ancestor of this class.
            //       requires __variant_move_ctor_base to be an ancestor of this class.
            __this->~_Base();
            __this->_M_index = variant_npos;
            new (__this) _Base(__XVI_STD_NS::move(*__that));
            __this->_M_index = __j;
            return *this;
        }
    }

    constexpr __variant_storage& operator=(__variant_storage&&)
    requires (!((is_move_constructible_v<_Types> && is_move_assignable_v<_Types>) && ...))
        = delete;

    constexpr __variant_storage& operator=(const __variant_storage&)
    requires ((is_trivially_copy_constructible_v<_Types> && is_trivially_copy_assignable_v<_Types> && is_trivially_destructible_v<_Types>) && ...)
        = default;;

    constexpr __variant_storage& operator=(const __variant_storage& __rhs)
    requires (((is_copy_constructible_v<_Types> && is_copy_assignable_v<_Types>) && ...)
        && !((is_trivially_copy_constructible_v<_Types> && is_trivially_copy_assignable_v<_Types> && is_trivially_destructible_v<_Types>) && ...))
    {
        auto __this = static_cast<_Base*>(this);
        auto __that = static_cast<const _Base*>(&__rhs);

        // Note: safe for self-assignment if the underlying types are safe.

        bool __this_empty = __this->_M_index == variant_npos;
        bool __that_empty = __that->_M_index == variant_npos;

        size_t __j = __that->_M_index;

        if (__this_empty && __that_empty)
        {
            return *this;
        }
        else if (!__this_empty && __that_empty)
        {
            // Note: requires __variant_destroy_base to be an ancestor of this class.
            __this->~_Base();
            __this->_M_index = variant_npos;
            return *this;
        }
        else if (__this->_M_index == __that->_M_index)
        {
            //! @TODO: evaluate recursive vs jump table.
            if (true || is_constant_evaluated())
            {
                __copy_assign(__this->_M_storage.__inner, __that->_M_storage.__inner, __that->_M_index);
                return *this;
            }
            
            _Base::__copy_assigns[__that->_M_index](*__this, *__that);
            return *this;
        }
        else if (_Base::__nothrow_copy_constructible[__j] || !_Base::__nothrow_move_constructible[__j])
        {
            // Note: requires __variant_destroy_base to be an ancestor of this class.
            //       requires __variant_copy_ctor_base to be an ancestor of this class.
            __this->~_Base();
            __this->_M_index = variant_npos;
            new (__this) _Base(*__that);
            __this->_M_index = __j;
            return *this;
        }
        else
        {
            // Note: requires __variant_move_assign_base to be an ancestor of this class.
            __this->operator=(_Base(*__that));
            return *this;
        }
    }

    constexpr __variant_storage& operator=(const __variant_storage&)
    requires (!((is_copy_constructible_v<_Types> && is_copy_assignable_v<_Types>) && ...))
        = delete;

    template <class _T, class... _Args>
    constexpr void __emplace_construct(_Args&&... __args)
    {
        new (__ptr<_T>(_Base::_M_storage.__inner)) _T(__XVI_STD_NS::forward<_Args>(__args)...);
        _Base::_M_index = __index_of_v<_T, _Types...>;
    }

    template <size_t _I, class... _Args>
    constexpr void __emplace_construct(_Args&&... __args)
    {
        using _T = typename __nth_type<_I, _Args...>::type;
        new (__ptr<_I>(_Base::_M_storage.__inner)) _T(__XVI_STD_NS::forward<_Args>(__args)...);
        _Base::_M_index = _I;
    }

    constexpr void __reset()
    {
        if (_Base::_M_index == variant_npos)
            return;
        
        //! @TODO: evaluate recursive vs jump table.
        if (true || is_constant_evaluated())
        {
            __destruct(_Base::_M_storage.__inner, _Base::_M_index);
            _Base::_M_index = variant_npos;
            return;
        }

        _Base::__dtors[_Base::_M_index](*this);
        _Base::_M_index = variant_npos;
    }

    constexpr void swap(__variant_storage& __rhs)
    {
        if (_Base::_M_index == variant_npos && __rhs._M_index == variant_npos)
            return;
        
        if (_Base::_M_index == __rhs._M_index)
        {
            //! @TODO: evaluate recursive vs jump table.
            if (true || is_constant_evaluated())
            {
                __swap_elem(_Base::_M_storage.__inner, __rhs._M_storage.__inner, _Base::_M_index);
                return;
            }

            _Base::__swap_fns[_Base::_M_index](*this, __rhs);
            return;
        }

        // Requires a three-move exchange. All of these are move-construct as the two variants have different active
        // indices (so the assigns will actually do a move-construct instead of a move-assign internally).
        __variant_storage __temp {__XVI_STD_NS::move(__rhs)};
        __rhs = __XVI_STD_NS::move(*this);
        *this = __XVI_STD_NS::move(__temp);
    }
};


struct __variant_fn_ignore {};

template <class _T, class _Ti> using __variant_type_selector_fn_detector = decltype(declval<_Ti&>() = {declval<_T>()});

template <class _T, class _Ti, bool = is_detected_v<__variant_type_selector_fn_detector, _T, _Ti>
                                      // If _Ti is [cv] bool then _T must be [cv][&/&&] bool to match.
                                      && (!is_same_v<remove_cv_t<_Ti>, bool> || is_same_v<remove_cvref_t<_T>, bool>)>
struct __variant_type_selector_fn_base
{
    // Overload shouldn't clash with any other. Hopefully.
    static __variant_fn_ignore __fn(__variant_fn_ignore, _Ti);
};

template <class _T, class _Ti>
struct __variant_type_selector_fn_base<_T, _Ti, true>
{
    static _Ti __fn(_Ti);
};

template <class _T, class... _Types> struct __variant_type_selector_helper;
template <class _T, class _Ti, class... _Types> struct __variant_type_selector_helper<_T, _Ti, _Types...>
    : __variant_type_selector_helper<_T, _Types...>,
      __variant_type_selector_fn_base<_T, _Ti>
{
    // Bring all overloads into scope.
    using __variant_type_selector_helper<_T, _Types...>::__fn;
    using __variant_type_selector_fn_base<_T, _Ti>::__fn;
};

template <class _T, class _Ti> struct __variant_type_selector_helper<_T, _Ti>
    : __variant_type_selector_fn_base<_T, _Ti>
{
    using __variant_type_selector_fn_base<_T, _Ti>::__fn;
};

template <class _T, class... _Types>
struct __variant_type_selector : __variant_type_selector_helper<_T, _Types...>
{
    using __variant_type_selector_helper<_T, _Types...>::__fn;
    template <class _U> using __detector = decltype(__fn(__XVI_STD_NS::forward<_U>(declval<_U>())));
    using type = detected_or_t<__variant_fn_ignore, __detector, _T>;
};

} // namespace __detail


class bad_variant_access
    : public exception
{
public:

    constexpr bad_variant_access() noexcept = default;
    const char* what() const noexcept override
        { return "bad variant access"; }
};


template <class... _Types>
class variant
    : private __detail::__variant_storage<_Types...>
{
public:

    using _T0 = typename __detail::__nth_type<0, _Types...>::type;

    constexpr variant() noexcept(is_nothrow_default_constructible_v<_T0>) = default;
    constexpr variant(const variant&) = default;
    constexpr variant(variant&&) noexcept((is_nothrow_move_constructible_v<_Types> && ...)) = default;

    template <class _T,
              class _Tj = typename __detail::__variant_type_selector<_T, _Types...>::type,
              class = enable_if_t<sizeof...(_Types) != 0
                                  && !is_same_v<remove_cvref_t<_T>, variant>
                                  && !__detail::__is_in_place_specialization_v<_T>
                                  && is_constructible_v<_Tj, _T>
                                  && !is_same_v<_Tj, __detail::__variant_fn_ignore>, void>>
    constexpr variant(_T&& __t) noexcept(is_nothrow_constructible_v<_Tj, _T>)
    {
        __storage_t::_M_index = variant_npos;
        __storage_t::template __emplace_construct<_Tj>(__XVI_STD_NS::forward<_T>(__t));
    }

    template <class _T, class... _Args,
              class = enable_if_t<__detail::__type_unique_in_pack_v<_T, _Types...>
                                  && is_constructible_v<_T, _Args...>, void>>
    constexpr explicit variant(in_place_type_t<_T>, _Args&&... __args)
    {
        __storage_t::_M_index = variant_npos;
        __storage_t::template __emplace_construct<_T>(__XVI_STD_NS::forward<_Args>(__args)...);
    }

    template <class _T, class _U, class... _Args,
              class = enable_if_t<__detail::__type_unique_in_pack_v<_T, _Types...>
                                  && is_constructible_v<_T, initializer_list<_U>&, _Args...>, void>>
    constexpr explicit variant(in_place_type_t<_T>, initializer_list<_U> __il, _Args&&... __args)
    {
        __storage_t::_M_index = variant_npos;
        __storage_t::template __emplace_construct<_T>(__il, __XVI_STD_NS::forward<_Args>(__args)...);
    }

    template <size_t _I, class... _Args,
              class = enable_if_t<(_I < sizeof...(_Types))
                                  && is_constructible_v<typename __detail::__nth_type<_I, _Types...>, _Args...>, void>>
    constexpr explicit variant(in_place_index_t<_I>, _Args&&... __args)
    {
        __storage_t::_M_index = variant_npos;
        __storage_t::template __emplace_construct<_I>(__XVI_STD_NS::forward<_Args>(__args)...);
    }

    template <size_t _I, class _U, class... _Args,
              class = enable_if_t<(_I < sizeof...(_Types))
                                  && is_constructible_v<typename __detail::__nth_type<_I, _Types...>, initializer_list<_U>&, _Args...>, void>>
    constexpr explicit variant(in_place_index_t<_I>, initializer_list<_U> __il, _Args&&... __args)
    {
        __storage_t::_M_index = variant_npos;
        __storage_t::template __emplace_construct<_I>(__il, __XVI_STD_NS::forward<_Args>(__args)...);
    }

    ~variant() = default;

    constexpr variant& operator=(const variant&) = default;
    constexpr variant& operator=(variant&&)
        noexcept(((is_nothrow_move_constructible_v<_Types>
                   && is_nothrow_move_assignable_v<_Types>) && ...)) = default;

    template <class _T,
              class _Tj = typename __detail::__variant_type_selector<_T, _Types...>::type,
              class = enable_if_t<sizeof...(_Types) != 0
                                  && !is_same_v<remove_cvref_t<_T>, variant>
                                  && (is_assignable_v<_Tj&, _T> && is_constructible_v<_Tj, _T>)
                                  && !is_same_v<_Tj, __detail::__variant_fn_ignore>, void>>
    constexpr variant& operator=(_T&& __t)
        noexcept(is_nothrow_assignable_v<_Tj&, _T> && is_nothrow_constructible_v<_Tj, _T>)
    {
        constexpr size_t _J = __detail::__index_of_v<_Tj, _Types...>;
        if (__storage_t::_M_index == _J)
        {
            __detail::template __get<_J>(__storage_t::_M_storage.__inner) = __XVI_STD_NS::forward<_T>(__t);
            return *this;
        }
        else if constexpr (is_nothrow_constructible_v<_Tj, _T> || !is_nothrow_move_constructible_v<_Tj>)
        {
            emplace<_J>(__XVI_STD_NS::forward<_T>(__t));
            return *this;
        }
        else
        {
            return operator=(variant(__XVI_STD_NS::forward<_T>(__t)));
        }
    }

    template <class _T, class... _Args,
              class = enable_if_t<is_constructible_v<_T, _Args...>
                                  && __detail::__type_unique_in_pack<_T, _Types...>::value, void>>
    _T& emplace(_Args&&... __args)
    {
        constexpr size_t _I = __detail::__index_of_v<_T, _Types...>;
        return emplace<_I>(__XVI_STD_NS::forward<_Args>(__args)...);
    }

    template <class _T, class _U, class... _Args,
              class = enable_if_t<is_constructible_v<_T, initializer_list<_U>&, _Args...>
                                  && __detail::__type_unique_in_pack<_T, _Types...>::value, void>>
    _T& emplace(initializer_list<_U> __il, _Args&&... __args)
    {
        constexpr size_t _I = __detail::__index_of_v<_T, _Types...>;
        return emplace<_I>(__il, __XVI_STD_NS::forward<_Args>(__args)...);
    }

    template <size_t _I, class... _Args,
              class _T = typename __detail::__nth_type<_I, _Types...>::type,
              class = enable_if_t<is_constructible_v<_T, _Args...>, void>>
    _T& emplace(_Args&&... __args)
    {
        __storage_t::__reset();
        __storage_t::template __emplace_construct<_T>(__XVI_STD_NS::forward<_Args>(__args)...);
        return __detail::template __get<_I>(__storage_t::_M_storage.__inner);
    }

    template <size_t _I, class _U, class... _Args,
              class _T = typename __detail::__nth_type<_I, _Types...>::type,
              class = enable_if_t<is_constructible_v<_T, initializer_list<_U>&, _Args...>, void>>
    _T& emplace(initializer_list<_U> __il, _Args&&... __args)
    {
        __storage_t::__reset();
        __storage_t::template __emplace_construct<_T>(__il, __XVI_STD_NS::forward<_Args>(__args)...);
        return __detail::template __get<_I>(__storage_t::_M_storage.__inner);
    }

    constexpr bool valueless_by_exception() const noexcept
    {
        return __storage_t::_M_index == variant_npos;
    }

    constexpr size_t index() const noexcept
    {
        return __storage_t::_M_index;
    }

    constexpr void swap(variant& __rhs)
        noexcept(((is_nothrow_move_constructible_v<_Types> && is_nothrow_swappable_v<_Types>) && ...))
    {
        __storage_t::swap(__rhs);
    }

    template <size_t _I>
    constexpr typename __detail::__nth_type<_I, _Types...>::type& __get()
    {
        if (index() != _I)
            __invalid_access();
        return __storage_t::template __get<_I>();
    }

    template <size_t _I>
    constexpr const typename __detail::__nth_type<_I, _Types...>::type& __get() const
    {
        if (index() != _I)
            __invalid_access();
        return __storage_t::template __get<_I>();
    }

    template <class _T>
    constexpr _T& __get()
    {
        if (index() != __detail::__index_of_v<_T, _Types...>)
            __invalid_access();
        return __storage_t::template __get<_T>();
    }

    template <class _T>
    constexpr const _T& __get() const
    {
        if (index() != __detail::__index_of_v<_T, _Types...>)
            __invalid_access();
        return __storage_t::template __get<_T>();
    }

    static constexpr bool __eq(const variant& __v, const variant& __w)
    {
        return __v.__storage_t::operator==(__w);
    }

    static constexpr bool __ne(const variant& __v, const variant& __w)
    {
        return __v.__storage_t::operator!=(__w);
    }

    static constexpr bool __lt(const variant& __v, const variant& __w)
    {
        return __v.__storage_t::operator<(__w);
    }

    static constexpr bool __gt(const variant& __v, const variant& __w)
    {
        return __v.__storage_t::operator>(__w);
    }

    static constexpr bool __le(const variant& __v, const variant& __w)
    {
        return __v.__storage_t::operator<=(__w);
    }

    static constexpr bool __ge(const variant& __v, const variant& __w)
    {
        return __v.__storage_t::operator>=(__w);
    }

#ifdef __XVI_CXX_UTILITY_NO_EXCEPTIONS
    [[noreturn]] void __invalid_access() const noexcept
    {
        terminate();
    }
#else
    [[noreturn]] void __invalid_access() const
    {
        throw bad_variant_access();
    }
#endif

private:

    template <class...> friend class variant;
    using __storage_t = __detail::__variant_storage<_Types...>;
};


template <class> struct variant_size;

template <class _T> struct variant_size<const _T> : integral_constant<size_t, variant_size<_T>::value> {};
template <class _T> struct variant_size<volatile _T> : integral_constant<size_t, variant_size<_T>::value> {};
template <class _T> struct variant_size<const volatile _T> : integral_constant<size_t, variant_size<_T>::value> {};

template <class _T> inline constexpr size_t variant_size_v = variant_size<_T>::value;

template <class... _Types> struct variant_size<variant<_Types...>> : integral_constant<size_t, sizeof...(_Types)> {};

template <size_t, class> struct variant_alternative;
template <size_t _I, class _T> struct variant_alternative<_I, const _T>
    { using type = add_const_t<typename variant_alternative<_I, _T>::type>; };
template <size_t _I, class _T> struct variant_alternative<_I, volatile _T>
    { using type = add_volatile_t<typename variant_alternative<_I, _T>::type>; };
template <size_t _I, class _T> struct variant_alternative<_I, const volatile _T>
    { using type = add_cv_t<typename variant_alternative<_I, _T>::type>; };

template <size_t _I, class _T> using variant_alternative_t = typename variant_alternative<_I, _T>::type;

template <size_t _I, class... _Types> struct variant_alternative<_I, variant<_Types...>>
    { using type = typename __detail::__nth_type<_I, _Types...>::type; };


template <class _T, class... _Types>
constexpr bool holds_alternative(const variant<_Types...>& __v) noexcept
{
    return __v.index() == __detail::__index_of_v<_T, _Types...>;
}


template <size_t _I, class... _Types>
constexpr variant_alternative_t<_I, variant<_Types...>>& get(variant<_Types...>& __v)
{
    return __v.template __get<_I>();
}

template <size_t _I, class... _Types>
constexpr variant_alternative_t<_I, variant<_Types...>>&& get(variant<_Types...>&& __v)
{
    return __XVI_STD_NS::move(__v.template __get<_I>());
}

template <size_t _I, class... _Types>
constexpr const variant_alternative_t<_I, variant<_Types...>>& get(const variant<_Types...>& __v)
{
    return __v.template __get<_I>();
}

template <size_t _I, class... _Types>
constexpr const variant_alternative_t<_I, variant<_Types...>>&& get(const variant<_Types...>&& __v)
{
    return __XVI_STD_NS::move(__v.template __get<_I>());
}

template <class _T, class... _Types>
constexpr _T& get(variant<_Types...>& __v)
{
    return __v.template __get<_T>();
}

template <class _T, class... _Types>
constexpr _T&& get(variant<_Types...>&& __v)
{
    return __XVI_STD_NS::move(__v.template __get<_T>());
}

template <class _T, class... _Types>
constexpr const _T& get(const variant<_Types...>& __v)
{
    return __v.template __get<_T>();
}

template <class _T, class... _Types>
constexpr const _T&& get(const variant<_Types...>&& __v)
{
    return __XVI_STD_NS::move(__v.template __get<_T>());
}


template <size_t _I, class... _Types>
constexpr add_pointer_t<variant_alternative_t<_I, variant<_Types...>>>
get_if(variant<_Types...>* __v) noexcept
{
    if (!__v || __v->index() != _I)
        return nullptr;
    return __XVI_STD_NS::addressof(get<_I>(*__v));
}

template <size_t _I, class... _Types>
constexpr add_pointer_t<const variant_alternative_t<_I, variant<_Types...>>>
get_if(const variant<_Types...>* __v) noexcept
{
    if (!__v || __v->index() != _I)
        return nullptr;
    return __XVI_STD_NS::addressof(get<_I>(*__v));
}

template <class _T, class... _Types>
constexpr add_pointer_t<_T>
get_if(variant<_Types...>* __v) noexcept
{
    constexpr size_t _I = __detail::__index_of_v<_T, _Types...>;
    return get_if<_I>(__v);
}

template <class _T, class... _Types>
constexpr add_pointer_t<const _T>
get_if(const variant<_Types...>* __v) noexcept
{
    constexpr size_t _I = __detail::__index_of_v<_T, _Types...>;
    return get_if<_I>(__v);
}


template <class... _Types>
constexpr bool operator==(const variant<_Types...>& __v, const variant<_Types...>& __w)
{
    return variant<_Types...>::__eq(__v, __w);
}

template <class... _Types>
constexpr bool operator!=(const variant<_Types...>& __v, const variant<_Types...>& __w)
{
    return variant<_Types...>::__ne(__v, __w);
}

template <class... _Types>
constexpr bool operator<(const variant<_Types...>& __v, const variant<_Types...>& __w)
{
    return variant<_Types...>::__lt(__v, __w);
}

template <class... _Types>
constexpr bool operator>(const variant<_Types...>& __v, const variant<_Types...>& __w)
{
    return variant<_Types...>::__gt(__v, __w);
}

template <class... _Types>
constexpr bool operator<=(const variant<_Types...>& __v, const variant<_Types...>& __w)
{
    return variant<_Types...>::__le(__v, __w);
}

template <class... _Types>
constexpr bool operator>=(const variant<_Types...>& __v, const variant<_Types...>& __w)
{
    return variant<_Types...>::__ge(__v, __w);
}


template <class... _Types,
          class = enable_if_t<((is_move_constructible_v<_Types> && is_swappable_v<_Types>) && ...), void>>
constexpr void swap(variant<_Types...>& __v, variant<_Types...>& __w) noexcept(noexcept(__v.swap(__w)))
{
    __v.swap(__w);
}


namespace __detail
{

template <size_t _M, class _Visitor, class _Variant>
constexpr auto __dispatch1_fn(_Visitor&& __v, _Variant&& __var)
{ 
    return __detail::_INVOKE(__XVI_STD_NS::forward<_Visitor>(__v), get<_M>(__XVI_STD_NS::forward<_Variant>(__var)));
}

template <size_t _M, class _R, class _Visitor, class _Variant>
constexpr _R __dispatch1r_fn(_Visitor&& __v, _Variant&& __var)
{
    return __detail::_INVOKE_R<_R>(__XVI_STD_NS::forward<_Visitor>(__v), get<_M>(__XVI_STD_NS::forward<_Variant>(__var)));
}

template <class _Visitor, class _Variant, size_t... _Idx>
constexpr auto __dispatch1_via_table(index_sequence<_Idx...>, _Visitor&& __vis, _Variant&& __var)
    -> invoke_result_t<_Visitor, variant_alternative_t<0, remove_reference_t<_Variant>>>
{
    using __dispatch1_fn_t = decltype(__dispatch1_fn<0, _Visitor, _Variant>);
    static_assert((is_same_v<__dispatch1_fn_t, decltype(__dispatch1_fn<_Idx, _Visitor, _Variant>)> && ...),
        "__XVI_STD_NS::visit requires all methods to have the same result type");
    
    constexpr __dispatch1_fn_t* __table[] = {&__dispatch1_fn<_Idx, _Visitor, _Variant>...};
    return __table[__var.index()](__XVI_STD_NS::forward<_Visitor>(__vis), __XVI_STD_NS::forward<_Variant>(__var));
}

template <class _R, class _Visitor, class _Variant, size_t... _Idx>
constexpr _R __dispatch1r_via_table(index_sequence<_Idx...>, _Visitor&& __vis, _Variant&& __var)
{
    using __dispatch1r_fn_t = decltype(__dispatch1r_fn<0, _R, _Visitor, _Variant>);
    constexpr __dispatch1r_fn_t* __table[] = {&__dispatch1r_fn<_Idx, _R, _Visitor, _Variant>...};
    return __table[__var.index()](__XVI_STD_NS::forward<_Visitor>(__vis), __XVI_STD_NS::forward<_Variant>(__var));
}

} // namespace __detail


template <class _Visitor, class... _Variants>
constexpr decltype(auto) visit(_Visitor&& __vis, _Variants&&... __vars)
{
    if constexpr (sizeof...(_Variants) == 0)
        return __detail::_INVOKE(__XVI_STD_NS::forward<_Visitor>(__vis));
    else if constexpr (sizeof...(_Variants) == 1)
    {
        // Dispatch via a table for constant-time behaviour.
        using _Variant = typename __detail::__nth_type<0, _Variants...>::type;
        using _V = remove_cvref_t<_Variant>;
        constexpr size_t _M = variant_size_v<_V>;

        return __detail::__dispatch1_via_table(make_index_sequence<_M>(),
                                               __XVI_STD_NS::forward<_Visitor>(__vis),
                                               __XVI_STD_NS::forward<_Variant>(__vars)...);
    }
    else
    {
        //! @TODO: implement.
    }
}

template <class _R, class _Visitor, class... _Variants>
constexpr _R visit(_Visitor&& __vis, _Variants&&... __vars)
{
    if constexpr (sizeof...(_Variants) == 0)
        return __detail::_INVOKE_R<_R>(__XVI_STD_NS::forward<_Visitor>(__vis));
    else if constexpr (sizeof...(_Variants) == 1)
    {
        // Dispatch via a table for constant-time behaviour.
        using _Variant = typename __detail::__nth_type<0, _Variants...>::type;
        constexpr size_t _M = variant_size_v<_Variant>;

        return __detail::__dispatch1r_via_table<_R>(make_index_sequence<_M>(),
                                                    __XVI_STD_NS::forward<_Visitor>(__vis),
                                                    __XVI_STD_NS::forward<_Variant>(__vars)...);
    }
    else
    {
        //! @TODO: implement.
    }
}


struct monostate {};
constexpr bool operator==(monostate, monostate) { return true; }
constexpr bool operator!=(monostate, monostate) { return false; }
constexpr bool operator< (monostate, monostate) { return false; }
constexpr bool operator> (monostate, monostate) { return false; }
constexpr bool operator<=(monostate, monostate) { return true; }
constexpr bool operator>=(monostate, monostate) { return true; }


//! @TODO: hash support.
template <class> struct hash;
template <class... _Types> struct hash<variant<_Types...>>;
template <> struct hash<monostate>;


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_VARIANT_H */
