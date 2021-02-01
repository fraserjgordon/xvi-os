#pragma once
#ifndef __SYSTEM_FIRMWARE_ARCH_X86_BIOS_DATAAREA_H
#define __SYSTEM_FIRMWARE_ARCH_X86_BIOS_DATAAREA_H


#include <cstddef>
#include <cstdint>


namespace System::Firmware::X86::BIOS
{


struct [[gnu::packed]] data_area_t
{
    std::uint16_t   serial_port_address[4];         // I/O port bases for up to 4 serial ports.
    std::uint16_t   parallel_port_address[3];       // I/O port bases for up to 3 parallel ports.
    std::uint16_t   ebda_segment;                   // Extended BIOS data area segment address.
    std::byte       _reserved_10[3];
    std::uint16_t   memory_kb;                      // kB of contiguous memory starting at address zero.
    std::byte       _reserved_15[0x52];
    std::uint16_t   reset_segment;                  // %cs for warm reset.
    std::uint16_t   reset_offset;                   // %ip for warm reset.
    std::byte       _reserved_6b[7];
    std::uint16_t   reset_flag;                     // Reset type.
    std::byte       _reserved_74[0x8C];
};

static_assert(sizeof(data_area_t) == 256);


} // namespace System::Firmware::X86::BIOS


#endif /* ifndef __SYSTEM_FIRMWARE_ARCH_X86_BIOS_DATAAREA_H */
