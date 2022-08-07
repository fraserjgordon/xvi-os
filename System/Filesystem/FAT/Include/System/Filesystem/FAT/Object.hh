#ifndef __SYSTEM_FILESYSTEM_FAT_OBJECT_H
#define __SYSTEM_FILESYSTEM_FAT_OBJECT_H


#if !defined(__XVI_NO_STDLIB)
#  include <cstddef>
#  include <cstdint>
#  include <functional>
#  include <span>
#  include <string_view>
#else
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#  include <System/C++/Utility/Function.hh>
#  include <System/C++/Utility/Span.hh>
#  include <System/C++/Utility/StringView.hh>
#endif


namespace System::Filesystem::FAT
{


class Filesystem;
class Object;


enum class ObjectType
{
    File,
    Directory,
    RootDirectory,
    VolumeLabel,
};


class ObjectInfo
{
public:

    ObjectInfo() = delete;
    ObjectInfo(const ObjectInfo&) = delete;
    ObjectInfo(ObjectInfo&&) = delete;

    ~ObjectInfo() = delete;

    ObjectInfo& operator=(const ObjectInfo&) = delete;
    ObjectInfo& operator=(ObjectInfo&&) = delete;


    std::string_view name() const;

    std::uint32_t size() const;

    ObjectType type() const;

    std::uint32_t startCluster() const;

    std::uint32_t clusterCount() const;

    std::uint32_t lastClusterLength() const;

    Filesystem& filesystem() const;
};


class Object
{
public:

    using read_blocks_callback_t = std::function<bool (std::uint32_t offset, std::span<const std::byte> data)>;


    const ObjectInfo& info() const;

    std::uint32_t readTo(std::uint32_t offset, std::uint32_t length, std::span<std::byte> out);

    void readBlocks(std::uint32_t offset, std::uint32_t length, read_blocks_callback_t);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_OBJECT_H */
