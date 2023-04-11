#include <System/C++/Allocator/New.hh>

#include <System/C++/Atomic/Atomic.hh>


//! @TODO: aligned operators.


using __XVI_STD_NS::size_t;
using __XVI_STD_NS::__detail::__align_val_t;
using __XVI_STD_NS::__detail::__nothrow_t;

#if defined(__XVI_HOSTED)
using std::bad_alloc;
#else
using __XVI_STD_NS::bad_alloc;
#endif


extern "C" [[gnu::weak]] void* __malloc(size_t) asm("System.Allocator.C.Malloc");
extern "C" [[gnu::weak]] void __free(void*) asm("System.Allocator.C.Free");


namespace __XVI_STD_ALLOCATOR_NS_DECL
{


atomic<new_handler> g_newHandler = nullptr;


new_handler get_new_handler() noexcept
{
    return g_newHandler;
}

new_handler set_new_handler(new_handler handler) noexcept
{
    return g_newHandler.exchange(handler);
}


} // namespace __XVI_STD_ALLOCATOR_NS_DECL


#if !defined(__XVI_HOSTED)
__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_new")]]
void* operator new(size_t);

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_new_array")]]
void* operator new[](size_t);

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_new_nothrow")]]
void* operator new(size_t, const __nothrow_t&) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_new_array_nothrow")]]
void* operator new[](size_t, const __nothrow_t&) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_new_aligned")]]
void* operator new(size_t, __align_val_t);

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_delete")]]
void operator delete(void*) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_delete_sized")]]
void operator delete(void*, size_t) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_delete_array")]]
void operator delete[](void*) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_delete_array_sized")]]
void operator delete[](void*, size_t) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_delete_nothrow")]]
void operator delete(void*, const __nothrow_t&) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_delete_array_nothrow")]]
void operator delete[](void*, const __nothrow_t&) noexcept;

__SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE
[[gnu::alias("__default_operator_delete_aligned")]]
void operator delete(void*, __align_val_t) noexcept;
#endif // if !defined(__XVI_HOSTED)


namespace __XVI_STD_ALLOCATOR_NS_DECL
{


namespace __detail
{


static void* __do_new(size_t size)
{
    if (!__malloc)
        throw bad_alloc();

    auto ptr = __malloc(size);
    while (!ptr)
    {
        // Call the new handler (if any) to try to free up some more space.
        auto handler = __XVI_STD_LANGSUPPORT_NS::get_new_handler();
        if (handler)
            handler();
        else
            return nullptr;

        // The handler returned; try to allocate again.
        ptr = __malloc(size);
    }

    return ptr;
}


// Note: the C++ standard explicitly requires that the nothrow variants of new call the throwing variant.
void* __default_operator_new(size_t size)
{
    void* ptr = __do_new(size);

    if (!ptr)
        throw bad_alloc();

    return ptr;
}

void* __default_operator_new_array(size_t size)
{
    return operator new(size);
}

void __default_operator_delete(void* ptr) noexcept
{
    if (ptr)
        __free(ptr);
}

void __default_operator_delete_sized(void* ptr, size_t) noexcept
{
    operator delete(ptr);
}

void __default_operator_delete_array(void* ptr) noexcept
{
    operator delete(ptr);
}

void __default_operator_delete_array_sized(void* ptr, size_t) noexcept
{
    operator delete[](ptr);
}

void* __default_operator_new_nothrow(size_t size, const __nothrow_t&) noexcept
{
    // If we know that ::operator new is the default implementation, we can skip it and call the allocation method
    // directly to avoid exceptions on allocation failure. We still need the try block, however, as the registered new
    // handler may throw.
    void* (*new_impl)(size_t) = &::operator new;
    if (new_impl == &__default_operator_new) try
    {
        return __do_new(size);
    }
    catch (const bad_alloc& e)
    {
        return nullptr;
    }

    try
    {
        return operator new(size);
    }
    catch (bad_alloc&)
    {
        return nullptr;
    }
}

void* __default_operator_new_array_nothrow(size_t size, const __nothrow_t&) noexcept
{
    // If we know that ::operator new is the default implementation, we can skip it and call the allocation method
    // directly to avoid exceptions on allocation failure. We still need the try block, however, as the registered new
    // handler may throw.
    void* (*new_impl)(size_t) = &::operator new[];
    if (new_impl == &__default_operator_new_array) try
    {
        return __do_new(size);
    }
    catch (const bad_alloc& e)
    {
        return nullptr;
    }

    try
    {
        return operator new[](size);
    }
    catch (bad_alloc&)
    {
        return nullptr;
    }
}

void __default_operator_delete_nothrow(void* ptr, const __nothrow_t&) noexcept
{
    operator delete(ptr);
}

void __default_operator_delete_array_nothrow(void* ptr, const __nothrow_t&) noexcept
{
    operator delete[](ptr);
}

void* __default_operator_new_aligned(std::size_t size, __align_val_t align)
{
    //! @TODO: properly.
    return operator new(size);
}

void __default_operator_delete_aligned(void* ptr, __align_val_t align) noexcept
{
    //! @TODO: properly.
    operator delete(ptr);
}


} // namespace __detail


} // namespace __XVI_STD_ALLOCATOR_NS_DECL
