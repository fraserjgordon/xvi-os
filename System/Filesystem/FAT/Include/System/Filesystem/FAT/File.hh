#ifndef __SYSTEM_FILESYSTEM_FAT_FILE_H
#define __SYSTEM_FILESYSTEM_FAT_FILE_H


#include <System/Filesystem/FAT/Object.hh>


namespace System::Filesystem::FAT
{


class File :
    public Object
{
public:

    using handle_t = std::unique_ptr<File, deleter_t>;


    bool truncate(std::uint32_t length = 0);
    bool append(std::span<const std::byte> data);
    bool write(std::uint32_t offset, std::span<const std::byte> data);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_FILE_H */
