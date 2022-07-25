#ifndef __SYSTEM_BOOT_IGNITER_TOOL_BOOTSECTOR_H
#define __SYSTEM_BOOT_IGNITER_TOOL_BOOTSECTOR_H



#include <cstddef>
#include <span>

#include <System/Filesystem/FAT/BPB.hh>

#include <System/Boot/Igniter/Tool/Disk.hh>


namespace System::Boot::Igniter
{


class Bootsector
{
public:

    void readFromDisk(const Disk&);
    void writeToDisk(Disk&);

    void applyStage0(std::span<const std::byte> stage0);

    void setBlockListBlock(std::uint32_t);


private:

    static constexpr std::size_t BootsectorSize = 512;

    template <class T> using unaligned = System::Filesystem::FAT::unaligned<T>;

    using bpb_oem_sig = System::Filesystem::FAT::bpb_oem_sig;
    using bpb_union = System::Filesystem::FAT::bpb_union;
    using fat_type = System::Filesystem::FAT::fat_type;
    using uint32le_t = unaligned<System::Utility::Endian::uint32le_t>;

    struct boot_opts
    {
        uint32le_t      list_block;     // LBA or CHS address of a sector containing the next stage sector list.
    };

    union layout_t
    {
        std::byte       bytes[BootsectorSize] = {};
        struct fields_t
        {
            std::byte   jmp[3];         // x86 "jmp" opcode to jump over the BIOS parameter block (BPB).
            bpb_oem_sig oem_signature;  // Identifiers creator of the filesystem.
            bpb_union   bpb;            // BIOS parameter block (may be smaller than this).
            boot_opts   opts;           // Bootsector options.   
            std::byte   code_0[124];    // Bootsector code.
            std::byte   timestamp[6];   // Sometimes used as a disk timestamp.
            std::byte   code_1[216];    // Bootsector code.
            std::byte   signature[6];   // Disk signature.
            std::byte   partitions[64]; // Partition table.
            std::byte   bootsig[2];     // Boot signature.
        } fields;
    };

    static_assert(sizeof(layout_t) == BootsectorSize);


    layout_t    m_sector;

    // Filesystem type that we think the bootsector is being used by.
    fat_type    m_fsType = fat_type::Unknown;


    void parseCurrentBootsector();

    bool validateStage0Loader(const layout_t& sector);
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_TOOL_BOOTSECTOR_H */
