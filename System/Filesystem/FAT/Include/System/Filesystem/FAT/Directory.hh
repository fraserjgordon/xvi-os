#ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORY_H
#define __SYSTEM_FILESYSTEM_FAT_DIRECTORY_H


#if !defined(__XVI_NO_STDLIB)
#  include <functional>
#else
#  include <System/C++/Utility/Function.hh>
#endif

#include <System/Filesystem/FAT/Object.hh>


namespace System::Filesystem::FAT
{


class Filesystem;
class ObjectInfo;


class Directory :
    public Object
{
public:

    using enumerate_callback_t = std::function<bool (const ObjectInfo&)>;

    void enumerate(enumerate_callback_t);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORY_H */
