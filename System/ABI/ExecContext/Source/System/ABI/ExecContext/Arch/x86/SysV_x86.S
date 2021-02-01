#include <System/ABI/ExecContext/Arch/x86/SysV_x86.hh>

#include <System/ABI/ExecContext/Private/Config.hh>


#if defined(__i386__)

__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:

    // Parameters:
    //  pair<...>*          return       4(%esp)
    //  sysv_x86_frame_t*   next         8(%esp)
    //  std::uint32_t       param       12(%esp)
    //
    // Expected return values:
    //  sysv_x86_frame_t*   prev        0(pair)
    //  std::uint32_t       param       4(pair)
    //
    
    .cfi_startproc
    .cfi_offset %esp, -4

    // Cache the stack pointer as it would have been at function entry.
    leal    4(%esp), %ecx
    
    // Load the parameters.
    movl    8(%esp), %eax
    movl    12(%esp), %edx

    // Push the registers that need to be preserved across function calls.
    subl    $__SYSV_X86_FRAME_SIZE-4, %esp      // %eip already pushed as a result of the call
    .cfi_adjust_cfa_offset (__SYSV_X86_FRAME_SIZE-4)
    movl    %ebx, __SYSV_X86_OFFSET_EBX(%esp)
    movl    %ebp, __SYSV_X86_OFFSET_EBP(%esp)
    movl    %esi, __SYSV_X86_OFFSET_ESI(%esp)
    movl    %edi, __SYSV_X86_OFFSET_EDI(%esp)
    movl    %ecx, __SYSV_X86_OFFSET_ESP(%esp)

    .cfi_rel_offset %ebx, __SYSV_X86_OFFSET_EBX
    .cfi_rel_offset %ebp, __SYSV_X86_OFFSET_EBP
    .cfi_rel_offset %esi, __SYSV_X86_OFFSET_ESI
    .cfi_rel_offset %edi, __SYSV_X86_OFFSET_EDI

    // Restore the registers from the new context.
    movl    __SYSV_X86_OFFSET_EBX(%eax), %ebx
    movl    __SYSV_X86_OFFSET_EBP(%eax), %ebp
    movl    __SYSV_X86_OFFSET_ESI(%eax), %esi
    movl    __SYSV_X86_OFFSET_EDI(%eax), %edi
    movl    __SYSV_X86_OFFSET_ESP(%eax), %ecx
    movl    __SYSV_X86_OFFSET_EIP(%eax), %eax

    // Switch stacks to the new context.
    xchgl   %ecx, %esp

    // Note: now that we've switched stacks, any unwinding will happen on the new stack.
    .cfi_def_cfa %esp, 4
    .cfi_same_value %ebx
    .cfi_same_value %ebp
    .cfi_same_value %esi
    .cfi_same_value %edi
    .cfi_register %eip, %eax

    // Write the output value.
    //
    // Note: the "value" return parameter has been in %edx all along.
    movl    4(%esp), %eax
    movl    %ecx, 0(%eax)
    movl    %edx, 4(%eax)

    // Return to the caller in the context that we just switched to. An indirect jump (rather than a return) is required
    // as there is no guarantee that the return address is on the stack.
    jmpl    *%eax

    .cfi_endproc

.LSwapContexts__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:

    // Parameters:
    //  sysv_x86_frame_t*   next        4(%esp)
    //  std::uint32_t       param       8(%esp)
    //
    // Expected return values:
    //  sysv_x86_frame_t*   prev        %eax    note: always null
    //  std::uint32_t       param       %edx
    //
    
    // Load the parameters into registers for easier access.
    .cfi_startproc
    .cfi_offset %eip, -4
    movl    4(%esp), %eax
    movl    8(%esp), %edx

    // Generate CFI instructions for this function as if we've already resumed the new context.
    .cfi_def_cfa %eax, __SYSV_X86_FRAME_SIZE
    .cfi_rel_offset %ebx, __SYSV_X86_OFFSET_EBX
    .cfi_rel_offset %esi, __SYSV_X86_OFFSET_ESI
    .cfi_rel_offset %edi, __SYSV_X86_OFFSET_EDI
    .cfi_rel_offset %ebp, __SYSV_X86_OFFSET_EBP
    .cfi_rel_offset %esp, __SYSV_X86_OFFSET_ESP
    .cfi_rel_offset %eip, __SYSV_X86_OFFSET_EIP

    // Restore the registers from the new context.
    movl    __SYSV_X86_OFFSET_EBX(%eax), %ebx
    movl    __SYSV_X86_OFFSET_ESI(%eax), %esi
    movl    __SYSV_X86_OFFSET_EDI(%eax), %edi
    movl    __SYSV_X86_OFFSET_EBP(%eax), %ebp
    movl    __SYSV_X86_OFFSET_ESP(%eax), %esp
    movl    __SYSV_X86_OFFSET_EIP(%eax), %ecx

    // Stack has been switched so correct the CFI information.
    .cfi_def_cfa %esp, 0
    .cfi_same_value %ebx
    .cfi_same_value %esi
    .cfi_same_value %edi
    .cfi_same_value %ebp
    .cfi_same_value %esp
    .cfi_register %eip, %ecx

    // Clear the pointer return value to null. The second return value is already in %edx.
    xorl    %eax, %eax

    // Return to the caller in the context that we just switched to. An indirect jump (rather than a return) is required
    // as there is no guarantee that the return address is on the stack.
    jmpl    *%ecx

    .cfi_endproc

.LResumeContext__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:

    // Parameters:
    //  sysv_x86_frame_t*   next        4(%esp)
    //
    // Expected return values:
    //  <unspecified>       n/a         %eax    note: value taken from context
    //  <unspecified>       n/a         %edx    note: value taken from context
    //

    // Load the parameters.
    movl    4(%esp), %eax

    // Restore most of the registers from the context. Not restored are:
    //  %eax - needed to point to the context.
    //  %eip
    movl    __SYSV_X86_OFFSET_EBX(%eax), %ebx
    movl    __SYSV_X86_OFFSET_ECX(%eax), %ecx
    movl    __SYSV_X86_OFFSET_EDX(%eax), %edx
    movl    __SYSV_X86_OFFSET_ESI(%eax), %esi
    movl    __SYSV_X86_OFFSET_EDI(%eax), %edi
    movl    __SYSV_X86_OFFSET_EBP(%eax), %ebp
    movl    __SYSV_X86_OFFSET_ESP(%eax), %esp

    // Push the target %eip to the stack then restore %eax
    pushl   __SYSV_X86_OFFSET_EIP(%eax)
    movl    __SYSV_X86_OFFSET_EAX(%eax), %eax

    // Jump to the target.
    retl

.LResumeContextFull__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:

    // Parameters:
    //  pair<...>*          return       4(%esp)
    //  sysv_x86_frame_t*   ctxt         8(%esp)
    //  std::uint32_t       param_first 12(%esp)
    //
    // Expected return values:
    //  sysv_x86_frame_t*   prev        0(pair) note: set to the input ctxt
    //  std::uint32_t       param       4(pair) note: set to the input param_first
    //

    .cfi_startproc
    .cfi_offset %eip, -4

    // Load the parameters into registers.
    movl    8(%esp), %eax
    movl    12(%esp), %edx

    // Calculate the value of %esp after this function returns.
    leal    4(%esp), %ecx

    // Push the registers that need to be preserved across function calls.
    movl    %ebx, __SYSV_X86_OFFSET_EBX(%eax)
    movl    %esi, __SYSV_X86_OFFSET_ESI(%eax)
    movl    %edi, __SYSV_X86_OFFSET_EDI(%eax)
    movl    %ebp, __SYSV_X86_OFFSET_EBP(%eax)
    movl    %ecx, __SYSV_X86_OFFSET_ESP(%eax)

    // Capture the return instruction pointer too.
    movl    0(%esp), %ecx
    movl    %ecx, __SYSV_X86_OFFSET_EIP(%eax)

    // Save the return values to the output location.
    movl    4(%esp), %ecx
    movl    %eax, 0(%ecx)
    movl    %edx, 4(%ecx)

    // And return.
    retl

    .cfi_endproc

.LCaptureContext__v1_0_end = .


.align 16


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CreateContext__v1_0,CreateContext,1.0)
.LCreateContext__v1_0:

    // Parameters:
    //   4(%esp)    void*               stack
    //   8(%esp)    std::size_t         stack_size
    //  12(%esp)    void (*)(...)       fn
    //  16(%esp)    void*               param
    //
    // Expected return values:
    //  %eax    sysv_x86_frame_t*   new_context

    .cfi_startproc
    .cfi_offset %eip, -4

    //! @TODO: fix CFI instructions for this function.

    // Free up some registers to make this a little easier.
    pushl   %ebp
    leal    -4(%esp), %ebp
    pushl   %esi
    pushl   %edi

    // Load the parameters for easier access.
    movl    4(%ebp), %edi
    movl    8(%ebp), %esi
    movl    12(%ebp), %edx
    movl    16(%ebp), %ecx

    // Calculate the stack top address. This is minus the space required for the context.
    leal    -__SYSV_X86_FRAME_SIZE(%edi, %esi), %eax

    // Push the address of the target function and the parameter that it will take.
    movl    %edx, -4(%eax)
    movl    %ecx, -8(%eax)

    // Calculate the stack pointer the new context will use.
    leal    -8(%eax), %esi

    // Set the parameters for a call to the wrapped function.
    //
    // The registers are set to the current values, in case they matter.
    movl    %ebx, __SYSV_X86_OFFSET_EBX(%eax)
    popl          __SYSV_X86_OFFSET_EDI(%eax)
    popl          __SYSV_X86_OFFSET_ESI(%eax)
    movl    $0,   __SYSV_X86_OFFSET_EBP(%eax)
    movl    %esi, __SYSV_X86_OFFSET_ESP(%eax)

    // Finally, set the execution address to our trampoline code.
    leal    .LCreateContextTrampoline, %edx
    movl    %edx, __SYSV_X86_OFFSET_EIP(%eax)

    // Context has been set up and the pointer to the context is already in the right place.
    popl    %ebp
    retl

    .cfi_endproc

.LCreateContext__v1_0_end = .

// Note that this function does not have any CFI descriptors as it is not possible to unwind through it (as any return
// into it is explicitly disallowed as a violation of the API contract of CreateContext).
.type .LCreateContextTrampoline, STT_FUNC
.size .LCreateContextTrampoline, .LCreateContextTrampoline_end - .
.LCreateContextTrampoline:

    // The contents of the stack at entry are as follows:
    //   0(%esp)    previous frame pointer (as passed to ResumeContext).
    //   4(%esp)    resume parameter (as passed to ResumeContext).
    //   8(%esp)    bound parameter (as passed to CreateContext).
    //  12(%esp)    target function (as passed to CreateContext).
    //
    // Because this is already a valid setup for a call to a three-parameter function, all we need to do is call it.
    call    *12(%esp)

    // In violation of the API contract, the function around which the context was created returned. The API specifies
    // undefined behaviour so invoking an undefined opcode is well within rights.
1:  ud2
    jmp     1b

.LCreateContextTrampoline_end = .

#endif
