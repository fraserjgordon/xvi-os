#if defined(__arch64__)

#include <System/ABI/ExecContext/Private/Config.h>


// 64-bit ABI stack bias.
#define BIAS    2047


// We need to be able to modify all global registers.
.register %g2, #ignore
.register %g3, #ignore


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      %i0:    sparc64_frame_t* next
    //      %i1:    std::uintptr_t opaque
    //
    // Returns:
    //      %i0:    sparc64_frame_t* prev
    //      %i1:    std::uintptr_t opaque

    // Allocate a new register window and flush all other windows to the stack.
    save    %sp, -(128+16), %sp     // ABI-required register save area plus two words for %sp and %pc.
    flushw

    // Store the previous stack pointer and the true return address.
    add     %i7, 8, %l0                 // Address of next instruction in caller.
    stx     %fp, [ %fp + BIAS - 16 ]    // Stack pointer at point of call.
    stx     %l0, [ %fp + BIAS - 8 ]     // Return address.

    // The context is now split across two stack frames: the %sp and %pc at the top of this frame and the %lX and %iX
    // at the bottom of the next frame.
    add     %fp, (BIAS - 16), %l1

    // Set the frame pointer to the new context's stack pointer: we'll switch stacks to that context when we return.
    ldx     [ %i0 + 8 ], %fp

    // Set the return address to the new context's return address.
    ldx     [ %i0 + 0 ], %i7

    // Prepare the return values.
    add     %sp, BIAS, %i0
    // %i1 is unmodified.

    // Return to the new context.
    return  %i7     // Return %pc is exact; no +8 needed.
        nop

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      %i0:    sparc64_frame_t* next
    //      %i1:    std::uintptr_t opaque
    //
    // Returns:
    //      %i0:    sparc64_frame_t* prev       note: always null
    //      %i1:    std::uintptr_t opaque

    // Allocate a new register window and flush all other windows to the stack.
    save    %sp, -(128+8), %sp      // ABI-required register save area plus two words for %sp and %pc.
    flushw

    // Load the return address and prepare for a switch to the new stack.
    ldx     [ %i0 + 0 ], %i7        // Return address.
    ldx     [ %i0 + 8 ], %fp        // Stack pointer.

    // Prepare the return values.
    clr     %i0
    // %i1 is unmodified.

    // Return to the new context and switch to its stack.
    return  %i7     // Return %pc is exact; no +8 needed.
        nop

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      %i0:    sparc64_full_frame_t* context
    //
    // Returns: <taken from the context>

    // Allocate a new register window and flush all other windows to the stack.
    save    %sp, -128, %sp       // ABI-required register save area.
    flushw

    // Load the key information into the output registers.
    mov     %i0, %o0                // Pointer to the new context.
    ldx     [ %i0 + 256 ], %o7      // Target %pc.

    // Load the output registers as specified (into the input registers of this window).
    ldx     [ %o0 + 192 ], %i0
    ldx     [ %o0 + 200 ], %i1
    ldx     [ %o0 + 208 ], %i2
    ldx     [ %o0 + 216 ], %i3
    ldx     [ %o0 + 224 ], %i4
    ldx     [ %o0 + 232 ], %i5
    ldx     [ %o0 + 240 ], %i6      // %fp; previous frame's stack pointer.
    ldx     [ %o0 + 248 ], %i7

    // Transfer the local registers to the save area in the new context's frame.
    ldx     [ %o0 + 128 ], %l0
    ldx     [ %o0 + 136 ], %l1
    ldx     [ %o0 + 144 ], %l2
    ldx     [ %o0 + 152 ], %l3
    ldx     [ %o0 + 160 ], %l4
    ldx     [ %o0 + 168 ], %l5
    ldx     [ %o0 + 176 ], %l6
    ldx     [ %o0 + 184 ], %l7
    stx     %l0, [ %fp + BIAS + 0 ]
    stx     %l1, [ %fp + BIAS + 8 ]
    stx     %l2, [ %fp + BIAS + 16 ]
    stx     %l3, [ %fp + BIAS + 24 ]
    stx     %l4, [ %fp + BIAS + 32 ]
    stx     %l5, [ %fp + BIAS + 40 ]
    stx     %l6, [ %fp + BIAS + 48 ]
    stx     %l7, [ %fp + BIAS + 56 ]

    // And the same for its input registers.
    ldx     [ %o0 + 64 ], %l0
    ldx     [ %o0 + 72 ], %l1
    ldx     [ %o0 + 80 ], %l2
    ldx     [ %o0 + 88 ], %l3
    ldx     [ %o0 + 96 ], %l4
    ldx     [ %o0 + 104 ], %l5
    ldx     [ %o0 + 112 ], %l6
    ldx     [ %o0 + 120 ], %l7
    stx     %l0, [ %fp + BIAS + 64 ]
    stx     %l1, [ %fp + BIAS + 72 ]
    stx     %l2, [ %fp + BIAS + 80 ]
    stx     %l3, [ %fp + BIAS + 88 ]
    stx     %l4, [ %fp + BIAS + 96 ]
    stx     %l5, [ %fp + BIAS + 104 ]
    stx     %l6, [ %fp + BIAS + 112 ]
    stx     %l7, [ %fp + BIAS + 120 ]

    // Load all the mutable global registers.
    ldx     [ %i0 + 8 ], %g1
    ldx     [ %i0 + 16 ], %g2
    ldx     [ %i0 + 24 ], %g3
    ldx     [ %i0 + 32 ], %g4
    ldx     [ %i0 + 40 ], %g5
    ldx     [ %i0 + 48 ], %g6
    ldx     [ %i0 + 56 ], %g7

    // Return to the target address and switch to its stack.
    return  %o7     // Return %pc is exact; no +8 needed.
        nop

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      %o0:    sparc64_frame_t* next
    //      %o1:    std::uintptr_t opaque
    //
    // Returns:
    //      %o0:    sparc64_frame_t* prev       note: always null
    //      %o1:    std::uintptr_t opaque

    // Ensure all other register windows are saved to the stack.
    flushw

    // Save the stack pointer and (true) return address.
    add     %sp, BIAS, %o2
    add     %o7, 8, %o3
    stx     %o2, [ %o0 + 0 ]    // Stack pointer.
    stx     %o3, [ %o0 + 8 ]    // Return address.

    // Save the input and local registers as requested.
    stx     %l0, [ %o0 + 16 ]
    stx     %l1, [ %o0 + 24 ]
    stx     %l2, [ %o0 + 32 ]
    stx     %l3, [ %o0 + 40 ]
    stx     %l4, [ %o0 + 48 ]
    stx     %l5, [ %o0 + 56 ]
    stx     %l6, [ %o0 + 64 ]
    stx     %l7, [ %o0 + 72 ]
    stx     %i0, [ %o0 + 80 ]
    stx     %i1, [ %o0 + 88 ]
    stx     %i2, [ %o0 + 96 ]
    stx     %i3, [ %o0 + 104 ]
    stx     %i4, [ %o0 + 112 ]
    stx     %i5, [ %o0 + 120 ]
    stx     %i6, [ %o0 + 128 ]
    stx     %i7, [ %o0 + 136 ]

    // Prepare the return values.
    clr     %o0
    // %o1 is unmodified.

    // Return to the caller.
    retl
        nop

.LCaptureContext__v1_0_end = .


#endif // if defined(__arch64__)
