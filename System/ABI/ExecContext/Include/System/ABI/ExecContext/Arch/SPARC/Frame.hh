#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_SPARC_FRAME_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_SPARC_FRAME_H


#include <System/C++/LanguageSupport/StdInt.hh>


namespace System::ABI::ExecContext
{


struct sparc32_frame_t
{
    std::uint32_t i[8];
    std::uint32_t l[8];

    std::uint32_t pc;
};

struct sparc32_full_frame_t
{
    std::uint32_t   g[8];
    std::uint32_t   i[8];
    std::uint32_t   l[8];
    std::uint32_t   o[8];

    std::uint32_t   pc;
};

struct sparc64_frame_t
{
    std::uint64_t   i[8];
    std::uint64_t   l[8];

    std::uint64_t   pc;
};

struct sparc64_full_frame_t
{
    std::uint64_t   g[8];
    std::uint64_t   i[8];
    std::uint64_t   l[8];
    std::uint64_t   o[8];

    std::uint64_t   pc;
};


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_SPARC_FRAME_H */
