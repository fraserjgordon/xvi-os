#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_O64_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_O64_H


#if defined(_ABIO64)
#  define __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS 12
#endif


#ifdef __cplusplus
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::ExecContext
{


// Callee-saved registers.
struct mips_o64_frame_t
{
    // Almost, but not quite, in register order.
    std::uint64_t   s[9];
    std::uint64_t   sp;
    std::uint64_t   fp;

    std::uint64_t   pc;
};

struct mips_o64_full_frame_t
{
    // Almost, but not quite, in register order.
    std::uint64_t   at;
    std::uint64_t   v[2];
    std::uint64_t   a[4];
    std::uint64_t   t[10];
    std::uint64_t   s[9];
    std::uint64_t   k[2];
    std::uint64_t   gp;
    std::uint64_t   sp;
    std::uint64_t   ra;

    std::uint64_t   pc;
};


#if defined(_ABIO64)
using mips_frame_t = mips_o64_frame_t;
using mips_full_frame_t = mips_o64_full_frame_t;
#endif


} // namespace System::ABI::ExecContext
#endif // ifdef __cplusplus


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_O64_H */
