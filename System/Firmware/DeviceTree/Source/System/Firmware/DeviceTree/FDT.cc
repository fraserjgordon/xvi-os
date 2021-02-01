#include <System/Firmware/DeviceTree/FDT.hh>


namespace System::Firmware::DeviceTree
{


const fdt_header* parseFDTHeader(std::span<const std::byte> bytes)
{
    // First check: the input span is big enough to contain an FDT header.
    if (bytes.size() < sizeof(fdt_header))
        return nullptr;

    // Check the magic number, length and version.
    auto header = reinterpret_cast<const fdt_header*>(bytes.data());
    if (header->magic != fdt_header::FdtMagic
        || header->compatible_version < fdt_header::FdtMinVersion
        || header->compatible_version > fdt_header::FdtMaxVersion
        || std::size_t(header->fdt_size) < sizeof(fdt_header)
        || std::size_t(header->fdt_size) > bytes.size())
    {
        return nullptr;
    }

    // Check that the various blocks of the FDT are within the span.
    std::uint32_t length = header->fdt_size;
    if (header->tree_offset > length
        || (header->tree_offset + header->tree_size) > length
        || header->strings_offset > length
        || (header->strings_offset + header->strings_size) > length
        || header->memory_reserve_offset > length)
    {
        return nullptr;
    }

    return header;
}


static inline auto treeSpan(const fdt_header* header, std::uint32_t offset = 0)
{
    std::span all(reinterpret_cast<const std::byte*>(header), header->fdt_size);
    auto tree = all.subspan(header->tree_offset, header->tree_size);
    auto subtree = tree.subspan(offset);
    return subtree;
}

static inline fdt_tree_parse_cursor toCursor(const fdt_header* header, std::span<const std::byte> s)
{
    auto tree = treeSpan(header);
    auto offset = s.data() - tree.data();
    return { reinterpret_cast<std::uintptr_t>(header), offset };
}

static inline std::pair<const fdt_header*, std::span<const std::byte>> fromCursor(const fdt_tree_parse_cursor& c)
{
    auto header = reinterpret_cast<const fdt_header*>(c.private_data[0]);
    auto subtree = treeSpan(header, c.private_data[1]);
    return { header, subtree };
}


fdt_tree_parse_cursor createFDTParseCursor(const fdt_header* header)
{
    auto tree = treeSpan(header);
    return toCursor(header, tree);
}


} // namespace System::Firmware::DeviceTree
