#pragma once
#ifndef __SYSTEM_CXX_UTILITY_SCOPEDALLOCATOR_H
#define __SYSTEM_CXX_UTILITY_SCOPEDALLOCATOR_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>
#include <System/C++/Utility/AllocatorTraits.hh>



namespace __XVI_STD_UTILITY_NS
{


// Forward declarations.
template <class, class...> class scoped_allocator_adaptor;


namespace __detail
{


struct __no_inner_allocator_t
{
    __no_inner_allocator_t select_on_container_copy_construction() const noexcept
    {
        return {};
    }
};

template <class... _Allocs>
struct __inner_allocator { using __type = scoped_allocator_adaptor<_Allocs...>; };

template <>
struct __inner_allocator<> { using __type = __no_inner_allocator_t; };


} // namespace __detail


template <class _OuterAlloc, class... _InnerAllocs>
class scoped_allocator_adaptor
    : public _OuterAlloc
{
private:

    using _OuterTraits = allocator_traits<_OuterAlloc>;

public:

    using outer_allocator_type = _OuterAlloc;
    using inner_allocator_type = conditional_t<sizeof...(_InnerAllocs) == 0,
                                               scoped_allocator_adaptor<_OuterAlloc>,
                                               typename __detail::__inner_allocator<_InnerAllocs...>::__type>;

    using value_type            = typename _OuterTraits::value_type;
    using size_type             = typename _OuterTraits::size_type;
    using difference_type       = typename _OuterTraits::difference_type;
    using pointer               = typename _OuterTraits::pointer;
    using const_pointer         = typename _OuterTraits::const_pointer;
    using void_pointer          = typename _OuterTraits::void_pointer;
    using const_void_pointer    = typename _OuterTraits::const_void_pointer;

    using propagate_on_container_copy_assignment = bool_constant<(_OuterTraits::propagate_on_container_copy_assignment::value
                                                                  || ...
                                                                  || allocator_traits<_InnerAllocs>::propagate_on_container_copy_assignment::value)>;
    using propagate_on_container_move_assignment = bool_constant<(_OuterTraits::propagate_on_container_move_assignment::value
                                                                  || ...
                                                                  || allocator_traits<_InnerAllocs>::propagate_on_container_move_assignment::value)>;
    using propagate_on_container_swap            = bool_constant<(_OuterTraits::propagate_on_container_swap::value
                                                                  || ...
                                                                  || allocator_traits<_InnerAllocs>::propagate_on_container_swap::value)>;
    using is_always_equal                        = bool_constant<(_OuterTraits::is_always_equal::value
                                                                  && ...
                                                                  && allocator_traits<_InnerAllocs>::is_always_equal::value)>;

    template <class _Tp>
    struct rebind
    {
        using other = scoped_allocator_adaptor<typename _OuterTraits::template rebind_alloc<_Tp>, _InnerAllocs...>;
    };

    scoped_allocator_adaptor()
        : _OuterAlloc(),
          _M_inner()
    {
    }

    template <class _OuterA2>
        requires std::is_constructible_v<_OuterAlloc, _OuterA2>
    scoped_allocator_adaptor(_OuterA2&& __outer_alloc, const _InnerAllocs&... __inner_allocs) noexcept
        : _OuterAlloc(std::forward<_OuterA2>(__outer_alloc)),
          _M_inner(__inner_allocs...)
    {
    }

    scoped_allocator_adaptor(const scoped_allocator_adaptor& __other) noexcept
        : _OuterAlloc(__other.outer_allocator()),
          _M_inner(__other.inner_allocator())
    {
    }

    scoped_allocator_adaptor(scoped_allocator_adaptor&& __other) noexcept
        : _OuterAlloc(std::move(__other.outer_allocator())),
          _M_inner(std::move(__other.inner_allocator()))
    {
    }

    template <class _OuterA2>
        requires std::is_constructible_v<_OuterAlloc, const _OuterA2&>
    scoped_allocator_adaptor(const scoped_allocator_adaptor<_OuterA2, _InnerAllocs...>& __other) noexcept
        : _OuterAlloc(__other.outer_allocator()),
          _M_inner(__other.inner_allocator())
    {
    }

    template <class _OuterA2>
        requires std::is_constructible_v<_OuterAlloc, _OuterA2>
    scoped_allocator_adaptor(scoped_allocator_adaptor<_OuterA2, _InnerAllocs...>&& __other) noexcept
        : _OuterAlloc(std::move(__other.outer_allocator())),
          _M_inner(__other.inner_allocator())
    {
    }

    scoped_allocator_adaptor& operator=(const scoped_allocator_adaptor&) = default;
    scoped_allocator_adaptor& operator=(scoped_allocator_adaptor&&) = default;

    ~scoped_allocator_adaptor() = default;

    inner_allocator_type& inner_allocator() noexcept
    {
        if constexpr (sizeof...(_InnerAllocs) == 0)
            return *this;
        else
            return _M_inner;
    }

    const inner_allocator_type& inner_allocator() const noexcept
    {
        if constexpr (sizeof...(_InnerAllocs) == 0)
            return *this;
        else
            return _M_inner;
    }

    outer_allocator_type& outer_allocator() noexcept
    {
        return static_cast<_OuterAlloc&>(*this);
    }
    
    const outer_allocator_type& outer_allocator() const noexcept
    {
        return static_cast<const _OuterAlloc&>(*this);
    }

    [[nodiscard]] pointer allocate(size_type __n)
    {
        return allocator_traits<_OuterAlloc>::allocate(outer_allocator(), __n);
    }

    [[nodiscard]] pointer allocate(size_type __n, const_void_pointer __hint)
    {
        return allocator_traits<_OuterAlloc>::allocate(outer_allocator(), __n, __hint);
    }

    void deallocate(pointer __p, size_type __n)
    {
        allocator_traits<_OuterAlloc>::deallocate(outer_allocator(), __p, __n);
    }

    size_type max_size() const
    {
        return allocator_traits<_OuterAlloc>::max_size(outer_allocator());
    }

    template <class _T, class... _Args>
    void construct(_T* __p, _Args&&... __args)
    {
        apply
        (
            [__p, this](auto&&... __newargs)
            {
                _OUTERMOST_ALLOC_TRAITS<scoped_allocator_adaptor>::construct
                (
                    _OUTERMOST(*this),
                    __p,
                    __XVI_STD_NS::forward<decltype(__newargs)>(__newargs)...
                );
            },
            uses_allocator_construction_args<_T>(inner_allocator(), std::forward<_Args>(__args)...)
        );
    }

    template <class _T>
    void destroy(_T* __p)
    {
        _OUTERMOST_ALLOC_TRAITS<scoped_allocator_adaptor>::destroy(_OUTERMOST(*this), __p);
    }

    scoped_allocator_adaptor select_on_container_copy_construction() const
    {
        return {_OuterTraits::select_on_container_copy_construction(outer_allocator()), _M_inner.select_on_container_copy_construction()};
    }

private:

    using __inner_t = typename __detail::__inner_allocator<_InnerAllocs...>::__type;

    [[no_unique_address]] __inner_t _M_inner {};

    template <class _X>
    static auto& _OUTERMOST(_X& __x) noexcept
    {
        if constexpr (requires(_X& __x) { __x.outer_allocator(); })
            return _OUTERMOST(__x.outer_allocator());
        else
            return __x;
    }

    template <class _X>
    using _OUTERMOST_ALLOC_TRAITS = allocator_traits<remove_reference_t<decltype(_OUTERMOST(declval<_X&>()))>>;

    // Used by select_on_container_copy_construction().
    scoped_allocator_adaptor(_OuterAlloc __a, __inner_t __inner) :
        _OuterAlloc(std::move(__a)),
        _M_inner(std::move(__inner))
    {
    }
};

template <class _OuterAlloc, class... _InnerAllocs>
scoped_allocator_adaptor(_OuterAlloc, _InnerAllocs...)
    -> scoped_allocator_adaptor<_OuterAlloc, _InnerAllocs...>;


template <class _OuterA1, class _OuterA2, class... _InnerAllocs>
bool operator==(const scoped_allocator_adaptor<_OuterA1, _InnerAllocs...>& __x, const scoped_allocator_adaptor<_OuterA2, _InnerAllocs...>& __y) noexcept
{
    if constexpr (sizeof...(_InnerAllocs) == 0)
        return __x.outer_allocator() == __y.outer_allocator();
    else
        return __x.outer_allocator() == __y.outer_allocator() && __x.inner_allocator() == __y.inner_allocator();
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_SCOPEDALLOCATOR_H */
