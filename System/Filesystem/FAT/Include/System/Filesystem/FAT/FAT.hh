#pragma once
#ifndef __SYSTEM_FS_FAT_FAT_H
#define __SYSTEM_FS_FAT_FAT_H


#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::Filesystem::FAT
{


class FatFS
{
public:

    using sectorid_t  = std::uint32_t;
    using clusterid_t = std::uint32_t;

    static constexpr clusterid_t NullCluster        = 0x00000000;
    static constexpr clusterid_t BadCluster         = 0xFFFFFFF7;
    static constexpr clusterid_t NoMoreClusters     = 0xFFFFFFF8;

protected:

    virtual clusterid_t get_rootdir_start() const = 0;
    virtual clusterid_t get_rootdir_length() const = 0;
    virtual clusterid_t get_rootdir_next(clusterid_t current) const = 0;

    virtual clusterid_t get_next_cluster(clusterid_t current) const = 0;
};


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FS_FAT_FAT_H */
