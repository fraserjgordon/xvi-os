#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_POWERPC_FRAME_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_POWERPC_FRAME_H


#include <System/ABI/ExecContext/Private/Config.hh>

#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Pair.hh>


namespace System::ABI::ExecContext
{


struct arm32_frame_t
{
    std::uint32_t   r4;
    std::uint32_t   r5;
    std::uint32_t   r6;
    std::uint32_t   r7;
    std::uint32_t   r8;
    std::uint32_t   r9;
    std::uint32_t   r10;
    std::uint32_t   r11;
    std::uint32_t   sp;     // r13
    std::uint32_t   lr;     // r14
    std::uint32_t   pc;     // r15 
};

struct arm32_full_frame_t
{
    std::uint32_t   r[16];
};

struct arm64_frame_t
{
    std::uint64_t   x19;
    std::uint64_t   x20;
    std::uint64_t   x21;
    std::uint64_t   x22;
    std::uint64_t   x23;
    std::uint64_t   x24;
    std::uint64_t   x25;
    std::uint64_t   x26;
    std::uint64_t   x27;
    std::uint64_t   x28;
    std::uint64_t   x29;
    std::uint64_t   pc;
};

struct arm64_full_frame_t
{
    std::uint64_t   x[32];
    std::uint64_t   pc;
};


#if defined(__arm__)
#  if defined(__aarch64__)
using arm_frame_t = arm64_frame_t;
using arm_full_frame_t = arm64_full_frame_t;
#  else
using arm_frame_t = arm32_frame_t;
using arm_full_frame_t = arm32_full_frame_t;
#  endif
#endif


#if defined(__arm__)
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
[[gnu::sysv_abi]]
std::pair<arm_frame_t*, std::uintptr_t>
SwapContexts(arm_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(SwapContexts);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
[[gnu::sysv_abi]]
void ResumeContext(arm_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContext);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
[[gnu::sysv_abi]]
void ResumeContextFull(arm_full_frame_t* next)
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
[[gnu::sysv_abi]]
std::pair<arm_frame_t*, std::uintptr_t>
CaptureContext(arm_frame_t* ctxt, std::uintptr_t param_first)
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
using create_context_fn_t = void (*)(arm_frame_t* prev_frame, std::uintptr_t call_param, std::uintptr_t bound_param);

//! @brief Creates a context by wrapping a call to a function.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::sysv_abi]]
arm_frame_t*
CreateContext(void* stack, std::size_t stack_size, create_context_fn_t, std::uintptr_t bound_param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContext);

//! @brief Creates a context by wrapping a call to a function using some copied data.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::sysv_abi]]
arm_frame_t*
CreateContextWithData(void* stack, std::size_t stack_size, create_context_fn_t fn, const void* data, std::size_t data_size)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContextWithData);


#endif /* ifdef __arm__ */


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_POWERPC_FRAME_H */
