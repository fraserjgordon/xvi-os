#include <System/C++/LanguageSupport/New.hh>

#include <System/C++/Atomic/Atomic.hh>


// Fallback definitions of new and delete.


using std::size_t;
using std::nothrow_t;
using std::align_val_t;

using std::bad_alloc;


extern "C" [[gnu::weak]] void* __malloc(size_t) asm("System.Allocator.C.Malloc");
extern "C" [[gnu::weak]] void __free(void*) asm("System.Allocator.C.Free");


// Note: the C++ standard explicitly requires that the nothrow variants of new call the throwing variant.


[[gnu::weak]] void* operator new(size_t size)
{
    if (!__malloc)
        throw bad_alloc();

    auto ptr = __malloc(size);
    if (!ptr)
        throw std::bad_alloc();

    return ptr;    
}

[[gnu::weak]] void* operator new[](size_t size)
{
    return operator new(size);
}

[[gnu::weak]] void operator delete(void* ptr) noexcept
{
    if (ptr)
        __free(ptr);
}

[[gnu::weak]] void operator delete(void* ptr, size_t) noexcept
{
    operator delete(ptr);
}

[[gnu::weak]] void operator delete[](void* ptr) noexcept
{
    operator delete(ptr);
}

[[gnu::weak]] void operator delete[](void* ptr, size_t) noexcept
{
    operator delete[](ptr);
}

[[gnu::weak]] void* operator new(size_t size, const nothrow_t&) noexcept
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

[[gnu::weak]] void* operator new[](size_t size, const nothrow_t&) noexcept
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

[[gnu::weak]] void operator delete(void* ptr, const nothrow_t&) noexcept
{
    operator delete(ptr);
}

[[gnu::weak]] void operator delete[](void* ptr, const nothrow_t&) noexcept
{
    operator delete[](ptr);
}

[[gnu::weak]] void* operator new(std::size_t size, std::align_val_t align)
{
    //! @TODO: properly.
    return operator new(size);
}

[[gnu::weak]] void operator delete(void* ptr, std::align_val_t align) noexcept
{
    //! @TODO: properly.
    operator delete(ptr);
}
