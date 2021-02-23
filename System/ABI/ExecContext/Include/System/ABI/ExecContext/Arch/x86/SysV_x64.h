#ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X64_H
#define __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X64_H


#include <System/ABI/ExecContext/Private/Config.h>


#if defined(__x86_64__)
#  define __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS     8
#endif


// Note: included from assembly files.
#ifdef __cplusplus
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/Utility/Pair.hh>

#include <System/ABI/ExecContext/Arch/x86/SSE.hh>


namespace System::ABI::ExecContext
{


//! @brief  Minimal stack frame that preserves all required registers.
//!
//! The callee-preserved registers plus the instruction pointer is enough to restore the execution context at a function
//! call boundary (as the compiler must assume non-preserved registers are clobbered).
struct sysv_x64_frame_t
{
    //! @name   Callee-preserved registers
    //! @brief  Callee-preserved registers under the SysV X86 calling convention.
    //!
    //! These registers are explicitly pushed to the stack by the callee (if the callee wants to use them -- it is free
    //! to leave them unmodified if it doesn't need the registers). As these are the registers that the caller expects
    //! to be preserved, they are the registers that need to be saved when performing a context switch (on a function
    //! call boundary -- an asynchronous context switch needs to preserve all register state).
    //!
    //! @{
    std::uint64_t   r12;    //!< General-purpose register.
    std::uint64_t   r13;    //!< General-purpose register.
    std::uint64_t   r14;    //!< General-purpose register.
    std::uint64_t   r15;    //!< General-purpose register.
    std::uint64_t   rbx;    //!< GOT base register (usually; may be re-purposed).
    std::uint64_t   rbp;    //!< Frame pointer register (usually; may be re-purposed).
    std::uint64_t   rsp;    //!< Stack pointer register (architectural).
    //! @}

    //! @brief Instruction pointer register.
    //!
    //! Unlike the other frame registers, this register isn't saved explicitly by the callee. Instead, it is implicitly
    //! pushed onto the stack when a call is performed.
    std::uint64_t   rip;
};

// Volatile (non-preserved) integer registers.
struct sysv_x64_volatile_t
{
    std::uint64_t   rax;    // Return register.
    std::uint64_t   rdx;    // Return register.
    std::uint64_t   rcx;    // Local register.
    std::uint64_t   rdi;    // Local register.
    std::uint64_t   rsi;    // Local register.
    std::uint64_t   r8;     // Local register.
    std::uint64_t   r9;     // Local register.
    std::uint64_t   r10;    // Local register;
    std::uint64_t   r11;    // Local register.
};

// All integer registers.
struct sysv_x64_integer_t :
    public sysv_x64_frame_t,
    public sysv_x64_volatile_t
{
};


#if defined(__x86_64__)
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
std::pair<sysv_x64_frame_t*, std::uintptr_t>
SwapContexts(sysv_x64_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(SwapContexts);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
[[gnu::sysv_abi]]
void ResumeContext(sysv_x64_frame_t* next, std::uintptr_t param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(ResumeContext);

//! @brief Resumes the given execution context and abandons the current context.
//!
//! @param[in]      next    the context to resume
//! @param[in]      param   an uninterpreted value to pass to the resumed context
//!
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[noreturn]]
[[gnu::sysv_abi]]
void ResumeContextFull(sysv_x64_integer_t* next)
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
std::pair<sysv_x64_frame_t*, std::uintptr_t>
CaptureContext(sysv_x64_frame_t* ctxt, std::uintptr_t param_first)
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
using create_context_fn_t = void (*)(sysv_x64_frame_t* prev_frame, std::uintptr_t call_param, std::uintptr_t bound_param);

//! @brief Creates a context by wrapping a call to a function.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::sysv_abi]]
sysv_x64_frame_t*
CreateContext(void* stack, std::size_t stack_size, create_context_fn_t, std::uintptr_t bound_param)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContext);

//! @brief Creates a context by wrapping a call to a function using some copied data.
__SYSTEM_ABI_EXECCONTEXT_EXPORT
[[gnu::sysv_abi]]
sysv_x64_frame_t*
CreateContextWithData(void* stack, std::size_t stack_size, create_context_fn_t fn, const void* data, std::size_t data_size)
    __SYSTEM_ABI_EXECCONTEXT_SYMBOL(CreateContextWithData);
#endif


}


#endif // ifdef __cplusplus


//! @name  Assembler convenience macros.
//!
//! These macros are provided as a convenience to the assembly-language file that implements (some of) the methods
//! defined in this header. They define the sizes of and field offsets within the structures that need to be accessed
//! from assembly language.
//!
//! @{
#define __SYSV_X64_FRAME_SIZE               64      //!< sizeof(sysv_x64_frame_t)
#define __SYSV_X64_INTEGER_SIZE             136     //!< sizeof(sysv_x64_integer_t)

#define __SYSV_X64_OFFSET_RIP               56      //!< offsetof(sysv_x64_frame_t, rip)
#define __SYSV_X64_OFFSET_RSP               48      //!< offsetof(sysv_x64_frame_t, rsp)
#define __SYSV_X64_OFFSET_RBP               40      //!< offsetof(sysv_x64_frame_t, rbp)
#define __SYSV_X64_OFFSET_RBX               32      //!< offsetof(sysv_x64_frame_t, rbx)
#define __SYSV_X64_OFFSET_R15               24      //!< offsetof(sysv_x64_frame_t, r15)
#define __SYSV_X64_OFFSET_R14               16      //!< offsetof(sysv_x64_frame_t, r14)
#define __SYSV_X64_OFFSET_R13               8       //!< offsetof(sysv_x64_frame_t, r13)
#define __SYSV_X64_OFFSET_R12               0       //!< offsetof(sysv_x64_frame_t, r12)

#define __SYSV_X64_OFFSET_R11               128     //!< offsetof(sysv_x64_integer_t, r11)
#define __SYSV_X64_OFFSET_R10               120     //!< offsetof(sysv_x64_integer_t, r10)
#define __SYSV_X64_OFFSET_R9                112     //!< offsetof(sysv_x64_integer_t, r9)
#define __SYSV_X64_OFFSET_R8                104     //!< offsetof(sysv_x64_integer_t, r8)
#define __SYSV_X64_OFFSET_RSI               96      //!< offsetof(sysv_x64_integer_t, rsi)
#define __SYSV_X64_OFFSET_RDI               88      //!< offsetof(sysv_x64_integer_t, rdi)
#define __SYSV_X64_OFFSET_RCX               80      //!< offsetof(sysv_x64_integer_t, rcx)
#define __SYSV_X64_OFFSET_RDX               72      //!< offsetof(sysv_x64_integer_t, rdx)
#define __SYSV_X64_OFFSET_RAX               64      //!< offsetof(sysv_x64_integer_t, rax)
//! @}


#ifdef __cplusplus
namespace System::ABI::ExecContext
{
// Validate the structure sizes match the #defines.
static_assert(sizeof(sysv_x64_frame_t) == __SYSV_X64_FRAME_SIZE);
static_assert(sizeof(sysv_x64_integer_t) == __SYSV_X64_INTEGER_SIZE);

// Validate the structure offsets match the #defines.
}
#endif // ifdef __cplusplus


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_ARCH_X86_SYSV_X64_H */
