#include <System/Boot/Igniter/Tool/Disk.hh>


namespace System::Boot::Igniter
{


Disk::Disk(const std::filesystem::path& path) :
    m_file(path)
{
}


std::size_t Disk::sectorSize() const
{
    return 512;
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


} // namespace System::Boot::Igniter
