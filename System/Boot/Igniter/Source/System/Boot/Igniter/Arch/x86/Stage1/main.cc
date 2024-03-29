#include <System/C++/Utility/CString.hh>
#include <System/C++/Utility/Span.hh>

#include <System/Executable/Image/ELF/Types.hh>
#include <System/Filesystem/FAT/Directory.hh>
#include <System/Filesystem/FAT/Filesystem.hh>
#include <System/Filesystem/FAT/Object.hh>

#include <System/Boot/Igniter/Arch/x86/Stage1/A20.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/BlockCache.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/DiskIO.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Heap.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage1/MemoryLayout.hh>
#include <System/Boot/Igniter/Multiboot/v1.hh>


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

    // Enable the "A20" address line to make addresses >1MB work properly.
    enableA20();

    Disk boot(boot_drive);

    //! @todo fixme
    auto fs = Filesystem::FAT::Filesystem::open(nullptr);

    if (!fs->mount())
    {
        log(priority::emergency, "failed to mount boot drive filesystem");
        return;
    }

    log(priority::debug, "boot drive filesystem mounted successfully");

    auto root = fs->rootDirectory();

    log(priority::debug, "opened root directory");

    using namespace Filesystem::FAT;

    Directory::handle_t dirXVI = fs->openDirectory(root->find("XVI"));
    Directory::handle_t dirBoot = fs->openDirectory(dirXVI->find("Boot"));

    File::handle_t fileStage2 = fs->openFile(dirBoot->find("Bootloader.stage2.x86"));

    log(priority::debug, "opened Stage2 {}: {} bytes", fileStage2->info().name(), fileStage2->info().size());

    // Search for the multiboot header, if any, in the image.
    std::span multiboot_search_temp {reinterpret_cast<std::byte*>(0x100000), 8192};  // 8kB at 1M.
    auto multiboot_search_len = fileStage2->readTo(0, 8192, multiboot_search_temp);
    std::uint32_t entry = 0;
    for (std::size_t i = 0; i < multiboot_search_len; i += 4)
    {
        auto hdr = reinterpret_cast<const multiboot_v1_os_header_extended*>(multiboot_search_temp.data() + i);
        if (hdr->isValid())
        {
            log(priority::debug, "multiboot: found v1 header at offset {}; flags={:#x}", i, hdr->flags);

            if (hdr->flags & hdr->FlagLoadPageAligned)
            {
                log(priority::debug, "multiboot: requires module page alignment");
            }

            if (hdr->flags & hdr->FlagRequireMemoryMap)
            {
                log(priority::debug, "muliboot: requires memory map");
            }

            if (hdr->flags & hdr->FlagRequireVideoMode)
            {
                log(priority::debug, "multiboot: requires video mode table");
            }

            if (hdr->flags & hdr->FlagHaveLoadInfo)
            {
                log(priority::debug, "multiboot: load info: header={:#x} loadstart={:#x} loadend={:#x} bssend={:#x} entry={:#x}",
                    hdr->ext.header_address, hdr->ext.load_start, hdr->ext.load_end, hdr->ext.bss_end, hdr->ext.entry
                );

                // Perform the load as specified in the load info table.
                auto offset = hdr->ext.header_address - i;
                auto file_size = hdr->ext.load_end - hdr->ext.load_start;
                auto bss_size = hdr->ext.bss_end - hdr->ext.load_end;
                std::span dest {reinterpret_cast<std::byte*>(hdr->ext.load_start), file_size + bss_size};
                fileStage2->readTo(hdr->ext.load_start - offset, file_size, dest);
                std::memset(dest.data() + file_size, 0, bss_size);
                entry = hdr->ext.entry;
            }
        }
    }

    // Create a multiboot information structure to pass to the next stage.
    multiboot_v1_info multiboot_info;

    // If we don't have multiboot load info, load according to the ELF header.
    if (entry == 0)
    {
        using namespace Executable::Image::ELF;

        file_header_32le exehdr;
        auto read = fileStage2->readTo(0, sizeof(exehdr), std::as_writable_bytes(std::span{&exehdr, 1}));

        if (read < sizeof(exehdr))
            log(priority::error, "failed to read ELF header");

        log(priority::debug, "ehdr: entry={:#x} phdroff={} shdroff={} flags={:#x} ehdrsize={} phdrsize={} phdrcount={} shdrsize={} shdrcount={} shdrnamesec={}",
            exehdr.entry_point.value(),
            exehdr.program_header_offset.value(),
            exehdr.section_header_offset.value(),
            exehdr.flags.value(),
            exehdr.file_header_size.value(),
            exehdr.program_header_size.value(),
            exehdr.program_header_count.value(),
            exehdr.section_header_size.value(),
            exehdr.section_header_count.value(),
            exehdr.section_header_name_section.value()
        );

        for (int i = 0; i < exehdr.program_header_count; ++i)
        {
            program_header_32le phdr;
            auto offset = exehdr.program_header_offset + exehdr.program_header_size * i;
            fileStage2->readTo(offset, sizeof(phdr), std::as_writable_bytes(std::span{&phdr, 1}));

            log(priority::debug, "phdr: type={} offset={:#x} va={:#x} pa={:#x} fsize={:#x} msize={:#x} flags={:#x} align={:#x}",
                static_cast<std::uint32_t>(phdr.type.value()),
                phdr.offset.value(),
                phdr.virtual_address.value(),
                phdr.physical_address.value(),
                phdr.file_size.value(),
                phdr.memory_size.value(),
                phdr.flags.value(),
                phdr.alignment.value()
            );

            if (phdr.type == SegmentType::Load)
            {
                log(priority::debug, "loading segment from {:#x}+{:x} to {:#x}+{:x}",
                    phdr.offset.value(), phdr.file_size.value(),
                    phdr.physical_address.value(), phdr.memory_size.value()
                );

                std::span load {reinterpret_cast<std::byte*>(phdr.physical_address.value()), phdr.memory_size};
                fileStage2->readTo(phdr.offset, phdr.file_size, load);
                std::memset(load.data() + phdr.file_size, 0, phdr.memory_size - phdr.file_size);
            }
        }

        entry = exehdr.entry_point;
    }

    asm volatile
    (
        "jmp    *%2"
        :
        : "a" (MultibootV1LoaderMagic),
          "b" (&multiboot_info),
          "r" (entry)
        : "memory"
    );
}


} // namespace System::Boot::Igniter
