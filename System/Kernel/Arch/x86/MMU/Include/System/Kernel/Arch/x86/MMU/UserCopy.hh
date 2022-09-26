#ifndef __SYSTEML_KERNEL_ARCH_X86_MMU_USERCOPY_H
#define __SYSTEML_KERNEL_ARCH_X86_MMU_USERCOPY_H


#include <cstddef>
#include <cstdint>


namespace System::Kernel::X86::MMU
{


void copyFromUserUnchecked(std::uintptr_t address, std::size_t size, void* dest);

void copyToUserUnchecked(const void* from, std::size_t size, std::uintptr_t address);


} // namespace System::Kernel::X86::MMU


#endif /* ifndef __SYSTEML_KERNEL_ARCH_X86_MMU_USERCOPY_H */
