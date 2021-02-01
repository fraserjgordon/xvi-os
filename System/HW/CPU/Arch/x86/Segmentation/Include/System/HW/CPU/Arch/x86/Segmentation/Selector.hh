#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_SELECTOR_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_SELECTOR_H


#include <cstdint>


namespace System::HW::CPU::X86
{


class SegmentSelector
{
public:

    static constexpr std::uint16_t RPL_MASK     = 0x0003;
    static constexpr std::uint16_t TI           = 0x0004;
    static constexpr std::uint16_t INDEX_MASK   = 0xFFF8;
    static constexpr int           INDEX_SHIFT  = 3;


    constexpr SegmentSelector() = default;
    constexpr SegmentSelector(const SegmentSelector&) = default;
    constexpr SegmentSelector(SegmentSelector&&) = default;

    constexpr explicit SegmentSelector(std::uint16_t value)
        : m_value(value)
    {
    }

    constexpr SegmentSelector& operator=(const SegmentSelector&) = default;
    constexpr SegmentSelector& operator=(SegmentSelector&&) = default;

    ~SegmentSelector() = default;


    constexpr int getRPL() const
    {
        return (m_value & RPL_MASK);
    }

    constexpr bool isLocal() const
    {
        return (m_value & TI);
    }

    constexpr bool isGlobal() const
    {
        return !isLocal();
    }

    constexpr int getIndex() const
    {
        return (m_value & INDEX_MASK) >> INDEX_SHIFT;
    }

    constexpr std::uint16_t getValue() const
    {
        return m_value;
    }

    static void setCS(SegmentSelector cs)
    {
        asm volatile
        (
            "   push    %0      \n\t"
            "   push    $1f     \n\t"
            "   lret            \n\t"
            "1:                 \n\t"
            :
            : "g" (cs.getValue())
            : "memory"
        );
    }

    static void setDS(SegmentSelector ds)
    {
        asm volatile
        (
            "mov    %0, %%ds"
            :
            : "r" (ds.getValue())
            : "memory"
        );
    }

    static void setES(SegmentSelector es)
    {
        asm volatile
        (
            "mov    %0, %%es"
            :
            : "r" (es.getValue())
            : "memory"
        );
    }

    static void setFS(SegmentSelector fs)
    {
        asm volatile
        (
            "mov    %0, %%fs"
            :
            : "r" (fs.getValue())
            : "memory"
        );
    }

    static void setGS(SegmentSelector gs)
    {
        asm volatile
        (
            "mov    %0, %%gs"
            :
            : "r" (gs.getValue())
            : "memory"
        );
    }

    static void setSS(SegmentSelector ss)
    {
        asm volatile
        (
            "mov    %0, %%ss"
            :
            : "r" (ss.getValue())
            : "memory"
        );
    }

    static void setLDTR(SegmentSelector ldt)
    {
        asm volatile
        (
            "lldt   %0"
            :
            : "r" (ldt.getValue())
            : "memory"
        );
    }

    static void setTR(SegmentSelector tss)
    {
        asm volatile
        (
            "ltr    %0"
            :
            : "r" (tss.getValue())
            : "memory"
        );
    }

    static SegmentSelector getCS()
    {
        std::uint16_t value;
        asm
        (
            "mov    %%cs, %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

    static SegmentSelector getDS()
    {
        std::uint16_t value;
        asm
        (
            "mov    %%ds, %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

    static SegmentSelector getES()
    {
        std::uint16_t value;
        asm
        (
            "mov    %%es, %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

    static SegmentSelector getFS()
    {
        std::uint16_t value;
        asm
        (
            "mov    %%fs, %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

    static SegmentSelector getGS()
    {
        std::uint16_t value;
        asm
        (
            "mov    %%gs, %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

    static SegmentSelector getSS()
    {
        std::uint16_t value;
        asm
        (
            "mov    %%ss, %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

    static SegmentSelector getLDTR()
    {
        std::uint16_t value;
        asm
        (
            "sldt   %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

    static SegmentSelector getTR()
    {
        std::uint16_t value;
        asm
        (
            "str    %0"
            : "=r" (value)
        );

        return SegmentSelector{value};
    }

private:

    // Defaults to the null selector.
    std::uint16_t m_value = 0;
};


} // namespace System::HW::CPU::X86


#endif // ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_SELECTOR_H
