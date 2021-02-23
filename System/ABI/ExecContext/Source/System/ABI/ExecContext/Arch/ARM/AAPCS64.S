#if defined(__aarch64__)

#include <System/ABI/ExecContext/Private/Config.h>


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      x0:     arm64_frame_t* next
    //      x1:     std::uintptr_t opaque
    //
    // Returns:
    //      x0:     arm64_frame_t* prev
    //      x1:     std::uintptr_t opaque

    // Create a new stack frame and save the non-volatile registers to it.
    sub     sp, sp, #112        // align(sizeof(arm64_frame_t), 16).
    mov     x3, x0
    mov     x2, sp
    str     x19, [sp, #0]
    str     x20, [sp, #8]
    str     x21, [sp, #16]
    str     x22, [sp, #24]
    str     x23, [sp, #32]
    str     x24, [sp, #40]
    str     x25, [sp, #48]
    str     x26, [sp, #56]
    str     x27, [sp, #64]
    str     x28, [sp, #72]
    str     x29, [sp, #80]
    str     x2, [sp, #88]
    str     lr, [sp, #96]

    // Load the non-volatile registers from the new context.
    mov     x0, x2
    ldr     x19, [x3, #0]
    ldr     x20, [x3, #8]
    ldr     x21, [x3, #16]
    ldr     x22, [x3, #24]
    ldr     x23, [x3, #32]
    ldr     x24, [x3, #40]
    ldr     x25, [x3, #48]
    ldr     x26, [x3, #56]
    ldr     x27, [x3, #64]
    ldr     x28, [x3, #72]
    ldr     x29, [x3, #80]
    ldr     x2, [x3, #88]
    ldr     lr, [x3, #96]

    // Switch to the new stack and return.
    mov     sp, x2
    ret

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      x0:     arm64_frame_t* next
    //      x1:     std::uintptr_t opaque
    //
    // Returns:
    //      x0:     arm64_frame_t* prev         note: always null
    //      x1:     std::uintptr_t opaque

    // Load the non-volatile registers from the new context.
    ldr     x19, [x0, #0]
    ldr     x20, [x0, #8]
    ldr     x21, [x0, #16]
    ldr     x22, [x0, #24]
    ldr     x23, [x0, #32]
    ldr     x24, [x0, #40]
    ldr     x25, [x0, #48]
    ldr     x26, [x0, #56]
    ldr     x27, [x0, #64]
    ldr     x28, [x0, #72]
    ldr     x29, [x0, #80]
    ldr     x2, [x0, #88]
    ldr     lr, [x0, #96]

    // Configure the outputs.
    mov     x0, #0
    // x1 is unchanged.

    // Switch to the new stack and return.
    mov     sp, x2
    ret

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      x0:     arm64_full_frame_t* next
    //
    // Returns: <taken from the context>

    // Start by loading the non-volatile registers from the new context.
    ldr     x19, [x0, #152]
    ldr     x20, [x0, #160]
    ldr     x21, [x0, #168]
    ldr     x22, [x0, #176]
    ldr     x23, [x0, #184]
    ldr     x24, [x0, #192]
    ldr     x25, [x0, #200]
    ldr     x26, [x0, #208]
    ldr     x27, [x0, #216]
    ldr     x28, [x0, #224]
    ldr     x29, [x0, #232]
    ldr     lr, [x0, #240]
    ldr     x2, [x0, #248]

    // Switch to the new stack.
    mov     sp, x2

    // Load the volatile registers.
    // Skipping x0 until last.
    ldr     x1, [x0, #8]
    ldr     x2, [x0, #16]
    ldr     x3, [x0, #24]
    ldr     x4, [x0, #32]
    ldr     x5, [x0, #40]
    ldr     x6, [x0, #48]
    ldr     x7, [x0, #56]
    ldr     x8, [x0, #64]
    ldr     x9, [x0, #72]
    ldr     x10, [x0, #80]
    ldr     x11, [x0, #88]
    ldr     x12, [x0, #96]
    ldr     x13, [x0, #104]
    ldr     x14, [x0, #112]
    ldr     x15, [x0, #120]
    ldr     x16, [x0, #128]
    ldr     x17, [x0, #136]
    ldr     x18, [x0, #144]

    // Load the last register and return.
    ldr     x0, [x0, #0]
    ret

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      x0:     arm64_frame_t* next
    //      x1:     std::uintptr_t opaque
    //
    // Returns:
    //      x0:     arm64_frame_t* prev         note: always null
    //      x1:     std::uintptr_t opaque

    // Store all of the non-volatile registers into the context.
    mov     x2, sp
    str     x19, [x0, #0]
    str     x20, [x0, #8]
    str     x21, [x0, #16]
    str     x22, [x0, #24]
    str     x23, [x0, #32]
    str     x24, [x0, #40]
    str     x25, [x0, #48]
    str     x26, [x0, #56]
    str     x27, [x0, #64]
    str     x28, [x0, #72]
    str     x29, [x0, #80]
    str     x2, [x0, #88]
    str     lr, [x0, #96]

    // Prepare the return values and return.
    mov     x0, #0
    // x1 is unchanged.
    ret

.LCaptureContext__v1_0_end = .


#endif // if defined(__aarch64__)
