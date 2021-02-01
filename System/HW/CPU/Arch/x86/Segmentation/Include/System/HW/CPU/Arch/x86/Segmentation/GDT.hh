#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_GDT_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_GDT_H


#include <System/HW/CPU/Arch/x86/Segmentation/DescriptorTable.hh>


namespace System::HW::CPU::X86
{


class GlobalDescriptorTable
    : public SegmentDescriptorTable
{
public:

    using SegmentDescriptorTable::SegmentDescriptorTable;


    // Structure used to describe the memory containing the GDT.
    struct [[gnu::packed]] alignas(std::uint16_t) gdt_pointer
    {
#if __x86_64__
        std::uint16_t limit;
        std::uint64_t base;
#else
        std::uint16_t limit;
        std::uint32_t base;
#endif
    };


    // Returns a segment selector for the given index. To be able to use the selector, this GDT must be active.
    constexpr SegmentSelector getSelector(int index, int rpl)
    {
        return SegmentDescriptorTable::getSelector(index, rpl, false);
    }

    // Makes this global descriptor table active on the executing CPU.
    void activate() const
    {
#if __x86_64__
        gdt_pointer descriptor =
        {
            .limit = getLimit(),
            .base = getBaseAddress()
        };
        asm volatile
        (
            "lgdtq  %0"
            :
            : "m" (descriptor)
            : "memory"
        );
#else
        gdt_pointer descriptor =
        {
            .limit = getLimit(),
            .base = getBaseAddress()
        };
        asm volatile
        (
            "lgdtl  %0"
            :
            : "m" (descriptor)
            : "memory"
        );
#endif
    }


    // Gets the GDT pointer for this CPU.
    static gdt_pointer getCurrentGdtPointer()
    {
        gdt_pointer p;
        asm
        (
#if __x86_64__
            "sgdtq  %0"
#else
            "sgdtl  %0"
#endif
            : "=m" (p)
            :
            :
        );
        return p;
    }

    // Returns a GDT object for the current CPU's GDT.
    static GlobalDescriptorTable getCurrent()
    {
        auto p = getCurrentGdtPointer();
        auto base = reinterpret_cast<SegmentDescriptor*>(p.base);
        auto count = (p.limit + 1) / sizeof(SegmentDescriptor);
        std::span entries {base, count};

        return GlobalDescriptorTable(entries);
    }

    template <std::size_t N>
    static GlobalDescriptorTable fromRaw(std::span<SegmentDescriptor, N> entries)
    {
        return GlobalDescriptorTable(entries);
    }
};


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_GDT_H */
