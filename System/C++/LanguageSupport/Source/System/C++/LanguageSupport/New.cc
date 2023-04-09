#include <System/C++/LanguageSupport/New.hh>

#include <System/C++/Atomic/Atomic.hh>


using __XVI_STD_NS::size_t;
using __XVI_STD_NS::align_val_t;
using __XVI_STD_NS::nothrow_t;

using __XVI_STD_LANGSUPPORT_NS::bad_alloc;


extern "C" [[gnu::weak]] void* __malloc(size_t) asm("System.Allocator.C.Malloc");
extern "C" [[gnu::weak]] void __free(void*) asm("System.Allocator.C.Free");


namespace __XVI_STD_LANGSUPPORT_NS_DECL
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


} // namespace __XVI_STD_LANGSUPPORT_NS_DECL


//! @TODO: aligned operators.

// Note: the C++ standard explicitly requires that the nothrow variants of new call the throwing variant.


#if !defined(__XVI_HOSTED)
void* operator new(size_t size)
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
            throw bad_alloc();

        // The handler returned; try to allocate again.
        ptr = __malloc(size);
    }

    return ptr;    
}

void* operator new[](size_t size)
{
    return operator new(size);
}

void operator delete(void* ptr) noexcept
{
    if (ptr)
        __free(ptr);
}

void operator delete(void* ptr, size_t) noexcept
{
    operator delete(ptr);
}

void operator delete[](void* ptr) noexcept
{
    operator delete(ptr);
}

void operator delete[](void* ptr, size_t) noexcept
{
    operator delete[](ptr);
}

void* operator new(size_t size, const nothrow_t&) noexcept
{
    try
    {
        return operator new(size);
    }
    catch (bad_alloc&)
    {
        return nullptr;
    }
}

void* operator new[](size_t size, const nothrow_t&) noexcept
{
    try
    {
        return operator new[](size);
    }
    catch (bad_alloc&)
    {
        return nullptr;
    }
}

void operator delete(void* ptr, const nothrow_t&) noexcept
{
    operator delete(ptr);
}

void operator delete[](void* ptr, const nothrow_t&) noexcept
{
    operator delete[](ptr);
}

void* operator new(std::size_t size, std::align_val_t align)
{
    //! @TODO: properly.
    return operator new(size);
}

void operator delete(void* ptr, std::align_val_t align) noexcept
{
    //! @TODO: properly.
    operator delete(ptr);
}
#endif // if !defined(__XVI_HOSTED)
