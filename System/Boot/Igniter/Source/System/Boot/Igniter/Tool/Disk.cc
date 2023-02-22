#include <System/Boot/Igniter/Tool/Disk.hh>

#include <System/IO/FileIO/IOException.hh>
#include <System/Utility/Logger/Logger.hh>


namespace System::Boot::Igniter
{


using System::Utility::Logger::DefaultFacility;
using System::Utility::Logger::log;
using System::Utility::Logger::priority;

namespace FIO = System::IO::FileIO;
namespace VD = System::Storage::VirtualDisk;


Disk::Disk(const std::filesystem::path& path, const options& opt) :
    m_path(path)
{
    // Open the named file as a raw disk image.
    auto handle = FIO::FileHandle::open({}, path, FIO::mode::write, FIO::creation::open_existing, FIO::caching::all);
    if (!handle)
        throw FIO::IOException(handle.error());

    m_virt = VD::RawVirtualDisk::createFrom(std::make_unique<FIO::FileHandle>(std::move(*handle)), {.sector_size = 512});

    log(DefaultFacility, priority::debug, "opened disk {}", m_path.c_str());

    // Attempt to infer the disk geometry from the file size if this is an ordinary file.
    if (!std::filesystem::is_block_file(m_path))
        inferDiskGeometry(opt);
}


std::size_t Disk::sectorSize() const
{
    return m_sectorSize;
}


void Disk::readSector(std::uint64_t index, std::span<std::byte> out) const
{
    auto res = m_virt.read(index, out);
    if (!res)
        throw FIO::IOException(res.error());
}


void Disk::writeSector(std::uint64_t index, std::span<const std::byte> in)
{
    if (in.size_bytes() != sectorSize())
        throw std::logic_error("invalid buffer size");

    auto res = m_virt.write(index, in);
    if (!res)
        throw FIO::IOException(res.error());
}


void Disk::inferDiskGeometry(const options& opt)
{
    log(DefaultFacility, priority::debug, "inferring disk geometry for non-block-device disk");

    // First step: assume the sector size is 512 bytes.
    m_sectorSize = 512;
    log(DefaultFacility, priority::debug, "assuming sector size is {} bytes", m_sectorSize);

    // Next, look at the total size to see if it is any of the standard floppy disk sizes.
    constexpr auto kB = 1024;
    auto size = m_virt.sectorCount() * m_virt.sectorSize();
    switch (size)
    {
        case 1440*kB:
            m_geometry = {80, 2, 18};
            break;

        //! @todo: other standard floppy disk sizes.
    }

    if (m_geometry)
    {
        log(DefaultFacility, priority::debug, "assuming disk geometry is a floppy disk with CHS{{{},{},{}}} based on {}kB size",
            m_geometry->cylinders, m_geometry->heads, m_geometry->sectors, size/kB
        );
    }
    else
    {
        log(DefaultFacility, priority::debug, "disk size is not a standard floppy disk size; assuming hard drive using LBA addressing");
    }
}


} // namespace System::Boot::Igniter
