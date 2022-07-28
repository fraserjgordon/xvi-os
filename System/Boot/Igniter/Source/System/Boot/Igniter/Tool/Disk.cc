#include <System/Boot/Igniter/Tool/Disk.hh>

#include <System/Utility/Logger/Logger.hh>


namespace System::Boot::Igniter
{


using System::Utility::Logger::DefaultFacility;
using System::Utility::Logger::log;
using System::Utility::Logger::priority;


Disk::Disk(const std::filesystem::path& path, const options& opt) :
    m_path(path),
    m_file(m_path)
{
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
    if (out.size_bytes() < sectorSize())
        throw std::logic_error("buffer too small");

    m_file.seekg(index * sectorSize());
    m_file.read(reinterpret_cast<char*>(out.data()), sectorSize());

    if (!m_file.good())
        throw std::runtime_error("I/O error");
}


void Disk::writeSector(std::uint64_t index, std::span<const std::byte> in)
{
    if (in.size_bytes() != sectorSize())
        throw std::logic_error("invalid buffer size");

    m_file.seekp(index * sectorSize());
    m_file.write(reinterpret_cast<const char*>(in.data()), sectorSize());

    if (!m_file.good())
        throw std::runtime_error("I/O error");
}


void Disk::inferDiskGeometry(const options& opt)
{
    log(DefaultFacility, priority::debug, "inferring disk geometry for non-block-device disk");

    // First step: assume the sector size is 512 bytes.
    m_sectorSize = 512;
    log(DefaultFacility, priority::debug, "assuming sector size is {} bytes", m_sectorSize);

    // Next, look at the total size to see if it is any of the standard floppy disk sizes.
    constexpr auto kB = 1024;
    auto size = std::filesystem::file_size(m_path);
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
