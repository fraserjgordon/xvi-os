#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>

#include <algorithm>
#include <span>
#include <string_view>

#include <System/Firmware/Arch/x86/BIOS/DataArea.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/RealMode.hh>

#include <System/Boot/Igniter/Memory/Bitmap.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>


namespace System::Boot::Igniter
{


namespace BIOS = System::Firmware::X86::BIOS;
namespace X86 = System::HW::CPU::X86;


// Symbols defining the limits of the in-memory image.
extern const std::byte _IMAGE_LOAD_START asm("_IMAGE_LOAD_START");
extern const std::byte _IMAGE_BSS_END asm("_IMAGE_BSS_END");


// Storage for a bitmap of pages discovered during first-pass memory probing. To keep the size of the bootloader image
// down, only a small (but hopefully sufficient) block of the address space is covered.
constexpr std::size_t MaxEarlyMemorySize = 128 * (1 << 20);
constexpr std::size_t MaxEarlyMemoryPages = MaxEarlyMemorySize / PageBitmap::PageSize;

static std::bitset<MaxEarlyMemoryPages> g_earlyMemoryPageBits;
static PageBitmap g_earlyPageBitmap = {0, g_earlyMemoryPageBits};


// The BIOS data area starts immediately after the real-mode interrupt vector table.
static constexpr X86::realmode_ptr<BIOS::data_area_t> g_biosDataArea {0x0040, 0x0000};


struct probe_info
{
    // Pointers to Multiboot information tables, if loaded by a Multiboot-compatible loader.
    const multiboot_v1_info*    multiboot_v1;
    const multiboot_v2_info*    multiboot_v2;

    // This flag is set if we think there's an IBM PC-style BIOS present.
    bool                        has_bios;
};


static probe_info s_probeInfo;


void setMultibootTablePointer(const multiboot_v1_info* info)
{
    s_probeInfo.multiboot_v1 = info;
}

void setMultibootTablePointer(const multiboot_v2_info* info)
{
    s_probeInfo.multiboot_v2 = info;
}


static void reserveBIOSMemory()
{
    // Mark the real-mode IVT and BIOS data area as unavailable.
    log(priority::debug, "    reserve: {:#010x}+{:x} (IVT and BIOS data area)", 0, 0x0500);
    g_earlyPageBitmap.markUnavailable(0x00000000, 0x0500);
}

static void reserveMultibootV1Memory()
{
    // Mark the multiboot information table as being unavailable.
    auto info = s_probeInfo.multiboot_v1;
    auto info_address = reinterpret_cast<std::uintptr_t>(info);
    log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot info table)", info_address, sizeof(multiboot_v1_info));
    g_earlyPageBitmap.markUnavailable(info_address, sizeof(multiboot_v1_info));
    
    if (info->flags & multiboot_v1_info::FlagCommandLine)
    {
        std::string_view cmdline {reinterpret_cast<const char*>(info->command_line.ptr)};
        log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot command line)", info->command_line.ptr, cmdline.length() + 1);
        g_earlyPageBitmap.markUnavailable(info->command_line.ptr, cmdline.length() + 1);
    }

    if (info->flags & multiboot_v1_info::FlagModules)
    {
        // Reserve the module information list itself.
        auto length = info->modules.count * sizeof(multiboot_module_t);
        log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot module list)", info->modules.ptr, length);
        g_earlyPageBitmap.markUnavailable(info->modules.ptr, length);

        // Then, reserve each module and it's associated command line.
        std::span modules {reinterpret_cast<const multiboot_module_t*>(info->modules.ptr), info->modules.count};
        for (const auto& module : modules)
        {
            log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot module)", module.start, module.end - module.start);
            g_earlyPageBitmap.markUnavailable(module.start, module.end - module.start);

            std::string_view cmdline {reinterpret_cast<const char*>(module.string)};
            log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot module string)", module.string, cmdline.length());
            g_earlyPageBitmap.markUnavailable(module.string, cmdline.length());
        }
    }

    //! @TODO: reserve space occupied by the debug symbols.

    if (info->flags & multiboot_v1_info::FlagMemoryMap)
    {
        log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot memory map)", info->memory_map.ptr, info->memory_map.length);
        g_earlyPageBitmap.markUnavailable(info->memory_map.ptr, info->memory_map.length);
    }

    if (info->flags & multiboot_v1_info::FlagDriveInfo)
    {
        log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot drive map)", info->drive_info.ptr, info->drive_info.length);
        g_earlyPageBitmap.markUnavailable(info->drive_info.ptr, info->drive_info.length);
    }

    // The BIOS configuration table is stored within BIOS-owned memory and doesn't need to be reserved here.

    if (info->flags & multiboot_v1_info::FlagBootLoaderInfo)
    {
        std::string_view name {reinterpret_cast<const char*>(info->bootloader_info.name_ptr)};
        log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot bootloader info)", info->bootloader_info.name_ptr, name.length());
        g_earlyPageBitmap.markUnavailable(info->bootloader_info.name_ptr, name.length());
    }

    if (info->flags & multiboot_v1_info::FlagAPM)
    {
        log(priority::debug, "    reserve: {:#010x}+{:x} (Multiboot APM info)", info->apm.table_ptr, sizeof(multiboot_apm_info_t));
        g_earlyPageBitmap.markUnavailable(info->apm.table_ptr, sizeof(multiboot_apm_info_t));
    }
}

static void earlyMemoryProbeMultibootV1()
{
    log(priority::debug, "Using Multiboot v1 information for early memory probe");

    // Do we have a proper memory map from the loader?
    auto info = s_probeInfo.multiboot_v1;
    if (info->flags & multiboot_v1_info::FlagMemoryMap)
    {
        // Span covering the entry list.
        std::span map_span {reinterpret_cast<const std::byte*>(info->memory_map.ptr), info->memory_map.length};

        // Process each entry.
        log(priority::debug, "Multiboot memory map:");
        while (map_span.size() >= sizeof(multiboot_mmap_entry_t))
        {
            auto p = reinterpret_cast<const multiboot_mmap_entry_t*>(map_span.data());

            log(priority::debug, "    size={} address={:#018x} length={:#018x} type={}", p->size, p->address, p->length, p->type);

            // Only type 1 memory is usable; all other types are reserved.
            if (p->type == 1)
            {
                // If the memory is above 4GB, it should be ignored for now.
                if (p->address <= std::numeric_limits<std::uint32_t>::max())
                {
                    // Ensure the length doesn't overflow, either.
                    auto end = p->address + p->length;
                    end = std::clamp<std::uint64_t>(end, 0, std::numeric_limits<std::uint32_t>::max());

                    auto start = static_cast<std::uint32_t>(p->address);
                    auto length = static_cast<std::uint32_t>(end) - start;
                    g_earlyPageBitmap.markAvailable(start, length);
                }
            }

            // Move on to the next entry. The stored size is 4 less than the actual size.
            map_span = map_span.subspan(p->size + 4);
        }
    }
    else
    {
        // Only simple memory information is available. It covers two areas: starting from address zero and starting
        // from 1MB.
        auto lower = info->simple_mem_info.kb_lower;
        auto upper = info->simple_mem_info.kb_upper;
        g_earlyPageBitmap.markAvailable(0x00000000, lower * 1024);
        g_earlyPageBitmap.markAvailable(0x00100000, upper * 1024);

        log(priority::debug, "No memory map available; {}kB lower, {}kB upper reported", lower, upper);
    }
}

static void earlyMemoryProbeFallback()
{
    log(priority::debug, "Using fallback heuristics for early memory probe");
    throw std::logic_error("earlyMemoryProbeFallback(): not implemented");
}


void performEarlyMemoryProbe()
{
    log(priority::trace, "Starting early memory probe");

    // For now, we always assume the presense of a BIOS.
    //! @TODO: disable when booted from EFI.
    s_probeInfo.has_bios = true;

    // Start by marking all of the early memory map as unavailable.
    g_earlyPageBitmap.markAllUnavailable();

    // If we have a multiboot information table and it has appropriate entries, use that to get memory information.
    constexpr auto MultibootMemoryFlags = multiboot_v1_info::FlagSimpleMemInfo | multiboot_v1_info::FlagMemoryMap;
    if (s_probeInfo.multiboot_v1 && (s_probeInfo.multiboot_v1->flags & MultibootMemoryFlags))
        earlyMemoryProbeMultibootV1();
    else
        earlyMemoryProbeFallback();

    // If a BIOS is present, reserve the memory that belongs to it.
    if (s_probeInfo.has_bios)
        reserveBIOSMemory();

    // Reserve any memory occupied by Multiboot information tables.
    if (s_probeInfo.multiboot_v1)
        reserveMultibootV1Memory();

    // Mark the in-memory image as reserved.
    auto image_start = reinterpret_cast<std::uint32_t>(&_IMAGE_LOAD_START);
    auto image_end = reinterpret_cast<std::uint32_t>(&_IMAGE_BSS_END);
    log(priority::debug, "    reserve: {:#010x}+{:x} (this bootloader)", image_start, image_end - image_start);
    g_earlyPageBitmap.markUnavailable(image_start, image_end - image_start);

    auto real_mode = g_earlyPageBitmap.getAvailableSize(0x00100000) / 1024;
    auto total = g_earlyPageBitmap.getAvailableSize() / 1024;
    log(priority::debug, "After early memory probe, {}kB real-mode / {}kB total available for init", real_mode, total);

    log(priority::trace, "Finished early memory probe");
}


} // namespace System::Boot::Igniter
