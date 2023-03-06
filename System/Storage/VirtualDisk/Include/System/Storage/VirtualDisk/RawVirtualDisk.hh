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
    RawVirtualDisk(RawVirtualDisk&&) noexcept = default;

    ~RawVirtualDisk() = default;

    void operator=(const RawVirtualDisk&) = delete;
    RawVirtualDisk& operator=(RawVirtualDisk&&) noexcept = default;


    // Inherited from BlockDev.
    std::size_t sectorSize() const noexcept override;
    lba_t sectorCount() const noexcept override;
    result_t<void> read(lba_t, buffer) const noexcept override;
    result_t<void> write(lba_t, const_buffer) noexcept override;


    static RawVirtualDisk createFrom(IO::FileIO::SharedIOHandle, const disk_params_t&) noexcept;

private:

    IO::FileIO::SharedIOHandle  m_io = nullptr;
    disk_params_t               m_params = {};
    lba_t                       m_length = 0;


    RawVirtualDisk(IO::FileIO::SharedIOHandle, const disk_params_t&) noexcept;
};


} // namespace System::Storage::VirtualDisk


#endif /* ifndef __SYSTEM_STORAGE_VIRTUALDISK_RAWVIRTUALDISK_H */
