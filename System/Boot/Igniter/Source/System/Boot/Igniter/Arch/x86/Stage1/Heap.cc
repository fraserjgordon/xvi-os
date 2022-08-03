#include <System/Boot/Igniter/Arch/x86/Stage1/Heap.hh>

#include <System/Allocator/Classic/Classic.hh>



namespace System::Boot::Igniter
{


using System::Allocator::Classic::Arena;


static Arena* s_heapArena = nullptr;


// Allocator hooks used by the C++ library.
extern "C" void* __malloc(std::size_t) asm("System.Allocator.C.Malloc");
extern "C" void __free(void*) asm("System.Allocator.C.Free");


void createHeap(std::uint32_t address, std::uint32_t size)
{
    s_heapArena = Arena::createFromMemory(reinterpret_cast<void*>(address), size);   
}


void* __malloc(std::size_t size)
{
    return s_heapArena->allocate(size).first;
}


void __free(void* ptr)
{
    s_heapArena->free(ptr);
}


}
