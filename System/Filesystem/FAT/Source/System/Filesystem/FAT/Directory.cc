#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/DirectoryImpl.hh>

#include <bit>
#include <cstring>

#include <System/Filesystem/FAT/DirEnt.hh>
#include <System/Filesystem/FAT/FilesystemImpl.hh>
#include <System/Filesystem/FAT/Logging.hh>
#include <System/Filesystem/FAT/ObjectImpl.hh>


namespace System::Filesystem::FAT
{


void Directory::enumerate(enumerate_callback_t callback)
{
    return DirectoryImpl::from(this).enumerate(callback);
}


ObjectInfo::handle_t Directory::find(std::string_view name)
{
    return ObjectInfo::handle_t{DirectoryImpl::from(this).find(name).release()->asObjectInfo()};
}


bool Directory::updateObjectMetadata(const ObjectInfo& info)
{
    return DirectoryImpl::from(this).updateObjectMetadata(ObjectInfoImpl::from(&info));
}


DirectoryImpl::DirectoryImpl(const ObjectInfoImpl& info) :
    ObjectImpl{info}
{
}


void DirectoryImpl::enumerate(enumerate_callback_t callback)
{
    enumerateInternal([&callback](ObjectInfoImpl& info) -> std::pair<bool, bool>
    {
        return {callback(*info.asObjectInfo()), false};
    });
}


void DirectoryImpl::enumerateInternal(enumerate_internal_callback_t callback)
{
    auto& fs = info().filesystem();

    // Accumulated long file name (LFN) parsing state.
    lfn_state_t lfn_state = {};

    // Iterate over the blocks that are used to store the directory entries.
    //
    // FAT directories don't have a size stored so we pass the maximum value as the limit.
    auto sector_size = fs.sectorSize();
    auto entries_per_sector = sector_size / sizeof(dirent_t);
    readBlocks(0, UINT32_MAX, [&](std::uint32_t offset, std::span<const std::byte> data) -> bool
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
            if (lfn_state.length > 0 && !lfn_state.validateShortNameEntry(entry))
                lfn_state = {};

            // Create a wrapper around this directory entry and its LFN, if any.
            auto info = createObjectInfo(entry, lfn_state);

            // We now have everything we need to invoke the callback.
            auto [more, update] = callback(info);
            
            // Write this entry back, if requested.
            if (update)
            {
                auto entry_offset = offset + (&entry - entries.data()) * sizeof(dirent_t);
                write(entry_offset, std::as_bytes(std::span{&info.dirent(), 1}));
            }

            // End the scan if the consumer is done.
            if (!more)
                return false;
        }

        // Continue reading blocks.
        return true;
    });
}


std::unique_ptr<ObjectInfoImpl> DirectoryImpl::find(std::string_view name)
{
    std::unique_ptr<ObjectInfoImpl> entry = nullptr;
    enumerate([name, &entry](const ObjectInfo& info)
    {
        if (auto i = ObjectInfoImpl::from(&info); i.name() == name)
        {
            entry = std::make_unique<ObjectInfoImpl>(i);
            return false;
        }

        return true;
    });

    return entry;
}


bool DirectoryImpl::updateObjectMetadata(const ObjectInfoImpl& updated_info)
{
    bool success = false;
    enumerateInternal([&updated_info, &success](ObjectInfoImpl& info) -> std::pair<bool, bool>
    {
        if (updated_info.name() == info.name())
        {
            // We've found the entry we want to update.
            info.setMetadata(updated_info);
            success = true;
            return {false, true};
        }

        // Keep searching.
        return {true, false};
    });

    return success;
}


std::unique_ptr<DirectoryImpl> DirectoryImpl::open(const ObjectInfoImpl& info)
{
    // Check that the named object describes a directory.
    if (info.type() != ObjectType::Directory && info.type() != ObjectType::RootDirectory)
        return nullptr;

    return std::unique_ptr<DirectoryImpl>{static_cast<DirectoryImpl*>(ObjectImpl::open(info).release())};
}


ObjectInfoImpl DirectoryImpl::createObjectInfo(const dirent_t& dirent, const lfn_state_t& lfn_state)
{
    // Are we taking the name from the LFN state or using the short name in the directory entry?
    std::string name;
    if (lfn_state.length > 0)
    {
        // We're using the long file name. We need to convert it from UTF-16 to UTF-8.
        std::u16string_view lfn{lfn_state.lfn.data(), lfn_state.length};
        name = info().filesystem().convertFromUTF16(lfn);
    }
    else
    {
        // We're using the short file name. We need to convert it from the filesystem's configured character set to
        // UTF-8. But first, we need to extract it and prune any trailing spaces.
        auto base_raw = dirent.getNameBase();
        auto ext_raw = dirent.getNameExtension();
        std::string_view base{reinterpret_cast<const char*>(base_raw.data()), dirent.getNameBaseLength()};
        std::string_view ext{reinterpret_cast<const char*>(ext_raw.data()), dirent.getNameExtensionLength()};

        name.reserve(base.size() + 1 + ext.size());
        name.append(base);

        // Don't append a trailing dot if there's no extension.
        if (ext.size() > 0)
            name.append(".").append(ext);

        name = info().filesystem().convertFromFilesystemCharset(name);
    }

    // Create the info object.
    return ObjectInfoImpl{info().filesystem(), std::move(name), dirent};
}


bool DirectoryImpl::lfn_state_t::applyLastLFNEntry(const lfn_dirent_t& entry)
{
    // Check that this is marked as the last LFN entry.
    constexpr auto LastOrdinalFlag = static_cast<std::uint8_t>(DirectoryEntryLastLFNBit);
    if (!(entry.ordinal & LastOrdinalFlag))
    {
        log(priority::debug, "bad LFN entry: missing \"last\" flag");
        return false;
    }

    // Check that the ordinal number is valid.
    auto ord = (entry.ordinal & ~LastOrdinalFlag);
    if (ord > MaxLFNOrdinal || ord < MinLFNOrdinal)
    {
        log(priority::debug, "bad LFN entry: ordinal {} invalid", ord);
        return false;
    }

    // We don't understand any LFN entries that aren't type 0.
    if (entry.type != 0)
    {
        log(priority::debug, "bad LFN entry: invalid entry type {}", entry.type);
        return false;
    }

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
    auto len = ordinal * CharsPerLFNEntry + offset + 1;
    if (len > MaxLFNNameLength)
    {
        log(priority::debug, "bad LFN entry: name length {} too long", len);
        return false;
    }

    // Store the expected length of the filename.
    length = len;

    // And copy the characters to the name array.
    std::span from{chars.data(), offset + 1};
    std::span to{std::span{lfn}.subspan(ordinal * CharsPerLFNEntry)};
    std::memcpy(to.data(), from.data(), std::min(to.size_bytes(), from.size_bytes()));

    return true;
}


bool DirectoryImpl::lfn_state_t::applyLFNEntry(const lfn_dirent_t& entry)
{
    // Check that this is not marked as the last LFN entry.
    constexpr auto LastOrdinalFlag = static_cast<std::uint8_t>(DirectoryEntryLastLFNBit);
    if ((entry.ordinal & LastOrdinalFlag))
    {
        log(priority::debug, "bad LFN entry: unexpected \"last\" flag");
        return false;
    }

    // Check that the ordinal number is the one we're expecting next.
    if (entry.ordinal != ordinal)
    {
        log(priority::debug, "bad LFN entry: bad ordinal {} (expected {})", entry.ordinal, ordinal);
        return false;
    }

    // We don't understand any LFN entries that aren't type 0.
    if (entry.type != 0)
    {
        log(priority::debug, "bad LFN entry: invalid entry type {}", entry.type);
        return false;
    }

    // If the checksum differs from previous entries, something is invalid.
    if (entry.checksum != checksum)
    {
        log(priority::debug, "bad LFN entry: checksum mismatch between LFN entries (got {}, expected {})", entry.checksum, checksum);
        return false;
    }

    // Copy the characters to the name string.
    auto chars = extractChars(entry);
    std::span from{chars};
    std::span to = std::span{lfn}.subspan((ordinal - 1) * CharsPerLFNEntry, CharsPerLFNEntry);
    std::memcpy(to.data(), from.data(), from.size_bytes());

    // Update the next expected ordinal.
    --ordinal;

    return true;
}


bool DirectoryImpl::lfn_state_t::validateShortNameEntry(const dirent_t& entry)
{
    // We should have reached ordinal zero before seeing this entry.
    if (ordinal != 0)
    {
        log(priority::debug, "bad LFN entry: missing entry #{}", ordinal);
        return false;
    }

    // Validate the checksum.
    std::span short_name{entry.name_base.data(), entry.name_base.size() + entry.name_ext.size()};
    std::uint8_t sum = 0;
    for (auto c : short_name)
        sum = std::rotr(sum, 1) + static_cast<std::uint8_t>(c);

    if (sum != checksum)
    {
        log(priority::debug, "bad LFN entry: checksum mismatch with short entry (got {}, expected {})", sum, checksum);
        return false;
    }

    // The entry matched correctly. Take this opportunity to remove any tail padding from the long name.
    while (length > 0 && lfn[length - 1] == 0xFFFF)
        --length;

    // There may be a single NUL as padding too.
    if (length > 0 && lfn[length - 1] == 0x0000)
        --length;

    // Check that the padding removal didn't result in an empty name.
    if (length == 0 || (length == 1 && (lfn[0] == 0x0000 || lfn[0] == 0xFFFF)))
    {
        log(priority::debug, "bad LFN entry: empty name");
        return false;
    }

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
