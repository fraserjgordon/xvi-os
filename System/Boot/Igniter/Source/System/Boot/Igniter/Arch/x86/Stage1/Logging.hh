#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_LOGGING_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_LOGGING_H


#if !defined(__SYSTEM_BOOT_IGNITER_STAGE1_NO_LOGGING)
#  include <System/Utility/Logger/Logger.hh>
#endif


namespace System::Boot::Igniter
{


using System::Utility::Logger::priority;


template <class... Args>
void log(priority p, std::string_view format, Args&&... args)
{
    using System::Utility::Logger::DefaultFacility;
    System::Utility::Logger::log(DefaultFacility, p, format, std::forward<Args>(args)...);
}


void configureLogging();


}


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_LOGGING_H */
