#ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_RSDT_H
#define __SYSTEM_FIRMWARE_ACPI_TABLE_RSDT_H


#include <System/Firmware/ACPI/Table/Common.hh>


namespace System::Firmware::ACPI
{


struct rsdt
{
    static constexpr std::array<char, 4> Signature = { 'R', 'S', 'D', 'T' };

    data_table_header           header;
    uint32le_t                  entry[1];


    bool signatureCorrect() const
    {
        return header.signature == Signature;
    }

    std::uint32_t entryCount() const
    {
        return (header.length - sizeof(header)) / sizeof(uint32le_t);
    }

    std::span<const uint32le_t> entries() const
    {
        return {&entry[0], entryCount()};
    }
};


struct xsdt
{
    static constexpr std::array<char, 4> Signature = { 'X', 'S', 'D', 'T' };

    data_table_header           header;
    uint64le_t                  entry[1];


    bool signatureCorrect() const
    {
        return header.signature == Signature;
    }

    std::uint32_t entryCount() const
    {
        return (header.length - sizeof(header)) / sizeof(uint64le_t);
    }

    std::span<const uint64le_t> entries() const
    {
        return {&entry[0], entryCount()};
    }
};


} // namespace System::Firmware::ACPI


#endif /* ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_RSDT_H */
