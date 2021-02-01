#ifndef __SYSTEM_ALLOCATOR_CLASSIC_CLASSIC_H
#define __SYSTEM_ALLOCATOR_CLASSIC_CLASSIC_H


#include <utility>

#include <System/Allocator/Classic/Private/Config.hh>


namespace System::Allocator::Classic
{


class Arena final
{
public:

    Arena(const Arena&) = delete;
    Arena(Arena&&) = delete;

    Arena& operator=(const Arena&) = delete;
    Arena& operator=(Arena&&) = delete;


    auto allocate(std::size_t size)
    {
        return arenaAllocate(this, size);
    }

    void free(void* ptr)
    {
        arenaFree(this, ptr);
    }

    auto reallocate(void* ptr, std::size_t size)
    {
        return arenaReallocate(this, ptr, size);
    }


    static Arena* createFromMemory(void* block, std::size_t size)
    {
        return arenaCreateFromMemory(block, size);
    }

    static void destroy(Arena* arena)
    {
        arenaDestroy(arena);
    }

private:

    Arena();
    ~Arena();

    __XVI_ALLOCATOR_CLASSIC_EXPORT static Arena*                         arenaCreateFromMemory(void*, std::size_t) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.Create);
    __XVI_ALLOCATOR_CLASSIC_EXPORT static void                           arenaDestroy(Arena*) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.Destroy);

    __XVI_ALLOCATOR_CLASSIC_EXPORT static std::pair<void*, std::size_t>  arenaAllocate(Arena*, std::size_t) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.Allocate);
    __XVI_ALLOCATOR_CLASSIC_EXPORT static std::pair<void*, std::size_t>  arenaAllocateAligned(Arena*, std::size_t, std::size_t) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.AllocateAligned);
    __XVI_ALLOCATOR_CLASSIC_EXPORT static void                           arenaFree(Arena*, void*) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.Free);
    __XVI_ALLOCATOR_CLASSIC_EXPORT static std::size_t                    arenaGetAllocationSize(Arena*, void*) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.GetAllocationSize);
    __XVI_ALLOCATOR_CLASSIC_EXPORT static std::size_t                    arenaResizeAllocation(Arena*, void*, std::size_t) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.ResizeAllocation);
    __XVI_ALLOCATOR_CLASSIC_EXPORT static std::pair<void*, std::size_t>  arenaReallocate(Arena*, void*, std::size_t) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.Reallocate);
    __XVI_ALLOCATOR_CLASSIC_EXPORT static void                           arenaAddMemoryBlock(Arena*, void*, std::size_t) __XVI_ALLOCATOR_CLASSIC_SYMBOL(Arena.AddMemoryBlock);
};


} // namespace System::Allocator::Classic


#endif /* ifndef __SYSTEM_ALLOCATOR_CLASSIC_CLASSIC_H */
