#include <System/Boot/Igniter/Tool/CHS.hh>


namespace System::Boot::Igniter
{


chs_address LBAToCHS(const chs_geometry& geometry, std::uint64_t lba)
{
    // Note that in CHS, sector addressing is 1-based.
    auto sectors_per_cylinder = geometry.sectors * geometry.heads;
    std::uint32_t cylinder = lba / sectors_per_cylinder;
    std::uint32_t head = (lba % sectors_per_cylinder) / geometry.sectors;
    std::uint32_t sector = 1 + lba % geometry.sectors;

    return {cylinder, head, sector};
}


std::uint64_t CHSToLBA(const chs_geometry& geometry, const chs_address& address)
{
    // Note that in CHS, sector addressing is 1-based.
    auto sectors_per_cylinder = geometry.sectors * geometry.heads;
    return (address.cylinder * sectors_per_cylinder) + (address.head * geometry.sectors) + (address.sector - 1);
}


} // namespace System::Boot::Igniter
