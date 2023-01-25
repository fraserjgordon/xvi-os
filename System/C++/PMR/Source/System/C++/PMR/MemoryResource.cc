#include <System/C++/PMR/MemoryResource.hh>


namespace __XVI_STD_PMR_NS::pmr
{


memory_resource::~memory_resource() = default;


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


atomic<memory_resource*> _G_default_memory_resource {nullptr};


} // namespace __detail


memory_resource* new_delete_resource() noexcept
{
    static __detail::__new_delete_memory_resource _S_rsrc;
    return &_S_rsrc;
}

memory_resource* null_memory_resource() noexcept
{
    static __detail::__null_memory_resource _S_rsrc;
    return &_S_rsrc;
}

memory_resource* set_default_resource(memory_resource* __p) noexcept
{
    return __detail::_G_default_memory_resource.exchange(__p);
}

memory_resource* get_default_resource() noexcept
{
    memory_resource* __p = __detail::_G_default_memory_resource;
    if (__p == nullptr)
        __p = new_delete_resource();

    return __p;
}


} // namespace __XVI_STD_PMR_NS::pmr
