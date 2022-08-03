#include <System/Utility/Logger/Logger.hh>

#include <System/Boot/Igniter/CHS.hh>
#include <System/Boot/Igniter/Tool/Blocklist.hh>
#include <System/Boot/Igniter/Tool/Bootsector.hh>
#include <System/Boot/Igniter/Tool/Disk.hh>


int main(int argc, char** argv)
{
    using namespace System::Boot::Igniter;

    using namespace System::Utility::Logger;

    enableLogToStderr();
    setMinimumPriority(priority::debug);

    Disk source("igniter-clean.img");
    Disk dest("igniter.img");

    std::filesystem::path stage1_name = "out/x86/x86/Stage1.bin";

    std::ifstream stage0("out/x86/x86/Stage0.bin");
    std::ifstream stage1(stage1_name.string());

    char buffer[512];

    blocklist_header blheader;
    blheader.load_offset = 0x0000;
    blheader.load_segment = 0x1000;
    blheader.next_blocklist = 0;
    blheader.blocks = {0};

    blocklist_block blblock;
    blblock.next_blocklist = 0;

    chs_geometry geometry = {80, 2, 18};

    std::uint64_t blHeaderSector = 1439;
    std::uint64_t nextBLSector = blHeaderSector;

    auto sectorSize = dest.sectorSize();
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
                auto prevBLSector = nextBLSector;
                --nextBLSector;

                log(DefaultFacility, priority::debug, "Allocated new blocklist block at sector {}", nextBLSector);

                // We're starting a new blocklist block.
                if (blsectors == 0)
                {
                    // This is the first additional blocklist block. Update the blocklist header block to point to it.
                    blheader.next_blocklist = EncodeCHS(LBAToCHS(geometry, nextBLSector));

                    log(DefaultFacility, priority::debug, "Previous blocklist block is header; will write it later");
                }
                else
                {
                    // We are moving from one blocklist block to another. Write the completed one.
                    blblock.next_blocklist = EncodeCHS(LBAToCHS(geometry, nextBLSector));
                    dest.writeSector(prevBLSector, std::as_bytes(std::span{&blblock, 1}));

                    log(DefaultFacility, priority::debug, "Previous blocklist block at sector {} written out", prevBLSector);
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

    auto size = std::filesystem::file_size(stage1_name);
    auto count = ((size + sectorSize - 1) / sectorSize);
    std::uint64_t sector = 1440;
    for (std::size_t i = 0; i < count; ++i)
    {
        stage1.read(buffer, sizeof(buffer));
        dest.writeSector(sector, std::as_bytes(std::span{buffer}));

        addToBlocklist(sector);

        ++sector;
    }

    log(DefaultFacility, priority::debug, "{} data sectors written", count);

    dest.writeSector(blHeaderSector, std::as_bytes(std::span{&blheader, 1}));

    if (sectorsAdded > blheader.blocks.size())
    {
        dest.writeSector(nextBLSector, std::as_bytes(std::span{&blblock, 1}));

        log(DefaultFacility, priority::debug, "Final blocklist block at sector {} written out", nextBLSector);
    }

    log(DefaultFacility, priority::debug, "{} blocklist sectors written", blHeaderSector - nextBLSector + 1);

    stage0.read(buffer, sizeof(buffer));

    Bootsector bootsector;
    bootsector.readFromDisk(source);
    bootsector.applyStage0(std::as_bytes(std::span{buffer}));
    bootsector.setBlockListBlock(1439);
    bootsector.writeToDisk(dest);

    return 0;
}
