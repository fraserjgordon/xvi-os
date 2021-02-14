#if !defined(__arch64__)

#include <System/ABI/ExecContext/Private/Config.hh>


#ifdef __sparc_v9__
#   define FLUSHW    flushw
#else
#   define FLUSHW    trap x03
#endif


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      %i0:    sparc32_frame_t* next
    //      %i1:    std::uintptr_t opaque
    //
    // Returns:
    //      [%sp + 64]:    pointer to pair<sparc32_frame_t*, std::uintptr_t>

    // Allocate a new register window and flush all other windows to the stack.
    save    %sp, -(64+8), %sp       // ABI-required register save area plus two words for %sp and %pc.
    FLUSHW

    // Store the previous stack pointer and the true return address.
    add     %i7, 8, %l0             // Address of next instruction in caller.
    st      %fp, [ %fp - 8 ]        // Stack pointer at point of call.
    st      %l0, [ %fp - 4 ]        // Return address.

    // The context is now split across two stack frames: the %sp and %pc at the top of this frame and the %lX and %iX
    // at the bottom of the next frame.
    add     %fp, -8, %l1

    // Set the frame pointer to the new context's stack pointer: we'll switch stacks to that context when we return.
    ld      [ %i0 + 4 ], %fp

    // Set the return address to the new context's return address.
    ld      [ %i0 + 0 ], %o7

    // From the new stack, get the pointer to the structure return area.
    ld      [ %fp + 64 ], %l2

    // Write the return values.
    st      %l1, [ %l2 + 0 ]    // Pointer to the context we just switched from.
    st      %i1, [ %l2 + 4 ]    // Opaque return data.

    // Return to the new context.
    mov     %l2, %i0
    jmp     %o7
      restore   %fp, 0, %sp

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      %i0:    sparc32_frame_t* next
    //      %i1:    std::uintptr_t opaque
    //
    // Returns:
    //      [%sp + 64]:    pointer to pair<sparc32_frame_t*, std::uintptr_t>

    // Allocate a new register window and flush all other windows to the stack.
    save    %sp, -(64+8), %sp       // ABI-required register save area plus two words for %sp and %pc.
    FLUSHW

    // Load the return address and prepare for a switch to the new stack.
    ld      [ %i0 + 0 ], %o7        // Return address.
    ld      [ %i0 + 4 ], %fp        // Stack pointer.

    // Load the return value pointer and write the return values.
    ld      [ %fp + 64 ], %i0
    st      %g0, [ %i0 + 0 ]        // Previous context pointer.
    st      %i1, [ %i0 + 4 ]        // Opaque data.

    // Return to the new context and switch to its stack.
    jmp     %o7
      restore   %fp, 0, %sp

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      %i0:    sparc32_full_frame_t* context
    //
    // Returns: <taken from the context>

    // Allocate a new register window and flush all other windows to the stack.
    save    %sp, -64, %sp       // ABI-required register save area.
    FLUSHW

    // Load the key information into the output registers.
    mov     %i0, %o0                // Pointer to the new context.
    ld      [ %i0 + 128 ], %o7      // Target %pc.

    // Load the output registers as specified (into the input registers of this window).
    ld      [ %o0 + 96 ], %i0
    ld      [ %o0 + 100 ], %i1
    ld      [ %o0 + 104 ], %i2
    ld      [ %o0 + 108 ], %i3
    ld      [ %o0 + 112 ], %i4
    ld      [ %o0 + 116 ], %i5
    ld      [ %o0 + 120 ], %i6      // %fp; previous frame's stack pointer.
    ld      [ %o0 + 124 ], %i7

    // Transfer the local registers to the save area in the new context's frame.
    ld      [ %o0 + 64 ], %l0
    ld      [ %o0 + 68 ], %l1
    ld      [ %o0 + 72 ], %l2
    ld      [ %o0 + 76 ], %l3
    ld      [ %o0 + 80 ], %l4
    ld      [ %o0 + 84 ], %l5
    ld      [ %o0 + 88 ], %l6
    ld      [ %o0 + 92 ], %l7
    st      %l0, [ %fp + 0 ]
    st      %l1, [ %fp + 4 ]
    st      %l2, [ %fp + 8 ]
    st      %l3, [ %fp + 12 ]
    st      %l4, [ %fp + 16 ]
    st      %l5, [ %fp + 20 ]
    st      %l6, [ %fp + 24 ]
    st      %l7, [ %fp + 28 ]

    // And the same for its input registers.
    ld      [ %o0 + 32 ], %l0
    ld      [ %o0 + 36 ], %l1
    ld      [ %o0 + 40 ], %l2
    ld      [ %o0 + 44 ], %l3
    ld      [ %o0 + 48 ], %l4
    ld      [ %o0 + 52 ], %l5
    ld      [ %o0 + 56 ], %l6
    ld      [ %o0 + 60 ], %l7
    st      %l0, [ %fp + 32 ]
    st      %l1, [ %fp + 36 ]
    st      %l2, [ %fp + 40 ]
    st      %l3, [ %fp + 44 ]
    st      %l4, [ %fp + 48 ]
    st      %l5, [ %fp + 52 ]
    st      %l6, [ %fp + 56 ]
    st      %l7, [ %fp + 60 ]

    // Load all the mutable global registers.
    ld      [ %i0 + 4 ], %g1
    ld      [ %i0 + 8 ], %g2
    ld      [ %i0 + 12 ], %g3
    ld      [ %i0 + 16 ], %g4
    ld      [ %i0 + 20 ], %g5
    ld      [ %i0 + 24 ], %g6
    ld      [ %i0 + 28 ], %g7

    // Return to the target address and switch to its stack.
    jmp     %o7
      restore   %fp, 0, %sp

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      %o0:    sparc32_frame_t* next
    //      %o1:    std::uintptr_t opaque
    //
    // Returns:
    //      [%sp + 64]:    pointer to pair<sparc32_frame_t*, std::uintptr_t>

    // Ensure all other register windows are saved to the stack.
    FLUSHW

    // Load the return value pointer.
    ld      [ %sp + 64 ], %o4

    // Save the stack pointer and (true) return address.
    add     %o7, 8, %o5
    st      %sp, [ %o0 + 0 ]    // Stack pointer.
    st      %o5, [ %o0 + 4 ]    // Return address.

    // Save the input and local registers as requested.
    st      %l0, [ %o0 + 8 ]
    st      %l1, [ %o0 + 12 ]
    st      %l2, [ %o0 + 16 ]
    st      %l3, [ %o0 + 20 ]
    st      %l4, [ %o0 + 24 ]
    st      %l5, [ %o0 + 28 ]
    st      %l6, [ %o0 + 32 ]
    st      %l7, [ %o0 + 36 ]
    st      %i0, [ %o0 + 40 ]
    st      %i1, [ %o0 + 44 ]
    st      %i2, [ %o0 + 48 ]
    st      %i3, [ %o0 + 52 ]
    st      %i4, [ %o0 + 56 ]
    st      %i5, [ %o0 + 60 ]
    st      %i6, [ %o0 + 64 ]
    st      %i7, [ %o0 + 68 ]

    // Store the return value.
    clr     [ %o4 + 0 ]
    st      %o1, [ %o4 + 4 ]

    // Return to the caller with the pointer to the return value.
    retl
      mov   %o4, %o0

.LCaptureContext__v1_0_end = .


#endif // if !defined(__arch64__)
