#if !defined(__LP64__)

#include <System/ABI/ExecContext/Private/Config.hh>


#ifdef __sparc_v9__
#  define FLUSHW    flushw
#else
#  define FLUSHW    trap x03
#endif


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      %i0:    sparc32_frame_t* next
    //      %i1:    std::uintptr_t opaque
    //
    // Returns:
    //      [%sp + 64]:    pair<sparc32_frame_t*, std::uintptr_t>

    // Allocate a new register window and flush all other windows to the stack.
    save    %sp, -(64+8), %sp       // ABI-required register save area plus two words for %sp and %pc.
    FLUSHW

    // Store the previous stack pointer and the true return address.
    add     %i7, 8, %l0             // Address of next instruction in caller.
    st      %fp, [ %fp - 8 ]        // Stack pointer at point of call.
    st      %l0, [ %fp - 4 ]        // Return address.

    // The context is now split across two stack frames: the %sp and %pc at the top of this frame and the %iX and %lX
    // at the bottom of the next frame.
    add     %fp, -8, %l1

    // Set the frame pointer to the new context's stack pointer: we'll switch stacks to that context when we return.
    ld      [ %i0 + 4 ], %fp

    // Set the return address to the new context's return address.
    ld      [ %i0 + 0 ], %i7

    // From the new stack, get the pointer to the structure return area.
    ld      [ %fp + 64 ], %l2

    // Write the return values.
    st      %l1, [ %l2 + 0 ]    // Pointer to the context we just switched from.
    st      %i1, [ %l2 + 4 ]    // Opaque return data.

    // Return to the new context.
    ret
      restore   %fp, 0, %sp

.LSwapContexts__v1_0_end = .


/*__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      $a0:    mips_o32_frame_t* context
    //      $a1:    std::uintptr_t opaque
    //
    // Returns:
    //      $v0:    mips_o32_frame_t* context     note: always null
    //      $v1:    std::uintptr_t opaque

    // Restore the non-volatile registers from the context.
    lw      $ra, 44($a0)
    lw      $gp, 36($a0)
    lw      $s8, 32($a0)
    lw      $s7, 28($a0)
    lw      $s6, 24($a0)
    lw      $s5, 20($a0)
    lw      $s4, 16($sp)
    lw      $s3, 12($a0)
    lw      $s2, 8($a0)
    lw      $s1, 4($a0)
    lw      $s0, 0($a0)
    lw      $sp, 40($a0)

    // Set the return values and return.
    move    $v0, $zero
    jr      $ra
      move  $v1, $a1

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      $a0:    mips_o32_full_frame_t* context
    //
    // Returns: <taken from the context>

    // Restore all of the registers.
    lw      $ra, 120($a0)       // Restored first to avoid pipeline delays.
    lw      $at, 0($a0)
    lw      $v0, 4($a0)
    lw      $v1, 8($a0)
    // $a0 is reloaded last.
    lw      $a1, 16($a0)
    lw      $a2, 20($a0)
    lw      $a3, 24($a0)
    lw      $t0, 28($a0)
    lw      $t1, 32($a0)
    lw      $t2, 36($a0)
    lw      $t3, 40($a0)
    lw      $t4, 44($a0)
    lw      $t5, 48($a0)
    lw      $t6, 52($a0)
    lw      $t7, 56($a0)
    lw      $t8, 60($a0)
    lw      $t9, 64($a0)
    lw      $s0, 68($a0)
    lw      $s1, 72($a0)
    lw      $s2, 76($a0)
    lw      $s3, 80($a0)
    lw      $s4, 84($a0)
    lw      $s5, 88($a0)
    lw      $s6, 92($a0)
    lw      $s7, 96($a0)
    lw      $s8, 100($a0)
    lw      $k0, 104($a0)
    lw      $k1, 108($a0)
    lw      $gp, 112($a0)
    lw      $sp, 116($a0)

    // Restore $a0 and return.
    jr      $ra
      lw    $a0, 12($a0)

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      $a0:    mips_o32_frame_t* context
    //      $a1:    std::uintptr_t opaque
    //
    // Returns:
    //      $v0:    mips_o32_frame_t* context
    //      $v1:    std::uintptr_t opaque

    // Store all of the non-volatile registers into the context.
    sw      $s0, 0($a0)
    sw      $s1, 4($a0)
    sw      $s2, 8($a0)
    sw      $s3, 12($a0)
    sw      $s4, 16($a0)
    sw      $s5, 20($a0)
    sw      $s6, 24($a0)
    sw      $s7, 28($a0)
    sw      $s8, 32($a0)
    sw      $gp, 36($a0)
    sw      $sp, 40($a0)
    sw      $ra, 44($a0)

    // Set the return values and return.
    move    $v0, $zero
    jr      $ra
      move  $v1, $a1

.LCaptureContext__v1_0_end = .
*/

#endif // if defined(_ABIO32)
