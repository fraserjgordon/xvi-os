#include <System/ABI/ExecContext/Arch/x86/SysV_x86.h>


#if defined(__i386__)

.global $System$ABI$ExecContext$SwapContexts
.type   $System$ABI$ExecContext$SwapContexts, @function
.size   $System$ABI$ExecContext$SwapContexts, .LSwapContexts.end - .
$System$ABI$ExecContext$SwapContexts:

    // Preserve the original value of %esp (before the call to this function).
    leal    4(%esp), %edx
    
    // Push the registers that need to be preserved across function calls.
    subl    $__SYSV_X86_FRAME_SIZE-4, %esp      // %eip already pushed as a result of the call
    movl    %edi, __SYSV_X86_OFFSET_EDI(%esp)
    movl    %esi, __SYSV_X86_OFFSET_ESI(%esp)
    movl    %ebx, __SYSV_X86_OFFSET_EBX(%esp)
    movl    %ebp, __SYSV_X86_OFFSET_EBP(%esp)
    movl    %ecx, __SYSV_X86_OFFSET_ESP(%esp)

    // Now that all the registers are preserved, get the address of the context we're switching to.
    movl    0(%edx), %ecx

    // Restore the registers from the new context.
    movl    __SYSV_X86_OFFSET_EDI(%ecx), %edi
    movl    __SYSV_X86_OFFSET_ESI(%ecx), %esi
    movl    __SYSV_X86_OFFSET_EBX(%ecx), %ebx
    movl    __SYSV_X86_OFFSET_EBP(%ecx), %ebp
    movl    __SYSV_X86_OFFSET_ESP(%ecx), %eax

    // Switch stacks to the new context. This conveniently puts the pointer to the previous context into where it needs
    // to be when we return.
    xchgl   %eax, %esp

    // Also grab the parameter that is to be passed to the new context. It is stored on the stack of the old context. It
    // forms the other half of the return value from this function.
    movl    4(%edx), %edx

    // Return to the caller in the context that we just switched to. An indirect jump (rather than a return) is required
    // as there is no guarantee that the return address is on the stack.
    jmpl    *__SYSV_X86_OFFSET_EIP(%ecx)

.LSwapContexts.end = .


.align 16


.global $System$ABI$ExecContext$ResumeContext
.type   $System$ABI$ExecContext$ResumeContext, @function
.size   $System$ABI$ExecContext$ResumeContext, .LResumeContext.end - .
$System$ABI$ExecContext$ResumeContext:

    // Get the address of the context being resumed and the parameter to pass to that context.
    movl    4(%esp), %ecx
    movl    8(%esp), %edx

    // Restore the registers from the context.
    movl    __SYSV_X86_OFFSET_EDI(%ecx), %edi
    movl    __SYSV_X86_OFFSET_ESI(%ecx), %esi
    movl    __SYSV_X86_OFFSET_EBX(%ecx), %ebx
    movl    __SYSV_X86_OFFSET_EBP(%ecx), %ebp
    movl    __SYSV_X86_OFFSET_ESP(%ecx), %esp

    // Return to the caller in the resumed context. The high part of the return value is already in %edx but the
    // previous context pointer needs to be set to null (because we didn't create one).
    xorl    %eax, %eax
    jmpl    *__SYSV_X86_OFFSET_EIP(%ecx)

.LResumeContext.end = .

#endif
