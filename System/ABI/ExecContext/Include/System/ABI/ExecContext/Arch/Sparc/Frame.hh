#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_SPARC_FRAME_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_SPARC_FRAME_H


#include <System/ABI/ExecContext/Private/Config.hh>

#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Pair.hh>


namespace System::ABI::ExecContext
{


struct sparc32_frame_t
{
    std::uint32_t   sp;     // Stored in %o6.
    std::uint32_t   pc;     // Calculated from the return address in %o7.

    std::uint32_t   l[8];
    std::uint32_t   i[8];
};

struct sparc32_full_frame_t
{
    // Note: in the v8+ ABI, the %gX and %oX are used as 64-bit registers by the compiler. Because these register sets
    // are volatile, they're not preserved across function calls. As supporting them as possibly-64-bit registers in
    // this frame structure would complicate things, the volatile nature of these registers is a convenient excuse to
    // only support the lower 32 bits of the registers when in 32-bit mode.
    std::uint32_t   g[8];
    std::uint32_t   i[8];
    std::uint32_t   l[8];
    std::uint32_t   o[8];

    // For a full frame, we store %pc explicitly
    std::uint32_t   pc;
};

struct sparc64_frame_t
{
    std::uint64_t   sp;     // Stored in %o6.
    std::uint64_t   pc;     // Calculated from the return address in %o7.    

    std::uint64_t   l[8];
    std::uint64_t   i[8];
};

struct sparc64_full_frame_t
{
    std::uint64_t   g[8];
    std::uint64_t   i[8];
    std::uint64_t   l[8];
    std::uint64_t   o[8];

    // For a full frame, we store %pc explicitly.
    std::uint64_t   pc;
};


#if defined(__sparc__)
#  if defined(__arch64__)
using sparc_frame_t = sparc64_frame_t;
using sparc_full_frame_t = sparc64_full_frame_t;
#  else
using sparc_frame_t = sparc32_frame_t;
using sparc_full_frame_t = sparc32_full_frame_t;
#  endif
#endif


#if defined(__sparc__)
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
std::pair<sparc_frame_t*, std::uintptr_t>
SwapContexts(sparc_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(SwapContexts);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
void ResumeContext(sparc_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContext);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
void ResumeContextFull(sparc_full_frame_t* next)
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
std::pair<sparc_frame_t*, std::uintptr_t>
CaptureContext(sparc_frame_t* ctxt, std::uintptr_t param_first)
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
using create_context_fn_t = void (*)(sparc_frame_t* prev_frame, std::uintptr_t call_param, std::uintptr_t bound_param);

//! @brief Creates a context by wrapping a call to a function.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
sparc_frame_t*
CreateContext(void* stack, std::size_t stack_size, create_context_fn_t, std::uintptr_t bound_param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContext);

//! @brief Creates a context by wrapping a call to a function using some copied data.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
sparc_frame_t*
CreateContextWithData(void* stack, std::size_t stack_size, create_context_fn_t fn, const void* data, std::size_t data_size)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContextWithData);

#endif


} // namespace System::ABI::ExecContext


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_SPARC_FRAME_H */
