#include <System/Utility/Logger/Logger.hh>

#include <System/Boot/Igniter/Tool/Blocklist.hh>
#include <System/Boot/Igniter/Tool/Bootsector.hh>
#include <System/Boot/Igniter/Tool/CHS.hh>
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

    auto size = std::filesystem::file_size(stage1_name);
    auto sectorSize = dest.sectorSize();
    auto count = ((size + sectorSize - 1) / sectorSize);
    std::uint64_t sector = 1440;
    for (std::size_t i = 0; i < count; ++i)
    {
        stage1.read(buffer, sizeof(buffer));
        dest.writeSector(sector, std::as_bytes(std::span{buffer}));

        auto chs = LBAToCHS({80, 2, 18}, sector);
        auto enc = EncodeCHS(chs);

        log(DefaultFacility, priority::debug, "Stage1 sector {} -> {},{},{} -> {:#x}", sector, chs.cylinder, chs.head, chs.sector, enc);

        blheader.blocks[i] = enc;
        blheader.blocks[i + 1] = 0;
        ++sector;
    }

    dest.writeSector(1439, std::as_bytes(std::span{&blheader, 1}));

    stage0.read(buffer, sizeof(buffer));

    Bootsector bootsector;
    bootsector.readFromDisk(source);
    bootsector.applyStage0(std::as_bytes(std::span{buffer}));
    bootsector.setBlockListBlock(1439);
    bootsector.writeToDisk(dest);

    return 0;
}
