#ifndef __SYSTEM_ABI_TLS_TLS_H
#define __SYSTEM_ABI_TLS_TLS_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/Utility/Span.hh>

#include <System/ABI/TLS/Private/Config.hh>


namespace System::ABI::TLS
{


using image_handle_t = std::size_t;


__SYSTEM_ABI_TLS_EXPORT void            setInitialData(std::size_t size, std::size_t alignment, std::span<const std::byte> initial_bytes) __SYSTEM_ABI_TLS_SYMBOL(SetInitialData);
__SYSTEM_ABI_TLS_EXPORT image_handle_t  registerImage(std::size_t size, std::size_t alignment, std::span<const std::byte> initial_bytes) __SYSTEM_ABI_TLS_SYMBOL(RegisterImage);
__SYSTEM_ABI_TLS_EXPORT void            unregisterImage(image_handle_t) __SYSTEM_ABI_TLS_SYMBOL(UnregisterImage);

__SYSTEM_ABI_TLS_EXPORT void            setThreadDataLayout(std::size_t size, std::size_t dtv_offset) __SYSTEM_ABI_TLS_SYMBOL(SetThreadDataLayout);

__SYSTEM_ABI_TLS_EXPORT void            configureInitialThread(std::span<std::byte> preallocated_space = {}) __SYSTEM_ABI_TLS_SYMBOL(ConfigureInitialThread);
__SYSTEM_ABI_TLS_EXPORT void            configureThisThread() __SYSTEM_ABI_TLS_SYMBOL(ConfigureThisThread);


} // namespace System::ABI::TLS


#endif /* ifndef __SYSTEM_ABI_TLS_TLS_H */
