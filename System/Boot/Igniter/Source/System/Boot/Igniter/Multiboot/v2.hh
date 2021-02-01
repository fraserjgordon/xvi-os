#ifndef __SYSTEM_BOOT_IGNITER_MULTIBOOT_V2_H
#define __SYSTEM_BOOT_IGNITER_MULTIBOOT_V2_H


#include <array>
#include <cstddef>
#include <cstdint>


namespace System::Boot::Igniter
{


enum class multiboot_v2_info_tag_type : std::uint32_t
{
    boot_command_line   = 1,
    boot_loader_name    = 2,
    module              = 3,
    basic_memory_info   = 4,
    bios_boot_device    = 5,
    memory_map          = 6,
    vbe_info            = 7,
    framebuffer_info    = 8,
    elf_symbols         = 9,
    apm_table           = 10,
    efi_32_system_table = 11,
    efi_64_system_table = 12,
    smbios_tables       = 13,
    acpi_rsdp_v1        = 14,
    acpi_rsdp_v2        = 15,
    network_info        = 16,
    efi_memory_map      = 17,
    efi_boot_services   = 18,
    efi_32_image_handle = 19,
    efi_64_image_handle = 20,
    image_load_address  = 21,
};

inline constexpr std::uint32_t MultibootV2LoaderMagic = 0x36d76289;

struct multiboot_v2_info
{
    std::uint32_t   total_size;
    std::uint32_t   reserved;
};

struct multiboot_v2_info_tag
{
    multiboot_v2_info_tag_type  type;
    std::uint32_t               size;

    struct boot_command_line;
    struct boot_loader_name;
    struct module;
    struct basic_memory_info;
    struct bios_boot_device;
    struct memory_map;
    struct vbe_info;
    struct framebuffer_info;
    struct elf_symbols;
    struct apm_table;
    struct efi_32_system_table;
    struct efi_64_system_table;
    struct smbios_tables;
    struct acpi_rsdp_v1;
    struct acpi_rsdp_v2;
    struct network_info;
    struct efi_memory_map;
    struct efi_boot_services;
    struct efi_32_image_handle;
    struct efi_64_image_handle;
    struct image_load_address;
};


struct multiboot_v2_os_header
{
    static constexpr std::uint32_t Magic = 0xe85250d6;
    
    enum class architecture : std::uint32_t
    {
        x86         = 0,
        mips32      = 4,
    };


    std::uint32_t   magic           = Magic;
    architecture    arch;
    std::uint32_t   header_length   = 0;
    std::uint32_t   checksum;


    constexpr void updateChecksum() noexcept
    {
        checksum = -(magic + static_cast<std::uint32_t>(arch) + header_length);
    }
};

struct multiboot_v2_os_tag
{
    std::uint16_t   type;
    std::uint16_t   flags;
    std::uint32_t   size;

    template <std::size_t N> struct info_request;
    struct exe_address_info;
    struct entry_address;
    struct efi_32_entry;
    struct efi_64_entry;
    struct console_flags;
    struct framebuffer_request;
    struct module_alignment;
    struct efi_boot_services;
    struct relocatable_header;
};

template <std::size_t N>
struct multiboot_v2_os_tag::info_request
{
    multiboot_v2_os_tag             header          = { 1, 0, sizeof(info_request) + (N * sizeof(std::uint32_t)) };
    std::array<std::uint32_t, N>    requested_tags;
};

struct multiboot_v2_os_tag::exe_address_info
{
    multiboot_v2_os_tag             header          = { 2, 0, sizeof(exe_address_info )};
    std::uint32_t                   os_header_addr;
    std::uint32_t                   load_addr;
    std::uint32_t                   load_end_addr;
    std::uint32_t                   bss_end_addr;
};

struct multiboot_v2_os_tag::entry_address
{
    multiboot_v2_os_tag             header          = { 3, 0, sizeof(entry_address) };
    std::uint32_t                   entry_addr;
};

struct multiboot_v2_os_tag::efi_32_entry
{
    multiboot_v2_os_tag             header          = { 8, 0, sizeof(efi_32_entry) };
    std::uint32_t                   entry_addr;
};

struct multiboot_v2_os_tag::efi_64_entry
{
    multiboot_v2_os_tag             header          = { 9, 0, sizeof(efi_64_entry) };
    std::uint32_t                   entry_addr;
};

struct multiboot_v2_os_tag::console_flags
{
    multiboot_v2_os_tag             header          = { 4, 0, sizeof(console_flags) };
    std::uint32_t                   console_flags;
};

struct multiboot_v2_os_tag::framebuffer_request
{
    multiboot_v2_os_tag             header          = { 5, 0, sizeof(framebuffer_request) };
    std::uint32_t                   width;
    std::uint32_t                   height;
    std::uint32_t                   depth;
};

struct multiboot_v2_os_tag::module_alignment
{
    multiboot_v2_os_tag             header          = { 6, 0, sizeof(module_alignment) };
};

struct multiboot_v2_os_tag::efi_boot_services
{
    multiboot_v2_os_tag             header          = { 7, 0, sizeof(efi_boot_services) };
};

struct multiboot_v2_os_tag::relocatable_header
{
    multiboot_v2_os_tag             header          = { 10, 0, sizeof(relocatable_header) };
    std::uint32_t                   min_address;
    std::uint32_t                   max_address;
    std::uint32_t                   alignment;
    std::uint32_t                   preference;
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_MULTIBOOT_V2_H */
