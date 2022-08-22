#ifndef __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H
#define __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H


#include <System/Filesystem/FAT/Object.hh>

#if !defined(__XVI_NO_STDLIB)
#  include <memory>
#  include <string>
#else
#  include <System/C++/Utility/String.hh>
#  include <System/C++/Utility/UniquePtr.hh>
#endif

#include <System/Filesystem/FAT/DirEnt.hh>


namespace System::Filesystem::FAT
{


// Forward declarations.
class FilesystemImpl;


class ObjectInfoImpl
{
public:

    // The info for a filesystem root directory needs special handling.
    struct root_dir_object_info_t {};
    static constexpr root_dir_object_info_t RootDirObjectInfo = {};


    constexpr ObjectInfoImpl() = default;
    constexpr ObjectInfoImpl(const ObjectInfoImpl&) = default;
    constexpr ObjectInfoImpl(ObjectInfoImpl&&) = default;

    ObjectInfoImpl(FilesystemImpl&, std::string name, const dirent_t&);
    ObjectInfoImpl(FilesystemImpl&, root_dir_object_info_t);

    constexpr ~ObjectInfoImpl() = default;

    constexpr ObjectInfoImpl& operator=(const ObjectInfoImpl&) = default;
    constexpr ObjectInfoImpl& operator=(ObjectInfoImpl&&) = default;


    std::string_view name() const;

    std::uint32_t size() const;

    ObjectType type() const;

    std::uint32_t startCluster() const;
    std::uint32_t nthCluster(std::uint32_t n) const;
    std::uint32_t clusterCount() const;
    std::uint32_t lastClusterLength() const;

    std::uint32_t startSector() const;
    std::uint32_t nthSector(std::uint32_t n) const;
    std::uint32_t sectorCount() const;
    std::uint32_t lastSectorLength() const;

    const dirent_t& dirent() const;

    FilesystemImpl& filesystem() const;

    void setMetadata(const ObjectInfoImpl&);
    void setSize(std::uint32_t size);
    void setCluster(std::uint32_t cluster);


    const ObjectInfo* asObjectInfo() const
    {
        return reinterpret_cast<const ObjectInfo*>(this);
    }

    static const ObjectInfoImpl& from(const ObjectInfo* info)
    {
        return reinterpret_cast<const ObjectInfoImpl&>(*info);
    }

private:

    // Filesystem this object resides in.
    FilesystemImpl* m_filesystem = {};

    // Raw directory entry containing everything except, possibly, the name.
    dirent_t        m_dirent = {};

    // Extracted name - this is stored separately as it may have been stored as a sequence of long file name entries
    // instead of being part of he main directory entry.
    std::string     m_name = {};

    // Cached object type (to avoid recomputing it on every query).
    ObjectType      m_type = ObjectType::File;

    // Cached size (for directories).
    mutable std::uint32_t   m_dirSize = 0;
};


class ObjectImpl
{
public:

    using read_blocks_callback_t = Object::read_blocks_callback_t;


    const ObjectInfoImpl& info() const;

    std::uint32_t readTo(std::uint32_t offset, std::uint32_t length, std::span<std::byte> out);

    void readBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t);

    bool truncate(std::uint32_t length = 0);
    bool append(std::span<const std::byte> data);
    bool write(std::uint32_t offset, std::span<const std::byte> data);


    static std::unique_ptr<ObjectImpl> open(const ObjectInfoImpl&);

    Object* asObject()
    {
        return reinterpret_cast<Object*>(this);
    }

    static const ObjectImpl& from(const Object* obj)
    {
        return reinterpret_cast<const ObjectImpl&>(*obj);
    }

protected:

    ObjectImpl(const ObjectInfoImpl&);

private:

    ObjectInfoImpl      m_info;


    // Structure used to pass cluster chain hints to avoid repeated lookups.
    struct cluster_hint
    {
        std::uint32_t   index = 0;
        std::uint32_t   cluster = 0;
    };

    // On-demand-allocated buffer used to generate sectors that are partially written.
    using buffer_t = std::unique_ptr<std::byte[]>;


    // FAT12 and FAT16 require special handling of the root directory.
    void readRootDirBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t);

    std::uint32_t clusterLookup(std::uint32_t index, cluster_hint& hint) const;
    std::span<std::byte> ensureBuffer(buffer_t& buffer);
    bool overwriteClusters(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length);
    bool overwriteCluster(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length);
    bool overwritePartialSector(std::uint32_t lba, std::uint32_t offset, std::span<const std::byte> data);
    bool overwriteSector(std::uint32_t lba, std::span<const std::byte> data);
    bool appendClusters(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length);
    bool appendCluster(std::uint32_t offset, std::span<const std::byte> data, cluster_hint& hint, buffer_t& buffer, std::uint32_t& valid_length);
    bool appendPartialSector(std::uint32_t lba, std::uint32_t offset, std::span<const std::byte> data, buffer_t& buffer);
    bool appendSector(std::uint32_t lba, std::span<const std::byte> data);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_OBJECTIMPL_H */
