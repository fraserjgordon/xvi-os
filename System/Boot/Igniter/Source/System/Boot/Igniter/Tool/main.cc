#include <System/Utility/Logger/Logger.hh>

#include <vector>

#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/File.hh>
#include <System/Filesystem/FAT/Filesystem.hh>

#include <System/Boot/Igniter/CHS.hh>
#include <System/Boot/Igniter/Tool/Blocklist.hh>
#include <System/Boot/Igniter/Tool/Bootsector.hh>
#include <System/Boot/Igniter/Tool/Disk.hh>


int main(int argc, char** argv)
{
    using namespace System::Boot::Igniter;

    using namespace System::Utility::Logger;

    enableLogToStderr();
    setMinimumPriority(priority::trace);

    //Disk source("igniter-clean.img");
    Disk dest("igniter.img");

    std::filesystem::path stage0_name = "out/x86/x86/Stage0.bin";
    std::filesystem::path stage1_name = "out/x86/x86/Stage1.bin";
    std::filesystem::path stage2_name = "out/x86/x86/Stage2.x86.bin";

    std::ifstream stage0(stage0_name.string());

    auto fs = System::Filesystem::FAT::Filesystem::open(
    {
        .sectors = 2880,
        .sectorSize = 512,

        .read = [&](std::uint64_t lba)
        {
            std::shared_ptr<std::byte> ptr(new std::byte[512]);
            dest.readSector(lba, std::span(ptr.get(), 512));
            return ptr;
        },

        .write = [&](std::uint64_t lba, std::size_t count, std::span<const std::byte> buffer)
        {
            if (count != 1)
                return false;

            dest.writeSector(lba, buffer);
            return true;
        }
    });

    log(DefaultFacility, priority::trace, "mounting disk...");
    fs->mount();

    log(DefaultFacility, priority::trace, "opening /XVI/...");
    auto dirXVI = fs->openDirectory(fs->rootDirectory()->find("XVI"));
    log(DefaultFacility, priority::trace, "opening /XVI/Boot/...");
    auto dirBoot = fs->openDirectory(dirXVI->find("Boot"));

    auto fileStage1 = fs->openFile(dirBoot->find("Bootloader.stage1"));
    auto fileStage1Blocklist = fs->openFile(dirBoot->find("Bootloader.stage1.blocklist"));
    auto fileStage2 = fs->openFile(dirBoot->find("Bootloader.stage2.x86"));

    fileStage1Blocklist->truncate();

    auto copy = [&dest](const std::filesystem::path& from, const System::Filesystem::FAT::File::handle_t& to)
    {
        char buffer[512];

        std::ifstream source(from.string());

        to->truncate();

        auto sectorSize = dest.sectorSize();
        auto size = std::filesystem::file_size(from);
        auto count = ((size + sectorSize - 1) / sectorSize);
        auto offset = 0;
        while (offset < size)
        {
            source.read(buffer, sizeof(buffer));
            auto len = source.gcount();
            to->append(std::as_bytes(std::span{buffer, len}));

            offset += len;
        }
    };

    blocklist_header blheader;
    blheader.load_offset = 0x0000;
    blheader.load_segment = 0x1000;
    blheader.next_blocklist = 0;
    blheader.blocks = {0};

    blocklist_block blblock;
    blblock.next_blocklist = 0;

    chs_geometry geometry = {80, 2, 18};

    auto sectorsAdded = 0;
    auto addToBlocklist = [&](std::uint64_t sector)
    {
        auto chs = LBAToCHS(geometry, sector);
        auto enc = EncodeCHS(chs);

        log(DefaultFacility, priority::debug, "Stage1 sector #{} {} -> {},{},{} -> {:#x}", sectorsAdded, sector, chs.cylinder, chs.head, chs.sector, enc);

        // Are we adding this to the first blocklist sector or a subsequent one?
        if (sectorsAdded < blheader.blocks.size())
        {
            // Simple - just add then append a terminator.
            blheader.blocks[sectorsAdded] = enc;
            ++sectorsAdded;
            if (sectorsAdded < blheader.blocks.size())
                blheader.blocks[sectorsAdded] = 0;
        }
        else
        {
            // Is this the first additional sector?
            auto x = sectorsAdded - blheader.blocks.size();
            auto blsectors = x / blblock.blocks.size();
            auto bloffset = x % blblock.blocks.size();

            if (bloffset == 0)
            {
                // We're starting a new blocklist block.
                if (blsectors == 0)
                {
                    // This is the first additional blocklist block. Write the header.
                    fileStage1Blocklist->append(std::as_bytes(std::span{&blheader, 1}));
                }
                else
                {
                    // We are moving from one blocklist block to another. Write the completed one.
                    fileStage1Blocklist->append(std::as_bytes(std::span{&blblock, 1}));
                }

                std::memset(&blblock, 0, sizeof(blblock));
            }

            // Add the sector and the terminator.
            blblock.blocks[bloffset] = enc;
            if (bloffset < (blblock.blocks.size() - 1))
                blblock.blocks[bloffset + 1] = 0;

            ++sectorsAdded;
        }
    };

    copy(stage1_name, fileStage1);
    dirBoot->updateObjectMetadata(fileStage1->info());

    copy(stage2_name, fileStage2);
    dirBoot->updateObjectMetadata(fileStage2->info());

    auto sector_count = fileStage1->info().sectorCount();
    for (std::uint32_t i = 0; i < sector_count; ++i)
    {
        auto sector = fileStage1->info().nthSector(i);
        addToBlocklist(sector);
    }

    if (sectorsAdded <= blheader.blocks.size())
        fileStage1Blocklist->append(std::as_bytes(std::span{&blheader, 1}));
    else
        fileStage1Blocklist->append(std::as_bytes(std::span{&blblock, 1}));

    dirBoot->updateObjectMetadata(fileStage1Blocklist->info());

    // Go back through the blocklist now that it has been written and update the 'next' pointers.
    sector_count = fileStage1Blocklist->info().sectorCount();
    std::uint32_t current_sector = fileStage1Blocklist->info().startSector();
    log(DefaultFacility, priority::debug, "first blocklist block: {}", current_sector);
    for (std::uint32_t i = 0; i < sector_count; ++i)
    {
        auto next_sector = (i < (sector_count - 1)) ? fileStage1Blocklist->info().nthSector(i + 1) : 0U;
        auto encoded = (i < (sector_count - 1)) ? EncodeCHS(LBAToCHS(geometry, next_sector)) : 0U;

        log(DefaultFacility, priority::debug, "blocklist block {} -> {}", current_sector, next_sector);

        if (i == 0)
        {
            dest.readSector(current_sector, std::as_writable_bytes(std::span{&blheader, 1}));
            blheader.next_blocklist = encoded;
            dest.writeSector(current_sector, std::as_bytes(std::span{&blheader, 1}));
        }
        else
        {
            dest.readSector(current_sector, std::as_writable_bytes(std::span{&blblock, 1}));
            blblock.next_blocklist = encoded;
            dest.writeSector(current_sector, std::as_bytes(std::span{&blblock, 1}));
        }

        current_sector = next_sector;
    }

    char buffer[512];
    stage0.read(buffer, sizeof(buffer));

    Bootsector bootsector;
    bootsector.readFromDisk(dest);
    bootsector.applyStage0(std::as_bytes(std::span{buffer}));
    bootsector.setBlockListBlock(fileStage1Blocklist->info().startSector());
    bootsector.writeToDisk(dest);

    return 0;
}
