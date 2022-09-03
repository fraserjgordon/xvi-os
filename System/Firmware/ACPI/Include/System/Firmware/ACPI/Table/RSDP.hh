#ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_RSDP_H
#define __SYSTEM_FIRMWARE_ACPI_TABLE_RSDP_H


#include <span>

#include <System/Firmware/ACPI/Table/Common.hh>


namespace System::Firmware::ACPI
{


struct rsdp
{
    static constexpr std::array<char, 8> Signature = { 'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ' };

    std::array<char, 8>         signature = Signature;
    std::uint8_t                checksum = 0;
    std::array<char, 6>         oem_id = {};
    std::uint8_t                revision = 0;
    uint32le_t                  rsdt_address = 0;
    uint32le_t                  length;
    uint64le_t                  xsdt_address = 0;
    std::uint8_t                extended_checksum = 0;
    std::uint8_t                _reserved[3];


    bool validSignature() const
    {
        return signature == Signature;
    }

    bool checksumCorrect() const
    {
        // Check the first 20 bytes (the ACPI 1.0 part of this table).
        auto bytes = std::as_bytes(std::span{this, 1}).subspan(0, 20);

        std::uint8_t sum = 0;
        for (auto byte : bytes)
            sum += static_cast<std::uint8_t>(byte);

        return sum == 0;
    }

    bool hasExtendedFields() const
    {
        return revision >= 2;
    }

    bool extendedChecksumCorrect() const
    {
        // Check the entire table (only valid for ACPI 2.0+).
        auto bytes = std::span { reinterpret_cast<const std::byte*>(this), length };

        std::uint8_t sum = 0;
        for (auto byte : bytes)
            sum += static_cast<std::uint8_t>(byte);

        return sum == 0;
    }
};


} // namespace System::Firmware::ACPI


#endif /* ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_RSDP_H */
