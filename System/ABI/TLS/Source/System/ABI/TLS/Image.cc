#include <System/ABI/TLS/ImageInternal.hh>

#include <System/ABI/TLS/Terminate.hh>


namespace System::ABI::TLS
{


// TLS data for the initial executable.
static image_info_t g_initialImage = {};

// TLS data for objects that are loaded at process creation time. These cannot be unloaded but accesses to them can be
// slightly optimised as they can be put directly into the thread data area rather than dynamically allocated on first
// access.
static std::size_t g_staticImageCount = 0;

// TLS data for objects that were loaded dynamically.
static std::size_t g_dynamicImageCount = 0;

// Flag indicating the end of the static TLS configuration phase. Any images registered after this point are dynamic.
static bool g_dynamicPhase = false;


void setInitialData(std::size_t size, std::size_t alignment, std::span<const std::byte> initial_bytes)
{
    // Can only be called once.
    if (g_initialImage.image_size != 0)
        __terminate();
    
    g_initialImage = {.image_size = size, .image_alignment = alignment, .initial_bytes = initial_bytes};
}

const image_info_t& getInitialImage()
{
    return g_initialImage;
}

std::span<const image_info_t> getStaticImages()
{
    // Should not be called until the static configuration has finished.
    if (!g_dynamicPhase)
        __terminate();

    //! @TODO: implement.
    return {};
}

std::size_t getImageCount()
{
    return 1 + g_staticImageCount + g_dynamicImageCount;
}

const image_info_t* getDynamicImage(image_handle_t id)
{
    // Should not be called until the static configuration has finished.
    if (!g_dynamicPhase)
        __terminate();

    //! @todo: implement.
    return nullptr;
}

void endStaticConfiguration()
{
    // Should only be called once.
    if (g_dynamicPhase)
        __terminate();

    g_dynamicPhase = true;
}


} // namespace System::ABI::TLS
