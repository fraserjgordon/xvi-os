#include <System/Utility/Logger/Logger.hh>

#include <System/Boot/Igniter/Tool/Bootsector.hh>
#include <System/Boot/Igniter/Tool/Disk.hh>


int main(int argc, char** argv)
{
    using namespace System::Boot::Igniter;

    System::Utility::Logger::enableLogToStderr();
    System::Utility::Logger::setMinimumPriority(System::Utility::Logger::priority::debug);

    Disk source("igniter-clean.img");
    Disk dest("igniter.img");

    char buffer[512];
    std::ifstream stage0("out/x86/x64/Stage0.bin");
    stage0.read(buffer, sizeof(buffer));

    Bootsector bootsector;
    bootsector.readFromDisk(source);
    bootsector.applyStage0(std::as_bytes(std::span{buffer}));
    bootsector.setBlockListBlock(42);
    bootsector.writeToDisk(dest);

    return 0;
}
