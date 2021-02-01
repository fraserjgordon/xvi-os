#pragma once
#ifndef __SYSTEM_FIRMWARE_EFI_TABLES_H
#define __SYSTEM_FIRMWARE_EFI_TABLES_H


#include <System/Firmware/EFI/API.hh>


namespace System::Firmware::EFI
{


// Forward declarations.
struct EFI_SYSTEM_TABLE;
struct EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
struct EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;
struct EFI_RUNTIME_SERVICES;
struct EFI_BOOT_SERVICES;
struct EFI_CONFIGURATION_TABLE;


enum class table_signature : UINT64
{
    BootServices    = 0x56524553544f4f42,
    RuntimeServices = 0x56524553544e5552,
    System          = 0x5453595320494249,
};


struct table_revision
{
    UINT32 raw;

    constexpr std::uint16_t get_major() const
    {
        return (raw >> 16);
    }

    constexpr std::uint16_t get_minor() const
    {
        return (raw & 0xffff) / 10;
    }

    constexpr std::uint8_t get_patch() const
    {
        return (raw & 0xffff) % 10;
    }

    constexpr bool operator==(const table_revision&) const noexcept = default;
    constexpr auto operator<=>(const table_revision&) const noexcept = default;


    static constexpr table_revision make(UINT16 major, UINT16 minor, UINT16 patch)
    {
        return {.raw = static_cast<UINT32>((major << 16) | (minor * 10 + patch))};
    }
};


struct EFI_TABLE_HEADER
{
    table_signature     signature;
    table_revision      revision;
    UINT32              header_size;
    UINT32              crc32;
    UINT32              reserved;
};


static constexpr auto v1_02 = table_revision::make(1, 0, 2);
static constexpr auto v1_10 = table_revision::make(1, 1, 0);
static constexpr auto v2_00 = table_revision::make(2, 0, 0);
static constexpr auto v2_10 = table_revision::make(2, 1, 0);
static constexpr auto v2_20 = table_revision::make(2, 2, 0);
static constexpr auto v2_30 = table_revision::make(2, 3, 0);
static constexpr auto v2_31 = table_revision::make(2, 3, 1);
static constexpr auto v2_40 = table_revision::make(2, 4, 0);
static constexpr auto v2_50 = table_revision::make(2, 5, 0);
static constexpr auto v2_60 = table_revision::make(2, 6, 0);
static constexpr auto v2_70 = table_revision::make(2, 7, 0);
static constexpr auto v2_80 = table_revision::make(2, 8, 0);

struct EFI_SYSTEM_TABLE
{
    EFI_TABLE_HEADER                    header;
    CHAR16*                             firmware_vendor;
    UINT32                              firmware_revision;
    EFI_HANDLE                          console_in_handle;
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL*     console_in_protocol;
    EFI_HANDLE                          console_out_handle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*    console_out_protocol;
    EFI_HANDLE                          error_handle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*    error_protocol;
    EFI_RUNTIME_SERVICES*               runtime_services;
    EFI_BOOT_SERVICES*                  boot_services;
    UINTN                               number_of_entries;
    EFI_CONFIGURATION_TABLE*            configuration_table;
};

struct EFI_BOOT_SERVICES
{
    EFI_TABLE_HEADER                    header;

    void* RaiseTPL;
    void* RestoreTPL;

    void* AllocatePages;
    void* FreePages;
    void* GetMemoryMap;
    void* AllocatePool;
    void* FreePool;

    void* CreateEvent;
    void* SetTimer;
    void* WaitForEvent;
    void* SignalEvent;
    void* CloseEvent;
    void* CheckEvent;

    void* InstallProtocolInterface;
    void* ReinstallProtocolInterface;
    void* UninstallProtocolInterface;
    void* HandleProtocol;
    void* reserved;
    void* RegisterProtocolNotify;
    void* LocateHandle;
    void* LocateDevicePath;
    void* InstallConfigurationTable;

    void* LoadImage;
    void* StartImage;
    void* Exit;
    void* UnloadImage;
    void* ExitBootServices;

    void* GetNextMonotonicCount;
    void* Stall;
    void* SetWatchdogTimer;

    void* ConnectController;
    void* DisconnectController;

    void* OpenProtocol;
    void* CloseProtocol;
    void* OpenProtocolInformation;

    void* ProtocolsPerHandle;
    void* LocateHandleBuffer;
    void* LocateProtocol;
    void* InstallMultipleProtocolInterfaces;
    void* UninstallMultipleProtocolInterfaces;

    void* CalculateCrc32;

    void* CopyMem;
    void* SetMem;
    void* CreateEventEx;
};

struct EFI_RUNTIME_SERVICES
{
    EFI_TABLE_HEADER    header;

    void* GetTime;
    void* SetTime;
    void* GetWakeupTime;
    void* SetWakeupTime;

    void* SetVirtualAddressMap;
    void* ConvertPointer;

    void* GetVariable;
    void* GetNextVariableName;
    void* SetVariable;

    void* GetNextHighMonotonicCount;
    void* ResetSystem;

    void* UpdateCapsule;
    void* QueryCapsuleCapabilities;

    void* QueryVariableInfo;
};

struct EFI_CONFIGURATION_TABLE
{
    EFI_GUID        VendorGuid;
    VOID*           VendorTable;
};

// Well-known configuration table GUIDs.
inline constexpr EFI_GUID EFI_ACPI_20_TABLE_GUID = {0x8868e871, 0xe4f1, 0x11d3, {0xbc,0x22,0x00,0x80,0xc7,0x3c,0x88,0x81}};
inline constexpr EFI_GUID EFI_ACPI_10_TABLE_GUID =  {0xeb9d2d30, 0x2d88, 0x11d3, {0x9a,0x16,0x00,0x90,0x27,0x3f,0xc1,0x4d}};
inline constexpr EFI_GUID EFI_SAL_SYSTEM_TABLE_GUID = {0xeb9d2d32, 0x2d88, 0x11d3, {0x9a,0x16,0x00,0x90,0x27,0x3f,0xc1,0x4d}};
inline constexpr EFI_GUID EFI_SMBIOS_TABLE_GUID =  {0xeb9d2d31, 0x2d88, 0x11d3, {0x9a,0x16,0x00,0x90,0x27,0x3f,0xc1,0x4d}};
inline constexpr EFI_GUID EFI_SMBIOS3_TABLE_GUID = {0xf2fd1544, 0x9794, 0x4a2c, {0x99,0x2e,0xe5,0xbb,0xcf,0x20,0xe3,0x94}};
inline constexpr EFI_GUID EFI_MPS_TABLE_GUID = {0xeb9d2d2f, 0x2d88, 0x11d3, {0x9a,0x16,0x00,0x90,0x27,0x3f,0xc1,0x4d}};


} // namespace System::Firmware::EFI


#endif /* ifndef __SYSTEM_FIRMWARE_EFI_TABLES_H */
