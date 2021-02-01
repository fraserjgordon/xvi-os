#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_EABI32_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_EABI32_H


#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::ExecContext
{


// Callee-saved registers.
struct mips_eabi32_frame_t
{
    std::uint32_t   s[8];
    std::uint32_t   gp;
    std::uint32_t   sp;
    std::uint32_t   fp;

    std::uint32_t   pc;
};

struct mips_eabi32_full_frame_t
{
    // Almost, but not quite, in register order.
    std::uint32_t   at;
    std::uint32_t   v[2];
    std::uint32_t   a[8];
    std::uint32_t   t[6];
    std::uint32_t   s[8];
    std::uint32_t   k[2];
    std::uint32_t   gp;
    std::uint32_t   sp;
    std::uint32_t   fp;
    std::uint32_t   ra;

    std::uint32_t   pc;
};


#if defined(__mips_eabi) && !defined(__mips64)
using mips_frame_t = mips_eabi32_frame_t;
using mips_full_frame_t = mips_eabi32_full_frame_t;
#endif


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_EABI32_H */
