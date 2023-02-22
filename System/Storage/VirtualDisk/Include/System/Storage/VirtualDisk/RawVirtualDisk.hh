#ifndef __SYSTEM_STORAGE_VIRTUALDISK_RAWVIRTUALDISK_H
#define __SYSTEM_STORAGE_VIRTUALDISK_RAWVIRTUALDISK_H


#include <memory>

#include <System/IO/FileIO/IOHandle.hh>

#include <System/Storage/VirtualDisk/VirtualDisk.hh>


namespace System::Storage::VirtualDisk
{


class RawVirtualDisk :
    public VirtualDisk
{
public:

    struct disk_params_t
    {
        std::uint32_t       sector_size = 512;
    };


    constexpr RawVirtualDisk() noexcept = default;
    RawVirtualDisk(const RawVirtualDisk&) = delete;
    constexpr RawVirtualDisk(RawVirtualDisk&&) noexcept = default;

    constexpr ~RawVirtualDisk() = default;

    void operator=(const RawVirtualDisk&) = delete;
    constexpr RawVirtualDisk& operator=(RawVirtualDisk&&) noexcept = default;


    // Inherited from VirtualDisk.
    std::uint32_t sectorSize() const noexcept override;
    lba_t sectorCount() const noexcept override;
    result<void> read(lba_t, buffer) const noexcept override;
    result<void> write(lba_t, const_buffer) noexcept override;


    static RawVirtualDisk createFrom(std::unique_ptr<IO::FileIO::IOHandle>, const disk_params_t&) noexcept;

private:

    std::unique_ptr<IO::FileIO::IOHandle>   m_io = nullptr;
    disk_params_t                           m_params = {};
    lba_t                                   m_length = 0;


    RawVirtualDisk(std::unique_ptr<IO::FileIO::IOHandle>, const disk_params_t&) noexcept;
};


} // namespace System::Storage::VirtualDisk


#endif /* ifndef __SYSTEM_STORAGE_VIRTUALDISK_RAWVIRTUALDISK_H */
