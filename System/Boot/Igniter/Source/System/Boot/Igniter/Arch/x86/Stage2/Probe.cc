#include <System/Boot/Igniter/Arch/x86/Stage2/Probe.hh>

#include <algorithm>
#include <array>
#include <map>
#include <span>
#include <string_view>

#include <System/Firmware/ACPI/Table/RSDP.hh>
#include <System/Firmware/ACPI/Table/RSDT.hh>
#include <System/Firmware/Arch/x86/BIOS/DataArea.hh>
#include <System/HW/CPU/Arch/x86/IO/IO.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/RealMode.hh>

#include <System/Boot/Igniter/Memory/Bitmap.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/Logging.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/MMU.hh>
#include <System/Boot/Igniter/Arch/x86/Stage2/V86.hh>


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
    const multiboot_v1_info*    multiboot_v1 = nullptr;
    const multiboot_v2_info*    multiboot_v2 = nullptr;

    // This flag is set if we think there's an IBM PC-style BIOS present.
    bool                        has_bios = false;

    // Limits of memory discovered during early probe. These aren't comprehensive but are a good hint for what needs to
    // be mapped when setting up the initial page tables.
    std::uint32_t               low_memory = 0;     // Contiguous RAM from 0 up to a max of 1MB.
    std::uint32_t               mid_memory = 0;     // Contiguous RAM from 1MB up to a max of 4GB.
    std::uint32_t               high_memory = 0;    // Contiguous RAM from 16MB up to a max of 4GB.

    // Pointer to the extended BIOS data area (EBDA).
    X86::realmode_ptr<void>     ebda = {};

    // ACPI root pointer table.
    const Firmware::ACPI::rsdp* acpi_rsdp = nullptr;

    // ACPI tables.
    using acpi_table_map = std::map<std::array<char, 4>, const Firmware::ACPI::data_table_header*>;
    //acpi_table_map              acpi_tables;
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


static void reserve(std::uintptr_t start, std::uintptr_t size, std::string_view description)
{
    log(priority::debug, "    reserve: {:#010x}+{:>8x} ({})", start, size, description);
    g_earlyPageBitmap.markUnavailable(start, size);
}


static void reserveBIOSMemory()
{
    // Do nothing if there's no traditional BIOS present.
    if (!s_probeInfo.has_bios)
        return;

    // Mark the real-mode IVT and BIOS data area as unavailable.
    reserve(0x00000000, 0x500, "IVT and BIOS data area");

    // Mark the extended BIOS data area as unavailable.
    static constexpr std::uint32_t EBDAEnd = 0xA0000;
    auto ebda_segment = g_biosDataArea->ebda_segment;
    auto ebda_address = (ebda_segment << 4);
    auto ebda_size = EBDAEnd - ebda_address;
    if (ebda_address != 0 && ebda_size != 0)
    {
        reserve(ebda_address, ebda_size, "EBDA");
        s_probeInfo.ebda = X86::realmode_ptr<void> { ebda_segment, 0 };
    }
}

static void reserveMultibootV1Memory()
{
    // Mark the multiboot information table as being unavailable.
    auto info = s_probeInfo.multiboot_v1;
    auto info_address = reinterpret_cast<std::uintptr_t>(info);
    reserve(info_address, sizeof(multiboot_v1_info), "Multiboot info table");
    
    if (info->flags & multiboot_v1_info::FlagCommandLine)
    {
        std::string_view cmdline {reinterpret_cast<const char*>(info->command_line.ptr)};
        reserve(info->command_line.ptr, cmdline.length() + 1, "Multiboot command line");
    }

    if (info->flags & multiboot_v1_info::FlagModules)
    {
        // Reserve the module information list itself.
        auto length = info->modules.count * sizeof(multiboot_module_t);
        reserve(info->modules.ptr, length, "multiboot module list");

        // Then, reserve each module and it's associated command line.
        std::span modules {reinterpret_cast<const multiboot_module_t*>(info->modules.ptr), info->modules.count};
        for (const auto& module : modules)
        {
            reserve(module.start, module.end - module.start, "Multiboot module");

            std::string_view cmdline {reinterpret_cast<const char*>(module.string)};
            reserve(module.string, cmdline.length(), "Multiboot module string");
        }
    }

    //! @TODO: reserve space occupied by the debug symbols.

    if (info->flags & multiboot_v1_info::FlagMemoryMap)
        reserve(info->memory_map.ptr, info->memory_map.length, "Multiboot memory map");

    if (info->flags & multiboot_v1_info::FlagDriveInfo)
        reserve(info->drive_info.ptr, info->drive_info.length, "Multiboot drive map");

    // The BIOS configuration table is stored within BIOS-owned memory and doesn't need to be reserved here.

    if (info->flags & multiboot_v1_info::FlagBootLoaderInfo)
    {
        std::string_view name {reinterpret_cast<const char*>(info->bootloader_info.name_ptr)};
        reserve(info->bootloader_info.name_ptr, name.length(), "Multiboot bootloader info");
    }

    if (info->flags & multiboot_v1_info::FlagAPM)
        reserve(info->apm.table_ptr, sizeof(multiboot_apm_info_t), "Multiboot APM info");
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

        s_probeInfo.low_memory = lower * 1024;
        s_probeInfo.mid_memory = upper * 1024;

        log(priority::debug, "No memory map available; {}kB lower, {}kB upper reported", lower, upper);
    }
}

static void earlyMemoryProbeFallback()
{
    log(priority::debug, "Using fallback heuristics for early memory probe");

    // Read the CMOS memory to get a rough estimate of the memory size.
    constexpr std::uint16_t CMOSAddressPort = 0x0070;
    constexpr std::uint16_t CMOSDataPort = 0x0071;
    constexpr std::uint8_t CMOSRegisterMemLow = 0x30;
    constexpr std::uint8_t CMOSRegisterMemHigh = 0x31;
    X86::outb(CMOSAddressPort, CMOSRegisterMemLow);
    auto low = X86::inb(CMOSDataPort);
    X86::outb(CMOSAddressPort, CMOSRegisterMemHigh);
    auto high = X86::inb(CMOSDataPort);
    auto cmos_size = ((high << 8) | low);

    log(priority::debug, "CMOS reports {}kB memory present", cmos_size);
    g_earlyPageBitmap.markAvailable(0, cmos_size * 1024);

    // For safety, assume that there's an (E)ISA MMIO hole from 15-16MB.
    constexpr std::uint32_t MMIOHoleStart = 0x00F00000;
    constexpr std::uint32_t MMIOHoleSize  = 0x00100000;
    reserve(MMIOHoleStart, MMIOHoleSize, "possible MMIO region");

    if (cmos_size * 1024 > (16U<<20))
    {
        // As above, assume there may be a whole from 15-16MB.
        s_probeInfo.mid_memory = 14U<<20;
        s_probeInfo.high_memory = (cmos_size * 1024) - (16U<<20);
    }
    else
    {
        // The reported memory doesn't go as far as the hole so we can ignore it for now.
        s_probeInfo.mid_memory = (cmos_size * 1024) - (1U<<20);
    }
}


void performEarlyMemoryProbe()
{
    log(priority::trace, "Starting early memory probe");

    // For now, we always assume the presense of a BIOS.
    //! @TODO: disable when booted from EFI.
    s_probeInfo.has_bios = true;

    // We simply assume that there is memory from 0 up to the start of the VGA MMIO area.
    s_probeInfo.low_memory = 0xA0000;

    // Start by marking all of the early memory map as unavailable.
    g_earlyPageBitmap.markAllUnavailable();

    // If we have a multiboot information table and it has appropriate entries, use that to get memory information.
    constexpr auto MultibootMemoryFlags = multiboot_v1_info::FlagSimpleMemInfo | multiboot_v1_info::FlagMemoryMap;
    if (s_probeInfo.multiboot_v1 && (s_probeInfo.multiboot_v1->flags & MultibootMemoryFlags) != 0)
        earlyMemoryProbeMultibootV1();
    else
        earlyMemoryProbeFallback();

    // If a BIOS is present, reserve the memory that belongs to it.
    if (s_probeInfo.has_bios)
        reserveBIOSMemory();

    // Reserve any memory occupied by Multiboot information tables.
    if (s_probeInfo.multiboot_v1)
        reserveMultibootV1Memory();

    // Reserve the PC-compatible MMIO & ROM range.
    constexpr std::uint32_t PCMMIOStart = 0x000A0000;
    constexpr std::uint32_t PCMMIOSize  = 0x00060000;
    reserve(PCMMIOStart, PCMMIOSize, "PC-compatible MMIO");

    // Mark the in-memory image as reserved.
    auto image_start = reinterpret_cast<std::uint32_t>(&_IMAGE_LOAD_START);
    auto image_end = reinterpret_cast<std::uint32_t>(&_IMAGE_BSS_END);
    reserve(image_start, image_end - image_start, "this bootloader");

    auto real_mode = g_earlyPageBitmap.getAvailableSize(0x00100000) / 1024;
    auto total = g_earlyPageBitmap.getAvailableSize() / 1024;
    log(priority::debug, "After early memory probe, {}kB real-mode / {}kB total available for init", real_mode, total);

    log(priority::trace, "Finished early memory probe");
}


std::uint32_t allocateEarlyRealModePage()
{
    // Any page below the 1MB limit will do.
    return g_earlyPageBitmap.allocatePage(1024*1024);
}


std::uint32_t allocateEarlyPage()
{
    return g_earlyPageBitmap.allocatePage();
}


void freeEarlyPage(std::uint32_t page)
{
    g_earlyPageBitmap.freePage(page);
}


static bool memoryProbeE820()
{
    // Output format from the BIOS E820 memory map.
    struct entry_t
    {
        std::uint64_t base = 0;
        std::uint64_t length = 0;
        std::uint32_t type = 0;
        std::uint32_t flags = 0;
    };

    log(priority::debug, "attemping memory probe via BIOS \"E820\" interface...");

    // Allocate some real-mode memory for the output.
    auto page = allocateEarlyRealModePage();
    X86::realmode_ptr<entry_t> out{page >> 4, 0};

    bool success = false;
    std::uint32_t offset = 0;
    while (true)
    {
        // Set the output buffer to a known state as the BIOS may not write all of it (e.g. the flags field).
        auto ptr = out.linear_ptr();
        *ptr = {};

        // Set up the parameters for the call.
        bios_call_params params;
        params.eax = 0x0000E820;
        params.edx = 0x534D4150;
        params.ecx = sizeof(entry_t);
        params.ebx = offset;
        params.es = out.segment();
        params.di = out.offset();

        // Make the call.
        v86CallBIOS(0x15, params);

        // Was the call successful?
        if (params.eflags.bits.CF || params.eax != 0x534D4150)
            break;
        
        success = true;
        log(priority::debug, "E820: {:#018x}+{:016x} {:>8} {:#010x}",
            ptr->base, ptr->length, ptr->type, ptr->flags
        );

        // Are we done?
        if (params.ebx == 0)
            break;

        offset = params.ebx;
    }

    g_earlyPageBitmap.freePage(page);

    if (!success)
        log(priority::debug, "E820: probe failed");

    return success;
}


static const Firmware::ACPI::rsdp* findACPI()
{
    auto check_rsdp = [](std::uint32_t address)
    {
        auto ptr = reinterpret_cast<const Firmware::ACPI::rsdp*>(address);
        return (ptr->validSignature() && ptr->checksumCorrect() && (!ptr->hasExtendedFields() || ptr->extendedChecksumCorrect()))
            ? ptr
            : nullptr;
    };

    // First step: search the EBDA for the pointer table.
    const Firmware::ACPI::rsdp* rsdp = nullptr;
    if (auto address = s_probeInfo.ebda.linear(); address != 0)
    {
        // ACPI says the table can be at a 16-byte offset in the first 1kB of the EBDA.
        for (std::uint32_t offset = 0; offset < 1024; offset += 16)
        {
            // Check we've not overrun the EBDA.
            if (address + offset >= 0xA0000)
                break;

            // Is this a valid RSDP?
            if (auto candidate = check_rsdp(address + offset); candidate)
            {
                rsdp = candidate;
                break;
            }
        }
    }

    // Next, check the BIOS ROM space.
    if (!rsdp)
    {
        std::uint32_t address = 0xE0000;
        std::uint32_t end = 0x100000;

        // ACPI says the table can be at a 16-byte offset within the BIOS ROM space.
        for (; address < end; address += 16)
        {
            // Is this a valid RSDP?
            if (auto candidate = check_rsdp(address); candidate)
            {
                rsdp = candidate;
                break;
            }
        }
    }

    if (rsdp)
    {
        log(priority::verbose, "ACPI support detected");
        log(priority::debug, "ACPI: RSDP detected at {:010} (revision {}); RSDT={:#010x} XSDT={:#018x}",
            static_cast<const void*>(rsdp),
            rsdp->revision,
            rsdp->rsdt_address.value(),
            rsdp->hasExtendedFields() ? rsdp->xsdt_address.value() : 0
        );
    }
    else
    {
        log(priority::verbose, "no ACPI support detected");
    }

    return rsdp;
}


static void handleACPITable(std::uint64_t address)
{
    auto header = reinterpret_cast<const Firmware::ACPI::data_table_header*>(static_cast<std::uint32_t>(address));
    auto checksum_ok = header->checksumCorrect();

    log(priority::debug, "ACPI: table {} at {:#010x}+{:x}{}",
        std::string_view{&header->signature[0], header->signature.size()},
        address, header->length.value(),
        checksum_ok ? "" : " (checksum invalid}"
    );

    // Record the table so we can use it later.
    //s_probeInfo.acpi_tables.emplace(header->signature, header);
}


static void probeACPI()
{
    // Scan the ACPI tables. We use the XSDT if it exists or the RSDT otherwise.
    auto rsdp = s_probeInfo.acpi_rsdp;
    bool valid = false;
    if (rsdp->xsdt_address)
    {
        auto xsdt = reinterpret_cast<const Firmware::ACPI::xsdt*>(static_cast<std::uint32_t>(rsdp->xsdt_address));
        if (xsdt->header.checksumCorrect() && xsdt->signatureCorrect())
        {
            valid = true;
            for (auto entry : xsdt->entries())
                handleACPITable(entry);
        }
        else
        {
            log(priority::error, "ACPI: XSDT detected but invalid");
        }
    }

    if (!valid)
    {
        auto rsdt = reinterpret_cast<const Firmware::ACPI::rsdt*>(rsdp->rsdt_address.value());
        if (rsdt->header.checksumCorrect() && rsdt->signatureCorrect())
        {
            valid = true;
            for (auto entry : rsdt->entries())
                handleACPITable(entry);
        }
        else
        {
            log(priority::error, "ACPI: RSDT detected but invalid");
        }
    }
}


void mapEarlyMemory()
{
    // Map the areas we discovered during the early memory probe.
    // Note that the low area is mapped with user permissions so we can use it with V86 mode.
    addEarlyMap(0x00000000, s_probeInfo.low_memory, EarlyMapFlag::RWXUC);
    addEarlyMap(0x00100000, s_probeInfo.mid_memory, EarlyMapFlag::RWXC);
    addEarlyMap(0x01000000, s_probeInfo.high_memory, EarlyMapFlag::RWXC);

    // Map the VGA memory area.
    addEarlyMap(0x000A0000, 0x00020000, EarlyMapFlag::RW);

    // Map the BIOS ROM area. Like the low area, it needs to be usable in V86 mode.
    addEarlyMap(0x000C0000, 0x00040000, EarlyMapFlag::RXUC);
}


void hardwareProbe()
{
    memoryProbeE820();

    // Search for ACPI.
    s_probeInfo.acpi_rsdp = findACPI();
    if (s_probeInfo.acpi_rsdp)
    {
        probeACPI();
    }
}


} // namespace System::Boot::Igniter
