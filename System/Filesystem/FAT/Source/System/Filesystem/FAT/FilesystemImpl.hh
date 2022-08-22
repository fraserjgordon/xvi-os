#ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEMIMPL_H
#define __SYSTEM_FILESYSTEM_FAT_FILESYSTEMIMPL_H


#include <System/Filesystem/FAT/Filesystem.hh>

#include <System/Filesystem/FAT/DirectoryImpl.hh>
#include <System/Filesystem/FAT/FileImpl.hh>


namespace System::Filesystem::FAT
{


class FilesystemImpl
{
public:

    using params_t = Filesystem::params_t;
    using cluster_chain_callback_t = Filesystem::cluster_chain_callback_t;

    static constexpr std::uint32_t FAT12ClusterMask     = 0x0000'0FFF;
    static constexpr std::uint32_t FAT16ClusterMask     = 0x0000'FFFF;
    static constexpr std::uint32_t FAT32ClusterMask     = 0x0FFF'FFFF;

    static constexpr std::uint32_t BadClusterMark       = 0xFFFF'FFF7;
    static constexpr std::uint32_t EndOfChain           = 0xFFFF'FFF8;
    static constexpr std::uint32_t FreeClusterMark      = 0x0000'0000;


    FilesystemImpl() = delete;
    FilesystemImpl(const FilesystemImpl&) = delete;
    FilesystemImpl(FilesystemImpl&&) = delete;

    FilesystemImpl(const params_t& params);

    ~FilesystemImpl();

    FilesystemImpl& operator=(const FilesystemImpl&) = delete;
    FilesystemImpl& operator=(FilesystemImpl&&) = delete;


    bool mount();

    DirectoryImpl* rootDirectory();

    std::uint32_t sectorSize() const;
    std::uint32_t clusterSize() const;
    std::uint32_t sectorsPerCluster() const;

    fat_type fatType() const;

    // FAT12/FAT16 only.
    std::uint32_t rootDirStartSector() const;
    std::uint32_t rootDirSectorCount() const;

    // FAT32 only.
    std::uint32_t rootDirFirstCluster() const;

    std::uint32_t getFATEntry(std::uint32_t cluster) const;
    std::uint32_t getFATEntryRaw(std::uint32_t cluster) const;
    bool isEndOfChain(std::uint32_t cluster) const;
    bool isBadClusterMark(std::uint32_t cluster) const;
    bool isFreeClusterMark(std::uint32_t cluster) const;
    void readClusterChain(std::uint32_t cluster, cluster_chain_callback_t) const;
    std::uint32_t nthChainEntry(std::uint32_t cluster, std::uint32_t n) const;
    std::uint32_t clusterToSector(std::uint32_t cluster) const;

    std::uint32_t findFreeCluster(bool mark_as_allocated);
    void writeClusterChainLink(std::uint32_t from, std::uint32_t to);
    void writeClusterChain(std::uint32_t from, std::span<std::uint32_t> chain);
    void freeClusterChain(std::uint32_t from);

    std::shared_ptr<const std::byte> readBlock(std::uint32_t lba) const;
    std::shared_ptr<std::byte> readBlockForUpdate(std::uint32_t lba);
    bool writeBlock(std::uint32_t lba, std::span<const std::byte>);

    std::string convertFromFilesystemCharset(std::string_view) const;
    std::string convertToFilesystemCharset(std::string_view) const;

    std::string convertFromUTF16(std::u16string_view) const;
    std::u16string convertToUTF16(std::string_view) const;

    std::unique_ptr<ObjectImpl> openObject(const ObjectInfoImpl&);
    std::unique_ptr<FileImpl> openFile(const ObjectInfoImpl&);
    std::unique_ptr<DirectoryImpl> openDirectory(const ObjectInfoImpl&);


    static bool isFATFilesystem(const bootsector&);


    Filesystem& asFilesystem()
    {
        return reinterpret_cast<Filesystem&>(*this);
    }

    static FilesystemImpl& from(Filesystem* fs)
    {
        return reinterpret_cast<FilesystemImpl&>(*fs);
    }

    static const FilesystemImpl& from(const Filesystem* fs)
    {
        return reinterpret_cast<const FilesystemImpl&>(*fs);
    }

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
    std::uint32_t   m_clusterCount      = 0;    // Number of clusters.
    std::uint32_t   m_clusterMask       = 0;    // Mask to apply to cluster numbers in the FAT.
    fat_type        m_type          = fat_type::Unknown;
    extended_signature  m_extendedBPB   = {};   // Determines which (if any) extended BPB fields are present.
    std::uint32_t   m_serial            = 0;    // Volume serial number.
    std::array<char, 11> m_name         = {};   // Volume name.
    std::array<char, 8> m_fsType        = {};   // Non-normative FS type string.

    std::unique_ptr<DirectoryImpl>  m_rootDir = {};


    std::string id() const;

    Filesystem::read_for_write_fn makeReadForWriteFunction();
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FILESYSTEM_FAT_FILESYSTEMIMPL_H */
