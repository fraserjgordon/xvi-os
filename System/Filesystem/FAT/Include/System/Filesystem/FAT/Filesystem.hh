#ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H
#define __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H


#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <new>
#include <span>
#include <string>

#include <System/Storage/Filesystem/Filesystem.hh>

#include <System/Filesystem/FAT/BPB.hh>
#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/File.hh>
#include <System/Filesystem/FAT/Object.hh>


namespace System::Filesystem::FAT
{


class Directory;


class Filesystem :
    public System::Storage::Filesystem::Filesystem
{
public:

    using cluster_chain_callback_t = std::function<bool (std::uint32_t index, std::uint32_t value)>;


    Filesystem() = delete;
    Filesystem(const Filesystem&) = delete;
    Filesystem(Filesystem&&) = delete;

    ~Filesystem() override;

    void operator delete(Filesystem* obj, std::destroying_delete_t)
    {
        deleteImpl(obj);
    }

    Filesystem& operator=(const Filesystem&) = delete;
    Filesystem& operator=(Filesystem&&) = delete;


    // Inherited from FS::Filesystem.
    BlockDev* blockDevice() noexcept override;
    const BlockDev* blockDevice() const noexcept override;


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


    static std::unique_ptr<Filesystem> open(std::unique_ptr<BlockDev> device)
    {
        return std::unique_ptr<Filesystem>{openImpl(device.release())};
    }

    static bool isFATFilesystem(const bootsector&);

private:

    static Filesystem* openImpl(BlockDev*);
    static void deleteImpl(Filesystem*);
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H */
