#ifndef __SYSTEM_FILESYSTEM_FAT_LOGGING_H
#define __SYSTEM_FILESYSTEM_FAT_LOGGING_H


#include <System/Utility/Logger/Logger.hh>


namespace System::Filesystem::FAT
{


using priority = System::Utility::Logger::priority;


template <class... Args>
void log(priority p, std::string_view format, Args&&... args)
{
    using namespace System::Utility::Logger;

    static logger s_logger{"Filesystem.FAT"};

    s_logger.log(p, format, std::forward<Args>(args)...);
}


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_LOGGING_H */
