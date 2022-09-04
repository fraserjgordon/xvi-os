#ifndef __SYSTEM_BOOT_BSX_BOOTABLE_H
#define __SYSTEM_BOOT_BSX_BOOTABLE_H


#include <array>
#include <cstddef>
#include <cstdint>


namespace System::Boot::BSX
{


struct bootable_info_ptr
{
    static constexpr std::array<char, 8> Signature = {'X', 'V', 'I', ' ', 'b', 'o', 'o', 't'};
    static constexpr std::size_t OpcodesLength = 128;

    std::byte           opcodes[OpcodesLength];     // Machine instructions; ignored.
    std::array<char, 8> signature;                  // Signature indentifying this structure.
    std::uint64         info_vaddr;                 // Virtual address of the bootable_info structure.
};


struct bootable_mapping
{
    std::uint64_t       vaddr;                      // Virtual address 
};


struct bootable_info
{
    // Protocol information.
    std::uint8_t        protocol_major;             // Major version of the BSX protocol implemented.
    std::uint8_t        protocol_minor;             // Minor version of the BSX protocol implemented.
    std::uint8_t        protocol_min_major;         // Minimum required BSX protocol major version.
    std::uint8_t        protocol_min_minor;         // Minimum required BSX protocol minor version.
    std::uint32_t       info_length;                // Length of this structure.

    // Boot memory requirements.
    std::uint64_t       min_free_memory;            // Minimum quantity of free memory required to boot.
};


} // namespace BSX


#endif /* ifndef __SYSTEM_BOOT_BSX_BOOTABLE_H */
