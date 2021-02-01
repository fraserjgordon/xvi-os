#ifndef __SYSTEM_FIRMWARE_DEVICETREE_FDT_H
#define __SYSTEM_FIRMWARE_DEVICETREE_FDT_H


#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string_view>

#include <System/Utility/Endian/Endian.hh>

#include <System/Firmware/DeviceTree/Private/Config.hh>


namespace System::Firmware::DeviceTree
{


using System::Utility::Endian::uint32be_t;
using System::Utility::Endian::uint64be_t;


enum class FdtTreeEvent : std::uint32_t
{
    BeginNode       = 0x00000001,   // Start of a new node.
    EndNode         = 0x00000002,   // End of a node.
    Property        = 0x00000003,   // Property definition.
    Nop             = 0x00000004,   // Ignored.
    End             = 0x00000009,   // End of tree.
};

struct fdt_header
{
    static constexpr std::uint32_t FdtMagic      = 0xd00dfeed;
    static constexpr std::uint32_t FdtMaxVersion = 17;
    static constexpr std::uint32_t FdtMinVersion = 16;

    uint32be_t      magic                   = FdtMagic;
    uint32be_t      fdt_size                = 0;
    uint32be_t      tree_offset             = 0;
    uint32be_t      strings_offset          = 0;
    uint32be_t      memory_reserve_offset   = 0;
    uint32be_t      version                 = FdtMaxVersion;
    uint32be_t      compatible_version      = FdtMinVersion;
    uint32be_t      boot_cpu_id             = 0;
    uint32be_t      strings_size            = 0;
    uint32be_t      tree_size               = 0;
};

struct fdt_memory_reservation
{
    uint64be_t      address                 = 0;
    uint64be_t      size                    = 0;
};


// Base class for FDT events.
struct fdt_event
{
    FdtTreeEvent            event_type      = FdtTreeEvent::Nop;
};

// Event information for the beginning of a new node.
struct fdt_node_begin_event : fdt_event
{
    std::string_view        node_name       = {};   // May contain unit name after an '@' character.
};

// Event information for the end of the current node.
struct fdt_node_end_event : fdt_event
{
};

// Event information for a property definition.
struct fdt_property_event : fdt_event
{
    std::string_view            name            = {};   // Property name.
    std::span<const std::byte>  value           = {};   // Unspecified interpretation.
};

// Union of all event types.
union fdt_event_union
{
    fdt_event               common;
    fdt_node_begin_event    node_begin;
    fdt_node_end_event      node_end;
    fdt_property_event      property;
};


// FDT tree parsing cursor.
struct fdt_tree_parse_cursor
{
    std::uintptr_t private_data[2] = {};
};


// Validates that the given block of memory contains a valid FDT. The following aspects are checked:
//  - the magic number is correct
//  - the version is understood by this library
//  - the sizes of the various sub-components are consistent
//
// On the other hand, the tree itself is not parsed or checked.
//
// Returns a pointer to the header if it appears valid or null otherwise.
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT
const fdt_header* parseFDTHeader(std::span<const std::byte>) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(ParseFDTHeader);

// Creates a tree parsing cursor from the given (pre-validated) FDT header.
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT
fdt_tree_parse_cursor createFDTParseCursor(const fdt_header*) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(CreateFDTParseCursor);

// Advances a parse tree cursor to the next event.
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT
bool advanceFDTParseCursor(fdt_tree_parse_cursor&) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(AdvanceFDTParseCursor);

// Reads the event at the current parse cursor.
__SYSTEM_FIRMWARE_DEVICETREE_EXPORT
fdt_event_union getFDTParseEvent(const fdt_tree_parse_cursor&) __SYSTEM_FIRMWARE_DEVICETREE_SYMBOL(GetFDTParseEvent);


} // System::Firmware::DeviceTree


#endif /* ifndef __SYSTEM_FIRMWARE_DEVICETREE_FDT_H */
