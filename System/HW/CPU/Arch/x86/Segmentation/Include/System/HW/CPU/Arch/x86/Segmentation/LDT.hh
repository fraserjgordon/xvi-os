#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_LDT_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_LDT_H


#include <System/HW/CPU/Arch/x86/Segmentation/DescriptorTable.hh>


namespace System::HW::CPU::X86
{


class LocalDescriptorTable
    : public SegmentDescriptorTable
{
public:

    using SegmentDescriptorTable::SegmentDescriptorTable;

    // Returns a segment selector for the given index. To be able to use the selector, this LDT must be active.
    constexpr SegmentSelector getSelector(int index, int rpl)
    {
        return SegmentDescriptorTable::getSelector(index, rpl, true);
    }

    // Returns a segment descriptor that can be added to a GDT to make this LDT usable. This is the 32-bit variant; do
    // not run "lldt" on this descriptor in 64-bit mode.
    //
    // This 32-bit variant will wrap the base address if located >4GiB!
    SegmentDescriptor getDescriptor32(std::uintptr_t virtual_to_physical_adjust = 0) const
    {
        return SegmentDescriptor::createSystem(SegmentType::LDT, static_cast<std::uint32_t>(getBaseAddress()) + virtual_to_physical_adjust, getLimit());
    }

    // Returns a segment descriptor that can be added to a GDT to make this LDT usable. This is the 64-bit variant; do
    // not run "lldt" on this descriptor in 16-/32-bit mode if located >4GiB as the upper 32 bits will be silently
    // ignored by the processor.
    SegmentDescriptor64 getDescriptor64() const
    {
        return SegmentDescriptor64::createSystem(SegmentType::LDT, getBaseAddress(), getLimit());
    }
};


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_GDT_H */
