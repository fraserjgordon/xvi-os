#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_TSS_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_TSS_H


#include <cstddef>
#include <bitset>

#include <System/HW/CPU/Arch/x86/ControlRegs/EFLAGS.hh>

#include <System/HW/CPU/Arch/x86/Segmentation/Descriptor.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/Selector.hh>


namespace System::HW::CPU::X86
{


struct tss32_t
{
public:

    SegmentSelector     previous_tss;   // Link to the previous TSS.
    std::uint16_t       :16;            // --RESERVED--
    std::uint32_t       esp0;           // PL0 stack pointer.
    SegmentSelector     ss0;            // PL0 stack segment.
    std::uint16_t       :16;            // --RESERVED--
    std::uint32_t       esp1;           // PL1 stack pointer.
    SegmentSelector     ss1;            // PL1 stack segment.
    std::uint16_t       :16;            // --RESERVED--
    std::uint32_t       esp2;           // PL2 stack pointer.
    SegmentSelector     ss2;            // PL2 stack segment.
    std::uint16_t       :16;            // --RESERVED--
    std::uint32_t       cr3;            // Page table base register.
    std::uint32_t       eip;            // Instruction pointer.
    eflags_t            eflags;         // Processor state flags.
    std::uint32_t       eax;            // General-purpose register.
    std::uint32_t       ecx;            // General-purpose register.
    std::uint32_t       edx;            // General-purpose register.
    std::uint32_t       ebx;            // General-purpose register.
    std::uint32_t       esp;            // Stack pointer.
    std::uint32_t       ebp;            // General-purpose register.
    std::uint32_t       esi;            // General-purpose register.
    std::uint32_t       edi;            // General-purpose register.
    SegmentSelector     es;             // Data segment selector.
    std::uint16_t       :16;            // --RESERVED--
    SegmentSelector     cs;             // Code segment selector.
    std::uint16_t       :16;            // --RESERVED--
    SegmentSelector     ss;             // Stack segment selector.
    std::uint16_t       :16;            // --RESERVED--
    SegmentSelector     ds;             // Data segment selector.
    std::uint16_t       :16;            // --RESERVED--
    SegmentSelector     fs;             // Extra segment selector.
    std::uint16_t       :16;            // --RESERVED--
    SegmentSelector     gs;             // Extra segment selector.
    std::uint16_t       :16;            // --RESERVED--
    SegmentSelector     ldt;            // Local descriptor table segment selector.
    std::uint16_t       :16;            // --RESERVED--
    std::uint16_t       trap:1;         // Causes a debug trap on task switch if set.
    std::uint16_t       :15;            // --RESERVED--
    std::uint16_t       iopbm_offset;   // Offset of I/O permission bitmap from TSS start.
};

static_assert(sizeof(tss32_t) == 104);

template <class CustomData, std::size_t IoPermissionsSize>
class CustomTSS32
{
public:

    template <class... Args>
    constexpr CustomTSS32(Args&&... args)
        : m_tss{},
          m_customData{std::forward<Args>(args)...},
          m_iopmSize{IoPermissionsSize},
          m_interruptRedirections{},
          m_ioPermissions{},
          m_ioPermissionsTerminator{0xFF}
    {
        // Configure the TSS I/O permission map offset.
        m_tss.iopbm_offset = offsetof(CustomTSS32, m_ioPermissions);
        
        // Intercept all interrupts by default.
        m_interruptRedirections.set();

        // Disallow all I/O port accesses by default.
        m_ioPermissions.set();
    }

    constexpr CustomTSS32(const CustomTSS32&) = default;
    constexpr CustomTSS32(CustomTSS32&&) = default;

    template <std::size_t N, class = std::enable_if_t<IoPermissionsSize != N, void>>
    constexpr CustomTSS32(const CustomTSS32<CustomData, N>& other)
        : m_tss{other.m_tss},
          m_customData{other.m_customData},
          m_iopmSize{IoPermissionsSize},
          m_interruptRedirections{other.m_interruptRedirections},
          m_ioPermissions{},
          m_ioPermissionsTerminator{0xFF}
    {
        // Default all I/O port accesses to being disallowed.
        m_ioPermissions.set();

        // Copy the common subsequence of I/O port bits.
        for (std::size_t i = 0; i < IoPermissionsSize && i < other.m_iopmSize; ++i)
            m_ioPermissions[i] = other.m_ioPermissions[i];
    }

    constexpr CustomTSS32& operator=(const CustomTSS32&) = default;
    constexpr CustomTSS32& operator=(CustomTSS32&&) = default;

    ~CustomTSS32() = default;

    // Returns the TSS.
    constexpr tss32_t& tss()
    {
        return m_tss;
    }

    // Returns the TSS.
    constexpr const tss32_t& tss() const
    {
        return m_tss;
    }

    // Size of the TSS (including custom data).
    constexpr std::size_t size() const
    {
        //! @TODO: this should be the run-time size.
        return sizeof(*this);
    }

    // Returns the custom data.
    constexpr CustomData& customData()
    {
        return m_customData;
    }

    // Returns the custom data.
    constexpr const CustomData& customData() const
    {
        return m_customData;
    }

    // Returns a segment descriptor appropriate for this TSS object.
    SegmentDescriptor createDescriptor() const
    {
        // TODO: this should be based on the run-time size.
        auto size = sizeof(*this);
        return SegmentDescriptor::createSystem(SegmentType::TSS, reinterpret_cast<std::uint32_t>(this), size - 1);
    }

    // Returns whether the given interrupt is redirected or not.
    constexpr bool isInterruptRedirected(std::uint8_t vector) const
    {
        return m_interruptRedirections.test(vector);
    }

    // Sets or unsets the given interrupt as redirected to the protected-mode handler.
    constexpr void setInterruptRedirected(std::uint8_t vector, bool redirected)
    {
        m_interruptRedirections.set(vector, redirected);
    }

    // Returns the size of the I/O permission bitmap. This might not be the same as the template parameter if this
    // object has been type-punned.
    constexpr std::size_t getIOPMSize() const
    {
        return m_iopmSize;
    }

    // Returns whether access to all ports in the given range is allowed.
    constexpr bool isIOPortAllowed(std::uint16_t start, std::uint16_t length) const
    {
        for (int i = start; i < (start + length); ++i)
        {
            // Clear = allowed; set = disallowed.
            if (m_ioPermissions.test(i))
                return false;
        }

        return true;
    }

    // Returns whether the given byte-sized I/O port access is allowed.
    constexpr bool isIOByteAllowed(std::uint16_t port) const
    {
        return isIOPortAllowed(port, 1);
    }

    // Returns whether the given word-sized I/O port access is allowed.
    constexpr bool isIOWordAllowed(std::uint16_t port) const
    {
        return isIOPortAllowed(port, 2);
    }

    // Returns whether the given doubleword-sized I/O port access is allowed.
    constexpr bool isIODWordAllowed(std::uint16_t port) const
    {
        return isIOPortAllowed(port, 4);
    }

    // Sets the permission for the given range of I/O ports.
    constexpr void setIOPortAllowed(std::uint16_t start, std::uint16_t length, bool allowed)
    {
        // Clear = allowed; set = disallowed.
        for (int i = start; i < (start + length); ++i)
            m_ioPermissions.set(i, !allowed);
    }

    // Sets the permissions for the given byte-sized I/O port access.
    constexpr void setIOByteAllowed(std::uint16_t port, bool allowed)
    {
        setIOPortAllowed(port, 1, allowed);
    }

    // Sets the permissions for the given word-sized I/O port access.
    constexpr void setIOWordAllowed(std::uint16_t port, bool allowed)
    {
        setIOPortAllowed(port, 2, allowed);
    }

    // Sets the permissions for the given doubleword-sized I/O port access.
    constexpr void setIODWordAllowed(std::uint16_t port, bool allowed)
    {
        setIOPortAllowed(port, 4, allowed);
    }

private:

    tss32_t                         m_tss;
    CustomData                      m_customData;
    std::size_t                     m_iopmSize;
    std::bitset<256>                m_interruptRedirections;
    std::bitset<IoPermissionsSize>  m_ioPermissions;
    const std::byte                 m_ioPermissionsTerminator;
};


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_TSS_H */
