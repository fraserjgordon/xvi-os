#include <System/C/StdLib/stdlib.h>

#include <System/ABI/String/String.hh>
#include <System/C++/LanguageSupport/Limits.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Bit.hh>


extern "C" void* __malloc(size_t) asm("System.Allocator.C.Malloc");
extern "C" void  __free(void*) asm("System.Allocator.C.Free");
extern "C" void* __realloc(void*, size_t) asm("System.Allocator.C.Realloc");


void free(void* ptr)
{
    __free(ptr);
}

void* malloc(size_t n)
{
    return __malloc(n);
}

void* realloc(void* ptr, size_t n)
{
    return __realloc(ptr, n);
}


void* aligned_alloc(size_t alignment, size_t size)
{
    // Validate the alignment. It must be a power-of-two below an appropriate limit.
    if (alignment > alignof(std::max_align_t) || !std::has_single_bit(alignment))
        return nullptr;

    // If the size is less than the alignment, allocate an object with a size equal to the alignment -- our allocator
    // API only guarantees alignment < min(size, alignof(max_align_t)).
    if (alignment > size)
        size = alignment;

    return malloc(size);
}

void* calloc(size_t nmemb, size_t size)
{
    // Check for overflow.
    if (std::numeric_limits<size_t>::max() / size < nmemb)
        return nullptr;

    auto memsize = nmemb * size;
    auto ptr = malloc(memsize);

    if (!ptr)
        return nullptr;

    __memset(ptr, 0, memsize);

    return ptr;    
}
