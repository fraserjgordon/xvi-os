#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_INTERRUPTS_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_INTERRUPTS_H


#include <cstdint>

#include <System/ABI/ExecContext/Arch/x86/GeneralRegs.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/Selector.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/TSS.hh>


namespace System::Boot::Igniter
{


namespace Selector
{

using SegmentSelector = System::HW::CPU::X86::SegmentSelector;

inline constexpr SegmentSelector Null      {0x00};
inline constexpr SegmentSelector CodeFlat  {0x08};
inline constexpr SegmentSelector DataFlat  {0x10};
inline constexpr SegmentSelector Code      {0x18};
inline constexpr SegmentSelector Data      {0x20};
inline constexpr SegmentSelector Tss       {0x28};
inline constexpr SegmentSelector CustomFs  {0x30};
inline constexpr SegmentSelector CustomGs  {0x38};

} // namespace Selector


struct interrupt_context
{
    struct segment_regs_t
    {
        std::uint16_t       gs;
        std::uint16_t       _gs_pad;
        std::uint16_t       fs;
        std::uint16_t       _fs_pad;
        std::uint16_t       es;
        std::uint16_t       _es_pad;
        std::uint16_t       ds;
        std::uint16_t       _ds_pad;
    } segment_regs;

    System::ABI::ExecContext::pushl_t general_regs;

    std::uint32_t vector;

    union
    {
        System::ABI::ExecContext::interrupt32_near_t<true>  near;
        System::ABI::ExecContext::interrupt32_far_t<true>   far;
        System::ABI::ExecContext::interrupt_v86_t<true>     v86;
    };
};


void configureInterruptTable();

void reconfigureInterruptTableForPaging();

void registerInterruptHandler(std::uint8_t vector, void (*handler)());

System::HW::CPU::X86::tss32_t& tss();


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE2_INTERRUPTS_H */
