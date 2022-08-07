#ifndef __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H
#define __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H


#include <System/Filesystem/FAT/Object.hh>

#if !defined(__XVI_NO_STDLIB)
#  include <string>
#else
#  include <System/C++/Utility/String.hh>
#endif

#include <System/Filesystem/FAT/DirEnt.hh>


namespace System::Filesystem::FAT
{


class ObjectInfoImpl
{
public:

    const ObjectInfo& asObjectInfo() const
    {
        return reinterpret_cast<const ObjectInfo&>(*this);
    }

private:

    // Raw directory entry containing everything except, possibly, the name.
    dirent_t        m_dirent;

    // Extracted name - this is stored separately as it may have been stored as a sequence of long file name entries
    // instead of being part of he main directory entry.
    std::string     m_name;
};


class ObjectImpl
{
public:

    using read_blocks_callback_t = Object::read_blocks_callback_t;


    const ObjectInfo& info() const;

    std::uint32_t readTo(std::uint32_t offset, std::uint32_t length, std::span<std::byte> out);

    void readBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t);

private:

    // FAT12 and FAT16 require special handling of the root directory.
    void readRootDirBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H */
