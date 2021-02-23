#if defined(__arm__)

#include <System/ABI/ExecContext/Private/Config.h>


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      r0:     arm32_frame_t* next
    //      r1:     std::uintptr_t opaque
    //
    // Returns:
    //      r0:     arm32_frame_t* prev
    //      r1:     std::uintptr_t opaque

    // Create a new stack frame and save the non-volatile registers to it.
    mov     r2, r0
    mov     sp, r12
    push    {r4-r12,lr}
    mov     r0, sp          // Output: pointer to the stored registers.

    // Switch to the new context.
    ldmia   r2, {r4-r11,sp,pc}

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      r0:     arm32_frame_t* next
    //      r1:     std::uintptr_t opaque
    //
    // Returns:
    //      r0:     arm32_frame_t* prev         note: always null
    //      r1:     std::uintptr_t opaque

    // Configure the output values then load the context.
    mov     r2, r0
    mov     r0, #0
    ldmia   r2, {r4-r11,sp,pc}

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      r0:     arm32_full_frame_t* next
    //
    // Returns: <taken from the context>

    // Just load everything from the context.
    ldmia   r0, {r0-pc}

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      r0:     arm32_frame_t* next
    //      r1:     std::uintptr_t opaque
    //
    // Returns:
    //      r0:     arm32_frame_t* prev         note: always null
    //      r1:     std::uintptr_t opaque

    // Store all of the non-volatile registers into the context.
    stmia   r0, {r4-r11,sp,lr}
#if defined(__THUMB_INTERWORK__)
    bx      lr
#else
    mov     pc, lr
#endif

.LCaptureContext__v1_0_end = .


#endif // if defined(__arm__)
