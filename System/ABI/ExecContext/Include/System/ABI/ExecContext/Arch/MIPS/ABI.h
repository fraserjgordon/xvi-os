#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_ABI_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_ABI_H


#include <System/ABI/ExecContext/Private/Config.h>


#if !defined(__mips__)
#  error cannot detect MIPS ABI for non-MIPS builds
#endif


#if defined(_ABIO32) || defined(__mips_o32)
#  include <System/ABI/ExecContext/Arch/MIPS/O32.h>
#elif defined(_ABIO64) || defined(__mips_o64)
#  include <System/ABI/ExecContext/Arch/MIPS/O64.h>
#elif defined(_ABIN32) || defined(__mips_n32)
#  include <System/ABI/ExecContext/Arch/MIPS/N32.h>
#elif defined(_ABIN64) || defined(__mips_n64)
#  include <System/ABI/ExecContext/Arch/MIPS/N64.h>
#elif defined(__mips_eabi)
#  if defined (__mips64)
#    include <System/ABI/ExecContext/Arch/MIPS/EABI64.h>
#  else
#    include <System/ABI/ExecContext/Arch/MIPS/EABI32.h>
#  endif
#else
#  error "Unknown MIPS ABI"
#endif


#ifdef __cplusplus
#  if defined(_ABIN32) || defined(__mips_n32)
static_assert(sizeof(System::ABI::ExecContext::mips_frame_t) == sizeof(std::uint64_t) * __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS);
#  else
static_assert(sizeof(System::ABI::ExecContext::mips_frame_t) == sizeof(std::uintptr_t) * __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS);
#  endif
#endif


#ifdef __cplusplus
#include <System/C++/Utility/Pair.hh>


namespace System::ABI::ExecContext
{

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
std::pair<mips_frame_t*, std::uintptr_t>
SwapContexts(mips_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(SwapContexts);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
void ResumeContext(mips_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContext);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
void ResumeContextFull(mips_full_frame_t* next)
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
std::pair<mips_frame_t*, std::uintptr_t>
CaptureContext(mips_frame_t* ctxt, std::uintptr_t param_first)
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
using create_context_fn_t = void (*)(mips_frame_t* prev_frame, std::uintptr_t call_param, std::uintptr_t bound_param);

//! @brief Creates a context by wrapping a call to a function.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
mips_frame_t*
CreateContext(void* stack, std::size_t stack_size, create_context_fn_t, std::uintptr_t bound_param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContext);

//! @brief Creates a context by wrapping a call to a function using some copied data.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
mips_frame_t*
CreateContextWithData(void* stack, std::size_t stack_size, create_context_fn_t fn, const void* data, std::size_t data_size)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContextWithData);


} // namespace System::ABI::ExecContext
#endif // ifdef __cplusplus


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_MIPS_ABI_H */
