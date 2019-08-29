#pragma once
#ifndef __SYSTEM_FIRMWARE_EFI_TABLES_H
#define __SYSTEM_FIRMWARE_EFI_TABLES_H


#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::Firmware::EFI
{


enum class table_signature : std::uint64_t
{
    BootServices    = 0x56524553544f4f42,
    RuntimeServices = 0x56524553544e5552,
    System          = 0x5453595320494249,
};


struct table_revision
{
    std::uint32_t   raw;

    constexpr std::uint16_t get_major() const
    {
        return (raw >> 16);
    }

    constexpr std::uint8_t get_minor() const
    {
        return (raw & 0xffff) / 10;
    }

    constexpr std::uint8_t get_patch() const
    {
        return (raw & 0xffff) % 10;
    }
};


struct table_header
{
    std::uint64_t       signature;
    table_revision      revision;
    std::uint32_t       header_size;
    std::uint32_t       crc32;
    std::uint32_t       reserved;
};


struct system_table
{
    table_header        header;
    char16_t*           firmware_vendor;
    std::uint32_t       firmware_revision;
    void*               console_in_handle;
    void*               console_in_protocol;
    void*               console_out_handle;
    void*               console_out_protocol;
    void*               error_handle;
    void*               error_protocol;
    void*               runtime_services;
    void*               boot_services;
    std::uintptr_t      number_of_entries;
    void*               configuration_table;
};


} // namespace System::Firmware::EFI


#endif /* ifndef __SYSTEM_FIRMWARE_EFI_TABLES_H */
