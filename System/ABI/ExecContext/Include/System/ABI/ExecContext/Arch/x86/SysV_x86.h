#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X86_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X86_H


#include <System/ABI/ExecContext/Private/Config.h>


#if defined(__i386__) && !defined(__x86_64__)
#  define __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS     6
#endif


// Note: included from assembly files.
#ifdef __cplusplus
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Pair.hh>

#include <System/ABI/ExecContext/Arch/x86/SSE.hh>


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
    public sysv_x86_frame_t,
    public sysv_x86_volatile_t
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
//! @returns                the context that switched to this context and an uninterpreted value from that context
//!
//! @todo: this needs a wrapper like CreateContext below.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::returns_twice]]
[[gnu::sysv_abi]]
std::pair<sysv_x86_frame_t*, std::uintptr_t>
SwapContexts(sysv_x86_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(SwapContexts);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
[[gnu::sysv_abi]]
void ResumeContext(sysv_x86_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContext);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
[[gnu::sysv_abi]]
void ResumeContextFull(sysv_x86_integer_t* next)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContextFull);

__SYSTEM_ABI_EXECCONTEXT_EXPORT
//[[gnu::returns_twice]]
[[gnu::sysv_abi]]
std::uint64_t
CaptureContextImpl(sysv_x86_frame_t* ctxt, std::uintptr_t param_first) 
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CaptureContext);

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
[[gnu::always_inline]]
[[gnu::returns_twice]]
inline std::pair<sysv_x86_frame_t*, std::uintptr_t>
CaptureContext(sysv_x86_frame_t* ctxt, std::uintptr_t param_first)
{
    // We need to wrap the real method as it returns the two values in %eax:%edx, which isn't compliant with the SysV
    // ABI. 64-bit integers, however, do get returned in that register pair...
    auto result = CaptureContextImpl(ctxt, param_first);
    return { reinterpret_cast<sysv_x86_frame_t*>(result & 0xFFFFFFFF), static_cast<std::uintptr_t>(result >> 32) };
}


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
using create_context_fn_t = void (*)(sysv_x86_frame_t* prev_frame, std::uintptr_t call_param, std::uintptr_t bound_param);

//! @brief Creates a context by wrapping a call to a function.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::sysv_abi]]
sysv_x86_frame_t*
CreateContext(void* stack, std::size_t stack_size, create_context_fn_t, std::uintptr_t bound_param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContext);

//! @brief Creates a context by wrapping a call to a function using some copied data.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::sysv_abi]]
sysv_x86_frame_t*
CreateContextWithData(void* stack, std::size_t stack_size, create_context_fn_t fn, const void* data, std::size_t data_size)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContextWithData);

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

#define __SYSV_X86_OFFSET_EDX               32
#define __SYSV_X86_OFFSET_ECX               28
#define __SYSV_X86_OFFSET_EAX               24


#ifdef __cplusplus
namespace System::ABI::ExecContext
{
// Validate the structure sizes match the #defines.
static_assert(sizeof(sysv_x86_frame_t) == __SYSV_X86_FRAME_SIZE);
static_assert(sizeof(sysv_x86_integer_t) == __SYSV_X86_INTEGER_SIZE);

// Validate the structure offsets match the #defines.
}
#endif // ifdef __cplusplus



#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X86_H */
