#ifndef __SYSTEM_ABI_TLS_IMAGEINTERNAL_H
#define __SYSTEM_ABI_TLS_IMAGEINTERNAL_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/Span.hh>

#include <System/ABI/TLS/TLS.hh>


namespace System::ABI::TLS
{


struct image_info_t
{
    std::size_t                 image_size;
    std::size_t                 image_alignment;
    std::span<const std::byte>  initial_bytes;
};


const image_info_t&             getInitialImage();
std::span<const image_info_t>   getStaticImages();

const image_info_t*             getDynamicImage(image_handle_t);

std::size_t                     getImageCount();

// Called once the first thread has been initialised. All images added before this point are static; all images added
// later are dynamic.
void endStaticConfiguration();


} // namespace System::ABI::TLS


#endif /* ifndef __SYSTEM_ABI_TLS_IMAGEINTERNAL_H */
