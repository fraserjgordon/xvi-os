#include <System/ABI/TLS/Arch/MIPS/TLS.hh>

#include <System/ABI/String/String.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/TLS/ImageInternal.hh>
#include <System/ABI/TLS/Terminate.hh>
#include <System/ABI/TLS/WeakMalloc.hh>


namespace System::ABI::TLS
{


// Note: this is essentially a carbon-copy of the PowerPC TLS ABI.


//! @todo: remove weak attribute.
extern "C" [[gnu::weak]] void __set_tp(std::uintptr_t base) asm("Syscall.MIPS.SetThreadPointer");


// The ABI-mandated size of the thread control block (TCB).
constexpr std::size_t TCBSize = 8;

// The ABI-mandated maximum size of the thread data area.
constexpr std::size_t ThreadDataSizeMax = (0x1000 - TCBSize);

// The ABI-mandated offset from the thread pointer to the beginning of the TCB.
constexpr std::size_t ThreadPointerOffset = (0x7000 + TCBSize);

// The ABI-mandated offset to apply to pointers into TLS blocks.
constexpr std::size_t TLSPtrOffset = 0x8000;


// ABI-mandated layout for the TCB.
struct TCB
{
    void**          dtv = nullptr;  // Pointer to the dynamic thread vector (DTV).
#if defined(_ABIO32) || defined(_ABIN32)
    std::uintptr_t  pad = 0;        // Unused.
#endif
};

static_assert(sizeof(TCB) == TCBSize);


// The size of the thread data area, excluding the TLS data.
static std::size_t g_threadDataSize = 0;

// Mini-DTV used for the initial thread when using pre-allocated memory.
static void* g_initialDTV[2];


static inline void* getThreadPointer()
{
    // The ABI specifies that this should be $r3 so that it has a fast-path emulation. This isn't strictly necessary and
    // the XVI MIPS kernel will work with any register.
    register void* r3 asm("3");
    asm ("rdhwr %0, $29" : "=r" (r3));
    return r3;
}

static inline void setThreadPointer(void* tp)
{
    __set_tp(reinterpret_cast<std::uintptr_t>(tp));
}

static inline TCB* getTCB()
{
    auto tp = getThreadPointer();
    return reinterpret_cast<TCB*>(reinterpret_cast<std::uintptr_t>(tp) - ThreadPointerOffset);
}

static inline void** getDTV()
{
    return getTCB()->dtv;
}

static inline void setDVT(void** vector)
{
    getTCB()->dtv = vector;
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

    // Finally, add in the size for the thread data and the TCB.
    size += g_threadDataSize;
    size += TCBSize;

    return size;
}

// Calculates the appropriate size of the DTV for the given number of modules.
static std::size_t dtvSize(std::size_t image_count)
{
    return (image_count + 1) * sizeof(void*);
}


void setThreadDataLayout(std::size_t size, std::size_t dtv_offset)
{
    // Die if already configured or if the given configuration is invalid.
    //  - the layout has not already been set.
    //  - the thead data size must not exceed the maximum.
    //  - the DVT offset is not supported and must be zero
    if (g_threadDataSize != 0 || size > ThreadDataSizeMax || dtv_offset == 0)
        __terminate();

    g_threadDataSize = size;
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
    void** dtv = nullptr;
    if (!data)
    {
        // Allocate the main thread-local storage.
        data = reinterpret_cast<std::byte*>(__malloc(data_size));
        if (!data)
            __terminate();

        // Allocate a DTV too.
        auto image_count = getImageCount();
        dtv = reinterpret_cast<void**>(__malloc(dtvSize(image_count)));
        if (!dtv)
            __terminate();
    }
    else
    {
        // We're using pre-allocated data so use the pre-allocated DTV too.
        dtv = g_initialDTV;
    }

    // The value of the thread pointer is at a particular offset from the TCB, which itself comes after any thread data.
    auto tls_blocks_start = data + g_threadDataSize + TCBSize;
    auto thread_pointer = tls_blocks_start + ThreadPointerOffset;

    // Copy the TLS template data for the already-loaded image into place.
    std::size_t offset = addImageSize(0, initial_image);
    __memcpy(tls_blocks_start, initial_image.initial_bytes.data(), initial_image.initial_bytes.size());
    for (const auto& i : static_images)
    {
        offset = addImageSize(offset, i);
        __memcpy(tls_blocks_start + offset, i.initial_bytes.data(), i.initial_bytes.size());
    }

    // Fill in the DTV.
    if (dtv)
    {
        // First entry is the number of valid entries in this DTV.
        dtv[0] = reinterpret_cast<void*>(getImageCount());

        // The second entry points to the TLS block for the initial executable.
        dtv[1] = tls_blocks_start;

        // The remaining entries point to the TLS blocks for each of the static images.
        offset = addImageSize(0, initial_image);
        std::size_t index = 2;
        for (const auto& i : static_images)
        {
            dtv[index++] = tls_blocks_start + offset;
            offset = addImageSize(offset, i);
        }
    }

    // Fill in the TCB.
    auto tcb = reinterpret_cast<TCB*>(tls_blocks_start) - 1;
    tcb->dtv = dtv;

    // Set the thread pointer.
    setThreadPointer(thread_pointer);
}


} // namespace System::ABI::TLS


void* __tls_get_addr(__tls_index* ti)
{
    //! @todo: this method isn't signal-safe! Needs reworked.
    
    using namespace System::ABI::TLS;

    // Get the pointer to the dynamic thread vector (DTV).
    auto* dtv = getDTV();

    // Get the generation number (the first entry in the DVT). It identifies the length of the vector.
    if (!dtv || reinterpret_cast<std::uintptr_t>(dtv[0]) < ti->ti_module) [[unlikely]]
    {
        // The DVT needs to be allocated (or extended). If there's no malloc implementation, we need to fail.
        if (!__malloc)
            __terminate();

        // Allocate a replacement vector.
        auto new_images = getImageCount();
        auto new_length = dtvSize(new_images);
        auto old_images = dtv ? reinterpret_cast<std::uintptr_t>(dtv[0]) : 0;
        auto new_vector = reinterpret_cast<void**>(__malloc(sizeof(void*) * new_length));
        if (!new_vector)
        {
            // There's no reasonable recovery mechanism so we need to fail.
            __terminate();
        }

        // Copy over the old DVT entries, zero the new ones and set the generation number.
        auto extra_images = new_images - old_images;
        __memcpy(&new_vector[1], &dtv[1], sizeof(void*) * old_images);
        __memset(&new_vector[old_images + 1], 0, extra_images * sizeof(void*));
        new_vector[0] = reinterpret_cast<void*>(new_images);

        // Update the saved DVT and free the old DVT.
        setDVT(new_vector);
        if (dtv != g_initialDTV) [[likely]]
            __free(dtv);
        dtv = new_vector;
    }

    // Do we need to allocate memory for these thread-local variables?
    if (dtv[ti->ti_module] == 0) [[unlikely]]
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
        dtv[ti->ti_module] = memory;
    }

    // The ABI requires the returnd pointer to be offset from the actual block start.
    auto ptr = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(dtv[ti->ti_module]) + ti->ti_offset + TLSPtrOffset);
    return ptr;
}
