#include <System/ABI/TLS/Arch/x86/TLS.hh>

#include <System/ABI/String/String.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/TLS/ImageInternal.hh>
#include <System/ABI/TLS/Terminate.hh>
#include <System/ABI/TLS/WeakMalloc.hh>


// System calls for configuring the segment registers.
//! @todo: remove weak attribute.
extern "C" [[gnu::weak]] void __set_fs_base(std::uintptr_t base) asm("Syscall.X86.SetFsBase");
extern "C" void __set_gs_base(std::uintptr_t base) asm("Syscall.X86.SetGsBase");


namespace System::ABI::TLS
{


// The size of the thread data area, excluding the TLS data.
static std::size_t g_threadDataSize = 0;

// The offset from the thread data pointer where the DTV pointer can be found.
static std::size_t g_dtvOffset = 0;

// Returns the linear pointer equivalent to %gs:0 (or %fs:0) for 64-bit (32-bit), respectively.
static inline void* getLinearPtr()
{
#if __x86_64__
    void* value = nullptr;
    asm
    (
        "movq   %%fs:0, %0"
        : "=r" (value)
        :
        :
    );
    return value;
#else
    void* value = nullptr;
    asm
    (
        "movl   %%gs:0, %0"
        : "=r" (value)
        :
        :
    );
    return value;
#endif
}


static inline std::uintptr_t* getDTV()
{
#if __x86_64__
    std::uintptr_t* value;
    asm
    (
        "movq   %%fs:(%1), %0"
        : "=r" (value)
        : "r" (g_dtvOffset)
        :
    );
    return value;
#else
    std::uintptr_t* value;
    asm
    (
        "movl   %%gs:(%1), %0"
        : "=r" (value)
        : "r" (g_dtvOffset)
        :
    );
    return value;
#endif
}

static inline void setDVT(std::uintptr_t* vector)
{
#if __x86_64__
    asm
    (
        "movq   %0, %%fs:(%1)"
        :
        : "r" (vector), "r" (g_dtvOffset)
        : "memory"
    );
#else
    asm
    (
        "movl   %0, %%gs:(%1)"
        :
        : "r" (vector), "r" (g_dtvOffset)
        : "memory"
    );
#endif
}

// Calculates the size of space needed for the initial image's TLS data, including any alignment padding.
static std::size_t initialImageSize(const image_info_t& initial_image)
{
    return (initial_image.image_size + initial_image.image_alignment - 1) & ~(initial_image.image_alignment - 1);
}

// Calculates the size needed when adding the given image to the existing size.
static std::size_t addImageSize(std::size_t size, const image_info_t& image)
{
    return (size + image.image_size + image.image_alignment - 1) & ~(image.image_alignment - 1);
}

// Calculates the size of the given set of static images.
static std::size_t threadDataSize(const image_info_t& initial_image, std::span<const image_info_t> static_images)
{
    // Size for the initial image.
    std::size_t size = initialImageSize(initial_image);

    // Add in the size for each of the static images.
    for (const auto& i : static_images)
        size = addImageSize(size, i);

    // Finally, add in the size for the thread data.
    size += g_threadDataSize;

    return size;
}


void setThreadDataLayout(std::size_t size, std::size_t dtv_offset)
{
    // Die if already configured or if the given configuration is invalid.
    //  - the size must be large enough for the linear pointer and the DVT pointer
    //  - the DTV pointer must be aligned properly
    //  - the DTV pointer must fit within the given size
    //  - the DVT pointer must not overlap the linear pointer (i.e must not be at offset zero).
    if (g_threadDataSize != 0 || size < 2*sizeof(void*) || dtv_offset % alignof(void*) != 0 || dtv_offset >= size || dtv_offset == 0)
        __terminate();

    g_threadDataSize = size;
    g_dtvOffset = dtv_offset;
}

void configureInitialThread(std::span<std::byte> preallocated)
{
    // This function can only be called once. If a second call is made, this call to end the static configuration phase
    // will terminate.
    endStaticConfiguration();

    // Get the initial image data and the list of static images.
    auto initial_image = getInitialImage();
    auto static_images = getStaticImages();

    // Preallocated space is incompatible with having static images beyond the initial image (because the static images
    // are still dynamically loaded and there's no way to guarantee the preallocated memory will be the right size).
    if (!preallocated.empty() && static_images.size() != 0)
        __terminate();

    // Get the amount of space required for the thread data area.
    auto data_size = threadDataSize(initial_image, static_images);

    // The data size has to exactly match the preallocated size (to catch any linker script errors).
    if (!preallocated.empty() && preallocated.size() != data_size)
        __terminate();

    // Allocate some memory for the thread data area.
    std::byte* data = preallocated.data();
    if (!data)
    {
        data = reinterpret_cast<std::byte*>(__malloc(data_size));
        if (!data)
            __terminate();
    }

    // The TLS data is arranged at a negative offset from the thread pointer. Calculate that thread pointer and copy the
    // TLS template data into place.
    auto thread_pointer = data + data_size - g_threadDataSize;
    auto offset = initialImageSize(initial_image);
    __memcpy(thread_pointer - offset, initial_image.initial_bytes.data(), initial_image.initial_bytes.size());
    for (const auto& i : static_images)
    {
        offset = addImageSize(offset, i);
        __memcpy(thread_pointer - offset, i.initial_bytes.data(), i.initial_bytes.size());
    }

    // Set the linear pointer and DTV fields.
    *reinterpret_cast<void**>(thread_pointer) = thread_pointer;
    *reinterpret_cast<void**>(thread_pointer + g_dtvOffset) = nullptr;

    // Configure the base address of the appropriate segment register.
#if __x86_64__
    __set_fs_base(reinterpret_cast<std::uintptr_t>(thread_pointer));
#else
    __set_gs_base(reinterpret_cast<std::uintptr_t>(thread_pointer));
#endif
}


} // namespace System::ABI::TLS


#if !defined(__x86_64__)
void* ___tls_get_addr(__tls_index* ti)
#else
void* __tls_get_addr(__tls_index* ti)
#endif
{
    //! @TODO: this method isn't signal-safe! Needs reworked.
    
    using namespace System::ABI::TLS;

    // Get the pointer to the dynamic thread vector (DTV).
    auto* dvt = getDTV();

    // Get the generation number (the first entry in the DVT). It identifies the length of the vector.
    if (!dvt || dvt[0] < ti->ti_module) [[unlikely]]
    {
        // The DVT needs to be allocated (or extended). If there's no malloc implementation, we need to fail.
        if (!__malloc)
            __terminate();

        // Allocate a replacement vector.
        auto new_images = getImageCount();
        auto new_length = new_images + 1;
        auto old_images = dvt ? dvt[0] : 0;
        auto new_vector = static_cast<std::uintptr_t*>(__malloc(sizeof(std::uintptr_t) * new_length));
        if (!new_vector)
        {
            // There's no reasonable recovery mechanism so we need to fail.
            __terminate();
        }

        // Copy over the old DVT entries, zero the new ones and set the generation number.
        auto extra_images = new_images - old_images;
        __memcpy(&new_vector[1], &dvt[1], sizeof(std::uintptr_t) * old_images);
        __memset(&new_vector[old_images + 1], 0, extra_images * sizeof(std::uintptr_t));
        new_vector[0] = new_images;

        // Update the saved DVT and free the old DVT.
        setDVT(new_vector);
        __free(dvt);
        dvt = new_vector;
    }

    // Do we need to allocate memory for these thread-local variables?
    if (dvt[ti->ti_module] == 0) [[unlikely]]
    {
        // Get the information about this module's thread-local variables.
        auto info = getDynamicImage(ti->ti_module);   
        if (!info)
            __terminate();     

        // Allocate space for this image's thread-local variables.
        //
        // Note: the DVT has been allocated so we don't need to check the validity of the __malloc import here.
        auto memory = static_cast<std::byte*>(__malloc(info->image_size));
        if (!memory)
            __terminate();

        // Copy over the initial contents of the thread-local variables.
        __memcpy(memory, info->initial_bytes.begin(), info->initial_bytes.size());

        // Zero the rest of the variables.
        __memset(memory + info->initial_bytes.size(), 0, info->image_size - info->initial_bytes.size());

        // Update the DVT entry for this module.
        dvt[ti->ti_module] = reinterpret_cast<std::uintptr_t>(memory);
    }

    // Return the pointer as requested.
    return reinterpret_cast<void*>(dvt[ti->ti_module] + ti->ti_offset);
}
