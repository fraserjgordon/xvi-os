#include <System/Boot/Igniter/Tool/Bootsector.hh>

#include <cstring>
#include <span>

#include <System/Utility/Logger/Logger.hh>

#include <System/Boot/Igniter/Tool/Blocklist.hh>
#include <System/Boot/Igniter/Tool/CHS.hh>


namespace System::Boot::Igniter
{


namespace FAT = System::Filesystem::FAT;

using System::Utility::Logger::DefaultFacility;
using System::Utility::Logger::log;
using System::Utility::Logger::priority;


void Bootsector::readFromDisk(const Disk& disk)
{
    // Read the first sector of the disk (the bootsector) into our cache.
    disk.readSector(0, std::as_writable_bytes(std::span{&m_sector, 1}));

    // Parse and check the sector that was just read.
    parseCurrentBootsector(disk);
}


void Bootsector::writeToDisk(Disk& disk)
{
    // Overwrite the first sector of the disk with the new bootsector.
    disk.writeSector(0, std::as_bytes(std::span{&m_sector, 1}));
}


void Bootsector::applyStage0(std::span<const std::byte> buffer)
{
    // Sanity check the incoming bootsector.
    if (buffer.size_bytes() != 512)
    {
        log(DefaultFacility, priority::error, "invalid Stage0 loader size: {} bytes (expected 512)", buffer.size_bytes());
        throw std::logic_error("unexpected Stage0 loader size");
    }

    // Check that the Stage0 loader meets our expectations.
    auto stage0 = reinterpret_cast<const layout_t*>(buffer.data());
    if (!validateStage0Loader(*stage0))
    {
        log(DefaultFacility, priority::error, "invalid Stage0 loader");
        throw std::logic_error("invalid Stage0 loader");
    }

    // Only a subset of the fields get copied into the existing bootloader (the bits that contain code).
    std::memcpy(m_sector.fields.jmp, stage0->fields.jmp, sizeof(m_sector.fields.jmp));
    std::memcpy(m_sector.fields.code_0, stage0->fields.code_0, sizeof(m_sector.fields.code_0));
    std::memcpy(m_sector.fields.code_1, stage0->fields.code_1, sizeof(m_sector.fields.code_1));

    // Also ensure that the boot signature is set.
    std::memcpy(m_sector.fields.bootsig, stage0->fields.bootsig, sizeof(m_sector.fields.bootsig));

    // Clear the boot options.
    std::memset(&m_sector.fields.opts, 0, sizeof(m_sector.fields.opts));

    // If we're applying this to a floppy disk bootsector, clear the partition table so that the Stage0 loader doesn't
    // try to load a partition bootsector.
    if (FAT::IsFloppyMediaType(m_sector.fields.bpb.common.medium_type))
    {
        log(DefaultFacility, priority::debug, "floppy medium type detected; clearing partition table");
        std::memset(m_sector.fields.partitions, 0, sizeof(m_sector.fields.partitions));
    }
    else
    {
        //! @todo - the logic here depends on MBR vs VBR...
        throw std::logic_error("not implemented");
    }
}


void Bootsector::setBlockListBlock(std::uint32_t lba)
{
    if (m_fsType != fat_type::FAT12 && m_fsType != fat_type::NTFS)
    {
        log(DefaultFacility, priority::error, "cannot set blocklist block: bootsector format is not supported");
        throw std::runtime_error("invalid bootsector format");
    }

    // The drive will use LBA addressing if it is anything except a floppy disk.
    std::uint32_t value;
    bool useLBA = m_sector.fields.bpb.common.medium_type == FAT::media_descriptor::FixedDisk;

    if (useLBA)
    {
        log(DefaultFacility, priority::debug, "setting blocklist block to LBA {}", lba);
        value = lba;
    }
    else
    {
        // Because it isn't possible to be sure which fields in the BPB are valid, we use the media type to look up the
        // expected disk geometry.
        auto geometry = FAT::GetExpectedGeometry(m_sector.fields.bpb.common.medium_type);

        std::uint32_t sectors_per_track = geometry.sectors;
        std::uint32_t head_count = geometry.heads;
        std::uint32_t cylinder_count = geometry.cylinders;
        log(DefaultFacility, priority::debug, "drive addressed using CHS; converting LBA {} using geometry {},{},{} (medium type 0x{:x})",
            lba,
            cylinder_count, head_count, sectors_per_track,
            m_sector.fields.bpb.common.medium_type
        );

        auto [cylinder, head, sector] = LBAToCHS({cylinder_count, head_count, sectors_per_track}, lba);
        
        log(DefaultFacility, priority::debug, "setting blocklist block to C,H,S {},{},{}", cylinder, head, sector);

        value = EncodeCHS({cylinder, head, sector});
    }

    m_sector.fields.opts.list_block = value;
}


void Bootsector::parseCurrentBootsector(const Disk& disk)
{
    // We need to detect what filesystem (if any) is currently making use of the bootsector.
    if (m_sector.fields.jmp[0] == std::byte{0xEB}
        && m_sector.fields.jmp[1] == std::byte{0x76}
        && m_sector.fields.jmp[2] == std::byte{0x90}
        && m_sector.fields.oem_signature == FAT::BPBSignatureExFAT)
    {
        // Once we've detected ExFAT, there's nothing more of use in the bootsector for us.
        m_fsType = fat_type::ExFAT;

        log(DefaultFacility, priority::debug, "existing bootsector detected as ExFAT");

        return;
    }

    // Check for a recognised jump instruction at the beginning of the bootsector.
    bool recognised_jump = false;
    if (m_sector.fields.jmp[0] == std::byte{0xEB} && m_sector.fields.jmp[2] == std::byte{0x90})
    {
        // We have an 8-bit jump followed by a nop.
        recognised_jump = true;
    }
    else if (m_sector.fields.jmp[0] == std::byte{0xE9})
    {
        // We have a 16-bit jump.
        recognised_jump = true;
    }

    if (!recognised_jump)
    {
        // We don't recognise the jump sequence at the beginning. The BPB is likely to be invalid. We can be sure, at
        // least, that this isn't a valid FAT filesystem.
        log(DefaultFacility, priority::debug, "unrecognised jump bytes in existing bootsector: {:x} {:x} {:x}",
            m_sector.fields.jmp[0], m_sector.fields.jmp[1], m_sector.fields.jmp[2]
        );

        log(DefaultFacility, priority::debug, "existing bootsector format unknown");
        m_fsType = fat_type::Unknown;

        // Without a valid BPB, there's nothing else for us to extract.
        return;
    }

    // Check whether this is actually an NTFS BPB.
    if (m_sector.fields.oem_signature == FAT::BPBSignatureNTFS)
    {
        // This is NTFS. The initial fields of the BPB are common with FAT BPBs.
        log(DefaultFacility, priority::debug, "existing bootsector detected as NTFS");
        m_fsType = fat_type::NTFS;
    }
    else
    {
        // If it's not something else, assume this is FAT. There's no way to be sure as FAT doesn't have any particular
        // magic number in the bootsector.
        //
        // For the purpose of the bootsector, we don't care what type of FAT it is either. Just assume FAT12 here.
        log(DefaultFacility, priority::debug, "assuming existing bootsector contains FAT BPB");
        m_fsType = fat_type::FAT12;
    }

    // Make sure that the media type is valid.
    if (!FAT::IsValidMediaType(m_sector.fields.bpb.common.medium_type))
    {
        log(DefaultFacility, priority::error, "existing bootsector has invalid medium type");
        throw std::runtime_error("invalid bootsector format");
    }
}


bool Bootsector::validateStage0Loader(const layout_t& sector)
{
    // First check: the "jump" instructions are a valid x86 short jump. The first byte is a short jump, the second is a
    // positive offset equal to the size of BPB and options area and the third byte is a nop.
    if (sector.fields.jmp[0] != std::byte{0xEB}
        || sector.fields.jmp[1] != std::byte{1 + sizeof(bpb_oem_sig) + sizeof(bpb_union) + sizeof(boot_opts)}
        || sector.fields.jmp[2] != std::byte{0x90})
    {
        log(DefaultFacility, priority::debug, "invalid jump instruction pattern: {:x} {:x} {:x}", sector.fields.jmp[0], sector.fields.jmp[1], sector.fields.jmp[2]);
        return false;
    }

    // Second check: the areas to be taken from the original bootsector are filled with 0xCC.
    auto isCCFilled = []<class T>(const T& field)
    {
        auto span = std::as_bytes(std::span{&field, 1});
        for (auto byte : span)
        {
            if (byte != std::byte{0xCC})
                return false;
        }

        return true;
    };

    if (!isCCFilled(sector.fields.oem_signature))
    {
        log(DefaultFacility, priority::debug, "invalid fill pattern for OEM signature area");
        return false;
    }

    if (!isCCFilled(sector.fields.bpb))
    {
        log(DefaultFacility, priority::debug, "invalid fill pattern for BPB area");
        return false;
    }

    if (!isCCFilled(sector.fields.opts))
    {
        log(DefaultFacility, priority::debug, "invalid fill pattern for options area");
        return false;
    }

    if (!isCCFilled(sector.fields.timestamp))
    {
        log(DefaultFacility, priority::debug, "invalid fill pattern for timestamp area");
        return false;
    }

    if (!isCCFilled(sector.fields.signature))
    {
        log(DefaultFacility, priority::debug, "invalid fill pattern for disk signature area");
        return false;
    }

    if (!isCCFilled(sector.fields.partitions))
    {
        log(DefaultFacility, priority::debug, "invalid fill pattern for partition table area");
        return false;
    }

    // Third check: the code areas do not begin with 0xCC. This checks that the reserved areas have not overflowed into
    // what we expect to be code areas. We can only check the first byte as the value may naturally appear in the code
    // as long as it isn't the first byte of an opcode. Disassembling the code is overkill so a first byte check is
    // a reasonable compromise.
    if (sector.fields.code_0[0] == std::byte{0xCC} || sector.fields.code_1[0] == std::byte{0xCC})
    {
        log(DefaultFacility, priority::debug, "unexpected 0xCC byte in code area");
        return false;
    }

    // Fourth check: the boot signature is valid.
    if (sector.fields.bootsig[0] != std::byte{0x55} || sector.fields.bootsig[1] != std::byte{0xAA})
    {
        log(DefaultFacility, priority::debug, "invalid boot signature: {:x} {:x}", sector.fields.bootsig[0], sector.fields.bootsig[1]);
        return false;
    }

    // All sanity checks passed.
    return true;
}


} // namespace System::Boot::Igniter
