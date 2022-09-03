#ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_COMMON_H
#define __SYSTEM_FIRMWARE_ACPI_TABLE_COMMON_H


#include <array>
#include <cstdint>

#include <System/Utility/Endian/Endian.hh>
#include <System/Utility/Endian/Unaligned.hh>


namespace System::Firmware::ACPI
{


using uint16le_t = System::Utility::Endian::uint16le_t;
using uint32le_t = System::Utility::Endian::uint32le_t;
using uint64le_t = System::Utility::Endian::uint64le_t;

using uint64ule_t = System::Utility::Endian::uint64ule_t;


struct data_table_header
{
    std::array<char, 4>     signature;
    uint32le_t              length;
    std::uint8_t            revision;
    std::uint8_t            checksum;
    std::array<char, 6>     oem_id;
    std::array<char, 8>     oem_tableid;
    uint32le_t              oem_revision;
    std::array<char, 4>     asl_compiler_id;
    uint32le_t              asl_compiler_revision;


    bool checksumCorrect() const
    {
        auto bytes = std::as_bytes(std::span{reinterpret_cast<const std::byte*>(this), length});

        std::uint8_t sum = 0;
        for (auto byte : bytes)
            sum += static_cast<std::uint8_t>(byte);

        return sum == 0;
    }
};


enum class AddressSpaceID : std::uint8_t
{
    SystemMemory            = 0,
    SystemIO                = 1,
    PCIConfiguration        = 2,
    EmbeddedController      = 3,
    SMBus                   = 4,
    SystemCMOS              = 5,
    PCIBARTarget            = 6,
    IPMI                    = 7,
    GeneralPurposeIO        = 8,
    GenericSerialBus        = 9,
    PlatformCommChannel     = 10,
    PlatformRuntimeMech     = 11,

    FixedFunctionHW         = 127,
};

using address_space_id_t = AddressSpaceID;


enum class AccessSize : std::uint8_t
{
    Undefined               = 0,
    Byte                    = 1,
    Word                    = 2,
    Dword                   = 3,
    Qword                   = 4,
};

using access_size_t = AccessSize;


struct [[gnu::packed]] generic_address
{
    address_space_id_t      address_space;
    std::uint8_t            bit_width;
    std::uint8_t            bit_offset;
    access_size_t           access_size;
    uint64ule_t             address;
};


} // namespace System::Firmware::ACPI


#endif /* ifndef __SYSTEM_FIRMWARE_ACPI_TABLE_COMMON_H */
