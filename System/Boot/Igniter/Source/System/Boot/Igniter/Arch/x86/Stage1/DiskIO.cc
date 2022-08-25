#include <System/Boot/Igniter/Arch/x86/Stage1/DiskIO.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/BIOSCall.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Logging.hh>


namespace System::Boot::Igniter
{


Disk::Disk(std::uint8_t drive) :
    m_drive(drive)
{
    // If this is a hard drive, query for whether the extended I/O functions are available.
    if (drive >= 0x80)
    {
        m_extensions = getDiskExtensionsVersion(drive);

        if (m_extensions)
            log(priority::debug, "BIOS supports disk extensions v{}.{} for drive {}", m_extensions >> 4, m_extensions & 0x0F, drive);
        else
            log(priority::warning, "BIOS does not support disk extensions for drive {}; CHS addressing will be used", drive);
    }

    // If extensions aren't available (including for floppy drives), attempt to get the drive geometry.
    if (m_extensions == 0)
    {
        m_geometry = getGeometry(drive);

        log(priority::debug, "BIOS reported geometry for drive {}: {},{},{}",
            drive,
            m_geometry->cylinders,
            m_geometry->heads,
            m_geometry->sectors
        );
    }
}


std::uint32_t Disk::read(std::uint64_t lba, std::uint32_t count, std::span<std::byte> buffer)
{
    // Check that the buffer is valid.
    if ((count * m_sectorSize) > buffer.size_bytes())
        return 0;

    // Can we read the drive by LBA or do we need to convert to CHS first?
    if (m_extensions > 0)
    {
        // LBA reading is possible.
        //! @todo implement
        return 0;
    }
    else
    {
        // Convert the request to CHS format.
        auto chs = LBAToCHS(*m_geometry, lba);

        // Clamp the count so that all reads are from within the same track.
        count = std::min({count, m_geometry->sectors - (chs.sector -  1)});

        // Attempt the read.
        for (int i = 0; i < ReadRetryCount; ++i)
        {
            if (readDiskCHS(m_drive, chs, count, buffer))
                return count;
        }

        // Read failed.
        return 0;
    }
}


std::uint8_t Disk::getDiskExtensionsVersion(std::uint8_t drive)
{
    bios_call_params params;
    params.ah = 0x41;
    params.bx = 0x55AA;
    params.dl = drive;

    BIOSCall(0x13, &params);

    // Check that the call was actually supported.
    if (params.eflags.bits.cf || params.bx != 0xAA55)
        return 0;

    // Return the extension version.
    return params.ah;
}


chs_geometry Disk::getGeometry(std::uint8_t drive)
{
    bios_call_params params;
    params.ah = 0x08;
    params.dl = drive;

    BIOSCall(0x13, &params);

    // Unable to get drive parameters for some reason.
    if (params.eflags.bits.cf)
        return {0, 0, 0};

    std::uint32_t sectors = (params.cl & 0x3F);
    std::uint32_t heads = (params.dh + 1);
    std::uint32_t cylinders = ((params.cl & 0xC0) << 2) + params.ch + 1U;

    return {cylinders, heads, sectors};
}


bool Disk::resetDisk(std::uint8_t drive)
{
    bios_call_params params;
    params.ah = 0x00;
    params.dl = drive;

    BIOSCall(0x13, &params);

    return !params.eflags.bits.cf;
}


bool Disk::readDiskCHS(std::uint8_t drive, const chs_address& address, std::uint32_t length, std::span<std::byte> buffer)
{
    // Sanity checks (based only on whether we can send the commands to the BIOS).
    if (length == 0 || length > 0xFF || address.sector > 63 || address.cylinder > 1023 || address.head > 0xFF)
        return false;

    // Check that the destination buffer can be accessed from real mode.
    auto linear = reinterpret_cast<std::uint32_t>(buffer.data());
    auto end = linear + buffer.size_bytes();
    auto segment = (linear >> 4);
    auto offset = (linear & 0x000F);
    if (end >= 0x10'0000 || (end - (segment << 4)) > 0xFFFF)
        return false;

    log(priority::trace, "Reading CHS sector {},{},{} from drive {}", address.cylinder, address.head, address.sector, drive);

    bios_call_params params;
    params.al = length;
    params.ah = 0x02;
    params.cl = address.sector | ((address.cylinder & 0x300) >> 2);
    params.ch = address.cylinder & 0xFF;
    params.dl = drive;
    params.dh = address.head;
    params.bx = offset;
    params.es = segment;

    BIOSCall(0x13, &params);

    return !params.eflags.bits.cf;
}



} // namespace System::Boot::Igniter
