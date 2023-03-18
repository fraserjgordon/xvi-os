#ifndef __SYSTEM_CXX_CORE_VARIANT_H
#define __SYSTEM_CXX_CORE_VARIANT_H


#include <System/C++/LanguageSupport/Compare.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/InitializerList.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>
#include <System/C++/Core/CtorTags.hh>
#include <System/C++/Core/IntegerSequence.hh>
#include <System/C++/Core/Invoke.hh>
#include <System/C++/Core/Swap.hh>
#include <System/C++/Core/TupleTraits.hh>


namespace __XVI_STD_CORE_NS_DECL
{


inline constexpr size_t variant_npos = static_cast<size_t>(-1);


namespace __detail
{

inline constexpr bool __variant_use_jump_table = false;

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
        return std::addressof(__u.__val);
    else
        return __ptr<_I - 1>(__u.__inner);
}

template <size_t _I, class... _Types>
constexpr auto __ptr(const __variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (_I == 0)
        return std::addressof(__u.__val);
    else
        return __ptr<_I - 1>(__u.__inner);
}

template <class _T, class... _Types>
constexpr _T* __ptr(__variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (std::is_same_v<_T, typename __variant_storage_union_base<_Types...>::__value_t>)
        return std::addressof(__u.__val);
    else
        return __ptr<_T>(__u.__inner);
}

template <class _T, class... _Types>
constexpr const _T* __ptr(const __variant_storage_union_base<_Types...>& __u) noexcept
{
    if constexpr (std::is_same_v<_T, typename __variant_storage_union_base<_Types...>::__value_t>)
        return std::addressof(__u.__val);
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

    new (__ptr<0>(__dest)) _T(std::move(__get<0>(__src)));
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

    __get<0>(__dest) = std::move(__get<0>(__src));
}

template <class _T, class... _Types, class... _Args>
constexpr void __emplace(__variant_storage_union_base<_T, _Types...>& __dest,
                         size_t __index,
                         _Args&&... __args)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            __emplace(__dest.__inner, __index - 1, std::forward<_Args>(__args)...);
        return;
    }

    new (__ptr<0>(__dest)) _T(std::forward<_Args>(__args)...);
}

template <class _T, class... _Types>
constexpr void __swap_elem(__variant_storage_union_base<_T, _Types...>& __lhs,
                           __variant_storage_union_base<_T, _Types...>& __rhs,
                           size_t __index)
{
    using std::swap;

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

template <class _T, class... _Types>
constexpr std::common_comparison_category_t<std::compare_three_way_result_t<_T>, std::compare_three_way_result_t<_Types>...>
__cmp(const __variant_storage_union_base<_T, _Types...>& __lhs,
      const __variant_storage_union_base<_T, _Types...>& __rhs,
      std::size_t __index)
{
    if (__index != 0)
    {
        if constexpr (sizeof...(_Types) > 0)
            return __cmp(__lhs.__inner, __rhs.__inner, __index - 1);
        return std::strong_ordering::equal;
    }

    return __get<0>(__lhs) <=> __get<0>(__rhs);
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

    constexpr __variant_storage_union(const __variant_storage_union&)
        requires (std::is_trivially_copy_constructible_v<_Types> && ...) = default;

    constexpr __variant_storage_union(const __variant_storage_union&)
        requires (!(std::is_trivially_copy_constructible_v<_Types> && ...))
    {
    }

    constexpr __variant_storage_union(__variant_storage_union&&)
        requires (std::is_trivially_move_constructible_v<_Types> && ...) = default;

    constexpr __variant_storage_union(__variant_storage_union&&)
        requires (!(std::is_trivially_move_constructible_v<_Types> && ...))
    {
    }

    constexpr ~__variant_storage_union() 
        requires (std::is_trivially_destructible_v<_Types> && ...) = default;

    // Non-trivial but empty destructor for unions with non-trivial members.
    constexpr ~__variant_storage_union()
        requires (!(is_trivially_destructible_v<_Types> && ...))
    {
    }

    constexpr __variant_storage_union& operator=(const __variant_storage_union&)
        requires (std::is_trivially_copy_assignable_v<_Types> && ...)
            && (std::is_trivially_copy_constructible_v<_Types> && ...)
            && (std::is_trivially_destructible_v<_Types> && ...) = default;

    constexpr __variant_storage_union& operator=(const __variant_storage_union&)
        requires (!((std::is_trivially_copy_assignable_v<_Types> && ...)
            && (std::is_trivially_copy_constructible_v<_Types> && ...)
            && (std::is_trivially_destructible_v<_Types> && ...)))
    {
        return *this;
    }

    constexpr __variant_storage_union& operator=(__variant_storage_union&&)
        requires (std::is_trivially_move_assignable_v<_Types> && ...)
            && (std::is_trivially_move_constructible_v<_Types> && ...)
            && (std::is_trivially_destructible_v<_Types> && ...) = default;

    constexpr __variant_storage_union& operator=(__variant_storage_union&&)
        requires (!((std::is_trivially_move_assignable_v<_Types> && ...)
            && (std::is_trivially_move_constructible_v<_Types> && ...)
            && (std::is_trivially_destructible_v<_Types> && ...)))
    {
        return *this;
    }
};


template <class... _Types>
struct __variant_storage
{
    using __union_t = __variant_storage_union<_Types...>;
    using _T0 = typename __variant_storage_union<_Types...>::__inner_t::__value_t;    

    using __compare_t = std::common_comparison_category_t<__detail::detected_or_t<std::false_type, std::compare_three_way_result, _Types>...>;

    __union_t _M_storage = {};
    size_t    _M_index = 0;


    constexpr __variant_storage()
    {
        // Explicitly default-construct using the default constructor of the first alternative in the variant.
        new (__ptr<0>()) _T0();
    }

    constexpr __variant_storage(const __variant_storage&)
        requires (std::is_trivially_copy_constructible_v<_Types> && ...) = default;

    constexpr __variant_storage(const __variant_storage& __rhs)
        requires (!(std::is_trivially_copy_constructible_v<_Types> && ...))
    {
        //! @TODO: evaluate recursive vs jump table.
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            if (__rhs._M_index != variant_npos)
                __copy_construct(_M_storage.__inner, __rhs._M_storage.__inner, __rhs._M_index);
            _M_index = __rhs._M_index;
            return;
        }

        if (__rhs._M_index != variant_npos)
            __copy_ctors[__rhs._M_index](_M_storage, __rhs._M_storage);
        _M_index = __rhs._M_index;
    }

    constexpr __variant_storage(__variant_storage&&)
        requires (std::is_trivially_move_constructible_v<_Types> && ...) = default;

    constexpr __variant_storage(__variant_storage&& __rhs)
        requires (!(is_trivially_move_constructible_v<_Types> && ...))
    {
        //! @TODO: evaluate recursive vs jump table.
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            if (__rhs._M_index != variant_npos)
                __move_construct(_M_storage.__inner, __rhs._M_storage.__inner, __rhs._M_index);
            _M_index = __rhs._M_index;
            return;
        }

        if (__rhs._M_index != variant_npos)
            __move_ctors(__rhs._M_index)(*this, __rhs);
        _M_index = __rhs._M_index;
    }

    constexpr ~__variant_storage()
        requires (std::is_trivially_destructible_v<_Types> && ...) = default;

    constexpr ~__variant_storage()
        requires (!(std::is_trivially_destructible_v<_Types> && ...))
    {
        if (_M_index == variant_npos)
            return;
        
        //! @TODO: evaluate recursive vs jump table.
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            __destruct(_M_storage.__inner, _M_index);
            _M_index = variant_npos;
            return;
        }

        __dtors(_M_index)(*this);
        _M_index = variant_npos;
    }

    constexpr __variant_storage& operator=(__variant_storage&&)
        requires ((std::is_trivially_move_constructible_v<_Types> && std::is_trivially_move_assignable_v<_Types> && std::is_trivially_destructible_v<_Types>) && ...) = default;

    constexpr __variant_storage& operator=(__variant_storage&& __rhs)
        requires (!((std::is_trivially_move_constructible_v<_Types> && std::is_trivially_move_assignable_v<_Types> && std::is_trivially_destructible_v<_Types>) && ...))
    {
        // Note: safe for self-assignment if the underlying types are safe.

        bool __this_empty = _M_index == variant_npos;
        bool __that_empty = __rhs._M_index == variant_npos;

        std::size_t __j = __rhs._M_index;

        if (__this_empty && __that_empty)
        {
            return *this;
        }
        else if (!__this_empty && __that_empty)
        {
            __reset();
            return *this;
        }
        else if (_M_index == __rhs._M_index)
        {
            //! @TODO: evaluate recursive vs jump table.
            if (!__variant_use_jump_table || std::is_constant_evaluated())
            {
                __move_assign(_M_storage.__inner, __rhs._M_storage.__inner, _M_index);
                return *this;
            }

            __move_assigns(_M_index)(*this, __rhs);
            return *this;
        }
        else
        {
            __reset();
            
            //! @todo: evaluate recursive vs jump table.
            if (__variant_use_jump_table || std::is_constant_evaluated())
            {
                __move_construct(_M_storage.__inner, __rhs._M_storage.__inner, __rhs._M_index);
                _M_index = __rhs._M_index;
                return *this;
            }

            __move_ctors(__rhs._M_index)(*this, __rhs);
            _M_index = __rhs._M_index;
            return *this;
        }
    }

    constexpr __variant_storage& operator=(const __variant_storage&)
        requires ((std::is_trivially_copy_constructible_v<_Types> && std::is_trivially_copy_assignable_v<_Types> && std::is_trivially_destructible_v<_Types>) && ...) = default;

    constexpr __variant_storage& operator=(const __variant_storage& __rhs)
        requires (!((std::is_trivially_copy_constructible_v<_Types> && std::is_trivially_copy_assignable_v<_Types> && std::is_trivially_destructible_v<_Types>) && ...))
    {
        // Note: safe for self-assignment if the underlying types are safe.

        bool __this_empty = _M_index == variant_npos;
        bool __that_empty = __rhs._M_index == variant_npos;

        std::size_t __j = __rhs._M_index;

        if (__this_empty && __that_empty)
        {
            return *this;
        }
        else if (!__this_empty && __that_empty)
        {
            __reset();
            return *this;
        }
        else if (_M_index == __rhs._M_index)
        {
            //! @TODO: evaluate recursive vs jump table.
            if (!__variant_use_jump_table || std::is_constant_evaluated())
            {
                __copy_assign(_M_storage.__inner, __rhs._M_storage.__inner, _M_index);
                return *this;
            }
            
            __copy_assigns(_M_index)(*this, __rhs);
            return *this;
        }
        else if (__nothrow_copy_constructible[__j] || !__nothrow_move_constructible[__j])
        {
            __reset();

            //! @todo: evaluate recursive vs jump table.
            if (!__variant_use_jump_table || std::is_constant_evaluated())
            {
                __copy_construct(_M_storage.__inner, __rhs._M_storage.__inner, __rhs._M_index);
                _M_index = __rhs._M_index;
                return *this;
            }

            __copy_ctors(_M_index)(this, __rhs);
            return *this;
        }
        else
        {
            // The type isn't nothrow copy constructible but is nothrow move constructible - we'll take a copy of the
            // original then move it into place, thereby preserving the current contents if an exception is thrown
            // during the copy.

            return operator=(__variant_storage(__rhs));
        }
    }

    template <class _T, class... _Args>
    constexpr void __emplace_construct(_Args&&... __args)
    {
        new (__ptr<_T>()) _T(std::forward<_Args>(__args)...);
        _M_index = __index_of_v<_T, _Types...>;
    }

    template <size_t _I, class... _Args>
    constexpr void __emplace_construct(_Args&&... __args)
    {
        using _T = typename __nth_type<_I, _Types...>::type;
        new (__ptr<_I>()) _T(std::forward<_Args>(__args)...);
        _M_index = _I;
    }

    constexpr void __reset()
    {
        if (_M_index == variant_npos)
            return;
        
        //! @TODO: evaluate recursive vs jump table.
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            __destruct(_M_storage.__inner, _M_index);
            _M_index = variant_npos;
            return;
        }

        __dtors(_M_index)(*this);
        _M_index = variant_npos;
    }

    constexpr void swap(__variant_storage& __rhs)
    {
        if (_M_index == variant_npos && __rhs._M_index == variant_npos)
            return;
        
        if (_M_index == __rhs._M_index)
        {
            //! @TODO: evaluate recursive vs jump table.
            if (!__variant_use_jump_table || std::is_constant_evaluated())
            {
                __swap_elem(_M_storage.__inner, __rhs._M_storage.__inner, _M_index);
                return;
            }

            __swap_fns(_M_index)(*this, __rhs);
            return;
        }

        // Requires a three-move exchange. All of these are move-construct as the two variants have different active
        // indices (so the assigns will actually do a move-construct instead of a move-assign internally).
        __variant_storage __temp {std::move(__rhs)};
        __rhs = std::move(*this);
        *this = std::move(__temp);
    }

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

    constexpr bool operator==(const __variant_storage& __v) const noexcept
    {
        if (_M_index != __v._M_index)
            return false;

        if (_M_index == variant_npos)
            return true;
        
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __variant_eq(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __eq_fns(_M_index)(*this, __v);
    }

    constexpr bool operator!=(const __variant_storage& __v) const noexcept
    {
        if (_M_index != __v._M_index)
            return true;

        if (_M_index == variant_npos)
            return false;
        
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __neq(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __neq_fns(_M_index)(*this, __v);
    }

    constexpr bool operator<(const __variant_storage& __v) const noexcept
    {
        if (__v._M_index == variant_npos)
            return false;

        if (_M_index == variant_npos)
            return true;

        if (_M_index < __v._M_index)
            return true;

        if (_M_index > __v._M_index)
            return false;
        
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __lt(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __lt_fns(_M_index)(*this, __v);
    }

    constexpr bool operator>(const __variant_storage& __v) const noexcept
    {
        if (_M_index == variant_npos)
            return false;

        if (__v._M_index == variant_npos)
            return true;

        if (_M_index > __v._M_index)
            return true;

        if (_M_index < __v._M_index)
            return false;
        
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __gt(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __gt_fns(_M_index)(*this, __v);
    }

    constexpr bool operator<=(const __variant_storage& __v) const noexcept
    {
        if (_M_index == variant_npos)
            return true;

        if (__v._M_index == variant_npos)
            return false;

        if (_M_index < __v._M_index)
            return true;

        if (_M_index > __v._M_index)
            return false;
        
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __le(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __le_fns(_M_index)(*this, __v);
    }

    constexpr bool operator>=(const __variant_storage& __v) const noexcept
    {
        if (__v._M_index == variant_npos)
            return true;

        if (_M_index == variant_npos)
            return false;

        if (_M_index > __v._M_index)
            return true;

        if (_M_index < __v._M_index)
            return false;
        
        if (!__variant_use_jump_table || std::is_constant_evaluated())
        {
            //! @TODO: evaluate recursive vs jump table.
            return __ge(_M_storage.__inner, __v._M_storage.__inner, _M_index);
        }

        return __ge_fns(_M_index)(*this, __v);
    }

    constexpr __compare_t operator<=>(const __variant_storage& __v) const noexcept
    {
        if (__v._M_index == _M_index)
        {
            if (!__variant_use_jump_table || std::is_constant_evaluated())
            {
                //! @todo: evaluate recursive vs jump table.
                return __cmp(_M_storage.__inner, __v._M_storage.__inner, _M_index);
            }

            return __cmp_fns(_M_index)(*this, __v);
        }

        if (_M_index < __v._M_index)
            return std::strong_ordering::less;
        else
            return std::strong_ordering::greater;
    }

    template <template <std::size_t> class _Fn, std::size_t... _Idx>
    static constexpr auto __get_fn_(std::size_t __i, std::index_sequence<_Idx...>)
    {
        using _Sig = decltype(_Fn<0>::__fn);
        constexpr _Sig* __fns[] = {&_Fn<_Idx>::__fn...};
        return __fns[__i];
    }

    template <template <std::size_t> class _Fn>
    static constexpr auto __get_fn(std::size_t __i)
    {
        constexpr auto _IndexSeq = std::make_index_sequence<sizeof...(_Types)>();
        return __get_fn_<_Fn>(__i, _IndexSeq);
    }

    // Copy constructor methods.
    template <std::size_t _I> struct __copy_ctor
    {
        static constexpr void __fn(__variant_storage& __to, const __variant_storage& __from)
            { new (__to.template __ptr<_I>()) __nth_type_t<_I, _Types...>(__from.template __get<_I>()); }
    };
    static constexpr auto __copy_ctors(std::size_t __i) { return __get_fn<__copy_ctors>(__i); }

    // Move constructor methods.
    template <std::size_t _I> struct __move_ctor
    {
        static constexpr void __fn(__variant_storage& __to, __variant_storage& __from)
            { new (__to.template __ptr<_I>()) __nth_type_t<_I, _Types...>(std::move(__from.template __get<_I>())); }
    };
    static constexpr auto __move_ctors(std::size_t __i) { return __get_fn<__move_ctor>(__i); }

    // Destructor methods.
    template <std::size_t _I> struct __dtor
    {
        static constexpr void __fn(__variant_storage& __v)
            { __v.template __get<_I>().~__nth_type_t<_I, _Types...>(); }
    };
    static constexpr auto __dtors(std::size_t __i) { return __get_fn<__dtor>(__i); }

    // Copy assignment methods.
    template <std::size_t _I> struct __copy_assign
    {
        static constexpr void __fn(__variant_storage& __to, const __variant_storage& __from)
            { __to.template __get<_I>() = __from.template __get<_I>(); }
    };
    static constexpr auto __copy_assigns(std::size_t __i) { return __get_fn<__copy_assign>(__i); }

    // Move assignment methods.
    template <std::size_t _I> struct __move_assign
    {
        static void __fn(__variant_storage& __to, __variant_storage& __from)
            { __to.template __get<_I>() = std::move(__from.template __get<_I>()); }
    };
    static constexpr auto __move_assigns(std::size_t __i) { return __get_fn<__move_assign>(__i); }

    // Swap methods.
    template <std::size_t _I> struct __swap_fn
    {
        static void __fn(__variant_storage& __l, __variant_storage& __r)
            { using std::swap; swap(__l.template __get<_I>(), __r.template __get<_I>()); }
    };
    static constexpr auto __swap_fns(std::size_t __i) { return __get_fn<__swap_fn>(__i); }

    // Equality methods.
    template <std::size_t _I> struct __eq_fn
    {
        static bool __fn(const __variant_storage& __l, const __variant_storage& __r)
            { return __l.template __get<_I>() == __r.template __get<_I>(); }
    };
    static constexpr auto __eq_fns(std::size_t __i) { return __get_fn<__eq_fn>(__i); }

    // Inequality methods.
    template <std::size_t _I> struct __neq_fn
    {
        static bool __fn(const __variant_storage& __l, const __variant_storage& __r)
            { return __l.template __get<_I>() != __r.template __get<_I>(); }
    };
    static constexpr auto __neq_fns(std::size_t __i) { return __get_fn<__neq_fn>(__i); }

    // Less-than methods.
    template <std::size_t _I> struct __lt_fn
    {
        static bool __fn(const __variant_storage& __l, const __variant_storage& __r)
            { return __l.template __get<_I>() < __r.template __get<_I>(); }
    };
    static constexpr auto __lt_fns(std::size_t __i) { return __get_fn<__lt_fn>(__i); }

    // Greater-than methods.
    template <std::size_t _I> struct __gt_fn
    {
        static bool __fn(const __variant_storage& __l, const __variant_storage& __r)
            { return __l.template __get<_I>() > __r.template __get<_I>(); }
    };
    static constexpr auto __gt_fns(std::size_t __i) { return __get_fn<__gt_fn>(__i); }

    // Less-or-equal methods.
    template <std::size_t _I> struct __le_fn
    {
        static bool __fn(const __variant_storage& __l, const __variant_storage& __r)
            { return __l.template __get<_I>() <= __r.template __get<_I>(); }
    };
    static constexpr auto __le_fns(std::size_t __i) { return __get_fn<__le_fn>(__i); }

    // Greater-or-equal methods.
    template <std::size_t _I> struct __ge_fn
    {
        static bool __fn(const __variant_storage& __l, const __variant_storage& __r)
            { return __l.template __get<_I>() >= __r.template __get<_I>(); }
    };
    static constexpr auto __ge_fns(std::size_t __i) { return __get_fn<__ge_fn>(__i); }

    // Three way comparison methods.
    template <std::size_t _I> struct __cmp_fn
    {
        static __compare_t __fn(const __variant_storage& __l, const __variant_storage& __r)
            { return __l.template __get<_I>() <=> __r.template __get<_I>(); }
    };
    static constexpr auto __cmp_fns(std::size_t __i) { return __get_fn<__cmp_fn>(__i); }

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
    template <class _U> using __detector = decltype(__fn(std::forward<_U>(declval<_U>())));
    using type = detected_or_t<__variant_fn_ignore, __detector, _T>;
};

template <class _T, class... _Types>
using __variant_type_selector_t = typename __variant_type_selector<_T, _Types...>::type;


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
{
public:

    using _T0 = typename __detail::__nth_type<0, _Types...>::type;

    constexpr variant() noexcept(is_nothrow_default_constructible_v<_T0>)
        requires std::is_default_constructible_v<_T0> = default;

    constexpr variant(const variant&)
        requires (std::is_copy_constructible_v<_Types> && ...) = default;

    constexpr variant(const variant&)
        requires (!(std::is_copy_constructible_v<_Types> && ...)) = delete;

    constexpr variant(variant&&) noexcept((is_nothrow_move_constructible_v<_Types> && ...))
        requires (std::is_move_constructible_v<_Types> && ...) = default;

    template <class _T,
              class _Tj = __detail::__variant_type_selector_t<_T, _Types...>>
        requires (sizeof...(_Types) != 0)
            && (!std::is_same_v<std::remove_cvref_t<_T>, variant>)
            && (!__detail::__is_in_place_type_specialization_v<std::remove_cvref_t<_T>>)
            && (!__detail::__is_in_place_index_specialization_v<std::remove_cvref_t<_T>>)
            && std::is_constructible_v<_Tj, _T>
            && (!std::is_same_v<_Tj, __detail::__variant_fn_ignore>)
    constexpr variant(_T&& __t) noexcept(is_nothrow_constructible_v<_Tj, _T>)
    {
        _M_storage._M_index = variant_npos;
        _M_storage.template __emplace_construct<_Tj>(std::forward<_T>(__t));
    }

    template <class _T, class... _Args>
        requires __detail::__type_unique_in_pack_v<_T, _Types...>
            && std::is_constructible_v<_T, _Args...>
    constexpr explicit variant(in_place_type_t<_T>, _Args&&... __args)
    {
        _M_storage._M_index = variant_npos;
        _M_storage.template __emplace_construct<_T>(std::forward<_Args>(__args)...);
    }

    template <class _T, class _U, class... _Args>
        requires __detail::__type_unique_in_pack_v<_T, _Types...>
            && std::is_constructible_v<_T, initializer_list<_U>&, _Args...>
    constexpr explicit variant(in_place_type_t<_T>, initializer_list<_U> __il, _Args&&... __args)
    {
        _M_storage._M_index = variant_npos;
        _M_storage.template __emplace_construct<_T>(__il, std::forward<_Args>(__args)...);
    }

    template <size_t _I, class... _Args>
        requires (_I < sizeof...(_Types))
            && std::is_constructible_v<__detail::__nth_type_t<_I, _Types...>, _Args...>
    constexpr explicit variant(in_place_index_t<_I>, _Args&&... __args)
    {
        _M_storage._M_index = variant_npos;
        _M_storage.template __emplace_construct<_I>(std::forward<_Args>(__args)...);
    }

    template <size_t _I, class _U, class... _Args>
        requires (_I < sizeof...(_Types))
            && std::is_constructible_v<__detail::__nth_type_t<_I, _Types...>, initializer_list<_U>&, _Args...>
    constexpr explicit variant(in_place_index_t<_I>, initializer_list<_U> __il, _Args&&... __args)
    {
        _M_storage._M_index = variant_npos;
        _M_storage.template __emplace_construct<_I>(__il, std::forward<_Args>(__args)...);
    }

    constexpr ~variant() = default;

    constexpr variant& operator=(const variant&)
        requires (std::is_copy_constructible_v<_Types> && ...)
            && (std::is_copy_assignable_v<_Types> && ...) = default;

    constexpr variant& operator=(const variant&)
        requires (!((std::is_copy_constructible_v<_Types> && ...)
            && (std::is_copy_assignable_v<_Types> && ...))) = delete;

    constexpr variant& operator=(variant&&)
        noexcept(((is_nothrow_move_constructible_v<_Types>
                   && is_nothrow_move_assignable_v<_Types>) && ...))
        requires (std::is_move_constructible_v<_Types> && ...)
            && (std::is_move_assignable_v<_Types> && ...) = default;

    template <class _T,
              class _Tj = typename __detail::__variant_type_selector<_T, _Types...>::type>
        requires (!std::is_same_v<std::remove_cvref_t<_T>, variant>)
            && std::is_assignable_v<_Tj, _T>
            && std::is_convertible_v<_Tj, _T>
            && (!std::is_same_v<_Tj, __detail::__variant_fn_ignore>)
    constexpr variant& operator=(_T&& __t)
        noexcept(is_nothrow_assignable_v<_Tj&, _T> && is_nothrow_constructible_v<_Tj, _T>)
    {
        constexpr std::size_t _J = __detail::__index_of_v<_Tj, _Types...>;
        if (_M_storage._M_index == _J)
        {
            _M_storage.template __get<_J>() = std::forward<_T>(__t);
            return *this;
        }
        else if constexpr (std::is_nothrow_constructible_v<_Tj, _T> || !std::is_nothrow_move_constructible_v<_Tj>)
        {
            emplace<_J>(std::forward<_T>(__t));
            return *this;
        }
        else
        {
            return operator=(variant(std::forward<_T>(__t)));
        }
    }

    template <class _T, class... _Args>
        requires std::is_constructible_v<_T, _Args...>
            && __detail::__type_unique_in_pack_v<_T, _Types...>
    _T& emplace(_Args&&... __args)
    {
        constexpr size_t _I = __detail::__index_of_v<_T, _Types...>;
        return emplace<_I>(std::forward<_Args>(__args)...);
    }

    template <class _T, class _U, class... _Args>
        requires std::is_constructible_v<_T, initializer_list<_U>&, _Args...>
            && __detail::__type_unique_in_pack_v<_T, _Types...>
    _T& emplace(initializer_list<_U> __il, _Args&&... __args)
    {
        constexpr size_t _I = __detail::__index_of_v<_T, _Types...>;
        return emplace<_I>(__il, std::forward<_Args>(__args)...);
    }

    template <size_t _I, class... _Args>
        requires std::is_constructible_v<__detail::__nth_type_t<_I, _Types...>, _Args...>
    __detail::__nth_type_t<_I, _Types...>& emplace(_Args&&... __args)
    {
        _M_storage.__reset();
        _M_storage.template __emplace_construct<_I>(std::forward<_Args>(__args)...);
        return _M_storage.template __get<_I>();
    }

    template <size_t _I, class _U, class... _Args>
        requires std::is_constructible_v<__detail::__nth_type_t<_I, _Types...>, initializer_list<_U>&, _Args...>
    __detail::__nth_type_t<_I, _Types...>& emplace(initializer_list<_U> __il, _Args&&... __args)
    {
        _M_storage.__reset();
        _M_storage.template __emplace_construct<_I>(__il, std::forward<_Args>(__args)...);
        return _M_storage.template __get<_I>();
    }

    constexpr bool valueless_by_exception() const noexcept
    {
        return _M_storage._M_index == variant_npos;
    }

    constexpr size_t index() const noexcept
    {
        return _M_storage._M_index;
    }

    constexpr void swap(variant& __rhs)
        noexcept(((is_nothrow_move_constructible_v<_Types> && is_nothrow_swappable_v<_Types>) && ...))
    {
        _M_storage.swap(__rhs._M_storage);
    }

    template <size_t _I>
    constexpr typename __detail::__nth_type<_I, _Types...>::type& __get()
    {
        if (index() != _I)
            __invalid_access();
        return _M_storage.template __get<_I>();
    }

    template <size_t _I>
    constexpr const typename __detail::__nth_type<_I, _Types...>::type& __get() const
    {
        if (index() != _I)
            __invalid_access();
        return _M_storage.template __get<_I>();
    }

    template <class _T>
    constexpr _T& __get()
    {
        if (index() != __detail::__index_of_v<_T, _Types...>)
            __invalid_access();
        return _M_storage.template __get<_T>();
    }

    template <class _T>
    constexpr const _T& __get() const
    {
        if (index() != __detail::__index_of_v<_T, _Types...>)
            __invalid_access();
        return _M_storage.template __get<_T>();
    }

    static constexpr bool __eq(const variant& __v, const variant& __w)
    {
        return __v.__storage_t::operator==(__w);
    }

    static constexpr bool __ne(const variant& __v, const variant& __w)
    {
        return __v.M_storage.operator!=(__w);
    }

    static constexpr bool __lt(const variant& __v, const variant& __w)
    {
        return __v._M_storage.operator<(__w);
    }

    static constexpr bool __gt(const variant& __v, const variant& __w)
    {
        return __v._M_storage.operator>(__w);
    }

    static constexpr bool __le(const variant& __v, const variant& __w)
    {
        return __v._M_storage.operator<=(__w);
    }

    static constexpr bool __ge(const variant& __v, const variant& __w)
    {
        return __v._M_storage.operator>=(__w);
    }

    static constexpr auto __cmp(const variant& __v, const variant& __w)
    {
        return __v._M_storage.operator<=>(__w);
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

    __storage_t _M_storage;
};


template <class> struct variant_size;

template <class _T> struct variant_size<const _T> : integral_constant<size_t, variant_size<_T>::value> {};
template <class _T> struct variant_size<volatile _T> : integral_constant<size_t, variant_size<_T>::value> {};
template <class _T> struct variant_size<const volatile _T> : integral_constant<size_t, variant_size<_T>::value> {};

template <class _T> inline constexpr size_t variant_size_v = variant_size<_T>::value;

template <class... _Types> struct variant_size<variant<_Types...>> : integral_constant<size_t, sizeof...(_Types)> {};

template <size_t, class> struct variant_alternative;
template <size_t _I, class _T> struct variant_alternative<_I, const _T>
    { using type = std::add_const_t<typename variant_alternative<_I, _T>::type>; };
template <size_t _I, class _T> struct variant_alternative<_I, volatile _T>
    { using type = std::add_volatile_t<typename variant_alternative<_I, _T>::type>; };
template <size_t _I, class _T> struct variant_alternative<_I, const volatile _T>
    { using type = std::add_cv_t<typename variant_alternative<_I, _T>::type>; };

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
    return std::move(__v.template __get<_I>());
}

template <size_t _I, class... _Types>
constexpr const variant_alternative_t<_I, variant<_Types...>>& get(const variant<_Types...>& __v)
{
    return __v.template __get<_I>();
}

template <size_t _I, class... _Types>
constexpr const variant_alternative_t<_I, variant<_Types...>>&& get(const variant<_Types...>&& __v)
{
    return static_cast<const variant_alternative_t<_I, variant<_Types...>>&&>(__v.template __get<_I>());
}

template <class _T, class... _Types>
constexpr _T& get(variant<_Types...>& __v)
{
    return __v.template __get<_T>();
}

template <class _T, class... _Types>
constexpr _T&& get(variant<_Types...>&& __v)
{
    return std::move(__v.template __get<_T>());
}

template <class _T, class... _Types>
constexpr const _T& get(const variant<_Types...>& __v)
{
    return __v.template __get<_T>();
}

template <class _T, class... _Types>
constexpr const _T&& get(const variant<_Types...>&& __v)
{
    return static_cast<const _T&&>(__v.template __get<_T>());
}


template <size_t _I, class... _Types>
constexpr std::add_pointer_t<variant_alternative_t<_I, variant<_Types...>>>
get_if(variant<_Types...>* __v) noexcept
{
    if (!__v || __v->index() != _I)
        return nullptr;
    return std::addressof(get<_I>(*__v));
}

template <size_t _I, class... _Types>
constexpr std::add_pointer_t<const variant_alternative_t<_I, variant<_Types...>>>
get_if(const variant<_Types...>* __v) noexcept
{
    if (!__v || __v->index() != _I)
        return nullptr;
    return std::addressof(get<_I>(*__v));
}

template <class _T, class... _Types>
constexpr std::add_pointer_t<_T>
get_if(variant<_Types...>* __v) noexcept
{
    constexpr size_t _I = __detail::__index_of_v<_T, _Types...>;
    return get_if<_I>(__v);
}

template <class _T, class... _Types>
constexpr std::add_pointer_t<const _T>
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

template <class... _Types>
    requires (std::three_way_comparable<_Types>  && ...)
constexpr std::common_comparison_category_t<std::compare_three_way_result_t<_Types>...>
operator<=>(const variant<_Types...>& __v, const variant<_Types...>& __w)
{
    return variant<_Types...>::__cmp(__v, __w);
}


template <class... _Types>
    requires ((std::is_move_constructible_v<_Types> && std::is_swappable_v<_Types>) && ...)
constexpr void swap(variant<_Types...>& __v, variant<_Types...>& __w) noexcept(noexcept(__v.swap(__w)))
{
    __v.swap(__w);
}


namespace __detail
{

template <size_t _M, class _Visitor, class _Variant>
constexpr auto __dispatch1_fn(_Visitor&& __v, _Variant&& __var)
{ 
    return __XVI_STD_NS::invoke(std::forward<_Visitor>(__v), get<_M>(std::forward<_Variant>(__var)));
}

template <size_t _M, class _R, class _Visitor, class _Variant>
constexpr _R __dispatch1r_fn(_Visitor&& __v, _Variant&& __var)
{
    return __XVI_STD_NS::invoke_r<_R>(std::forward<_Visitor>(__v), get<_M>(std::forward<_Variant>(__var)));
}

template <class _Visitor, class _Variant, size_t... _Idx>
constexpr auto __dispatch1_via_table(index_sequence<_Idx...>, _Visitor&& __vis, _Variant&& __var)
    -> invoke_result_t<_Visitor, variant_alternative_t<0, remove_reference_t<_Variant>>>
{
    using __dispatch1_fn_t = decltype(__dispatch1_fn<0, _Visitor, _Variant>);
    static_assert((std::is_same_v<__dispatch1_fn_t, decltype(__dispatch1_fn<_Idx, _Visitor, _Variant>)> && ...),
        "std::visit requires all methods to have the same result type");
    
    constexpr __dispatch1_fn_t* __table[] = {&__dispatch1_fn<_Idx, _Visitor, _Variant>...};
    return __table[__var.index()](std::forward<_Visitor>(__vis), std::forward<_Variant>(__var));
}

template <class _R, class _Visitor, class _Variant, size_t... _Idx>
constexpr _R __dispatch1r_via_table(index_sequence<_Idx...>, _Visitor&& __vis, _Variant&& __var)
{
    using __dispatch1r_fn_t = decltype(__dispatch1r_fn<0, _R, _Visitor, _Variant>);
    constexpr __dispatch1r_fn_t* __table[] = {&__dispatch1r_fn<_Idx, _R, _Visitor, _Variant>...};
    return __table[__var.index()](std::forward<_Visitor>(__vis), std::forward<_Variant>(__var));
}

} // namespace __detail


template <class _Visitor, class... _Variants>
constexpr decltype(auto) visit(_Visitor&& __vis, _Variants&&... __vars)
{
    if constexpr (sizeof...(_Variants) == 0)
        return __XVI_STD_NS::invoke(std::forward<_Visitor>(__vis));
    else if constexpr (sizeof...(_Variants) == 1)
    {
        // Dispatch via a table for constant-time behaviour.
        using _Variant = typename __detail::__nth_type_t<0, _Variants...>;
        using _V = std::remove_cvref_t<_Variant>;
        constexpr size_t _M = variant_size_v<_V>;

        return __detail::__dispatch1_via_table(std::make_index_sequence<_M>(),
                                               std::forward<_Visitor>(__vis),
                                               std::forward<_Variant>(__vars)...);
    }
    else
    {
        //! @TODO: implement.
        static_assert(sizeof...(_Variants) == 1);
    }
}

template <class _R, class _Visitor, class... _Variants>
constexpr _R visit(_Visitor&& __vis, _Variants&&... __vars)
{
    if constexpr (sizeof...(_Variants) == 0)
        return __XVI_STD_NS::invoke_r<_R>(__XVI_STD_NS::forward<_Visitor>(__vis));
    else if constexpr (sizeof...(_Variants) == 1)
    {
        // Dispatch via a table for constant-time behaviour.
        using _Variant = typename __detail::__nth_type_t<0, _Variants...>;
        constexpr size_t _M = variant_size_v<_Variant>;

        return __detail::__dispatch1r_via_table<_R>(std::make_index_sequence<_M>(),
                                                    std::forward<_Visitor>(__vis),
                                                    std::forward<_Variant>(__vars)...);
    }
    else
    {
        //! @TODO: implement.
        static_assert(sizeof...(_Variants) == 1);
    }
}


struct monostate {};
constexpr bool operator==(monostate, monostate) { return true; }
constexpr std::strong_ordering operator<=>(monostate, monostate) { return std::strong_ordering::equal; }


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_VARIANT_H */
