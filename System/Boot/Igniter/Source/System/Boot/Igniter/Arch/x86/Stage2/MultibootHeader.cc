#include <System/Boot/Igniter/Multiboot/v1.hh>
#include <System/Boot/Igniter/Multiboot/v2.hh>


#include <cstddef>


namespace System::Boot::Igniter
{


extern "C"
{

extern std::byte _IMAGE_LOAD_START[];
extern std::byte _IMAGE_LOAD_END[];
extern std::byte _IMAGE_BSS_END[];
extern std::byte _MULTIBOOT_V1_HDR[];
extern std::byte _MULTIBOOT_V2_HDR[];
extern std::byte _ENTRY_MULTIBOOT[];
extern std::byte _ENTRY_EFI32[];
extern std::byte _ENTRY_EFI64[];

extern void _IgniterStage2MultibootEntry();
extern void _IgniterStage2Efi32Entry();
extern void _IgniterStage2Efi64Entry();

}


[[gnu::section("multiboot.header.v1")]]
extern const multiboot_v1_os_header_extended g_multibootV1OsHeader =
{
    multiboot_v1_os_header::create(multiboot_v1_os_header::FlagHaveLoadInfo),

    {
        .header_address = reinterpret_cast<std::uint32_t>(&_MULTIBOOT_V1_HDR),
        .load_start = reinterpret_cast<std::uint32_t>(&_IMAGE_LOAD_START),
        .load_end = reinterpret_cast<std::uint32_t>(&_IMAGE_LOAD_END),
        .bss_end = reinterpret_cast<std::uint32_t>(&_IMAGE_BSS_END),
        .entry = reinterpret_cast<std::uint32_t>(&_ENTRY_MULTIBOOT),
    }
};


struct multiboot_v2_taglist
{
    multiboot_v2_os_header                  header;
    multiboot_v2_os_tag::exe_address_info   exe_info;
    multiboot_v2_os_tag::entry_address      multiboot_entry;
    multiboot_v2_os_tag::efi_32_entry       efi32_entry;
    multiboot_v2_os_tag::efi_64_entry       efi64_entry;
    multiboot_v2_os_tag::efi_boot_services  efi_boot_services;
    multiboot_v2_os_tag::relocatable_header relocatable;
};

[[gnu::section("multiboot.header.v2")]]
extern const multiboot_v2_taglist g_multibootV2OsHeader = []()
{
    multiboot_v2_taglist tl;

    tl.header.arch = multiboot_v2_os_header::architecture::x86;
    tl.header.header_length = sizeof(multiboot_v2_taglist);
    tl.header.updateChecksum();

    tl.exe_info.os_header_addr = reinterpret_cast<std::uint32_t>(&_MULTIBOOT_V2_HDR);
    tl.exe_info.load_addr = reinterpret_cast<std::uint32_t>(&_IMAGE_LOAD_START);
    tl.exe_info.load_end_addr = reinterpret_cast<std::uint32_t>(&_IMAGE_LOAD_END);
    tl.exe_info.bss_end_addr = reinterpret_cast<std::uint32_t>(&_IMAGE_BSS_END);

    tl.multiboot_entry.entry_addr = reinterpret_cast<std::uint32_t>(&_ENTRY_MULTIBOOT);
    tl.efi32_entry.entry_addr = reinterpret_cast<std::uint32_t>(&_ENTRY_EFI32);
    tl.efi64_entry.entry_addr = reinterpret_cast<std::uint32_t>(&_ENTRY_EFI64);

    tl.relocatable.min_address = 0;
    tl.relocatable.max_address = ~0U;
    tl.relocatable.alignment = 1;
    tl.relocatable.preference = 2;  // Highest possible address.

    return tl;
}();


} // namespace System::Boot::Igniter
