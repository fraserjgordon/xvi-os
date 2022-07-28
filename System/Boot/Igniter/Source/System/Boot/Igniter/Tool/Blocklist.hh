#ifndef __SYSTEM_BOOT_IGNITER_TOOL_BLOCKLIST_H
#define __SYSTEM_BOOT_IGNITER_TOOL_BLOCKLIST_H


#include <array>
#include <cstdint>

#include <System/Utility/Endian/Endian.hh>

#include <System/Boot/Igniter/Tool/CHS.hh>


namespace System::Boot::Igniter
{


using uint16le_t = System::Utility::Endian::uint16le_t;
using uint32le_t = System::Utility::Endian::uint32le_t;


struct blocklist_header
{
    uint16le_t      load_offset;
    uint16le_t      load_segment;

    uint32le_t      next_blocklist;

    std::array<uint32le_t, 126> blocks;
};

struct blocklist_block
{
    uint32le_t      next_blocklist;

    std::array<uint32le_t, 127> blocks;
};

static_assert(sizeof(blocklist_header) == 512);
static_assert(sizeof(blocklist_block) == 512);


// Converts a CHS address to the encoded form expected by the Stage0 loader.
std::uint32_t EncodeCHS(const chs_address&);

// Returns the number of block list blocks needed for an image that is N blocks long.
std::size_t BlockListLength(std::size_t n);


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_TOOL_BLOCKLIST_H */
