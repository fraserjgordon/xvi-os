#include <System/C++/Utility/Span.hh>

#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/Filesystem.hh>
#include <System/Filesystem/FAT/Object.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/BlockCache.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/DiskIO.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Heap.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/MemoryLayout.hh>


namespace System::Boot::Igniter
{


extern "C" void Stage1(std::uint8_t);

void Stage1(std::uint8_t boot_drive)
{
    // As the very first thing (even before logging anything), ensure that a heap is set up.
    createHeap(HEAP_START, HEAP_SIZE);

    // Set up logging and print the banner.
    configureLogging();
    log(priority::notice, "XVI Igniter stage 1 booting...");

    Disk boot(boot_drive);

    Filesystem::FAT::Filesystem fs
    {{
        .sectors = 2880,
        .sectorSize = 512,
        .read = [&](std::uint64_t lba) -> std::shared_ptr<const std::byte>
        {
            auto [found, data] = BlockCache::instance().findOrAllocate(lba);
            if (found) [[likely]]
                return std::static_pointer_cast<const std::byte>(data);

            // Data not in the cache; read it now.
            boot.read(lba, 1, std::span{data.get(), 512});

            return std::static_pointer_cast<const std::byte>(data);
        }
    }};

    if (!fs.mount())
    {
        log(priority::emergency, "failed to mount boot drive filesystem");
        return;
    }

    log(priority::debug, "boot drive filesystem mounted successfully");

    /*auto* root = fs.rootDirectory();

    root->enumerate([&](const Filesystem::FAT::ObjectInfo& info)
    {
        return true;
    });*/
}


} // namespace System::Boot::Igniter
