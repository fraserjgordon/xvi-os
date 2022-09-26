#include <System/Kernel/Arch/x86/MMU/UserCopy.hh>

#include <cstring>

#include <System/Kernel/Runpatch/Arch/x86/Runpatch.hh>


namespace System::Kernel::X86::MMU
{


static inline void enableAccessToUserMemory()
{
    asm volatile
    (
        RUNPATCH_IF_ENABLED("x86.SMAP", "stac")
        ::: "memory"
    );
}

static inline void disableAccessToUserMemory()
{
    asm volatile
    (
        RUNPATCH_IF_ENABLED("x86.SMAP", "clac")
        ::: "memory"
    );
}



void copyFromUserUnchecked(std::uintptr_t address, std::size_t size, void* to)
{
    enableAccessToUserMemory();
    std::memcpy(to, reinterpret_cast<void*>(address), size);
    disableAccessToUserMemory();
}

void copyToUserUnchecked(const void* from, std::size_t size, std::uintptr_t address)
{
    enableAccessToUserMemory();
    std::memcpy(reinterpret_cast<void*>(address), from, size);
    disableAccessToUserMemory();
}


} // namespace System::Kernel::X86::MMU
