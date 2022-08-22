#ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H
#define __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H


#if !defined(__XVI_NO_STDLIB)
#  include <cstddef>
#  include <cstdint>
#  include <functional>
#  include <memory>
#  include <new>
#  include <span>
#  include <string>
#else
#  include <System/C++/LanguageSupport/New.hh>
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#  include <System/C++/Utility/Function.hh>
#  include <System/C++/Utility/SharedPtr.hh>
#  include <System/C++/Utility/Span.hh>
#  include <System/C++/Utility/String.hh>
#  include <System/C++/Utility/UniquePtr.hh>
#endif

#include <System/Filesystem/FAT/BPB.hh>
#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/File.hh>
#include <System/Filesystem/FAT/Object.hh>


namespace System::Filesystem::FAT
{


class Directory;


class Filesystem
{
public:

    using read_fn = std::function<std::shared_ptr<const std::byte> (std::uint64_t lba)>;
    using write_fn = std::function<bool (std::uint64_t lba, std::size_t count, std::span<const std::byte> in)>;
    using read_for_write_fn = std::function<std::shared_ptr<std::byte> (std::uint64_t lba)>;

    using cluster_chain_callback_t = std::function<bool (std::uint32_t index, std::uint32_t value)>;

    struct params_t
    {
        std::uint64_t   sectors = 0;
        std::size_t     sectorSize = 512;

        read_fn             read = {};
        write_fn            write = {};
        read_for_write_fn   read_for_write = {};
    };


    Filesystem() = delete;
    Filesystem(const Filesystem&) = delete;
    Filesystem(Filesystem&&) = delete;

    ~Filesystem() = delete;

    void operator delete(Filesystem* obj, std::destroying_delete_t)
    {
        deleteImpl(obj);
    }

    Filesystem& operator=(const Filesystem&) = delete;
    Filesystem& operator=(Filesystem&&) = delete;


    bool mount();

    Directory* rootDirectory();

    Object::handle_t openObject(const ObjectInfo*);
    File::handle_t openFile(const ObjectInfo*);
    Directory::handle_t openDirectory(const ObjectInfo*);

    std::uint32_t sectorsPerCluster() const;
    std::uint32_t clusterToSector(std::uint32_t cluster) const;

    Object::handle_t openObject(const ObjectInfo::handle_t& handle)
    {
        return openObject(handle.get());
    }

    File::handle_t openFile(const ObjectInfo::handle_t& handle)
    {
        return openFile(handle.get());
    }

    Directory::handle_t openDirectory(const ObjectInfo::handle_t& handle)
    {
        return openDirectory(handle.get());
    }


    static std::unique_ptr<Filesystem> open(const params_t& params)
    {
        return std::unique_ptr<Filesystem>{openImpl(&params)};
    }

    static bool isFATFilesystem(const bootsector&);

private:

    static Filesystem* openImpl(const params_t*);
    static void deleteImpl(Filesystem*);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H */
