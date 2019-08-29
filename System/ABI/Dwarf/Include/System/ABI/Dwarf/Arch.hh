#pragma once
#ifndef __SYSTEM_ABI_DWARF_ARCH_H
#define __SYSTEM_ABI_DWARF_ARCH_H


#ifdef __amd64__
#  include <System/ABI/Dwarf/RegistersX86_64.hh>
namespace System::ABI::Dwarf { using FrameTraitsNative = FrameTraitsX86_64; }
#else
#  error Unsupported architecture
#endif


#endif /* ifndef __SYSTEM_ABI_DWARF_ARCH_H */
