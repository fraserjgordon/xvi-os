#ifndef __SYSTEM_TEXT_ENCODING_HEAP_HH
#define __SYSTEM_TEXT_ENCODING_HEAP_HH


#include <cstddef>
#include <span>


namespace System::Text::Encoding
{


struct conversion_heap
{
    using allocate_fn_t = void* (*)(void*, std::size_t min_size, std::size_t align, std::size_t* actual_size);
    using deallocate_fn_t = void (*)(void*, void* ptr, std::size_t size, std::size_t align);
    using reallocate_fn_t = void* (*)(void*, void* orig_ptr, std::size_t orig_size, std::size_t req_size, std::size_t align, std::size_t* actual_size);
    using expand_fn_t = void* (*)(void*, void* orig_ptr, std::size_t orig_size, std::size_t align, std::size_t expand_left, std::size_t expand_right, std::size_t* actual_size);
    using shrink_fn_t = void* (*)(void*, void* orig_ptr, std::size_t orig_size, std::size_t align, std::size_t reduce_left, std::size_t reduce_right, std::size_t* actual_size);

    void* user_data                 = nullptr;
    allocate_fn_t allocate_fn       = nullptr;
    deallocate_fn_t deallocate_fn   = nullptr;
    reallocate_fn_t reallocate_fn   = nullptr;
    expand_fn_t expand_fn           = nullptr;
    shrink_fn_t shink_fn            = nullptr;

    std::span<std::byte> allocate(std::size_t min_size, std::size_t align) const
    {
        std::size_t size;
        auto ptr = allocate_fn(user_data, min_size, align, &size);

        if (!ptr)
            return {};

        return {reinterpret_cast<std::byte*>(ptr), size};
    }

    void deallocate(void* ptr, std::size_t size, std::size_t align) const
    {
        deallocate_fn(user_data, ptr, size, align);
    }

    std::span<std::byte> reallocate(std::span<std::byte> orig, std::size_t req_size, std::size_t align) const
    {
        std::size_t size;
        auto ptr = reallocate_fn(user_data, orig.data(), orig.size(), req_size, align, &size);

        if (!ptr)
            return {};

        return {reinterpret_cast<std::byte*>(ptr), size};
    }

    std::span<std::byte> reallocate(void* ptr, std::size_t orig_size, std::size_t req_size, std::size_t align) const
    {
        return reallocate(std::span{reinterpret_cast<std::byte*>(ptr), orig_size}, req_size, align);
    }

    std::span<std::byte> expand(std::span<std::byte> orig, std::size_t align, std::size_t expand_left, std::size_t expand_right) const;

    std::span<std::byte> expand(void* ptr, std::size_t orig_size, std::size_t align, std::size_t expand_left, std::size_t expand_right) const;

    std::span<std::byte> shrink(std::span<std::byte> orig, std:: size_t align, std::size_t reduce_left, std::size_t reduce_right) const;

    std::span<std::byte> shrink(void* ptr, std::size_t orig_size, std::size_t align, std::size_t reduce_left, std::size_t reduce_right) const;
};


} // namespace System::Text::Encoding


#endif /* ifndef __SYSTEM_TEXT_ENCODING_HEAP_HH */
