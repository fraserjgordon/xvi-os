#include <System/C++/Utility/Span.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/DiskIO.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Heap.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Logging.hh>


namespace System::Boot::Igniter
{


extern "C" void Stage1(std::uint8_t);

void Stage1(std::uint8_t boot_drive)
{
    // As the very first thing (even before logging anything), ensure that a heap is set up.
    createHeap(0x80000, 0x10000);

    // Set up logging and print the banner.
    configureLogging();
    log(priority::notice, "XVI Igniter stage 1 booting...");

    Disk boot(boot_drive);

    std::byte buffer[512];
    boot.read(0, 1, std::span{buffer, 512});
}


} // namespace System::Boot::Igniter
