#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_POWERPC_FRAME_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_POWERPC_FRAME_H


#include <System/ABI/ExecContext/Private/Config.hh>

#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Pair.hh>


namespace System::ABI::ExecContext
{


// Althougth the frames defined in this file may have the same register layout, they are semantically different. In
// order to prevent programming mistakes, they are defined as distinct types to prevent, for example, a frame being
// captured as one ABI and then being restored as another.


// 32-bit SysV ABI. Also used for the EABI but r2 is interpreted differently (SDA2 pointer rather than thread pointer).
//
// The layout of this structure is designed to comply with the SysV ABI stack frame layout requirements.
struct ppc32_sysv_frame_t
{
    // The stack pointer for the previous frame is stored in a backchain field at 0(%r1).
    std::uint32_t   r1;
    
    // Denoted as the link register save field in the ABI.
    std::uint32_t   pc;

    // Padding to meet ABI requirements.
    std::uint32_t   pad[1];
    
    // Only some fields of the condition register are non-volatile.
    std::uint32_t   cr;
    
    // Registers with specific ABI-defined uses.
    std::uint32_t   r2;     // Thread pointer / small data area 2 (SDA2) pointer.
    std::uint32_t   r13;    // Small data area (SDA) pointer.

    // Non-volatile integer registers.
    std::uint32_t   r14;
    std::uint32_t   r15;
    std::uint32_t   r16;
    std::uint32_t   r17;
    std::uint32_t   r18;
    std::uint32_t   r19;
    std::uint32_t   r20;
    std::uint32_t   r21;
    std::uint32_t   r22;
    std::uint32_t   r23;
    std::uint32_t   r24;
    std::uint32_t   r25;
    std::uint32_t   r26;
    std::uint32_t   r27;
    std::uint32_t   r28;
    std::uint32_t   r29;
    std::uint32_t   r30;    // May be used as a GOT pointer.
    std::uint32_t   r31;    // May be used as a frame pointer.
};

// SysV ABI stack frames must have quadword size.
static_assert(sizeof(ppc32_sysv_frame_t) % 16 == 0, "invalid SysV ABI frame size");


struct ppc32_sysv_full_frame_t
{
    // The stack pointer for the previous frame is stored in a backchain field at 0(%r1).
    std::uint32_t   r1;
    
    // Denoted as the link register save field in the ABI.
    std::uint32_t   pc;

    // Padding to meet ABI requirements.
    std::uint32_t   pad[2];
    
    // Only some fields of the condition register are non-volatile.
    std::uint32_t   cr;
    
    // Registers with specific ABI-defined uses.
    std::uint32_t   r2;     // Thread pointer / small data area 2 (SDA2) pointer.

    // Volatile registers.
    std::uint32_t   r0;
    std::uint32_t   r3;
    std::uint32_t   r4;
    std::uint32_t   r5;
    std::uint32_t   r6;
    std::uint32_t   r7;
    std::uint32_t   r8;
    std::uint32_t   r9;
    std::uint32_t   r10;
    std::uint32_t   r11;
    std::uint32_t   r12;

    // Registers with specific ABI-defined uses.
    std::uint32_t   r13;    // Small data area (SDA) pointer.

    // Non-volatile integer registers.
    std::uint32_t   r14;
    std::uint32_t   r15;
    std::uint32_t   r16;
    std::uint32_t   r17;
    std::uint32_t   r18;
    std::uint32_t   r19;
    std::uint32_t   r20;
    std::uint32_t   r21;
    std::uint32_t   r22;
    std::uint32_t   r23;
    std::uint32_t   r24;
    std::uint32_t   r25;
    std::uint32_t   r26;
    std::uint32_t   r27;
    std::uint32_t   r28;
    std::uint32_t   r29;
    std::uint32_t   r30;    // May be used as a GOT pointer.
    std::uint32_t   r31;    // May be used as a frame pointer.
};

// SysV ABI stack frames must have quadword size.
static_assert(sizeof(ppc32_sysv_full_frame_t) % 16 == 0, "invalid SysV ABI frame size");


// 64-bit ELFv1 ABI (also known as the AIX 64-bit ABI).
struct ppc64_elfv1_frame_t
{
    // Stack frame header.
    std::uint64_t   r1;             // Stack pointer;
    std::uint64_t   cr;
    std::uint64_t   pc;
    std::uint64_t   reserved[2];    // Reserved for compiler/linker use.
    std::uint64_t   r2;             // TOC pointer.
    
    // Padding.
    std::uint64_t   padding[1];

    // Registers with specific ABI-defined uses.
    std::uint64_t   r13;    // Thread-local storage pointer.

    // Non-volatile integer registers.
    std::uint64_t   r14;
    std::uint64_t   r15;
    std::uint64_t   r16;
    std::uint64_t   r17;
    std::uint64_t   r18;
    std::uint64_t   r19;
    std::uint64_t   r20;
    std::uint64_t   r21;
    std::uint64_t   r22;
    std::uint64_t   r23;
    std::uint64_t   r24;
    std::uint64_t   r25;
    std::uint64_t   r26;
    std::uint64_t   r27;
    std::uint64_t   r28;
    std::uint64_t   r29;
    std::uint64_t   r30;
    std::uint64_t   r31;    // May be used as a frame pointer.
};

static_assert(sizeof(ppc64_elfv1_frame_t) % 16 == 0, "invalid ELFv1 frame size");


struct ppc64_elfv1_full_frame_t
{
    // Stack frame header.
    std::uint64_t   r1;             // Stacj pointer;
    std::uint64_t   cr;
    std::uint64_t   pc;
    std::uint64_t   reserved[2];    // Reserved for compiler/linker use.
    std::uint64_t   r2;             // TOC pointer.
    
    // Volatile registers.
    std::uint64_t   r0;
    std::uint64_t   r3;
    std::uint64_t   r4;
    std::uint64_t   r5;
    std::uint64_t   r6;
    std::uint64_t   r7;
    std::uint64_t   r8;
    std::uint64_t   r9;
    std::uint64_t   r10;
    std::uint64_t   r11;
    std::uint64_t   r12;

    // Registers with specific ABI-defined uses.
    std::uint64_t   r13;    // Thread-local storage pointer.

    // Non-volatile integer registers.
    std::uint64_t   r14;
    std::uint64_t   r15;
    std::uint64_t   r16;
    std::uint64_t   r17;
    std::uint64_t   r18;
    std::uint64_t   r19;
    std::uint64_t   r20;
    std::uint64_t   r21;
    std::uint64_t   r22;
    std::uint64_t   r23;
    std::uint64_t   r24;
    std::uint64_t   r25;
    std::uint64_t   r26;
    std::uint64_t   r27;
    std::uint64_t   r28;
    std::uint64_t   r29;
    std::uint64_t   r30;
    std::uint64_t   r31;    // May be used as a frame pointer.
};

static_assert(sizeof(ppc64_elfv1_frame_t) % 16 == 0, "invalid ELFv1 full frame size");


struct ppc64_elfv2_frame_t
{
    // Stack frame header.
    std::uint64_t   r1;             // Stack pointer;
    std::uint64_t   cr;
    std::uint64_t   pc;
    std::uint64_t   r2;             // TOC pointer.

    // Volatile registers.
    std::uint64_t   r0;
    std::uint64_t   r3;
    std::uint64_t   r4;
    std::uint64_t   r5;
    std::uint64_t   r6;
    std::uint64_t   r7;
    std::uint64_t   r8;
    std::uint64_t   r9;
    std::uint64_t   r10;
    std::uint64_t   r11;
    std::uint64_t   r12;

    // Registers with specific ABI-defined uses.
    std::uint64_t   r13;            // Thread-local storage pointer.

    // Non-volatile integer registers.
    std::uint64_t   r14;
    std::uint64_t   r15;
    std::uint64_t   r16;
    std::uint64_t   r17;
    std::uint64_t   r18;
    std::uint64_t   r19;
    std::uint64_t   r20;
    std::uint64_t   r21;
    std::uint64_t   r22;
    std::uint64_t   r23;
    std::uint64_t   r24;
    std::uint64_t   r25;
    std::uint64_t   r26;
    std::uint64_t   r27;
    std::uint64_t   r28;
    std::uint64_t   r29;
    std::uint64_t   r30;
    std::uint64_t   r31;    // May be used as a frame pointer.
};

static_assert(sizeof(ppc64_elfv2_frame_t) % 16 == 0, "invalid ELFv2 frame size");



struct ppc64_elfv2_full_frame_t
{
    // Stack frame header.
    std::uint64_t   r1;             // Stack pointer;
    std::uint64_t   cr;
    std::uint64_t   pc;
    std::uint64_t   r2;             // TOC pointer.

    // Padding.
    std::uint64_t   padding[1];

    // Registers with specific ABI-defined uses.
    std::uint64_t   r13;            // Thread-local storage pointer.

    // Non-volatile integer registers.
    std::uint64_t   r14;
    std::uint64_t   r15;
    std::uint64_t   r16;
    std::uint64_t   r17;
    std::uint64_t   r18;
    std::uint64_t   r19;
    std::uint64_t   r20;
    std::uint64_t   r21;
    std::uint64_t   r22;
    std::uint64_t   r23;
    std::uint64_t   r24;
    std::uint64_t   r25;
    std::uint64_t   r26;
    std::uint64_t   r27;
    std::uint64_t   r28;
    std::uint64_t   r29;
    std::uint64_t   r30;
    std::uint64_t   r31;    // May be used as a frame pointer.
};

static_assert(sizeof(ppc64_elfv2_full_frame_t) % 16 == 0, "invalid ELFv2 full_frame size");



#if defined(_ARCH_PPC)
#  if defined(_CALL_SYSV)
using powerpc_frame_t = ppc32_sysv_frame_t;
using powerpc_full_frame_t = ppc32_sysv_full_frame_t;
#  elif _CALL_ELF == 1
using powerpc_frame_t = ppc64_elfv1_frame_t;
using powerpc_full_frame_t = ppc64_elfv1_full_frame_t;
#  elif _CALL_ELF == 2
using powerpc_frame_t = ppc64_elfv2_frame_t;
using powerpc_full_frame_t = ppc64_elfv2_full_frame_t;
#  else
#    error unknown PowerPC ABI
#  endif
#endif


#if defined(_ARCH_PPC)
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
//! @returns                the context that switched to this context and an uninterpreted value from that context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::returns_twice]]
std::pair<powerpc_frame_t*, std::uintptr_t>
SwapContexts(powerpc_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(SwapContexts);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
void ResumeContext(powerpc_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContext);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
void ResumeContextFull(powerpc_full_frame_t* next)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContextFull);

//! @brief Captures the execution context without switching to another context.
//!
//! The first time this function returns, it will return the input context pointer and the given parameter. On
//! subsequent returns it'll return the previous context (possibly null, if non-resumable) and whatever parameter value
//! was passed to the SwapContexts or ResumeContext call that performed the switch.
//!
//! @param[in]     ctxt         pointer to storage for the captured context.
//! @param[in]     param_first  an uninterpreted value to return the first time this function returns.
//!
//! @returns                    the context that switched to this context and an uninterpreted value from that context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::returns_twice]]
std::pair<powerpc_frame_t*, std::uintptr_t>
CaptureContext(powerpc_frame_t* ctxt, std::uintptr_t param_first)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CaptureContext);


//! @brief  Function signature suitable for use with @ref CreateContext() or @ref CreateContextWithData().
//!
//! @param[in]  prev_frame  A pointer to the context that was active before this function was called. May be null if the
//!                         previous context cannot be returned to.
//! @param[in]  call_param  A single pointer-sized parameter passed to the context when it is executed. The value of
//!                         this parameter may be different each time the context is executed.
//! @param[in]  bound_param A single pointer-sized parameter that was specified at the time the context was created.
//!                         parameter will have the same value each time the context is executed.
//!
//! @returns                Nothing. It is a fatal error for this function to return normally (i.e. it must always
//!                         terminate by calling @ref ResumeContext or equivalent).
//!
//! @warning    If this function returns, behaviour of the program is undefined. It is likely -- but not guaranteed --
//!             that returning will cause program termination. This applies regardless of how this function returns
//!             (e.g. by returning normally, throwing an exception, unwinding the stack in some other way, etc).
//!
using create_context_fn_t = void (*)(powerpc_frame_t* prev_frame, std::uintptr_t call_param, std::uintptr_t bound_param);

//! @brief Creates a context by wrapping a call to a function.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
powerpc_frame_t*
CreateContext(void* stack, std::size_t stack_size, create_context_fn_t, std::uintptr_t bound_param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContext);

//! @brief Creates a context by wrapping a call to a function using some copied data.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
powerpc_frame_t*
CreateContextWithData(void* stack, std::size_t stack_size, create_context_fn_t fn, const void* data, std::size_t data_size)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContextWithData);


#endif /* ifdef _ARCH_PPC */


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_POWERPC_FRAME_H */
