#ifndef __SYSTEM_UTILITY_LOGGER_PROCESSOR_H
#define __SYSTEM_UTILITY_LOGGER_PROCESSOR_H


#include <System/Utility/Logger/Private/Config.hh>

#if defined(__XVI_NO_STDLIB)
#  include <System/C++/LanguageSupport/StdDef.hh>
#else
#  include <cstddef>
#endif

#include <System/Utility/Logger/Logger.hh>
#include <System/Utility/Logger/Message.hh>


namespace System::Utility::Logger
{


struct opaque_processor_t;
using opaque_processor = opaque_processor_t*;

using message_processor_callback = void(*)(void*, opaque_message);
using encoded_processor_callback = void(*)(void*, const std::byte*, std::size_t);

struct buffering_params
{
    std::byte*          buffer_ptr;
    std::size_t         buffer_size;
};

__SYSTEM_UTILITY_LOGGER_EXPORT opaque_processor registerMessageProcessor(message_processor_callback, void*) __SYSTEM_UTILITY_LOGGER_SYMBOL(RegisterMessageProcessor);
__SYSTEM_UTILITY_LOGGER_EXPORT opaque_processor registerEncodedProcessor(encoded_processor_callback, void*) __SYSTEM_UTILITY_LOGGER_SYMBOL(RegisterEncodedProcessor);
__SYSTEM_UTILITY_LOGGER_EXPORT opaque_processor registerBufferProcessor(const buffering_params*) __SYSTEM_UTILITY_LOGGER_SYMBOL(RegisterBufferProcessor);
__SYSTEM_UTILITY_LOGGER_EXPORT void unregisterProcessor(opaque_processor) __SYSTEM_UTILITY_LOGGER_SYMBOL(UnregisterProcessor);

__SYSTEM_UTILITY_LOGGER_EXPORT std::size_t readFromBufferProcessor(opaque_processor, std::ptrdiff_t offset, std::byte*, std::size_t, bool remove) __SYSTEM_UTILITY_LOGGER_SYMBOL(ReadFromBufferProcessor);


inline auto registerBufferProcessor(std::span<std::byte> s)
{
    buffering_params params { .buffer_ptr = s.data(), .buffer_size = s.size() };
    return registerBufferProcessor(&params);
}


} // namespace System::Utility::Logger


#endif /* ifndef __SYSTEM_UTILITY_LOGGER_PROCESSOR_H */
