#pragma once
#ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_TASK_H
#define __SYSTEM_HW_CPU_ARCH_X86_VM86_TASK_H


#include <System/C++/Utility/Bitset.hh>
#include <System/C++/Utility/UniquePtr.hh>


namespace System::HW::CPU::X86
{


class VM86Task
{
public:

    // Bitmap type for recording I/O port accesses. Bits corresponding to accessed I/O ports are set to 1, all other
    // bits are left as zero.
    using io_bitmap_t       = std::bitset<65536>;

private:

    bool m_allowUnrestrictedIO = false;     // If true, the task can access all I/O ports unrestricted.
    bool m_allowInterruptControl = false;   // If true, the task can directly control the CPU's interrupt flag.
    bool m_traceIO = false;                 // If true, a bitmap of I/O port accesses is recorded.

    // I/O tracing bitmap. This will be null if tracing is not enabled.
    std::unique_ptr<io_bitmap_t> m_ioTraceMap;
};


} // namespace System::HW::CPU::X86


#endif /* ifndef __SYSTEM_HW_CPU_ARCH_X86_VM86_TASK_H */
