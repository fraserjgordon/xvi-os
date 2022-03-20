#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_DESCRIPTOR_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_DESCRIPTOR_H


#include <System/HW/CPU/Arch/x86/Segmentation/Selector.hh>


namespace System::HW::CPU::X86
{


enum class SegmentType
{
    // System segment types.
    TSS_16              = 0x01,     // 16-bit TSS.
    LDT                 = 0x02,     // Local descriptor table.
    TSS_16_BUSY         = 0x03,     // 16-bit busy TSS.
    CALL_GATE_16        = 0x04,     // 16-bit call gate.
    TASK_GATE           = 0x05,     // Task gate.
    INTERRUPT_GATE_16   = 0x06,     // 16-bit interrupt gate.
    TRAP_GATE_16        = 0x07,     // 16-bit trap gate.
    TSS                 = 0x09,     // 32-/64-bit TSS.
    TSS_BUSY            = 0x0B,     // 32-/64-bit busy TSS.
    CALL_GATE           = 0x0C,     // 32-/64-bit call gate.
    INTERRUPT_GATE      = 0x0E,     // 32-/64-bit interrupt gate.
    TRAP_GATE           = 0x0F,     // 32-/64-bit trap gate.
    
    // Code/data segment types.
    R           = 0x10,     // Read-only.
    RA          = 0x11,     // Read-only, accessed.
    RW          = 0x12,     // Read+Write.
    RWA         = 0x13,     // Read+Write, accessed.
    ER          = 0x14,     // Expand-down read-only.
    ERA         = 0x15,     // Expand-down read-only, accessed.
    ERW         = 0x16,     // Expand-down read+write.
    ERWA        = 0x17,     // Expand-down read+write, accessed.
    X           = 0x18,     // Execute-only.
    XA          = 0x19,     // Execute-only, accessed.
    RX          = 0x1A,     // Read+Execute.
    RXA         = 0x1B,     // Read+Execute, accessed.
    CX          = 0x1C,     // Conforming execute-only.
    CXA         = 0x1D,     // Conforming execute-only, accessed.
    CRX         = 0x1E,     // Conforming execute+read.
    CRXA        = 0x1F,     // Conforming execute+read, accessed.
};


struct segment_descriptor_fields_t
{
    std::uint64_t   limit_low:16;       // Low 16 bits of segment limit.
    std::uint64_t   base_low:24;        // Low 24 bits of segment base address.
    std::uint64_t   type:4;             // Segment type.
    std::uint64_t   s:1;                // Clear for system segments, set for code/data segments.
    std::uint64_t   dpl:2;              // Descriptor privilege level.
    std::uint64_t   p:1;                // Segment present.
    std::uint64_t   limit_high:4;       // Top 4 bits of segment limit.
    std::uint64_t   os:1;               // Available for OS use (unused).
    std::uint64_t   l:1;                // Set for 64-bit segments, clear for 16-/32-bit segments.
    std::uint64_t   db:1;               // Set for 32-bit segments, clear for 16-/64-bit segments.
    std::uint64_t   g:1;                // If set, limit field is in units of 4k (otherwise: bytes).
    std::uint64_t   base_high:8;        // Top 8 bits of segment base address.
};

struct segment_descriptor_gate_fields_t
{
    std::uint64_t   offset_low:16;      // Low 16 bits of target address.
    std::uint64_t   selector:16;        // Code- or task-segment selector.
    std::uint64_t   param_count:5;      // Parameter count (call gates only).
    std::uint64_t   :3;                 // --RESERVED--
    std::uint64_t   type:4;             // Segment type.
    std::uint64_t   s:1;                // Clear for all gate descriptors.
    std::uint64_t   dpl:2;              // Descriptor privilege level.
    std::uint64_t   p:1;                // Segment present.
    std::uint64_t   offset_high:16;     // Top 16 bits of target address.
};

struct segment_descriptor_fields_64_t
    : public segment_descriptor_fields_t
{
    std::uint64_t   base_ext:32;        // Top 32-bits of 64-bit base address.
    std::uint64_t   :8;                 // --RESERVED--
    std::uint64_t   mbz:5;              // --MUST BE ZERO--
    std::uint64_t   :19;                // --RESERVED--
};

struct segment_descriptor_gate_fields_64_t
    : public segment_descriptor_gate_fields_t
{
    std::uint64_t   offset_ext:32;      // Top 32-bit of 64-bit target address.
    std::uint64_t   :5;                 // --RESERVED--
    std::uint64_t   mbz:5;              // --MUST BE ZERO--
    std::uint64_t   :19;                // --RESERVED--
};

union segment_descriptor_t
{
    segment_descriptor_fields_t         fields;
    segment_descriptor_gate_fields_t    gate;
    std::uint64_t                       raw = 0;
};

union segment_descriptor_64_t
{
    segment_descriptor_fields_64_t      fields;
    segment_descriptor_gate_fields_64_t gate;
    std::uint64_t                       raw[2] = {0, 0};
};

static_assert(sizeof(segment_descriptor_t) == sizeof(std::uint64_t));
static_assert(sizeof(segment_descriptor_64_t) == 2*sizeof(std::uint64_t));


class SegmentDescriptor
{
public:

    constexpr SegmentDescriptor() = default;
    constexpr SegmentDescriptor(const SegmentDescriptor&) = default;
    constexpr SegmentDescriptor(SegmentDescriptor&&) = default;

    constexpr explicit SegmentDescriptor(const segment_descriptor_t& d)
        : m_descriptor(d)
    {
    }

    constexpr SegmentDescriptor& operator=(const SegmentDescriptor&) = default;
    constexpr SegmentDescriptor& operator=(SegmentDescriptor&&) = default;

    ~SegmentDescriptor() = default;


    constexpr const segment_descriptor_t& get() const
    {
        return m_descriptor;
    }

    constexpr segment_descriptor_t& get()
    {
        return m_descriptor;
    }


    static constexpr SegmentDescriptor createEmpty()
    {
        return SegmentDescriptor{};
    }

    // Note: the only supported segment types are the LDT segment type and the various TSS segment types.
    static constexpr SegmentDescriptor createSystem(SegmentType type, std::uint32_t base, std::uint32_t limit)
    {
        // Automatically select byte or page granularity.
        bool g = (limit >= (1 << 20));
        if (g)
            limit >>= 12;

        return SegmentDescriptor{{.fields =
        {
            .limit_low = (limit & 0xFFFF),
            .base_low = (base & 0x00FFFFFF),
            .type = (static_cast<unsigned int>(type) & 0x0F),
            .s = 0,         // System segments always have this flag clear.
            .dpl = 0,       // Non-zero DPL does not make sense for TSS/LDT segments.
            .p = 1,         // Reasonable default: segment is present.
            .limit_high = ((limit & 0xF0000) >> 16),
            .os = 0,        // Initialise the OS-use flag to zero.
            .l = 0,         // Flag not used for system segments.
            .db = 0,        // Flag not used for system segments.
            .g = g,
            .base_high = static_cast<std::uint8_t>((base & 0xFF000000) >> 24)
        }}};
    }

    // Note: the only supported segment types are the call/interrupt/trap/task gate types.
    static constexpr SegmentDescriptor createGate(SegmentType type, unsigned int dpl, SegmentSelector selector, std::uint32_t offset, unsigned int param_count = 0)
    {
        return SegmentDescriptor{{.gate =
        {
            .offset_low = (offset & 0xFFFF),
            .selector = selector.getValue(),
            .param_count = (param_count & 0x1F),
            .type = (static_cast<unsigned int>(type) & 0x0F),
            .s = 0,         // Gates (as system segments) always have this flag clear.
            .dpl = (dpl & 0x03),
            .p = 1,         // Reasonable default: segment is present.
            .offset_high = static_cast<std::uint16_t>(((offset >> 16) & 0xFFFF))
        }}};
    }

    // Note: the only supported segment types are the various code and data segment types.
    // Note: only valid values for the size parameter are 16, 32 or 64.
    // Note: only valid values for the DPL parameter are 0, 1, 2 or 3.
    static constexpr SegmentDescriptor createUser(SegmentType type, unsigned int size = 32, unsigned int dpl = 0, std::uint32_t base = 0, std::uint32_t limit = 0xFFFFFFFF)
    {
        // Automatically select byte or page granularity.
        bool g = (limit >= (1 << 20));
        if (g)
            limit >>= 12;

        // Select the appropriate segment size bits.
        bool db = (size == 32);
        bool l = (size == 64);
        
        return SegmentDescriptor{{.fields =
        {
            .limit_low = (limit & 0xFFFF),
            .base_low = (base & 0x00FFFFFF),
            .type = (static_cast<unsigned int>(type) & 0x0F),
            .s = 1,         // Non-system segments always have this flag set.
            .dpl = (dpl & 0x03),
            .p = 1,         // Reasonable default: segment is present.
            .limit_high = ((limit & 0xF0000) >> 16),
            .os = 0,        // Initialise the OS-use flag to zero.
            .l = l,
            .db = db,
            .g = g,
            .base_high = static_cast<std::uint8_t>((base & 0xFF000000) >> 24)
        }}};
    }

    // Wrapper around createUser for creating kernel-mode segments that allow the use of virtual addresses before paging
    // has been enabled.
    //
    // Because of this specific purpose, many of the fields are fixed (in particular, 32-bit DPL0 segments only).
    static constexpr SegmentDescriptor createPrePaging(SegmentType type, std::uint32_t vaddr, std::uint32_t paddr)
    {
        return createUser(type, 32, 0, paddr - vaddr, 0xFFFFFFFF);
    }

private:

    segment_descriptor_t    m_descriptor = {};
};


class SegmentDescriptor64
{
public:

    constexpr SegmentDescriptor64() = default;
    constexpr SegmentDescriptor64(const SegmentDescriptor64&) = default;
    constexpr SegmentDescriptor64(SegmentDescriptor64&&) = default;

    constexpr explicit SegmentDescriptor64(const segment_descriptor_64_t& d)
        : m_descriptor(d)
    {
    }

    constexpr SegmentDescriptor64& operator=(const SegmentDescriptor64&) = default;
    constexpr SegmentDescriptor64& operator=(SegmentDescriptor64&&) = default;

    ~SegmentDescriptor64() = default;


    constexpr const segment_descriptor_64_t& get() const
    {
        return m_descriptor;
    }

    constexpr segment_descriptor_64_t& get()
    {
        return m_descriptor;
    }


    static constexpr SegmentDescriptor64 createEmpty()
    {
        return SegmentDescriptor64{};
    }

    // Note: the only supported segment types are the LDT segment type and the various TSS segment types.
    static constexpr SegmentDescriptor64 createSystem(SegmentType type, std::uint64_t base, std::uint32_t limit)
    {
        // Create a 32-bit descriptor containing all but the high 32 bits of the base address.
        auto low = SegmentDescriptor::createSystem(type, static_cast<std::uint32_t>(base), limit);

        // Fill in the rest of the descriptor.
        segment_descriptor_64_t d = {{low.get().fields, static_cast<std::uint32_t>(base >> 32), 0}};
        return SegmentDescriptor64{d};
    }

    // Note: the only supported segment types are the call/interrupt/trap/task gate types.
    // Note: unlike for 16-/32-bit segments, no 64-bit gates support a parameter count.
    static constexpr SegmentDescriptor64 createGate(SegmentType type, unsigned int dpl, SegmentSelector selector, std::uint64_t offset)
    {
        // Create a 32-bit descriptor containing all but the high 32 bits of the base address.
        auto low = SegmentDescriptor::createGate(type, dpl, selector, static_cast<std::uint32_t>(offset), 0);

        // Fill in the rest of the descriptor.
        segment_descriptor_64_t d = {{low.get().fields, static_cast<std::uint32_t>(offset >> 32), 0}};
        return SegmentDescriptor64{d};
    }

private:

    segment_descriptor_64_t m_descriptor = {};
};


} // namespace System::HW::CPU::x86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_DESCRIPTOR_H */
