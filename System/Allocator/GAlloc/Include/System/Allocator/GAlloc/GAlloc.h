#pragma once
#ifndef __SYSTEM_ALLOCATOR_GALLOC_GALLOC_H
#define __SYSTEM_ALLOCATOR_GALLOC_GALLOC_H


#include <System/C/LanguageSupport/StdBool.h>
#include <System/C/LanguageSupport/StdDef.h>

#include <System/Allocator/GAlloc/Export.h>


#ifdef __cplusplus
extern "C" {
#endif


struct galloc_opaque_context;
typedef struct galloc_opaque_context* galloc_context;

struct galloc_allocation_
{
    void*   ptr;
    size_t  size;
};

typedef struct galloc_allocation_ galloc_allocation;

struct galloc_opaque_suballocator;
typedef struct galloc_opaque_suballocator* galloc_suballocator;

typedef void                (*galloc_suballocator_free_fn)(void*);
typedef void                (*galloc_suballocator_free_sized_fn)(void*, size_t);
typedef bool                (*galloc_suballocator_resize_fn)(void*, size_t);
typedef size_t              (*galloc_suballocator_get_size_fn)(void*);
typedef galloc_allocation   (*galloc_suballocator_reallocate_fn)(void*, size_t);
typedef galloc_allocation   (*galloc_suballocator_reallocate_aligned_fn)(void*, size_t, size_t);

struct galloc_suballocator_info_
{
    galloc_suballocator_free_fn                 free_fn;
    galloc_suballocator_free_sized_fn           free_sized_fn;
    galloc_suballocator_resize_fn               resize_fn;
    galloc_suballocator_get_size_fn             get_size_fn;
    galloc_suballocator_reallocate_fn           reallocate_fn;
    galloc_suballocator_reallocate_aligned_fn   reallocate_aligned_fn;
};

typedef galloc_suballocator_info_ galloc_suballocator_info;


__GALLOC_EXPORT
galloc_allocation
galloc_allocate(size_t size)
    __GALLOC_SYMBOL(galloc_allocate);

__GALLOC_EXPORT
galloc_allocation
galloc_allocate_aligned(size_t size, size_t alignment)
    __GALLOC_SYMBOL(galloc_allocate_aligned);

__GALLOC_EXPORT
galloc_allocation
galloc_resize(void* allocation, size_t new_size)
    __GALLOC_SYMBOL(galloc_resize);

__GALLOC_EXPORT
galloc_allocation
galloc_reallocate(void* allocation, size_t new_size)
    __GALLOC_SYMBOL(galloc_reallocate);

__GALLOC_EXPORT
galloc_allocation
galloc_reallocate_aligned(void* allocation, size_t new_size, size_t new_alignment)
    __GALLOC_SYMBOL(galloc_reallocate_aligned);

__GALLOC_EXPORT
void
galloc_free(void* allocation)
    __GALLOC_SYMBOL(galloc_free);

__GALLOC_EXPORT
void
galloc_free_sized(void* allocation, size_t size)
    __GALLOC_SYMBOL(galloc_free_sized);

__GALLOC_EXPORT
size_t
galloc_get_size(void* allocation)
    __GALLOC_SYMBOL(galloc_get_size);

__GALLOC_EXPORT
galloc_suballocator
galloc_create_suballocator(const galloc_suballocator_info*)
    __GALLOC_SYMBOL(galloc_add_suballocator);

__GALLOC_EXPORT
void
galloc_delete_suballocator(galloc_suballocator)
    __GALLOC_SYMBOL(galloc_delete_suballocator);

__GALLOC_EXPORT
void
galloc_suballocator_register_arena(galloc_suballocator, void*, size_t)
    __GALLOC_SYMBOL(galloc_suballocator_register_arena);

__GALLOC_EXPORT
void
galloc_suballocator_unregister_arena(galloc_suballocator, void*)
    __GALLOC_SYMBOL(galloc_suballocator_unregister_arena);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_ALLOCATOR_GALLOC_GALLOC_H */
