#pragma once
#ifndef __SYSTEM_CXX_UTILITY_MEMORYRESOURCE_H
#define __SYSTEM_CXX_UTILITY_MEMORYRESOURCE_H


#include <System/C++/Atomic/Atomic.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/C++/Utility/Memory.hh>
#include <System/C++/Utility/StdExcept.hh>
#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS::pmr
{


class memory_resource
{
public:

    memory_resource() = default;
    memory_resource(const memory_resource&) = default;

    virtual ~memory_resource() = default;

    memory_resource& operator=(const memory_resource&) = default;

    [[nodiscard]] void* allocate(size_t __bytes, size_t __alignment = __max_align)
    {
        return do_allocate(__bytes, __alignment);
    }

    void deallocate(void* __p, size_t __bytes, size_t __alignment = __max_align)
    {
        do_deallocate(__p, __bytes, __alignment);
    }

    bool is_equal(const memory_resource& __other) const noexcept
    {
        return do_is_equal(__other);
    }

private:

    static constexpr size_t __max_align = alignof(max_align_t);

    virtual void* do_allocate(size_t __bytes, size_t __alignment) = 0;
    virtual void  do_deallocate(void* __p, size_t __bytes, size_t __alignment) = 0;

    virtual bool  do_is_equal(const memory_resource&) const noexcept = 0;
};


inline bool operator==(const memory_resource& __a, const memory_resource& __b) noexcept
{
    return &__a == &__b || __a.is_equal(__b);
}

inline bool operator!=(const memory_resource& __a, const memory_resource& __b) noexcept
{
    return !(__a == __b);
}


namespace __detail
{

class __new_delete_memory_resource final
    : public memory_resource
{
private:

    void* do_allocate(size_t __bytes, size_t __alignment) final override
    {
        return ::operator new(__bytes, align_val_t(__alignment));
    }

    void do_deallocate(void* __p, size_t __bytes, size_t __alignment) final override
    {
        ::operator delete(__p, __bytes, align_val_t(__alignment));
    }

    bool do_is_equal(const memory_resource& __other) const noexcept final override
    {
        return this == &__other;
    }
};

class __null_memory_resource final
    : public memory_resource
{
private:

    void* do_allocate(size_t, size_t) final override
    {
        __XVI_CXX_UTILITY_THROW(bad_alloc());
    }

    void do_deallocate(void*, size_t, size_t) final override
    {
    }

    bool do_is_equal(const memory_resource& __other) const noexcept final override
    {
        return this == &__other;
    }
};


inline atomic<memory_resource*> _G_default_memory_resource {nullptr};

} // namespace __detail


inline memory_resource* new_delete_resource() noexcept
{
    static __detail::__new_delete_memory_resource _S_rsrc;
    return &_S_rsrc;
}

inline memory_resource* null_memory_resource() noexcept
{
    static __detail::__null_memory_resource _S_rsrc;
    return &_S_rsrc;
}

inline memory_resource* set_default_resource(memory_resource* __p) noexcept
{
    return __detail::_G_default_memory_resource.exchange(__p);
}

inline memory_resource* get_default_resource() noexcept
{
    memory_resource* __p = __detail::_G_default_memory_resource;
    if (__p == nullptr)
        __p = new_delete_resource();

    return __p;
}


template <class _Tp = byte>
class polymorphic_allocator
{
public:

    using value_type = _Tp;

    polymorphic_allocator() noexcept
        : polymorphic_allocator(get_default_resource())
    {
    }

    polymorphic_allocator(memory_resource* __r)
        : _M_rsrc(__r)
    {
    }

    polymorphic_allocator(const polymorphic_allocator&) = default;

    template <class _U>
    polymorphic_allocator(const polymorphic_allocator<_U>& __other) noexcept
        : _M_rsrc(__other._M_rsrc)
    {
    }

    polymorphic_allocator& operator=(const polymorphic_allocator&) = delete;

    [[nodiscard]] _Tp* allocate(size_t __n)
    {
        if (SIZE_MAX / sizeof(_Tp) < __n)
            __XVI_CXX_UTILITY_THROW(length_error("array length overflow"));

        return static_cast<_Tp*>(_M_rsrc->allocate(__n * sizeof(_Tp), alignof(_Tp)));
    }

    void deallocate(_Tp* __p, size_t __n)
    {
        _M_rsrc->deallocate(__p, __n * sizeof(_Tp), alignof(_Tp));
    }

    void* allocate_bytes(size_t __nbytes, size_t __alignment = alignof(max_align_t))
    {
        return _M_rsrc->allocate(__nbytes, __alignment);
    }

    void deallocate_bytes(void* __p, size_t __nbytes, size_t __alignment = alignof(max_align_t))
    {
        _M_rsrc->deallocate(__p, __nbytes, __alignment);
    }

    template <class _T>
    _T* allocate_object(size_t __n = 1)
    {
        if (SIZE_MAX / sizeof(_T) < __n)
            __XVI_CXX_UTILITY_THROW(length_error("allocation length overflow"));

        return static_cast<_T*>(allocate_bytes(__n * sizeof(_T), alignof(_T)));
    }

    template <class _T>
    void deallocate_object(_T* __p, size_t __n = 1)
    {
        deallocate_bytes(__p, __n * sizeof(_T), alignof(_T));
    }

    template <class _T, class... _CtorArgs>
    _T* new_object(_CtorArgs&&... __ctor_args)
    {
        _T* __p = allocate_object<_T>();
        __XVI_CXX_UTILITY_TRY
        {
            construct(__p, __XVI_STD_NS::forward<_CtorArgs>(__ctor_args)...);
        }
        __XVI_CXX_UTILITY_CATCH(...)
        {
            deallocate_object(__p);
            __XVI_CXX_UTILITY_RETHROW;
        }

        return __p;
    }

    template <class _T>
    void delete_object(_T* __p)
    {
        destroy(__p);
        deallocate_object(__p);
    }

    template <class _T, class... _Args>
    void construct(_T* __p, _Args&&... __args)
    {
        uninitialized_construct_using_allocator(__p, *this, __XVI_STD_NS::forward<_Args>(__args)...);
    }

    template <class _T>
    void destroy(_T* __p)
    {
        __p->~_T();
    }

    polymorphic_allocator select_on_container_copy_construction() const
    {
        return polymorphic_allocator();
    }

    memory_resource* resource() const
    {
        return _M_rsrc;
    }

private:

    memory_resource* _M_rsrc;
};


template <class _T1, class _T2>
bool operator==(const polymorphic_allocator<_T1>& __x, const polymorphic_allocator<_T2>& __y) noexcept
{
    return __x.resource() == __y.resource();
}

template <class _T1, class _T2>
bool operator!=(const polymorphic_allocator<_T1>& __x, const polymorphic_allocator<_T2>& __y) noexcept
{
    return !(__x == __y);
}


struct pool_options
{
    size_t max_blocks_per_chunk = 0;
    size_t largest_required_pool_block = 0;
};


// Declared but not implemented in this library.
class synchronized_pool_resource;
class unsychronized_pool_resource;
class monotonic_buffer_resource;


} // namespace __XVI_STD_UTILITY_NS::pmr


#endif /* ifndef __SYSTEM_CXX_UTILITY_MEMORYRESOURCE_H */
