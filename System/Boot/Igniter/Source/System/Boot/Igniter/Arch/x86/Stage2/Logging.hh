#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_LOGGING_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_LOGGING_H


#include <cstddef>
#include <memory>
#include <span>

#include <System/Utility/Logger/Logger.hh>
#include <System/Utility/Logger/Message.hh>
#include <System/Utility/Logger/Processor.hh>


namespace System::Boot::Igniter
{


using priority = System::Utility::Logger::priority;


inline System::Utility::Logger::opaque_processor g_logBuffer = nullptr;


template <class... Args>
void log(priority p, Args&&... args)
    requires requires { System::Utility::Logger::log(System::Utility::Logger::DefaultFacility, p, std::forward<Args>(args)...); }
{
    System::Utility::Logger::log(System::Utility::Logger::DefaultFacility, p, std::forward<Args>(args)...);
}

inline void configureLogging(std::span<std::byte> buffer, priority prio = priority::info)
{
    g_logBuffer = System::Utility::Logger::registerBufferProcessor(buffer);
    System::Utility::Logger::setMinimumPriority(prio);
}

template <class F>
void readLogBuffer(F callback)
{
    using namespace System::Utility::Logger;

    std::unique_ptr<std::byte[]> buffer = nullptr;
    std::size_t buffer_size = 0;

    std::ptrdiff_t offset = 0;

    while (true)
    {
        auto size = readFromBufferProcessor(g_logBuffer, offset, buffer.get(), buffer_size, false);
        if (size == 0)
            break;

        if (size > buffer_size)
        {
            buffer.reset();
            buffer.reset(new std::byte[size]);
            buffer_size = size;

            readFromBufferProcessor(g_logBuffer, offset, buffer.get(), buffer_size, false);
        }

        offset += size;

        auto msg = decodeMessage(buffer.get(), size);
        callback(msg);
        freeMessage(msg);
    }
}


}


#endif /* __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_LOGGING_H */
