#include <System/Boot/Igniter/Tool/Blocklist.hh>

#include <type_traits>


namespace System::Boot::Igniter
{


std::uint32_t EncodeCHS(const chs_address& address)
{
    auto [cylinder, head, sector] = address;
    return ((head & 0xFF) << 8) | ((sector & 0x3F) << 16) | ((cylinder & 0xFF) << 24) | ((cylinder & 0x300) << 18);
}


std::size_t BlockListLength(std::size_t n)
{
    constexpr auto first = std::tuple_size_v<decltype(blocklist_header::blocks)>;
    constexpr auto second = std::tuple_size_v<decltype(blocklist_block::blocks)>;
    if (n <= first)
        return 1;

    return ((n - first) + (second - 1)) / second;
}


} // namespace System::Boot::Igniter
