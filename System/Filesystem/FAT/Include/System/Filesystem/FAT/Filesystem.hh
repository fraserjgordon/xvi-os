#ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H
#define __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H


#if !defined(__XVI_NO_STDLIB)
#  include <cstddef>
#  include <cstdint>
#  include <functional>
#  include <memory>
#  include <span>
#  include <string>
#else
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#  include <System/C++/Utility/Function.hh>
#  include <System/C++/Utility/SharedPtr.hh>
#  include <System/C++/Utility/Span.hh>
#  include <System/C++/Utility/String.hh>
#endif

#include <System/Filesystem/FAT/BPB.hh>


namespace System::Filesystem::FAT
{


class Directory;


class Filesystem
{
public:

    using read_fn = std::function<std::shared_ptr<const std::byte> (std::uint64_t lba)>;
    using write_fn = std::function<bool (std::uint64_t lba, std::size_t count, std::span<const std::byte> in)>;

    using cluster_chain_callback_t = std::function<bool (std::uint32_t index, std::uint32_t value)>;

    struct params_t
    {
        std::uint64_t   sectors = 0;
        std::size_t     sectorSize = 512;

        read_fn         read = {};
        write_fn        write = {};
    };


    Filesystem() = delete;
    Filesystem(const Filesystem&) = delete;
    Filesystem(Filesystem&&) = delete;

    Filesystem(const params_t& params);

    ~Filesystem();

    Filesystem& operator=(const Filesystem&) = delete;
    Filesystem& operator=(Filesystem&&) = delete;


    bool mount();

    Directory* rootDirectory();

    std::uint32_t sectorSize() const;
    std::uint32_t clusterSize() const;
    std::uint32_t sectorsPerCluster() const;

    fat_type fatType() const;

    // FAT12/FAT16 only.
    std::uint32_t rootDirStartSector() const;
    std::uint32_t rootDirSectorCount() const;

    // FAT32 only.
    std::uint32_t rootDirFirstCluster() const;

    void readClusterChain(std::uint32_t cluster, cluster_chain_callback_t) const;
    std::uint32_t clusterToSector(std::uint32_t cluster) const;

    std::shared_ptr<const std::byte> readBlock(std::uint32_t lba);


    static bool isFATFilesystem(const bootsector&);

private:

    // Parameters for accessing the filesystem.
    params_t    m_params = {};

    // Values calculated or extracted from the BPB.
    std::uint32_t   m_sectorSize        = 0;    // Size (in bytes) of each sector.
    std::uint32_t   m_sectorCount       = 0;    // Total number of sectors.
    std::uint32_t   m_fatStart          = 0;    // Start sector of the first FAT.
    std::uint32_t   m_fatSize           = 0;    // Number of sectors in each FAT.
    std::uint32_t   m_fatCount          = 0;    // Number of copies of the FAT.
    std::uint32_t   m_sectorsPerCluster = 0;    // Number of sectors in each cluster.
    std::uint32_t   m_rootDirStart      = 0;    // First sector (FAT12/16) / cluster (FAT32) of the root directory.
    std::uint32_t   m_rootDirLength     = 0;    // Size (in sectors) of the root dir (FAT12/16 only).
    std::uint32_t   m_clusterSize       = 0;    // Size (in bytes) of each cluster.
    std::uint32_t   m_firstCluster      = 0;    // Sector number of the first data cluster (cluster 1).
    fat_type        m_type          = fat_type::Unknown;
    extended_signature  m_extendedBPB   = {};   // Determines which (if any) extended BPB fields are present.
    std::uint32_t   m_serial            = 0;    // Volume serial number.
    std::array<char, 11> m_name         = {};   // Volume name.
    std::array<char, 8> m_fsType        = {};   // Non-normative FS type string.


    std::string id() const;
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEM_H */
