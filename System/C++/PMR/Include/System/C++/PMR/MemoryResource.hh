#ifndef __SYSTEM_CXX_UTILITY_MEMORYRESOURCE_H
#define __SYSTEM_CXX_UTILITY_MEMORYRESOURCE_H


#include <System/C++/Atomic/Atomic.hh>
#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/New.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Memory.hh>
#include <System/C++/Utility/StdExcept.hh>

#include <System/C++/PMR/Private/Config.hh>


namespace __XVI_STD_PMR_NS::pmr
{


__XVI_CXX_PMR_EXPORT class memory_resource
{
public:

    memory_resource() = default;
    memory_resource(const memory_resource&) = default;

    virtual ~memory_resource();

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


__XVI_CXX_PMR_EXPORT memory_resource* new_delete_resource() noexcept;
__XVI_CXX_PMR_EXPORT memory_resource* null_memory_resource() noexcept;

__XVI_CXX_PMR_EXPORT memory_resource* get_default_resource() noexcept;
__XVI_CXX_PMR_EXPORT memory_resource* set_default_resource(memory_resource*) noexcept;


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
        if (__XVI_STD_NS::numeric_limits<size_t>::max() / sizeof(_Tp) < __n)
            throw __XVI_STD_NS::length_error("array length overflow");

        return static_cast<_Tp*>(_M_rsrc->allocate(__n * sizeof(_Tp), alignof(_Tp)));
    }

    void deallocate(_Tp* __p, size_t __n)
    {
        _M_rsrc->deallocate(__p, __n * sizeof(_Tp), alignof(_Tp));
    }

    [[nodiscard]] void* allocate_bytes(size_t __nbytes, size_t __alignment = alignof(max_align_t))
    {
        return _M_rsrc->allocate(__nbytes, __alignment);
    }

    void deallocate_bytes(void* __p, size_t __nbytes, size_t __alignment = alignof(max_align_t))
    {
        _M_rsrc->deallocate(__p, __nbytes, __alignment);
    }

    template <class _T>
    [[nodiscard]] _T* allocate_object(size_t __n = 1)
    {
        if (__XVI_STD_NS::numeric_limits<size_t>::max() / sizeof(_T) < __n)
            throw length_error("allocation length overflow");

        return static_cast<_T*>(allocate_bytes(__n * sizeof(_T), alignof(_T)));
    }

    template <class _T>
    void deallocate_object(_T* __p, size_t __n = 1)
    {
        deallocate_bytes(__p, __n * sizeof(_T), alignof(_T));
    }

    template <class _T, class... _CtorArgs>
    [[nodiscard]] _T* new_object(_CtorArgs&&... __ctor_args)
    {
        _T* __p = allocate_object<_T>();
        try
        {
            construct(__p, __XVI_STD_NS::forward<_CtorArgs>(__ctor_args)...);
        }
        catch (...)
        {
            deallocate_object(__p);
            throw;;
        }

        return __p;
    }

    template <class _T>
    void delete_object(_T* __p)
    {
        allocator_traits<polymorphic_allocator>::destroy(*this, __p);
        deallocate_object(__p);
    }

    template <class _T, class... _Args>
    void construct(_T* __p, _Args&&... __args)
    {
        uninitialized_construct_using_allocator(__p, *this, std::forward<_Args>(__args)...);
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


struct pool_options
{
    size_t max_blocks_per_chunk = 0;
    size_t largest_required_pool_block = 0;
};


__XVI_CXX_PMR_EXPORT class synchronized_pool_resource :
    public memory_resource
{
public:

    synchronized_pool_resource(const pool_options& __opts, memory_resource* __upstream);

    synchronized_pool_resource() :
        synchronized_pool_resource(pool_options{}, get_default_resource())
    {
    }

    explicit synchronized_pool_resource(memory_resource* __upstream) :
        synchronized_pool_resource(pool_options{}, __upstream)
    {
    }

    explicit synchronized_pool_resource(const pool_options& __opts) :
        synchronized_pool_resource(__opts, get_default_resource())
    {
    }

    synchronized_pool_resource(const synchronized_pool_resource&) = delete;

    virtual ~synchronized_pool_resource();

    synchronized_pool_resource& operator=(const synchronized_pool_resource&) = delete;

    void release();

    memory_resource* upstream_resource() const;

    pool_options options() const;

protected:

    void* do_allocate(size_t __bytes, size_t __alignment) override;
    void do_deallocate(void* __p, size_t __bytes, size_t __alignment) override;
    bool do_is_equal(const memory_resource& __other) const noexcept override;
};


__XVI_CXX_PMR_EXPORT class unsynchronized_pool_resource :
    public memory_resource
{
public:

    unsynchronized_pool_resource(const pool_options& __opts, memory_resource* __upstream);
    
    unsynchronized_pool_resource() :
        unsynchronized_pool_resource(pool_options{}, get_default_resource())
    {
    }

    explicit unsynchronized_pool_resource(memory_resource* __upstream) :
        unsynchronized_pool_resource(pool_options{}, __upstream)
    {
    }

    explicit unsynchronized_pool_resource(const pool_options& __opts) :
        unsynchronized_pool_resource(__opts, get_default_resource())
    {
    }

    unsynchronized_pool_resource(const unsynchronized_pool_resource&) = delete;

    virtual ~unsynchronized_pool_resource();

    unsynchronized_pool_resource& operator=(const unsynchronized_pool_resource&) = delete;

    void release();

    memory_resource* upstream_resource() const;

    pool_options options() const;

protected:

    void* do_allocate(size_t __size, size_t __alignment) override;
    void do_deallocate(void* __p, size_t __size, size_t __alignment) override;
    bool do_is_equal(const memory_resource& __other) const noexcept override;
};


__XVI_CXX_PMR_EXPORT class monotonic_buffer_resource :
    public memory_resource
{
public:

    explicit monotonic_buffer_resource(memory_resource* __upstream);

    monotonic_buffer_resource(size_t __initial_size, memory_resource* __upstream);

    monotonic_buffer_resource(void* __buffer, size_t __buffer_size, memory_resource* __upstream);

    monotonic_buffer_resource() :
        monotonic_buffer_resource(get_default_resource())
    {
    }

    explicit monotonic_buffer_resource(size_t __initial_size) :
        monotonic_buffer_resource(__initial_size, get_default_resource())
    {
    }

    monotonic_buffer_resource(void* __buffer, size_t __initial_size) :
        monotonic_buffer_resource(__buffer, __initial_size, get_default_resource())
    {
    }

    monotonic_buffer_resource(const monotonic_buffer_resource&) = delete;

    virtual ~monotonic_buffer_resource();

    monotonic_buffer_resource& operator=(const monotonic_buffer_resource&) = delete;

    void release();

    memory_resource* upstream_resource() const;

protected:

    void* do_allocate(size_t __bytes, size_t __alignment) override;
    void do_deallocate(void* __p, size_t __bytes, size_t __alignment) override;
    bool do_is_equal(const memory_resource& __other) const noexcept override;
};


} // namespace __XVI_STD_UTILITY_NS::pmr


#endif /* ifndef __SYSTEM_CXX_UTILITY_MEMORYRESOURCE_H */
