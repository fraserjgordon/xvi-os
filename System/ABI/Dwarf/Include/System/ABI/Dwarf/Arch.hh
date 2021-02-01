#pragma once
#ifndef __SYSTEM_ABI_DWARF_ARCH_H
#define __SYSTEM_ABI_DWARF_ARCH_H


#if defined(__x86_64__)
#  include <System/ABI/Dwarf/RegistersX86_64.hh>
namespace System::ABI::Dwarf { using FrameTraitsNative = FrameTraitsX86_64; }
#elif defined(__i386__)
#  include <System/ABI/Dwarf/Registers_x86.hh>
namespace System::ABI::Dwarf { using FrameTraitsNative = FrameTraitsX86; }
#elif defined(__arm__)
#  include <System/ABI/Dwarf/RegistersARM.hh>
#elif defined(__mips__)
#  include <System/ABI/Dwarf/RegistersMIPS.hh>
#elif defined(_ARCH_PPC)
#  include <System/ABI/Dwarf/RegistersPPC.hh>
#elif defined(__sparc__)
#  include <System/ABI/Dwarf/RegistersSparc.hh>
#else
#  error Unsupported architecture
#endif


#endif /* ifndef __SYSTEM_ABI_DWARF_ARCH_H */
