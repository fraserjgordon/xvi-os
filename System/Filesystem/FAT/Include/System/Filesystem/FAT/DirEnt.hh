#pragma once
#ifndef __SYSTEM_FS_FAT_DIRENT_H
#define __SYSTEM_FS_FAT_DIRENT_H


#include <array>
#include <chrono>

#include <System/Utility/Endian/Unaligned.hh>


namespace System::Filesystem::FAT
{


using System::Utility::Endian::uint16le_t;
using System::Utility::Endian::uint32le_t;
using System::Utility::Endian::char16le_t;
using System::Utility::Endian::unaligned;


//! FAT time field.
//!
//! Note that FAT only stores times in this format with two-second precision; odd seconds are rounded off.
struct time_t
{
    uint16le_t                  raw;

    //!< Returns the time-of-day this field represents.
    constexpr std::chrono::hh_mm_ss<std::chrono::seconds> time_of_day() const
    {
        using namespace std::chrono;
        
        // The low 5 bits contain the count of seconds, divided by two.
        std::uint16_t value = raw;
        seconds s = seconds{(value & 0b11111) * 2};
        
        // Then another 6 bits for the count of minutes.
        minutes m = minutes{(value >> 5) & 0b111111};

        // And the final 5 bits contain the count of hours.
        hours h = hours{(value >> 11) & 0b11111};

        return std::chrono::hh_mm_ss<seconds>(h + m + s);
    }
};

//! FAT date field.
struct date_t
{
    uint16le_t                  raw;

    //!< Returns the Year/Month/Day this field represents.
    constexpr std::chrono::year_month_day year_month_day() const
    {
        using namespace std::chrono;

        // The low 5 bits contain the day of the month.
        std::uint16_t value = raw;
        day d = day{value & 0b11111U};

        // The next 4 bits contain the month.
        month m = month{(value >> 5) & 0b1111U};

        // The remaining 7 bits contain the number of years since 1980.
        year y = year{1980} + years{(value >> 9) & 0b1111111U};

        return y/m/d;
    }
};


// Special values for the first byte of a filename.
constexpr std::byte DirectoryEntryUnused    = std::byte{0xE5};  //!< Indicates this directory entry is unused.
constexpr std::byte DirectoryEntryEnd       = std::byte{0x00};  //!< This entry and all later entries are unused.
constexpr std::byte DirectoryEntryEscaped   = std::byte{0x05};  //!< Used if the name needs to begin with byte 0xE5.
constexpr std::byte DirectoryEntryLastLFNBit= std::byte{0x40};  //!< Set in the ordinal field of the last entry of a LFN.

// Values for the directory entry attribute byte.
constexpr std::byte AttributeReadOnly       = std::byte{0x01};  //!< File should not be modified.
constexpr std::byte AttributeHidden         = std::byte{0x02};  //!< File should not show in directory listings.
constexpr std::byte AttributeSystem         = std::byte{0x04};  //!< File is an important system file.
constexpr std::byte AttributeVolumeLabel    = std::byte{0x08};  //!< Directory entry is really a volume label.
constexpr std::byte AttributeDirectory      = std::byte{0x10};  //!< File contains directory entries.
constexpr std::byte AttributeArchive        = std::byte{0x20};  //!< File dirty bit; set on modification.

//! All known attribute bits.
constexpr std::byte AttributeMask           = std::byte{0x2F};

//! This combination of attribute flags indicates the directory entry is a long file name (LFN) entry.
constexpr std::byte AttributeLFN            = std::byte{0x0F};  // AttributeReadOnly|AttributeHidden|AttributeSystem|AttributeVolumeLabel;


//! FAT on-disk directory entry structure.
struct dirent_t
{
    std::array<std::byte, 8>    name_base;      //!< Base part of file name. (e.g. README)
    std::array<std::byte, 3>    name_ext;       //!< File name extension (e.g. .TXT)
    std::byte                   attributes;     //!< File attributes.
    std::byte                   nt_reserved;    //!< Ignored.
    std::uint8_t                ctime_tenths;   //!< File creation time, tenths of a second.
    time_t                      ctime_time;     //!< File creation time, time of day.
    date_t                      ctime_date;     //!< File creation time, date.
    date_t                      atime_date;     //!< File last-access date.
    uint16le_t                  cluster_high;   //!< High 16 bits of the first cluster of the file.
    time_t                      wtime_time;     //!< File last-write time, time of day.
    date_t                      wtime_date;     //!< File last-write time, date.
    uint16le_t                  cluster_low;    //!< Low 16 bits of the first cluster of the file.
    uint32le_t                  size;           //!< File size, in bytes.

    using deciseconds = std::chrono::duration<std::chrono::milliseconds::rep, std::deci>;

    //! Helper because creation time has extra precision.
    std::chrono::hh_mm_ss<deciseconds> get_ctime_time_of_day() const;

    //! Returns the combined cluster number.
    constexpr std::uint32_t get_start_cluster() const
    {
        return (std::uint32_t{cluster_high} << 16) | std::uint16_t{cluster_low};
    }

    //! Returns true if this is an empty entry.
    constexpr bool is_empty() const
    {
        return name_base[0] == DirectoryEntryUnused;
    }

    //! Returns true if this is an empty entry *and* all later entries are also empty
    constexpr bool is_terminator() const
    {
        return name_base[0] == DirectoryEntryEnd;
    }

    //! Utility for checking attribute bits.
    constexpr bool hasAttribute(std::byte attribute) const
    {
        return std::uint8_t(attributes) & std::uint8_t(attribute);
    }

    //! Utility for checking attribute bits.
    constexpr bool hasAttribute(std::byte attribute, std::byte mask) const
    {
        return std::byte{std::uint8_t(attributes) & std::uint8_t(mask)} == attribute;
    }

    //! Returns true if this is an LFN entry.
    constexpr bool isLFNEntry() const
    {
        return hasAttribute(AttributeLFN, AttributeMask);
    }

    //! Returns the ordinal (LFN entry order) of this entry.
    constexpr std::uint8_t lfnOrdinal() const
    {
        return static_cast<std::uint8_t>(name_base[0]);
    }

    //! Returns true if this is the last LFN entry for a file.
    constexpr bool isLastLFNEntry() const
    {
        return isLFNEntry() && (lfnOrdinal() & static_cast<std::uint8_t>(DirectoryEntryLastLFNBit));
    }

    //! Returns the base name of this entry with any escaping corrected.
    constexpr std::array<std::byte, 8> get_name_base() const
    {
        std::array<std::byte, 8> name = name_base;
        if (name[0] == DirectoryEntryEscaped)
            name[0] = std::byte{0xE5};

        return name;
    }

    //! Returns the length of the basename (i.e without trailing spaces).
    constexpr std::size_t get_name_base_length() const
    {
        std::size_t length = 8;
        while (length > 0 && name_base[length - 1] == std::byte{0x20})
            length -= 1;
        return length;
    }

    //! Returns the length of the name extension (i.e without trailing spaces).
    constexpr std::size_t get_name_extension_length() const
    {
        std::size_t length = 3;
        while (length > 0 && name_ext[length - 1] == std::byte{0x20})
            length -= 1;
        return length;
    }
};

static_assert(sizeof(dirent_t) == 32);

//! FAT on-disk directory entry structure as used for long file name entries.
struct lfn_dirent_t
{
    std::byte                               ordinal;    //!< Sequence number for this LFN component.
    std::array<unaligned<char16le_t>, 5>    name1;      //!< 5 UTF-16 code units of the name.
    std::byte                               attributes; //!< Must be AttributeLFN.
    std::byte                               type;       //!< Directory entry type; always zero.
    std::byte                               checksum;   //!< Checksum of the name of the corresponding short entry.
    std::array<unaligned<char16le_t>, 6>    name2;      //!< 6 UTF-16 code units of the name.
    uint16le_t                              cluster_low;//!< Must be zero.
    std::array<unaligned<char16le_t>, 2>    name3;      //!< 2 UTF-16 code units of the name.
};

static_assert(sizeof(lfn_dirent_t) == 32);


} // namespace System::Filesystem::FAT


#endif /* ifndef __SYSTEM_FS_FAT_DIRENT_H */
