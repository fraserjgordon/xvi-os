#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <System/Allocator/Classic/Classic.hh>


namespace System::Boot::Igniter
{


namespace Allocator = System::Allocator::Classic;

static std::array<std::byte, 65536> s_initialHeap;
static Allocator::Arena* s_arena = nullptr;


extern "C" void* allocate(std::size_t size) asm("System.Allocator.C.Malloc");
extern "C" void free(void* ptr) asm("System.Allocator.C.Free");
extern "C" void* realloc(void* ptr, std::size_t size) asm("System.Allocator.C.Realloc");

void* allocate(std::size_t size)
{
    if (!s_arena)
        s_arena = Allocator::Arena::createFromMemory(s_initialHeap.data(), s_initialHeap.size());

    return s_arena->allocate(size).first;
}

void free(void* ptr)
{
    s_arena->free(ptr);
}

void* realloc(void* ptr, std::size_t size)
{
    return s_arena->reallocate(ptr, size).first;
}


} // namespace System::Boot::Igniter
