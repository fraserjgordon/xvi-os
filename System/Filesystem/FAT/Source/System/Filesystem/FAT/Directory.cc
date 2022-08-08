#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/DirectoryImpl.hh>

#if !defined(__XVI_NO_STDLIB)
#  include <bit>
#  include <cstring>
#else
#  include <System/C++/Utility/Bit.hh>
#  include <System/C++/Utility/CString.hh>
#endif

#include <System/Filesystem/FAT/DirEnt.hh>
#include <System/Filesystem/FAT/Filesystem.hh>
#include <System/Filesystem/FAT/Object.hh>


namespace System::Filesystem::FAT
{


void Directory::enumerate(enumerate_callback_t callback)
{
    return DirectoryImpl::from(this).enumerate(callback);
}


void DirectoryImpl::enumerate(enumerate_callback_t callback)
{
    auto& fs = info().filesystem();
    auto size = info().size();

    // Accumulated long file name (LFN) parsing state.
    lfn_state_t lfn_state = {};

    // Iterate over the blocks that are used to store the directory entries.
    auto sector_size = fs.sectorSize();
    auto entries_per_sector = sector_size / sizeof(dirent_t);
    readBlocks(0, size, [&](std::uint32_t, std::span<const std::byte> data) -> bool
    {
        // Read each directory entry in turn.
        auto entries = std::span{reinterpret_cast<const dirent_t*>(data.data()), entries_per_sector};
        for (const auto& entry : entries)
        {
            // Check the first byte of the filename to get the directory entry type.
            switch (entry.name_base[0])
            {
                case DirectoryEntryEnd:
                    // There are no more directory entries. Enumeration is complete.
                    return false;

                case DirectoryEntryUnused:
                    // This entry isn't being used.
                    //
                    // If we have accumulated LFN state, it needs to be cleared - most likely, a file with a LFN was 
                    // deleted on a system that doesn't understand LFNs.
                    lfn_state = {};
                    continue;

                default:
                    // This directory entry is in use. We'll need to process it.
                    break;
            }

            // Are we dealing with an LFN entry?
            if (entry.isLFNEntry())
            {
                const auto& lfn_entry = reinterpret_cast<const lfn_dirent_t&>(entry);

                // Is this the "last" LFN entry?
                //
                // Contrary to the name, "last" LFN entries actually come first in the directory (they're in reverse
                // order).
                if (entry.isLastLFNEntry())
                {
                    // Reset the LFN state and begin again with this entry.
                    lfn_state = {};
                    lfn_state.applyLastLFNEntry(lfn_entry);
                }
                else
                {
                    // Attempt to apply this to the LFN state. If it fails, we have an invalid LFN sequence so we'll
                    // reset the LFN state.
                    if (!lfn_state.applyLFNEntry(lfn_entry))
                        lfn_state = {};
                }

                // Other than the name, LFN entries contain nothing useful. Move on to the next entry.
                continue;
            }

            // This is not an LFN entry. Check it against our LFN state to see if they match. If they do, we'll use the
            // LFN state for the name, otherwise we'll use the short name in this entry.
            if (!lfn_state.validateShortNameEntry(entry))
                lfn_state = {};

            // Create a wrapper around this directory entry and its LFN, if any.
            auto info = createObjectInfo(entry, lfn_state);

            // We now have everything we need to invoke the callback.
            auto result = callback(info.asObjectInfo());
            if (!result)
                return false;
        }

        // Continue reading blocks.
        return true;
    });
}


ObjectInfoImpl DirectoryImpl::createObjectInfo(const dirent_t& dirent, const lfn_state_t& lfn_state)
{
    // Are we taking the name from the LFN state or using the short name in the directory entry?
    std::string name;
    if (lfn_state.length > 0)
    {
        // We're using the long file name. We need to convert it from UTF-16 to UTF-8.
        //! @todo implement.
    }
    else
    {
        // We're using the short file name. We need to convert it from the filesystem's configured character set to
        // UTF-8.
        //! @todo implement.
    }

    // Create the info object.
    //return ObjectInfoImpl{info().filesystem(), std::move(name), dirent};
}


bool DirectoryImpl::lfn_state_t::applyLastLFNEntry(const lfn_dirent_t& entry)
{
    // Check that this is marked as the last LFN entry.
    constexpr auto LastOrdinalFlag = static_cast<std::uint8_t>(DirectoryEntryLastLFNBit);
    if (!(entry.ordinal & LastOrdinalFlag))
        return false;

    // Check that the ordinal number is valid.
    auto ord = (entry.ordinal & ~LastOrdinalFlag);
    if (ord > MaxLFNOrdinal || ord < MinLFNOrdinal)
        return false;

    // We don't understand any LFN entries that aren't type 0.
    if (entry.type != 0)
        return false;

    // Set up the LFN state.
    lfn = {};
    ordinal = ord - 1;
    checksum = entry.checksum;

    // Note that for the 21st LFN entry, the length calculation may overflow 8 bits.
    auto offset = CharsPerLFNEntry - 1;

    // Check for trailing padding in the name.
    auto chars = extractChars(entry);
    while (offset > 0 && chars[offset] == 0xFFFF)
        --offset;

    // A single trailing NUL is also permitted.
    if (offset > 0 && chars[offset] == u'\0')
        --offset;

    // Check that we've now reduced the name to something that fits.
    auto len = (ordinal - 1) * CharsPerLFNEntry + offset + 1;
    if (len > MaxLFNNameLength)
        return false;

    // Store the expected length of the filename.
    length = len;

    // And copy the characters to the name array.
    std::span from{chars.data(), offset + 1};
    std::span to{std::span{lfn}.subspan((ordinal - 1) * CharsPerLFNEntry)};
    std::memcpy(to.data(), from.data(), std::min(to.size_bytes(), from.size_bytes()));

    return true;
}


bool DirectoryImpl::lfn_state_t::applyLFNEntry(const lfn_dirent_t& entry)
{
    // Check that this is not marked as the last LFN entry.
    constexpr auto LastOrdinalFlag = static_cast<std::uint8_t>(DirectoryEntryLastLFNBit);
    if ((entry.ordinal & LastOrdinalFlag))
        return false;

    // Check that the ordinal number is the one we're expecting next.
    if (entry.ordinal != ordinal)
        return false;

    // We don't understand any LFN entries that aren't type 0.
    if (entry.type != 0)
        return false;

    // If the checksum differs from previous entries, something is invalid.
    if (entry.checksum != checksum)
        return false;

    // Copy the characters to the name string.
    auto chars = extractChars(entry);
    std::span from{chars};
    std::span to = std::span{lfn}.subspan((ordinal - 1) * CharsPerLFNEntry, CharsPerLFNEntry);
    std::memcpy(from.data(), to.data(), from.size_bytes());

    // Update the next expected ordinal.
    --ordinal;

    return true;
}


bool DirectoryImpl::lfn_state_t::validateShortNameEntry(const dirent_t& entry)
{
    // We should have reached ordinal zero before seeing this entry.
    if (ordinal != 0)
        return false;

    // Validate the checksum.
    std::span short_name{entry.name_base.data(), entry.name_base.size() + entry.name_ext.size()};
    std::uint8_t sum = 0;
    for (auto c : short_name)
        sum = std::rotr(sum, 1) + static_cast<std::uint8_t>(c);

    if (sum != checksum)
        return false;

    // The entry matched correctly. Take this opportunity to remove any tail padding from the long name.
    while (length > 0 && lfn[length - 1] == 0xFFFF)
        --length;

    // There may be a single NUL as padding too.
    if (length > 0 && lfn[length - 1] == 0x0000)
        --length;

    // Check that the padding removal didn't result in an empty name.
    if (length == 0 || (length == 1 && (lfn[0] == 0x0000 || lfn[0] == 0xFFFF)))
        return false;

    return true;
}


std::array<char16_t, 13> DirectoryImpl::lfn_state_t::extractChars(const lfn_dirent_t& entry)
{
    return
    {
        entry.name1[0], 
        entry.name1[1],
        entry.name1[2],
        entry.name1[3],
        entry.name1[4],
        entry.name2[0],
        entry.name2[1],
        entry.name2[2],
        entry.name2[3],
        entry.name2[4],
        entry.name2[5],
        entry.name3[0],
        entry.name3[1],
    };
}


} // namespace System::Filesystem::FAT
