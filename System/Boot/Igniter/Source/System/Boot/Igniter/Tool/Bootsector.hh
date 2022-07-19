#ifndef __SYSTEM_BOOT_IGNITER_TOOL_BOOTSECTOR_H
#define __SYSTEM_BOOT_IGNITER_TOOL_BOOTSECTOR_H



#include <cstddef>

#include <System/Filesystem/FAT/BPB.hh>


namespace System::Boot::Igniter
{


class Bootsector
{
private:

    static constexpr std::size_t BootsectorSize = 512;

    template <class T> using unaligned = System::Filesystem::FAT::unaligned<T>;

    using bpb_union = System::Filesystem::FAT::bpb_union;
    using uint32le_t = unaligned<System::Utility::Endian::uint32le_t>;

    union layout_t
    {
        std::byte       bytes[BootsectorSize] = {};
        struct fields_t
        {
            std::byte   jmp[3];         // x86 "jmp" opcode to jump over the BIOS parameter block (BPB).
            bpb_union   bpb;            // BIOS parameter block (may be smaller than this).
            uint32le_t  list_block;     // LBA or CHS address of a sector containing the next stage sector list.
            std::byte   code_0[132];    // Bootsector code.
            std::byte   misc[6];        // ???
            std::byte   code_1[216];    // Bootsector code.
            std::byte   signature[6];   // ???
            std::byte   partitions[64]; // Partition table.
            std::byte   bootsig[2];     // Boot signature.
        } fields;
    };

    static_assert(sizeof(layout_t) == BootsectorSize);
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_TOOL_BOOTSECTOR_H */
