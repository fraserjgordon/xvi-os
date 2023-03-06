#ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORY_H
#define __SYSTEM_FILESYSTEM_FAT_DIRECTORY_H


#include <functional>

#include <System/Filesystem/FAT/File.hh>
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

    using handle_t = std::unique_ptr<Directory, deleter_t>;

    void enumerate(enumerate_callback_t);

    ObjectInfo::handle_t find(std::string_view name);

    bool updateObjectMetadata(const ObjectInfo& info);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_DIRECTORY_H */
