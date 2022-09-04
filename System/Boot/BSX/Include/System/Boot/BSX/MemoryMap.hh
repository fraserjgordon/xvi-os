#ifndef __SYSTEM_BOOT_BSX_MEMORYMAP_H
#define __SYSTEM_BOOT_BSX_MEMORYMAP_H


#include <cstdint>


namespace System::Boot::BSX
{



using memory_flag_t = std::uint32_t;

namespace MemoryFlag
{

constexpr memory_flag_t OrdinaryRAM         = 0x00000001;   // Hint that memory is suitable for use as ordinary RAM.

constexpr memory_flag_t Read                = 0x00000002;   // Non-instruction data may be read from this memory.
constexpr memory_flag_t Write               = 0x00000004;   // Memory may be written to.
constexpr memory_flag_t Execute             = 0x00000008;   // Instructions may be fetched from this memory.

constexpr memory_flag_t CacheNone           = 0x00000010;   // Memory supports uncached accesses.
constexpr memory_flag_t CacheWriteBack      = 0x00000020;   // Memory supports cached accesses with write-back semantics.
constexpr memory_flag_t CacheWriteThrough   = 0x00000040;   // Memory supports cached accesses with write-through semantics.
constexpr memory_flag_t CacheWriteCombine   = 0x00000080;   // Memory supports uncached but write-combining accesses.

constexpr memory_flag_t Hotpluggable        = 0x00001000;   // Memory can be hotplugged.


constexpr memory_flag_t NonVolatile         = 0x00010000;   // Memory contents are preserved across shutdowns.
constexpr memory_flag_t SideEffects         = 0x00020000;   // Accessing this memory may cause side-effects.
constexpr memory_flag_t SleepPreserve       = 0x00040000;   // OS required to preserve contents across sleeps.
constexpr memory_flag_t Local               = 0x00080000;   // Memory is visible only to the CPU but not from the system bus.
constexpr memory_flag_t Incoherent          = 0x00100000;   // Inter-CPU cache coherency is not available for this memory.
constexpr memory_flag_t Disabled            = 0x00200000;   // Memory is disabled but may become usable later (e.g. hotplug).


// Flags that are harmless if the receiver doesn't understand them.
constexpr memory_flag_t CompatibleMask      = 0x0000FFFF;

// Flags that must cause the receiver to ignore the memory if it doesn't understand them.
constexpr memory_flag_t IncompatibleMask    = ~CompatibleMask;

}


enum class MemoryUse : std::uint32_t
{
    Unknown             = 0,            // Purpose of the memory is not known; must not be used.
    Free                = 1,            // Memory is not known to contain anything useful.
    MMIO                = 2,            // Memory is an MMIO range and not real memory.
    Firmware            = 3,            // Memory is used or reserved by the firmware.
    Kernel              = 4,            // Memory contains the kernel code and data.
    BootLoader          = 5,            // Memory contains the bootloader code and data.
};

using memory_use_t = MemoryUse;


struct memory_map_entry
{
    std::uint64_t       base = 0;
    std::uint64_t       length = 0;

    memory_flag_t       flags = 0;
    memory_use_t        use = MemoryUse::Unknown;
};


} // namespace System::Boot::BSX


#endif /* ifndef __SYSTEM_BOOT_BSX_MEMORYMAP_H */
