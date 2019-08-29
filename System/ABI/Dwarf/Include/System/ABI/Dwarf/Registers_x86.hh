#pragma once
#ifndef __SYSTEM_ABI_DWARF_REGISTERS_X86_H
#define __SYSTEM_ABI_DWARF_REGISTERS_X86_H


#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::Dwarf
{


enum class reg_x86 : std::uint8_t
{
    eax             = 0,
    ecx             = 1,
    edx             = 2,
    ebx             = 3,
    esp             = 4,
    ebp             = 5,
    esi             = 6,
    edi             = 7,
    return_address  = 8,
    eflags          = 9,
    st0             = 11,
    st1             = 12,
    st2             = 13,
    st3             = 14,
    st4             = 15,
    st5             = 16,
    st6             = 17,
    st7             = 18,
    xmm0            = 21,
    xmm1            = 22,
    xmm2            = 23,
    xmm3            = 24,
    xmm4            = 25,
    xmm5            = 26,
    xmm6            = 27,
    xmm7            = 28,
    mm0             = 29,
    mm1             = 30,
    mm2             = 31,
    mm3             = 32,
    mm4             = 33,
    mm5             = 34,
    mm6             = 35,
    mm7             = 36,
    mxcsr           = 39,
    es              = 40,
    cs              = 41,
    ss              = 42,
    ds              = 43,
    fs              = 44,
    gs              = 45,
    tr              = 48,
    ldtr            = 49,
};


} // namespace System::ABI::Dwarf


#endif /* ifndef __SYSTEM_ABI_DWARF_REGISTERS_X86_H */
