#include <System/ABI/C++/Unwind/Unwinder.hh>


namespace System::ABI::CXX
{


// Weak imports of the memory allocation functions.
extern "C" [[gnu::weak]] void* malloc(std::size_t) asm("System.Allocator.C.Malloc");
extern "C" [[gnu::weak]] void  free(void*) asm("System.Allocator.C.Free");


Unwinder::~Unwinder() {}


void Unwinder::operator delete(void*)
{
    // Unwinder instances are all static (i.e never get deleted).
}


Unwinder::Context::~Context() {}

void* Unwinder::Context::operator new(std::size_t size) noexcept
{
    return malloc(size);
}

void Unwinder::Context::operator delete(void* ptr, std::size_t) noexcept
{
    free(ptr);
}


} // namespace System::ABI::CXX
