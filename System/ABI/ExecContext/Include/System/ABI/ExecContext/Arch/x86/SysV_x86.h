//! @file       System/ABI/ExecContext/Arch/x86/SysV_x64.h
//! @brief      Support for the SysV ABI for x86/x64 


#pragma once
#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X86_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X86_H


#ifdef __cplusplus


#include <cstdint>
#include <utility>

#include <Arch/x86/SSE.h>


namespace System::ABI::ExecContext
{


// The callee-preserved registers plus the instruction pointer is enough to restore the execution context at a function
// call boundary (as the compiler must assume non-preserved registers are clobbered).
struct sysv_x86_frame_t
{
    // Callee-preserved registers under the SysV X86 calling convention.
    std::uint32_t   edi;    // General-purpose.
    std::uint32_t   esi;    // General-purpose.
    std::uint32_t   ebx;    // GOT base (usually; may be re-purposed).
    std::uint32_t   ebp;    // Frame pointer (usually; may be re-purposed).
    std::uint32_t   esp;    // Stack pointer (architectural).

    // Instruction pointer.
    std::uint32_t   eip;
};

// Volatile (non-preserved) integer registers.
struct sysv_x86_volatile_t
{
    std::uint32_t   eax;    // Return register.
    std::uint32_t   ecx;    // Local register.
    std::uint32_t   edx;    // Local register.
};

// All integer registers.
struct sysv_x86_integer_t :
    public sysv_x86_volatile_t,
    public sysv_x86_frame_t
{
};


#if defined(__i386__)
//! @brief Swaps stacks and execution state with the given context.
//!
//! This function implements the key operation for stackful co-routines: it switches from one execution stack to
//! another. The old context is saved before the switch and a pointer to it is returned, allowing the original context
//! to be resumed.
//!
//! If a context is to be resumed multiple times, the frame *must* be copied to some other location as it is likely to
//! be overwritten after the first resumption of the context. In a C or C++ program, the context should be thought of as
//! being similar to a setjmp context -- returning from the function that contained the call to this function will lead
//! to undefined behaviour on subsequent resumptions of the context.
//!
//! @param[in]      next    the execution context to switch to
//! @param[in]      param   an uninterpreted parameter to pass to the new context
//!
//! @returns        param   an uninterpreted value passed from the previous context
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::returns_twice]]
std::pair<sysv_x86_frame_t*, std::uintptr_t>
SwapContexts(sysv_x86_frame_t* next, std::uintptr_t param) asm("$System$ABI$ExecContext$SwapContexts");

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
void ResumeContext(sysv_x86_frame_t* next, std::uintptr_t param) asm("$System$ABI$ExecContext$ResumeContext");

#endif


}


#endif // ifdef __cplusplus


#define __SYSV_X86_FRAME_SIZE               24
#define __SYSV_X86_INTEGER_SIZE             36

#define __SYSV_X86_OFFSET_EIP               20
#define __SYSV_X86_OFFSET_ESP               16
#define __SYSV_X86_OFFSET_EBP               12
#define __SYSV_X86_OFFSET_EBX               8
#define __SYSV_X86_OFFSET_ESI               4
#define __SYSV_X86_OFFSET_EDI               0


#ifdef __cplusplus
namespace System::ABI::ExecContext
{
// Validate the structure sizes match the #defines.
static_assert(sizeof(sysv_x86_frame_t) == __SYSV_X86_FRAME_SIZE);
static_assert(sizeof(sysv_x86_integer_t) == __SYSV_X86_INTEGER_SIZE);

// Validate the structure offsets match the #defines.
}
#endif



#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X86_H */
