#include <System/HW/CPU/Arch/x86/Segmentation/DescriptorTable.hh>

#include <System/C++/Atomic/Atomic.hh>


namespace System::HW::CPU::X86
{


using namespace std;


void SegmentDescriptorTable::writeEntry(int index, const SegmentDescriptor& new_entry)
{
    // Ensure that the "present" flag of the old entry is cleared.
    auto& slot = m_table[index];
    slot.get().fields.p = 0;

    atomic_thread_fence(memory_order::acquire);

    // Write the new value but with the "present" flag also cleared.
    SegmentDescriptor new_not_present = new_entry;
    new_not_present.get().fields.p = 0;
    slot = new_not_present;

    atomic_thread_fence(memory_order::release);

    // Mark the new entry as "present" (if appropriate).
    slot.get().fields.p = new_entry.get().fields.p;
}

void SegmentDescriptorTable::writeEntry(int index, const SegmentDescriptor64& new_entry)
{
    // Ensure that the "present" flag of both of the old entries are cleared.
    auto& slot1 = m_table[index];
    auto& slot2 = m_table[index + 1];
    slot1.get().fields.p = 0;
    slot2.get().fields.p = 0;

    atomic_thread_fence(memory_order::acquire);

    // Write the new value but with the "present" flag also cleared.
    SegmentDescriptor64 new_not_present = new_entry;
    new_not_present.get().fields.p = 0;
    slot1.get().raw = new_not_present.get().raw[0];
    slot2.get().raw = new_not_present.get().raw[1];

    atomic_thread_fence(memory_order::release);

    // Mark the new entry as "present" (if appropriate).
    slot1.get().fields.p = new_entry.get().fields.p;
}


} // namespace System::HW::CPU::X86
