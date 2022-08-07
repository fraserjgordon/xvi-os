#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/DirectoryImpl.hh>

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


} // namespace System::Filesystem::FAT
