#ifndef __SYSTEM_STORAGE_VIRTUALDISK_VIRTUALDISK_H
#define __SYSTEM_STORAGE_VIRTUALDISK_VIRTUALDISK_H


#include <cstddef>
#include <cstdint>
#include <span>

#include <System/Storage/BlockDevice/BlockDev.hh>


namespace System::Storage::VirtualDisk
{


class VirtualDisk :
    public System::Storage::BlockDevice::BlockDev
{
public:

    // Inherited from class BlockDev.
    std::size_t sectorSize() const noexcept override;
};


} // namespace System::Storage::VirtualDisk


#endif /* ifndef __SYSTEM_STORAGE_VIRTUALDISK_VIRTUALDISK_H */
