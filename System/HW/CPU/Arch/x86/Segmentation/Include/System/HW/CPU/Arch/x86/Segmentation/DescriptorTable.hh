#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_DESCRIPTORTABLE_H
#define __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_DESCRIPTORTABLE_H


#include <cstddef>
#include <span>

#include <System/HW/CPU/Arch/x86/Segmentation/Descriptor.hh>
#include <System/HW/CPU/Arch/x86/Segmentation/Selector.hh>


namespace System::HW::CPU::X86
{


class SegmentDescriptorTable
{
public:

    // Returns a reference to the entry at the given index. The entry is read-only as special handling is needed when
    // updating entries in order to avoid a segment descriptor being accessed by the CPU while partially-updated.
    constexpr const SegmentDescriptor& getEntry(int index) const
    {
        return m_table[index];
    }

    // Returns the number of entries in the table.
    constexpr std::size_t getSize() const noexcept
    {
        return m_count;
    }

    // Updates a table entry in a way that prevents the CPU from trying to use a partially-updated entry.
    void writeEntry(int index, const SegmentDescriptor& entry);

    // Updates a table entry in a way that prevents the CPU from trying to use a partially-updated entry.
    //
    // Note: this variant updates the entries at both index and index+1.
    void writeEntry(int index, const SegmentDescriptor64& entry);

protected:

    // Default constucts to an empty table.
    constexpr SegmentDescriptorTable() = default;

    // Takes ownership of the given table.
    constexpr SegmentDescriptorTable(SegmentDescriptorTable&&) = default;

    // Non-copyable.
    SegmentDescriptorTable(const SegmentDescriptorTable&) = delete;

    // Takes ownership of the given table.
    constexpr SegmentDescriptorTable& operator=(SegmentDescriptorTable&&) = default;

    // Non-copyable.
    SegmentDescriptorTable& operator=(const SegmentDescriptorTable&) = delete;

    // Creates a table using existing (statically-allocated) memory.
    template <std::size_t N>
    constexpr explicit SegmentDescriptorTable(std::span<SegmentDescriptor, N> existing)
        : m_table(&existing.front()),
          m_count(N == std::dynamic_extent ? existing.size() : N)
    {
    }

    ~SegmentDescriptorTable() = default;

    // Resizes the table to the given number of entries.
    void resize(std::size_t entries);

    // Frees the resources allocated for the table.
    void clear() noexcept;

    std::uintptr_t getBaseAddress() const
    {
        return reinterpret_cast<std::uintptr_t>(m_table);
    }

    constexpr std::uint16_t getLimit() const
    {
        return static_cast<std::uint16_t>((m_count * sizeof(SegmentDescriptor)) - 1);
    }

    constexpr SegmentDescriptor* getTable()
    {
        return m_table;
    }


    // Generates the appropriate segment selector for the given index. The index isn't checked for validity; the
    // processor will do that when the segment is loaded.
    static constexpr SegmentSelector getSelector(int index, int rpl, bool ldt) noexcept
    {
        auto value = (index << SegmentSelector::INDEX_SHIFT) | (rpl & SegmentSelector::RPL_MASK);
        
        if (ldt)
            value |= SegmentSelector::TI;

        return SegmentSelector{static_cast<std::uint16_t>(value)};
    }

private:

    // Pointer to and number of entries in the descriptor table.
    SegmentDescriptor*  m_table = nullptr;
    std::uint16_t       m_count = 0;
};



} // namespace System::HW:CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_SEGMENTATION_DESCRIPTORTABLE_H */
