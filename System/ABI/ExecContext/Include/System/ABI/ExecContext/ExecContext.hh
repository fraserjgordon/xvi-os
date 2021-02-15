#ifndef __SYSTEM_ABI_EXECCONTEXT_EXECCONTEXT_H
#define __SYSTEM_ABI_EXECCONTEXT_EXECCONTEXT_H


#if defined(__arm__) || defined(__aarch64__)
#  include <System/ABI/ExecContext/Arch/ARM/Frame.hh>

namespace System::ABI::ExecContext
{

using frame_t = arm_frame_t;
using full_frame_t = arm_full_frame_t;

}
#elif defined(__mips__)
#  include <System/ABI/ExecContext/Arch/MIPS/ABI.hh>

namespace System::ABI::ExecContext
{

using frame_t = mips_frame_t;
using full_frame_t = mips_full_frame_t;

}
#elif defined(_ARCH_PPC)
#  include <System/ABI/ExecContext/Arch/PowerPC/Frame.hh>

namespace System::ABI::ExecContext
{

using frame_t = powerpc_frame_t;
using full_frame_t = powerpc_full_frame_t;

}
#elif defined(__sparc__)
#  include <System/ABI/ExecContext/Arch/Sparc/Frame.hh>

namespace System::ABI::ExecContext
{

using frame_t = sparc_frame_t;
using full_frame_t = sparc_full_frame_t;

}
#elif defined(__x86_64__)
// Must come before the __i386__ test.
#  include <System/ABI/ExecContext/Arch/x86/SysV_x64.hh>

namespace System::ABI::ExecContext
{

using frame_t = sysv_x64_frame_t;
using full_frame_t = sysv_x64_integer_t;

}
#elif defined(__i386__) 
#  include <System/ABI/ExecContext/Arch/x86/SysV_x86.hh>

namespace System::ABI::ExecContext
{

using frame_t = sysv_x86_frame_t;
using full_frame_t = sysv_x86_integer_t;

}
#else
#  error "Unknown architecture"
#endif


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_EXECCONTEXT_H */
