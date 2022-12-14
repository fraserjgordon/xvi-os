#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_O32_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_O32_H


#if defined(_ABIO32) || defined(__mips_o32)
#  define __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS 12
#endif


#ifdef __cplusplus
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::ExecContext
{


// Callee-saved registers.
struct mips_o32_frame_t
{
    // Almost, but not quite, in register order.
    std::uint32_t   s[9];
    std::uint32_t   gp;
    std::uint32_t   sp;

    std::uint32_t   pc;
};

struct mips_o32_full_frame_t
{
    // Almost, but not quite, in register order.
    std::uint32_t   at;
    std::uint32_t   v[2];
    std::uint32_t   a[4];
    std::uint32_t   t[10];
    std::uint32_t   s[9];
    std::uint32_t   k[2];
    std::uint32_t   gp;
    std::uint32_t   sp;
    std::uint32_t   ra;

    std::uint32_t   pc;
};


#if defined(_ABIO32) || defined(__mips_o32)
using mips_frame_t = mips_o32_frame_t;
using mips_full_frame_t = mips_o32_full_frame_t;
#endif


} // namespace System::ABI::ExecContext
#endif // ifdef __cplusplus


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_O32_H */
