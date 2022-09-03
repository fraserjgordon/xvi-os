#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_PROBE_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_PROBE_H


#include <System/Boot/Igniter/Multiboot/v1.hh>
#include <System/Boot/Igniter/Multiboot/v2.hh>


namespace System::Boot::Igniter
{


void setMultibootTablePointer(const multiboot_v1_info*);
void setMultibootTablePointer(const multiboot_v2_info*);

// Detects memory in a conservative way. The goal is to identify enough free pages of memory so that the MMU can be
// enabled and so that a proper dynamic heap can be set up to allow for more thorough memory probing.
//
// In particular, this method:
//  - probes the BIOS data areas for memory information
//  - parses any memory information passed via Multiboot tables
//  - marks potentially-occupied memory as being unavailble (e.g this loader, any multiboot tables, etc).
void performEarlyMemoryProbe();


// Allocates pages of memory located in the lower 1MB of address space (i.e the addresses that are accessible in real
// mode).
std::uint32_t allocateEarlyRealModePage();

// Allocates pages of memory early in the boot process.
std::uint32_t allocateEarlyPage();

// Frees a page of memory that was allocated early.
void freeEarlyPage(std::uint32_t);


// Called by the MMU code to request that the early memory mappings are added.
void mapEarlyMemory();


void hardwareProbe();


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_PROBE_H */
