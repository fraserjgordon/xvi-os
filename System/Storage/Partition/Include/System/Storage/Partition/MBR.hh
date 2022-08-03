#ifndef __SYSTEM_STORAGE_PARTITION_MBR_H
#define __SYSTEM_STORAGE_PARTITION_MBR_H


#if defined(__XVI_NO_STDLIB)
#  include <System/C++/LanguageSupport/StdDef.hh>
#  include <System/C++/LanguageSupport/StdInt.hh>
#  include <System/C++/Utility/Array.hh>
#else
#  include <array>
#  include <cstddef>
#  include <cstdint>
#endif

#include <System/Utility/Endian/Unaligned.hh>


namespace System::Storage::Partition
{



struct CHSAddress
{
    std::array<std::uint8_t, 3> data = {};

    std::uint8_t head() const noexcept
    {
        return data[0];
    }

    std::uint16_t sector() const noexcept
    {
        return data[1] & 0x3F;
    }

    std::uint16_t cylinder() const noexcept
    {
        return ((data[1] & 0xC0) << 2) | data[2];
    }
};

static_assert(sizeof(CHSAddress) == 3);


// This list of partition types is incomplete - only those with direct relevance to partitioning (i.e those types used
// to mark extended partitions) are recorded here. Other values exist, with varying degrees of standardisation, but do
// not have an entry here.
enum class PartitionType : std::uint8_t
{
    ExtendedCHS         = 0x05,     // Extended partition, CHS addressing.
    ExtendedLBA         = 0x0F,     // Extended partition, LBA addressing.

    DisklabelDragonfly  = 0x6C,     // Further partitioned via DragonflyBSD disklabel.
    DisklabelFreeBSD    = 0xA5,     // Further partitioned via FreeBSD disklabel.
    DisklabelOpenBSD    = 0xA6,     // Further partitioned via OpenBSD disklabel.
    DisklabelNetBSD     = 0xA9,     // Further partitioned via NetBSD disklabel.

    GPT                 = 0xEE,     // Protective MBR partition for a GPT disk.
};


struct MBRPartitionRecord
{
    // Only one flag bit is defined: whether the partition is "bootable" or not. (Actual interpretation of this bit is
    // entirely up to the boot loader and it may or may not actually matter).
    static constexpr std::uint8_t   Bootable = 0x80;

    std::uint8_t                    flags       = 0;
    CHSAddress                      startCHS    = {};
    std::uint8_t                    type        = 0;
    CHSAddress                      endCHS      = {};
    Utility::Endian::uint32ule_t    startLBA    = 0;
    Utility::Endian::uint32ule_t    endLBA      = 0;
};

static_assert(sizeof(MBRPartitionRecord) == 16);
static_assert(alignof(MBRPartitionRecord) == 1);


struct MBR
{
    // Expected boot signature value, to mark the MBR as valid.
    static constexpr std::uint16_t      BootSignatureMagic = 0xAA55;

    std::byte                           _pad[0x1BE]     = {};
    std::array<MBRPartitionRecord, 4>   partitions      = {};
    Utility::Endian::uint16le_t         bootSignature   = BootSignatureMagic;
};

static_assert(sizeof(MBR) == 512);


struct EBR
{
    // Expected boot signature value, to mark the EBR as valid.
    static constexpr std::uint16_t      BootSignatureMagic = 0xAA55;

    std::byte                           _pad[0x1BE]     = {};
    MBRPartitionRecord                  innerPartition  = {};
    MBRPartitionRecord                  nextEBR         = {};
    std::byte                           _zeroes[0x20]   = {};
    Utility::Endian::uint16le_t         bootSignature   = BootSignatureMagic;
};

static_assert(sizeof(EBR) == 512);


} // namespace System::Storage::Partition


#endif /* ifndef __SYSTEM_STORAGE_PARTITION_MBR_H */
