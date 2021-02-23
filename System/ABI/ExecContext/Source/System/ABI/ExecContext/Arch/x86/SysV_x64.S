#include <System/ABI/ExecContext/Arch/x86/SysV_x64.h>

#include <System/ABI/ExecContext/Private/Config.h>


#if defined(__x86_64__)

__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:

    // Parameters:
    //  sysv_x64_frame_t*   next        %rdi
    //  std::uint64_t       param       %rsi
    //
    // Expected return values:
    //  sysv_x64_frame_t*   prev        %rax
    //  std::uint64_t       param       %rdx
    //
    
    .cfi_startproc
    .cfi_offset %rip, -8

    // Cache the stack pointer as it would have been at function entry.
    leaq    8(%rsp), %rcx
    
    // Push the registers that need to be preserved across function calls.
    subq    $__SYSV_X64_FRAME_SIZE-8, %rsp      // %rip already pushed as a result of the call
    .cfi_adjust_cfa_offset (__SYSV_X64_FRAME_SIZE-8)
    movq    %r12, __SYSV_X64_OFFSET_R12(%rsp)
    movq    %r13, __SYSV_X64_OFFSET_R13(%rsp)
    movq    %r14, __SYSV_X64_OFFSET_R14(%rsp)
    movq    %r15, __SYSV_X64_OFFSET_R15(%rsp)
    movq    %rbx, __SYSV_X64_OFFSET_RBX(%rsp)
    movq    %rbp, __SYSV_X64_OFFSET_RBP(%rsp)
    movq    %rcx, __SYSV_X64_OFFSET_RSP(%rsp)

    .cfi_rel_offset %r12, __SYSV_X64_OFFSET_R12
    .cfi_rel_offset %r13, __SYSV_X64_OFFSET_R13
    .cfi_rel_offset %r14, __SYSV_X64_OFFSET_R14
    .cfi_rel_offset %r15, __SYSV_X64_OFFSET_R15
    .cfi_rel_offset %rbx, __SYSV_X64_OFFSET_RBX
    .cfi_rel_offset %rbp, __SYSV_X64_OFFSET_RBP

    // Restore the registers from the new context.
    movq    __SYSV_X64_OFFSET_R12(%rdi), %r12
    movq    __SYSV_X64_OFFSET_R13(%rdi), %r13
    movq    __SYSV_X64_OFFSET_R14(%rdi), %r14
    movq    __SYSV_X64_OFFSET_R15(%rdi), %r15
    movq    __SYSV_X64_OFFSET_RBX(%rdi), %rbx
    movq    __SYSV_X64_OFFSET_RBP(%rdi), %rbp
    movq    __SYSV_X64_OFFSET_RSP(%rdi), %rax
    movq    __SYSV_X64_OFFSET_RIP(%rdi), %rcx

    // Switch stacks to the new context. This conveniently puts the pointer to the previous context into where it needs
    // to be when we return.
    xchgq   %rax, %rsp

    // Note: now that we've switched stacks, any unwinding will happen on the new stack.
    .cfi_def_cfa %rsp, 8
    .cfi_same_value %r12
    .cfi_same_value %r13
    .cfi_same_value %r14
    .cfi_same_value %r15
    .cfi_same_value %rbx
    .cfi_same_value %rbp
    .cfi_register %rip, %rcx

    // Also grab the parameter that is to be passed to the new context. It forms the other half of the return value
    // from this function.
    movq    %rsi, %rdx

    // Return to the caller in the context that we just switched to. An indirect jump (rather than a return) is required
    // as there is no guarantee that the return address is on the stack.
    jmpq    *%rcx

    .cfi_endproc

.LSwapContexts__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:

    // Parameters:
    //  sysv_x64_frame_t*   next        %rdi
    //  std::uint64_t       param       %rsi
    //
    // Expected return values:
    //  sysv_x64_frame_t*   prev        %rax    note: always null
    //  std::uint64_t       param       %rdx
    //
    
    // Generate CFI instructions for this function as if we've already resumed the new context.
    .cfi_startproc simple
    .cfi_def_cfa %rdi, __SYSV_X64_FRAME_SIZE
    .cfi_rel_offset %r12, __SYSV_X64_OFFSET_R12
    .cfi_rel_offset %r13, __SYSV_X64_OFFSET_R13
    .cfi_rel_offset %r14, __SYSV_X64_OFFSET_R14
    .cfi_rel_offset %r15, __SYSV_X64_OFFSET_R15
    .cfi_rel_offset %rbx, __SYSV_X64_OFFSET_RBX
    .cfi_rel_offset %rbp, __SYSV_X64_OFFSET_RBP
    .cfi_rel_offset %rsp, __SYSV_X64_OFFSET_RSP
    .cfi_rel_offset %rip, __SYSV_X64_OFFSET_RIP

    // Restore the registers from the new context.
    movq    __SYSV_X64_OFFSET_R12(%rdi), %r12
    movq    __SYSV_X64_OFFSET_R13(%rdi), %r13
    movq    __SYSV_X64_OFFSET_R14(%rdi), %r14
    movq    __SYSV_X64_OFFSET_R15(%rdi), %r15
    movq    __SYSV_X64_OFFSET_RBX(%rdi), %rbx
    movq    __SYSV_X64_OFFSET_RBP(%rdi), %rbp
    movq    __SYSV_X64_OFFSET_RSP(%rdi), %rsp
    movq    __SYSV_X64_OFFSET_RIP(%rdi), %rcx

    // Stack has been switched so correct the CFI information.
    .cfi_def_cfa %rsp, 0
    .cfi_same_value %r12
    .cfi_same_value %r13
    .cfi_same_value %r14
    .cfi_same_value %r15
    .cfi_same_value %rbx
    .cfi_same_value %rbp
    .cfi_same_value %rsp
    .cfi_register %rip, %rcx

    // Clear the pointer return value to null and move the parameter to the second return value.
    xorq    %rax, %rax
    movq    %rsi, %rdx

    // Return to the caller in the context that we just switched to. An indirect jump (rather than a return) is required
    // as there is no guarantee that the return address is on the stack.
    jmpq    *%rcx

    .cfi_endproc

.LResumeContext__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:

    // Parameters:
    //  sysv_x64_frame_t*   next        %rdi
    //
    // Expected return values:
    //  <unspecified>       n/a         %rax    note: value taken from context
    //  <unspecified>       n/a         %rdx    note: value taken from context
    //

    // Restore most of the registers from the context. Not restored are:
    //  %rdi - needed to point to the context.
    //  %rip
    movq    __SYSV_X64_OFFSET_R8(%rdi), %r8
    movq    __SYSV_X64_OFFSET_R9(%rdi), %r9
    movq    __SYSV_X64_OFFSET_R10(%rdi), %r10
    movq    __SYSV_X64_OFFSET_R11(%rdi), %r11
    movq    __SYSV_X64_OFFSET_R12(%rdi), %r12
    movq    __SYSV_X64_OFFSET_R13(%rdi), %r13
    movq    __SYSV_X64_OFFSET_R14(%rdi), %r14
    movq    __SYSV_X64_OFFSET_R15(%rdi), %r15
    movq    __SYSV_X64_OFFSET_RAX(%rdi), %rax
    movq    __SYSV_X64_OFFSET_RBX(%rdi), %rbx
    movq    __SYSV_X64_OFFSET_RCX(%rdi), %rcx
    movq    __SYSV_X64_OFFSET_RDX(%rdi), %rdx
    movq    __SYSV_X64_OFFSET_RSI(%rdi), %rsi
    movq    __SYSV_X64_OFFSET_RBP(%rdi), %rbp
    movq    __SYSV_X64_OFFSET_RSP(%rdi), %rsp

    // Push the target %rip to the stack then restore %rdi
    pushq   __SYSV_X64_OFFSET_RIP(%rdi)
    movq    __SYSV_X64_OFFSET_RDI(%rdi), %rdi

    // Jump to the target.
    retq

.LResumeContextFull__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:

    // Parameters:
    //  sysv_x64_frame_t*   ctxt        %rdi
    //  std::uint64_t       param_first %rsi
    //
    // Expected return values:
    //  sysv_x64_frame_t*   prev        %rax    note: set to the input %rdi
    //  std::uint64_t       param       %rdx    note: set to the input %rsi
    //

    .cfi_startproc
    .cfi_offset %rip, -8

    // Calculate the value of %rsp after this function returns.
    leaq    8(%rsp), %rdx

    // Push the registers that need to be preserved across function calls.
    movq    %r12, __SYSV_X64_OFFSET_R12(%rdi)
    movq    %r13, __SYSV_X64_OFFSET_R13(%rdi)
    movq    %r14, __SYSV_X64_OFFSET_R14(%rdi)
    movq    %r15, __SYSV_X64_OFFSET_R15(%rdi)
    movq    %rbx, __SYSV_X64_OFFSET_RBX(%rdi)
    movq    %rbp, __SYSV_X64_OFFSET_RBP(%rdi)
    movq    %rdx, __SYSV_X64_OFFSET_RSP(%rdi)

    // Capture the return instruction pointer too.
    movq    0(%rsp), %rcx
    movq    %rcx, __SYSV_X64_OFFSET_RIP(%rdi)

    // Set the return values.
    movq    %rdi, %rax
    movq    %rsi, %rdx

    // And return.
    retq

    .cfi_endproc

.LCaptureContext__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CreateContext__v1_0,CreateContext,1.0)
.LCreateContext__v1_0:

    // Parameters:
    //  %rdi    void*               stack
    //  %rsi    std::size_t         stack_size
    //  %rdx    void (*)(...)       fn
    //  %rcx    void*               param
    //
    // Expected return values:
    //  %rax    sysv_x64_frame_t*   new_context

    .cfi_startproc
    .cfi_offset %rip, -8

    // Calculate the stack top address. This is minus the space required for the context.
    leaq    -__SYSV_X64_FRAME_SIZE(%rdi, %rsi), %rax

    // Push a return address so that a function that does return (violating the API contract) will fail noisily rather
    // than silently corrupting something.
    leaq    .LCreateContextFnReturned(%rip), %rsi
    movq    %rsi, -8(%rax)

    // Push the address of the target function and the parameter that it will take.
    movq    %rdx, -16(%rax)
    movq    %rcx, -24(%rax)

    // Calculate the stack pointer the new context will use.
    leaq    -24(%rax), %rsi

    // Set the parameters for a call to the wrapped function.
    //
    // The registers are set to the current values, in case they matter.
    movq    %r12, __SYSV_X64_OFFSET_R12(%rax)
    movq    %r13, __SYSV_X64_OFFSET_R13(%rax)
    movq    %r14, __SYSV_X64_OFFSET_R14(%rax)
    movq    %r15, __SYSV_X64_OFFSET_R15(%rax)
    movq    %rbx, __SYSV_X64_OFFSET_RBX(%rax)
    movq    $0,   __SYSV_X64_OFFSET_RBP(%rax)
    movq    %rsi, __SYSV_X64_OFFSET_RSP(%rax)

    // Finally, set the execution address to our trampoline code.
    leaq    .LCreateContextTrampoline(%rip), %rdi
    movq    %rdi, __SYSV_X64_OFFSET_RIP(%rax)

    // Context has been set up and the pointer to the context is already in the right place.
    retq

.LCreateContextFnReturned:

    // In violation of the API contract, the function around which the context was created returned. The API specifies
    // undefined behaviour so invoking an undefined opcode is well within rights.
    ud2
    jmp     .LCreateContextFnReturned

    .cfi_endproc

.LCreateContext__v1_0_end = .

// Note that this function does not have any CFI descriptors as it is not possible to unwind through it (as any return
// into it is explicitly disallowed as a violation of the API contract of CreateContext).
.type .LCreateContextTrampoline, STT_FUNC
.size .LCreateContextTrampoline, .LCreateContextTrampoline_end - .
.LCreateContextTrampoline:

    // Pop the bound parameter and the destination off the stack. The per-call parameters need to be moved from the
    // return value registers to the parameter registers (they were placed there when the ResumeContext or SwapContext
    // function that executed this context jumped here).
    movq    %rax, %rdi
    movq    %rdx, %rsi
    popq    %rdx
    popq    %rcx

    // Jump to the destination - there's already a return address on the stack.
    jmp     *%rcx

.LCreateContextTrampoline_end = .

#endif
