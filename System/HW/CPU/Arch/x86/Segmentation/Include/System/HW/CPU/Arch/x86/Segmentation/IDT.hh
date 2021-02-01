#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_IDT_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_IDT_H


#include <System/HW/CPU/Arch/x86/Segmentation/DescriptorTable.hh>


namespace System::HW::CPU::X86
{


class InterruptDescriptorTable32
    : public SegmentDescriptorTable
{
public:

    using SegmentDescriptorTable::SegmentDescriptorTable;

#if !__x86_64__
    // Makes this global descriptor table active on the executing CPU.
    //
    // It is not possible to activate 32-bit IDTs in 64-bit mode.
    void activate() const
    {
        struct [[gnu::packed]] { std::uint16_t limit; std::uint32_t base; } descriptor =
        {
            .limit = getLimit(),
            .base = getBaseAddress()
        };
        asm volatile
        (
            "lidtl  %0"
            :
            : "m" (descriptor)
            : "memory"
        );
    }
#endif

    template <std::size_t N>
    static InterruptDescriptorTable32 fromRaw(std::span<SegmentDescriptor, N> entries)
    {
        return InterruptDescriptorTable32(entries);
    }
};

class InterruptDescriptorTable64
    : public SegmentDescriptorTable
{
public:

    using SegmentDescriptorTable::SegmentDescriptorTable;

    // Shadow the constructor using pre-allocated memory as 64-bit IDTs are more easily thought about as a table of
    // 16-byte descriptors (rather than the usual 8-byte descriptors).
    template <std::size_t N>
    constexpr InterruptDescriptorTable64(std::span<SegmentDescriptor64, N> existing);

    // Shadow the base-class getEntry() method as it is convenient to directly map interrupt vectors to table indices.
    const SegmentDescriptor64& getEntry(int index) const
    {
        return reinterpret_cast<const SegmentDescriptor64&>(SegmentDescriptorTable::getEntry(index * 2));
    }

    // Shadow the base-class getSize() method as this table has only half as many entries at it first appears.
    constexpr std::size_t getSize() const
    {
        return SegmentDescriptorTable::getSize() / 2;
    }

    // Shadow the base-class writeEntry method for the same reason as we shadow getEntry().
    void writeEntry(int index, const SegmentDescriptor64& entry)
    {
        SegmentDescriptorTable::writeEntry(index * 2, entry);
    }

#if __x86_64__
    // Makes this interrupt descriptor table active on the executing CPU.
    //
    // It is not possible to activate 64-bit IDTs in 16- or 32-bit modes.
    void activate() const
    {
        struct { std::uint16_t limit; std::uint64_t base; } descriptor =
        {
            .limit = getLimit(),
            .base = getBaseAddress()
        };
        asm volatile
        (
            "lidtq  %0"
            :
            : "m" (descriptor)
            : "memory"
        );
    }
#endif

private:

    // Returns the table as an array of SegmentDescriptor64.
    SegmentDescriptor64* getTable64()
    {
        return reinterpret_cast<SegmentDescriptor64*>(getTable());
    }
};


#if __x86_64__
using InterruptDescriptorTable = InterruptDescriptorTable64;
#else
using InterruptDescriptorTable = InterruptDescriptorTable32;
#endif


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_GDT_H */
