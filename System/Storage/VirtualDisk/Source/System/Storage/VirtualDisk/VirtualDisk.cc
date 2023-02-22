#include <System/Storage/VirtualDisk/VirtualDisk.hh>


namespace System::Storage::VirtualDisk
{


std::uint32_t VirtualDisk::sectorSize() const noexcept
{
    // Assume 512-byte sectors by default.
    return 512;
}


} // namespace System::Storage::VirtualDisk
