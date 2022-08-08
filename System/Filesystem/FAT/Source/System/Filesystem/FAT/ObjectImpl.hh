#ifndef __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H
#define __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H


#include <System/Filesystem/FAT/Object.hh>

#if !defined(__XVI_NO_STDLIB)
#  include <string>
#else
#  include <System/C++/Utility/String.hh>
#endif

#include <System/Filesystem/FAT/DirEnt.hh>
#include <System/Filesystem/FAT/Filesystem.hh>


namespace System::Filesystem::FAT
{


class ObjectInfoImpl
{
public:

    constexpr ObjectInfoImpl() = default;
    constexpr ObjectInfoImpl(const ObjectInfoImpl&) = default;
    constexpr ObjectInfoImpl(ObjectInfoImpl&&) = default;

    ObjectInfoImpl(Filesystem&, std::string name, const dirent_t&);

    constexpr ~ObjectInfoImpl() = default;

    constexpr ObjectInfoImpl& operator=(const ObjectInfoImpl&) = default;
    constexpr ObjectInfoImpl& operator=(ObjectInfoImpl&&) = default;


    std::string_view name() const;

    std::uint32_t size() const;

    ObjectType type() const;

    std::uint32_t startCluster() const;

    std::uint32_t clusterCount() const;

    std::uint32_t lastClusterLength() const;

    Filesystem& filesystem() const;


    const ObjectInfo& asObjectInfo() const
    {
        return reinterpret_cast<const ObjectInfo&>(*this);
    }

    static const ObjectInfoImpl& from(const ObjectInfo* info)
    {
        return reinterpret_cast<const ObjectInfoImpl&>(*info);
    }

private:

    // Filesystem this object resides in.
    Filesystem*     m_filesystem = {};

    // Raw directory entry containing everything except, possibly, the name.
    dirent_t        m_dirent = {};

    // Extracted name - this is stored separately as it may have been stored as a sequence of long file name entries
    // instead of being part of he main directory entry.
    std::string     m_name = {};

    // Cached object type (to avoid recomputing it on every query).
    ObjectType      m_type = ObjectType::File;
};


class ObjectImpl
{
public:

    using read_blocks_callback_t = Object::read_blocks_callback_t;


    const ObjectInfoImpl& info() const;

    std::uint32_t readTo(std::uint32_t offset, std::uint32_t length, std::span<std::byte> out);

    void readBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t);

private:

    ObjectInfoImpl      m_info;


    // FAT12 and FAT16 require special handling of the root directory.
    void readRootDirBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H */
