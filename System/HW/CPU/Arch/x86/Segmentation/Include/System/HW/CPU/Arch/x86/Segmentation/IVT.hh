#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_IVT_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_IVT_H


#include <cstddef>
#include <cstdint>

#include <System/HW/CPU/Arch/x86/Segmentation/RealMode.hh>


namespace System::HW::CPU::X86
{


struct ivt_entry_t
{
    realmode_ptr<void>  target;
};


class InterruptVectorTable
{
public:

    // Real-mode interrupt vector tables are generally 256 entries (1024 bytes) long and at physical address zero. It is
    // possible to change this but it isn't normally done for compatibility reasons.
    static constexpr std::size_t    IVT_ENTRIES         = 256;
    static constexpr std::uintptr_t IVT_BASE_ADDRESS    = 0x00000000;

    // Gets the destination address for the given vector.
    constexpr realmode_ptr<void> getDestination(std::uint8_t vector) const noexcept
    {
        return m_vectors[vector].target;
    }

private:

    // The vector table itself.
    ivt_entry_t     m_vectors[IVT_ENTRIES];
};

static_assert(sizeof(ivt_entry_t) == 4);
static_assert(sizeof(InterruptVectorTable) == 1024);


} // namespace System::HW:CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_IVT_H */
