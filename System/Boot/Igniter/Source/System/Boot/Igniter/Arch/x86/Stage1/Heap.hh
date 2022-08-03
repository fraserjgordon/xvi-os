#ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_HEAP_H
#define __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_HEAP_H


#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::Boot::Igniter
{


void createHeap(std::uint32_t address, std::uint32_t size);


} // namespace System::Boot::Igniter


#endif /* ifndef __SYSTEM_BOOT_IGNITER_ARCH_X86_STAGE1_HEAP_H */
