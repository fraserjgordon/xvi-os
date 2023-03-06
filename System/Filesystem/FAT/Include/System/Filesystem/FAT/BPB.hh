#pragma once
#ifndef __SYSTEM_FS_FAT_BPB_H
#define __SYSTEM_FS_FAT_BPB_H


#include <array>
#include <cstddef>
#include <cstdint>

#include <System/Utility/Endian/Unaligned.hh>


namespace System::Filesystem::FAT
{


using System::Utility::Endian::uint16le_t;
using System::Utility::Endian::uint32le_t;
using System::Utility::Endian::unaligned;


using bpb_oem_sig = std::array<char, 8>;

consteval bpb_oem_sig make_oem_sig(const char (&sig)[9])
{
    return {sig[0], sig[1], sig[2], sig[3], sig[4], sig[5], sig[6], sig[7]};
}

// Some OEM signatures that have special semantics:
constexpr bpb_oem_sig BPBSignatureDefault   = make_oem_sig("MSWIN4.1"); 
constexpr bpb_oem_sig BPBSignatureNTFS      = make_oem_sig("NTFS    ");
constexpr bpb_oem_sig BPBSignatureExFAT     = make_oem_sig("EXFAT   ");


// FAT types, as determined from the BPB.
enum class fat_type
{
    FAT12       = 12,               //!< For partitions with <4085 clusters.
    FAT16       = 16,               //!< For partitions with >=4085 but <65525 clusters.
    FAT32       = 32,               //!< For partitions with >=65525 clusters.

    // Non-FAT types that are also useful to detect.
    ExFAT       = -3,
    NTFS        = -2,

    Unknown     = -1,               //!< Unrecognised or unknown FAT type.
};

// Max filesystem sizes for each FAT type.
constexpr std::uint32_t MaxClustersFAT12        = 4084;
constexpr std::uint32_t MaxClustersFAT16        = 65524;


// FAT media descriptors (defines the geometry of the drive).
//
// Note that some media types are shared across different drive types but with different meanings. So for floppy drives,
// the meaning of the media byte depends on the type of drive (3.5", 5.25" or 8").
//
// There is an additional complication with 8" drives: two incompatible floppy types share the same media byte value. To
// distinguish them an attempt should be made to read the single-density address mark to determine single vs double
// density.
enum class media_descriptor : std::uint8_t
{
    // Non-floppy media.
    FixedDisk               = 0xF8, //!< Anything that is not a floppy disk.

    // 3.5" floppy disks.
    Floppy35_HighDensity    = 0xF0, //!< 3.5" high-density floppy (1440kB/2880kB).
    Floppy35_720k           = 0xF9, //!< 3.5" double-density floppy (720kB).

    // 5.25" floppy disks.
    Floppy525_1200k         = 0xF9, //!< 5.25" 15-sector double-sided high-density floppy (1200kB).
    Floppy525_180k          = 0xFC, //!< 5.25" 9-sector single-sided double-density floppy (180kB).
    Floppy525_360k          = 0xFD, //!< 5.25" 9-sector double-sided double-density floppy (360kB).
    Floppy525_160k          = 0xFE, //!< 5.25" 8-sector single-sided double-density floppy (160kB).
    Floppy525_320k          = 0xFF, //!< 5.25" 8-sector double-sided double-density floppy (320kB).

    // 8" floppy disks.
    Floppy8_500k            = 0xFD, //!< 8" double-sided single-density floppy (500kB).
    Floppy8_250k            = 0xFE, //!< 8" single-sided single-density floppy (250kB).
    Floppy8_1200k           = 0xFE, //!< 8" double-sided double-density floppy (1200kB).
};


//! Returns true if the given media byte is a valid floppy disk media type.
bool IsFloppyMediaType(media_descriptor);

//! Returns true if the given media byte is valid.
bool IsValidMediaType(media_descriptor);


// Structure representing drive geometries implied by different media type bytes.
struct media_descriptor_geometry
{
    std::uint16_t   cylinders   = 0;    //!< Number of cylinders (tracks).
    std::uint16_t   heads       = 0;    //!< Number of drive heads.
    std::uint32_t   sectors     = 0;    //!< Number of sectors per track.
};


//! Looks up an expected geometry given a media type byte.
//!
//! @returns the expected geometry or zeroes if a non-floppy media type is given.
media_descriptor_geometry GetExpectedGeometry(media_descriptor);


// Signature used to recognise extended BPBs.
enum class extended_signature : std::uint8_t
{
    VolumeSerial        = 0x28,     //!< Only volume serial number is present.
    EBPB                = 0x29,     //!< All EBPB fields are present.
};


// Fields that are always present in the BPB.
struct bpb_common
{
    unaligned<uint16le_t>   sector_size;            //!< Sector size in bytes. Usually 512.
    std::uint8_t            sectors_per_cluster;    //!< Number of sectors per FAT cluster.
    unaligned<uint16le_t>   reserved_sectors;       //!< Number of reserved sectors (for the bootsector, etc).
    std::uint8_t            fat_count;              //!< Number of FAT copies.
    unaligned<uint16le_t>   max_root_dir_entries;   //!< Number of preallocated root dir entries. Zero for FAT32.
    unaligned<uint16le_t>   sector_count;           //!< Total number of sectors. If zero, stored as 32 bits elsewhere.
    media_descriptor        medium_type;            //!< Type of disk drive.
    unaligned<uint16le_t>   sectors_per_fat;        //!< Number of sectors per FAT. Zero for FAT32.

    //! Determines a FAT filesystem's FAT variant based on the BPB.
    FAT::fat_type fat_type() const; 
};

// Fields present from MS-DOS 3.0 until MS-DOS 3.31.
struct bpb_v3_0 : bpb_common
{
    unaligned<uint16le_t>   sectors_per_track;      //!< Sectors per track in CHS geometry.
    unaligned<uint16le_t>   head_count;             //!< Number of heads in CHS geometry.
    unaligned<uint16le_t>   hidden_sectors;         //!< Number of sectors on the drive that precede this partition.
};

// Fields present from MS-DOS 3.2 until MS-DOS 3.31.
struct bpb_v3_2 : bpb_v3_0
{
    unaligned<uint16le_t>   total_sectors;          //!< Total sector count including hidden sectors.
};

// Fields present from MS-DOS 3.31 onwards.
struct bpb_v3_31 : bpb_common
{
    unaligned<uint16le_t>   sectors_per_track;      //!< Sectors per track in CHS geometry.
    unaligned<uint16le_t>   head_count;             //!< Number of heads in CHS geometry.
    unaligned<uint32le_t>   hidden_sectors;         //!< Number of sectors on the drive that precede this partition.
    unaligned<uint32le_t>   sector_count_32;        //!< 32-bit sector count (if sector_count is zero).
};

// Fields present from MS-DOS 4.0 onwards for FAT12/FAT16 volumes.
struct extended_bpb : bpb_v3_31
{
    std::uint8_t            drive_number;           //!< BIOS drive number to boot from.
    std::uint8_t            reserved;               //!< Should be ignored.
    extended_signature      ext_signature;          //!< Indicates extension field validity.
    unaligned<uint32le_t>   volume_serial;          //!< Volume serial number.
    std::array<char, 11>    volume_label;           //!< Volume label string.
    std::array<char, 8>     fs_type_label;          //!< Filesystem type label; for display purposes only.
};

// Fields present for FAT32 volumes.
struct fat32_bpb : bpb_v3_31
{
    unaligned<uint32le_t>   sectors_per_fat_32;     //!< 32-bit count of sectors per FAT, if sectors_per_fat is zero.
    unaligned<uint16le_t>   mirroring_flags;        //!< FAT mirroring flags.
    std::uint8_t            version_minor;          //!< FAT32 version minor number.
    std::uint8_t            version_major;          //!< FAT32 version major number.
    unaligned<uint32le_t>   root_dir_cluster;       //!< First cluster of the root directory.
    unaligned<uint16le_t>   fs_info_sector;         //!< Sector number of the FS information sector.
    unaligned<uint16le_t>   backup_bootsector;      //!< First sector of the bootsector backup copy.
    std::byte               reserved[12];           //!< Should be ignored.
    std::uint8_t            drive_number;           //!< BIOS drive number to boot from.
    std::uint8_t            reserved2;              //!< Should be ignored.
    extended_signature      ext_signature;          //!< Indicates extension field validity.
    unaligned<uint32le_t>   volume_serial;          //!< Volume serial number.
    std::array<char, 11>    volume_label;           //!< Volume label string.
    std::array<char, 8>     fs_type_label;          //!< Filesystem type labe; for display purposes only.
};

static_assert(sizeof(bpb_common) == 13);
static_assert(sizeof(bpb_v3_0) == 19);
static_assert(sizeof(bpb_v3_2) == 21);
static_assert(sizeof(bpb_v3_31) == 25);
static_assert(sizeof(extended_bpb) == 51);
static_assert(sizeof(fat32_bpb) == 79);


//! Union over all BPB types.
union bpb_union
{
    bpb_common      common;     //!< Fields common to all BPB types.
    bpb_v3_0        v3_0;       //!< Fields present from MS-DOS 3.0 until 3.31.
    bpb_v3_2        v3_2;       //!< Fields present from MS-DOS 3.2 until 3.31.
    bpb_v3_31       v3_31;      //!< Fields present from MS-DOS 3.31 onwards.
    extended_bpb    ext;        //!< Extended BPB for FAT12/FAT16 from MS-DOS 4.0 onwards.
    fat32_bpb       fat32;      //!< Extended BPB for FAT32 volumes.
};

static_assert(sizeof(bpb_union) == sizeof(fat32_bpb));

//! Disk timestamp structure.
struct disk_timestamp
{
    uint16le_t      zero;       //!< Should be zero.
    std::uint8_t    drive_num;  //!< BIOS drive number when the disk was formatted.
    std::uint8_t    seconds;    //!< Seconds field; [0-59].
    std::uint8_t    minutes;    //!< Minutes field; [0-59].
    std::uint8_t    hours;      //!< Hours field; [0-23].
};

static_assert(sizeof(disk_timestamp) == 6);

// Packed CHS (Cylinder-Head-Sector) disk address.
struct packed_chs
{
    std::byte       packed[3];
};

static_assert(sizeof(packed_chs) == 3);

// Partition table entry.
struct partition
{
    std::uint8_t            flags;      //!< Bit 7: if set, drive is bootable.
    packed_chs              start_chs;  //!< CHS address of the first sector of the partition.
    std::uint8_t            type;       //!< Partition/filesystem type.
    packed_chs              end_chs;    //!< CHS address of the last sector of the partition.
    unaligned<uint32le_t>   start_lba;  //!< Linear address of the first sector of the partition.
    unaligned<uint32le_t>   end_lba;    //!< Linear address of the last sector of the partition.
};

static_assert(sizeof(partition) == 16);

struct bootsector
{
    std::byte               boot_jump[3];       //!< x86 opcode(s) to jump over the BPB when used as a boot sector.
    bpb_oem_sig             oem_signature;      //!< Identifies the system that formatted the disk.
    bpb_union               bpb;                //!< BPB structures.
    std::byte               boot_code_1[128];   //!< Bootloader code and data.
    disk_timestamp          timestamp;          //!< Disk timestamp.
    std::byte               boot_code_2[216];   //!< Bootloader code and data.
    uint32le_t              disk_signature;     //!< ???
    uint16le_t              disk_signature_2;   //!< ???
    partition               partition_table[4]; //!< Partition table (partitioned media only).
    std::byte               boot_signature[2];  //!< Set to {0x55, 0xAA} if this is a bootsector.
};

static_assert(sizeof(bootsector) == 512);


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FS_FAT_BPB_H */
