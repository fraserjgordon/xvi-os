#ifndef __SYSTEM_STORAGE_BLOCKDEVICE_CHS_H
#define __SYSTEM_STORAGE_BLOCKDEVICE_CHS_H


#include <cstdint>


namespace System::Storage::BlockDevice
{


struct chs_geometry
{
    std::uint32_t cylinders;
    std::uint32_t heads;
    std::uint32_t sectors;
};


struct chs_address
{
    std::uint32_t cylinder;
    std::uint32_t head;
    std::uint32_t sector;
};


chs_address LBAToCHS(const chs_geometry&, std::uint64_t lba);

std::uint64_t CHSToLBA(const chs_geometry&, const chs_address&);


} // namespace System::Storage::BlockDevice


#endif /* ifndef __SYSTEM_STORAGE_BLOCKDEVICE_CHS_H */
