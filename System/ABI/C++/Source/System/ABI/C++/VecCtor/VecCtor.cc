#include <System/ABI/C++/VecCtor.hh>

#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/Limits.hh>

#include <System/ABI/C++/Exception.hh>


namespace __cxxabiv1
{


void __cxa_throw_bad_array_new_length()
{
    throw std::bad_array_new_length();
}

template <class Dealloc>
static inline void* vec_new(std::size_t element_count, std::size_t element_size, std::size_t padding_size, void (*ctor)(void*), void (*dtor)(void*), void* (*alloc)(std::size_t), Dealloc dealloc)
{
    // Check that the requested array size is valid.
    if (std::numeric_limits<std::size_t>::max() / element_size > element_count)
        throw std::bad_array_new_length();
    
    // Allocate the array. This may throw or it may return nullptr.
    auto array_size = element_count * element_size;
    auto storage_size = array_size + padding_size;
    auto storage_ptr = alloc(storage_size);
    if (storage_ptr == nullptr)
        return nullptr;

    // Adjust the pointers to take any padding into account.
    void* array_ptr = storage_ptr;
    if (padding_size > 0)
    {
        // Skip the front padding on the array.
        array_ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(storage_ptr) + padding_size);
        
        // Set the cookie to the number of elements in the array.
        auto cookie_ptr = reinterpret_cast<std::size_t*>(array_ptr) - 1;
        *cookie_ptr = element_count;
    }

    // Call the constructor on each of the elements in the array.
    try
    {
        __cxa_vec_ctor(array_ptr, element_count, element_size, ctor, dtor);
    }
    catch (...)
    {
        // Deallocate the memory.
        dealloc(storage_ptr, storage_size);

        // Rethrow the exception.
        throw;
    }

    // Return a pointer to the array.
    return array_ptr;
}

template <class Dealloc>
static inline void vec_delete(void* array_ptr, std::size_t element_size, std::size_t padding_size, void (*dtor)(void*), Dealloc dealloc)
{
    // If we have a destructor, we need to call it on each element.
    auto storage_ptr = array_ptr;
    std::size_t storage_size = 0;
    if (dtor)
    {
        // Having a non-null destructor implies that padding_size is non-zero and so we have an array cookie.
        storage_ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(array_ptr) - padding_size);
        auto cookie_ptr = reinterpret_cast<std::size_t*>(array_ptr) - 1;
        auto element_count = *cookie_ptr;
        storage_size = padding_size + element_count + element_size;

        // Destroy each of the elements. If this throws, the storage still needs to be deallocated.
        try
        {
            __cxa_vec_dtor(array_ptr, element_count, element_size, dtor);
        }
        catch (...)
        {
            // Deallocate the storage and re-throw the exception.
            dealloc(storage_ptr, storage_size);
            throw;
        }
    }

    // Destructors have been run. Deallocate the storage.
    dealloc(storage_ptr, storage_size);
}

void* __cxa_vec_new(std::size_t element_count, std::size_t element_size, std::size_t padding_size, void (*ctor)(void*), void (*dtor)(void*))
{
    return __cxa_vec_new2(element_count, element_size, padding_size, ctor, dtor, &::operator new[], &::operator delete[]);
}

void* __cxa_vec_new2(std::size_t element_count, std::size_t element_size, std::size_t padding_size, void (*ctor)(void*), void (*dtor)(void*), void* (*alloc)(std::size_t), void (*dealloc)(void*))
{
    return vec_new(element_count, element_size, padding_size, ctor, dtor, alloc, [dealloc](void* ptr, std::size_t)
    {
        dealloc(ptr);
    });
}

void* __cxa_vec_new3(std::size_t element_count, std::size_t element_size, std::size_t padding_size, void (*ctor)(void*), void (*dtor)(void*), void* (*alloc)(std::size_t), void (*dealloc)(void*, std::size_t))
{
    return vec_new(element_count, element_size, padding_size, ctor, dtor, alloc, dealloc);
}

void __cxa_vec_delete(void* array_ptr, std::size_t element_size, std::size_t padding_size, void (*dtor)(void*))
{
    __cxa_vec_delete2(array_ptr, element_size, padding_size, dtor, &::operator delete[]);
}

void __cxa_vec_delete2(void* array_ptr, std::size_t element_size, std::size_t padding_size, void (*dtor)(void*), void (*dealloc)(void*))
{
    vec_delete(array_ptr, element_size, padding_size, dtor, [dealloc](void* ptr, std::size_t)
    {
        dealloc(ptr);
    });
}

void __cxa_vec_delete3(void* array_ptr, std::size_t element_size, std::size_t padding_size, void (*dtor)(void*), void (*dealloc)(void*, std::size_t))
{
    vec_delete(array_ptr, element_size, padding_size, dtor, dealloc);
}

void __cxa_vec_ctor(void* array_ptr, std::size_t element_count, std::size_t element_size, void (*ctor)(void*), void (*dtor)(void*))
{
    // Do nothing if there is no constructor.
    if (!ctor)
        return;
    
    // Call the constructor on each of the elements in the array.
    std::size_t init_index = 0;
    try
    {
        for (; init_index < element_count; ++init_index)
        {
            auto element = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(array_ptr) + init_index * element_size);
            ctor(element);
        }
    }
    catch (...)
    {
        // Clean up the constructed elements.
        __cxa_vec_cleanup(array_ptr, init_index, element_size, dtor);

        // Rethrow the exception.
        throw;
    }
}

void __cxa_vec_cctor(void* dest_array, void* src_array, std::size_t element_count, std::size_t element_size, void (*cctor)(void*, void*), void (*dtor)(void*))
{
    // Do nothing if there is no constructor.
    if (!cctor)
        return;
    
    // Call the constructor on each of the elements in the array.
    std::size_t init_index = 0;
    try
    {
        for (; init_index < element_count; ++init_index)
        {
            auto src_element = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(src_array) + init_index * element_size);
            auto dest_element = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(dest_array) + init_index * element_size);
            cctor(dest_element, src_element);
        }
    }
    catch (...)
    {
        // Clean up the constructed elements.
        __cxa_vec_cleanup(dest_array, init_index, element_size, dtor);

        // Rethrow the exception.
        throw;
    }
}

void __cxa_vec_dtor(void* array_ptr, std::size_t element_count, std::size_t element_size, void (*dtor)(void*))
{
    // Do nothing if there is no destructor to run.
    if (!dtor)
        return;

    std::size_t index = element_count;
    try
    {
        // Try to destroy each of the elements.
        for (; index > 0; --index)
        {
            auto element = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(array_ptr) + (index - 1) * element_size);
            dtor(element);
        }
    }
    catch (...)
    {
        // The destructor for this element failed; try to destroy the remaining elements.
        if (index > 0)
            __cxa_vec_cleanup(array_ptr, index - 1, element_size, dtor);

        // Rethrow the exception.
        throw;
    }
}

void __cxa_vec_cleanup(void* array_ptr, std::size_t element_count, std::size_t element_size, void (*dtor)(void*))
{
    // Do nothing if there is no destructor to run.
    if (!dtor)
        return;

    try
    {
        // Try to destroy each of the elements.
        for (std::size_t index = element_count; index > 0; --index)
        {
            auto element = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(array_ptr) + (index - 1) * element_size);
            dtor(element);
        }
    }
    catch (...)
    {
        // Fatal error: an exception was thrown from the destructor.
        System::ABI::CXX::terminate();
    }
}


} // namespace __cxxabiv1
