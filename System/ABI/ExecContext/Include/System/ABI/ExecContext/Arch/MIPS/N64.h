#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_N64_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_N64_H


#if defined(_ABI64) || defined(__mips_n64)
#  define __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS 12
#endif


#ifdef __cplusplus
#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::ExecContext
{


// Callee-saved registers.
struct mips_n64_frame_t
{
    std::uint64_t   s[9];
    std::uint64_t   gp;
    std::uint64_t   sp;

    std::uint64_t   pc;
};

struct mips_n64_full_frame_t
{
    // Almost, but not quite, in register order.
    std::uint64_t   at;
    std::uint64_t   v[2];
    std::uint64_t   a[8];
    std::uint64_t   t[6];
    std::uint64_t   s[9];
    std::uint64_t   k[2];
    std::uint64_t   gp;
    std::uint64_t   sp;

    std::uint64_t   pc;
};


#if defined(_ABI64) || defined(__mips_n64)
using mips_frame_t = mips_n64_frame_t;
using mips_full_frame_t = mips_n64_full_frame_t;
#endif


} // namespace System::ABI::ExecContext
#endif // ifdef __cplusplus


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_N64_H */
