#ifndef __SYSTEM_FIRMWARE_EFI_API_H
#define __SYSTEM_FIRMWARE_EFI_API_H


#include <array>
#include <cstddef>
#include <cstdint>


#if defined(__x86_64__)
#  define EFIAPI    __attribute__((ms_abi))
#elif defined(__i386__)
#  define EFIAPI    __attribute__((cdecl))
#else
#  error Unknown EFI calling convention for this architecture
#endif


namespace System::Firmware::EFI
{


using BOOLEAN       = bool;
using INTN          = std::intptr_t;
using UINTN         = std::uintptr_t;
using INT8          = std::int8_t;
using UINT8         = std::uint8_t;
using INT16         = std::int16_t;
using UINT16        = std::uint16_t;
using INT32         = std::int32_t;
using UINT32        = std::uint32_t;
using INT64         = std::int64_t;
using UINT64        = std::uint64_t;

using CHAR8         = char;
using CHAR16        = char16_t;

using VOID          = void;

struct EFI_GUID { std::uint32_t a alignas(std::uint64_t); std::uint16_t b; std::uint16_t c; std::array<std::uint8_t, 8> d; };

using EFI_STATUS    = UINTN;
using EFI_HANDLE    = VOID*;
using EFI_EVENT     = VOID*;
using EFI_LBA       = UINT64;
using EFI_TPL       = UINTN;

struct EFI_MAC_ADDRESS { std::array<std::byte, 32> address; };

struct EFI_IPv4_ADDRESS { std::array<std::byte, 4> address; };

struct EFI_IPv6_ADDRESS { std::array<std::byte, 16> address; };

union EFI_IP_ADDRESS { std::array<std::byte, 16> address; };



} // namespace System::Firmware::EFI


#endif /* ifndef __SYSTEM_FIRMWARE_EFI_API_H */
