#ifndef __SYSTEM_BOOT_IGNITER_MULTIBOOT_V1_H
#define __SYSTEM_BOOT_IGNITER_MULTIBOOT_V1_H


#include <cstdint>


namespace System::Boot::Igniter
{


struct multiboot_v1_os_header
{
    static constexpr std::uint32_t Magic    = 0x1badb002;

    static constexpr std::uint32_t FlagLoadPageAligned  = 0x00000001;
    static constexpr std::uint32_t FlagRequireMemoryMap = 0x00000002;
    static constexpr std::uint32_t FlagRequireVideoMode = 0x00000004;

    static constexpr std::uint32_t FlagHaveLoadInfo     = 0x00010000;


    std::uint32_t       magic;
    std::uint32_t       flags;
    std::uint32_t       checksum;


    constexpr void updateChecksum() noexcept
    {
        checksum = -(magic + flags);
    }

    constexpr bool isValid() const noexcept
    {
        return magic == Magic && checksum == -(magic + flags);
    }

    static constexpr multiboot_v1_os_header create(std::uint32_t flags = 0)
    {
        return {.magic = Magic, .flags = flags, .checksum = -(Magic + flags)};
    }
};

struct multiboot_v1_os_header_extensions
{
    std::uint32_t       header_address      = 0;
    std::uint32_t       load_start          = 0;
    std::uint32_t       load_end            = 0;
    std::uint32_t       bss_end             = 0;
    std::uint32_t       entry               = 0;

    std::uint32_t       video_mode_type     = 0;
    std::uint32_t       video_width         = 0;
    std::uint32_t       video_height        = 0;
    std::uint32_t       video_depth         = 0;
};

struct multiboot_v1_os_header_extended : multiboot_v1_os_header
{
    multiboot_v1_os_header_extensions  ext = {};
};


// A specific register (%eax on x86) is set to this value to indicate that the software was loaded by a multiboot-
// compliant loader.
inline constexpr std::uint32_t MultibootV1LoaderMagic = 0x2badb002;

struct multiboot_v1_info
{
    std::uint32_t       flags = 0;
    
    struct simple_mem_info_t
    {
        std::uint32_t       kb_lower = 0;
        std::uint32_t       kb_upper = 0;
    } simple_mem_info;

    struct boot_device_t
    {
        std::uint32_t       device_id = 0;
    } boot_device;

    struct command_line_t
    {
        std::uint32_t       ptr = 0;
    } command_line;

    struct modules_t
    {
        std::uint32_t       count = 0;
        std::uint32_t       ptr = 0;
    } modules;

    union symbols_t
    {
        struct elf_t
        {
            std::uint32_t   entry_count;
            std::uint32_t   entry_size;
            std::uint32_t   table_ptr;
            std::uint32_t   string_table_ptr;
        } elf = {};

        struct aout_t
        {
            std::uint32_t   symbol_table_size;
            std::uint32_t   string_table_size;
            std::uint32_t   ptr;
        } aout;
    } symbols;

    struct memory_map_t
    {
        std::uint32_t       length = 0;
        std::uint32_t       ptr = 0;
    } memory_map;

    struct drive_info_t
    {
        std::uint32_t       length = 0;
        std::uint32_t       ptr = 0;
    } drive_info;

    struct bios_config_table_t
    {
        std::uint32_t       ptr = 0;
    } bios_config_table;

    struct bootloader_info_t
    {
        std::uint32_t       name_ptr = 0;
    } bootloader_info;

    struct apm_t
    {
        std::uint32_t       table_ptr = 0;
    } apm;

    struct vbe_t
    {
        std::uint32_t       control_info = 0;
        std::uint32_t       mode_info = 0;
        std::uint16_t       mode = 0;
        std::uint16_t       interface_segment = 0;
        std::uint16_t       interface_offset = 0;
        std::uint16_t       interface_length = 0;
    } vbe;

    struct framebuffer_t
    {
        std::uint64_t       address = 0;
        std::uint32_t       pitch = 0;
        std::uint32_t       width = 0;
        std::uint32_t       height = 0;
        std::uint8_t        bpp = 0;
        std::uint8_t        type = 0;
        std::uint8_t        colour_info[6] = {0, 0, 0, 0, 0, 0};
    } framebuffer;


    static constexpr std::uint32_t FlagSimpleMemInfo        = 0x00000001;
    static constexpr std::uint32_t FlagBootDevice           = 0x00000002;
    static constexpr std::uint32_t FlagCommandLine          = 0x00000004;
    static constexpr std::uint32_t FlagModules              = 0x00000008;
    static constexpr std::uint32_t FlagSymbolsAout          = 0x00000010;
    static constexpr std::uint32_t FlagSymbolsElf           = 0x00000020;
    static constexpr std::uint32_t FlagMemoryMap            = 0x00000040;
    static constexpr std::uint32_t FlagDriveInfo            = 0x00000080;
    static constexpr std::uint32_t FlagBiosConfigTable      = 0x00000100;
    static constexpr std::uint32_t FlagBootLoaderInfo       = 0x00000200;
    static constexpr std::uint32_t FlagAPM                  = 0x00000400;
    static constexpr std::uint32_t FlagVBE                  = 0x00000800;
    static constexpr std::uint32_t FlagFramebuffer          = 0x00001000;
};

struct multiboot_module_t
{
    std::uint32_t   start;
    std::uint32_t   end;
    std::uint32_t   string;
    std::uint32_t   reserved;
};

struct [[gnu::packed]] multiboot_mmap_entry_t
{
    std::uint32_t   size;
    std::uint64_t   address;
    std::uint64_t   length;
    std::uint32_t   type;
};

struct multiboot_apm_info_t
{
    std::uint16_t   version;
    std::uint16_t   code_segment;
    std::uint32_t   offset;
    std::uint16_t   code_segment_pm16;
    std::uint16_t   data_segment;
    std::uint16_t   flags;
    std::uint16_t   code_segment_length;
    std::uint16_t   code_segment_pm16_length;
    std::uint16_t   data_segment_length;
};


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_MULTIBOOT_V1_H */
