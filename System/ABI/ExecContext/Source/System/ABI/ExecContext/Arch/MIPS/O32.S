#if defined(_ABIO32) || defined(__mips_o32)

#include <System/ABI/ExecContext/Private/Config.h>


.set noat
.set noreorder


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,_1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      $a0:    mips_o32_frame_t* next
    //      $a1:    std::uintptr_t opaque
    //
    // Returns:
    //      $v0:    mips_o32_frame_t* prev
    //      $v1:    std::uintptr_t opaque

    // Save the current context to the stack.
    move    $t0, $sp
    addiu   $sp, $sp, -48   // sizeof(mips_o32_frame_t)
    sw      $s0, 0($sp)
    sw      $s1, 4($sp)
    sw      $s2, 8($sp)
    sw      $s3, 12($sp)
    sw      $s4, 16($sp)
    sw      $s5, 20($sp)
    sw      $s6, 24($sp)
    sw      $s7, 28($sp)
    sw      $s8, 32($sp)
    sw      $gp, 36($sp)
    sw      $t0, 40($sp)    // Stack pointer.
    sw      $ra, 44($sp)

    // Swap stacks.
    move    $v0, $sp
    move    $sp, $a0

    // Restore the new context from the stack.
    lw      $ra, 44($sp)
    lw      $gp, 36($sp)
    lw      $s8, 32($sp)
    lw      $s7, 28($sp)
    lw      $s6, 24($sp)
    lw      $s5, 20($sp)
    lw      $s4, 16($sp)
    lw      $s3, 12($sp)
    lw      $s2, 8($sp)
    lw      $s1, 4($sp)
    lw      $s0, 0($sp)
    lw      $sp, 40($sp)
    jr      $ra
      move  $v1, $a1

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,_1.0)
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


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,_1.0)
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


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,_1.0)
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


#endif // if defined(_ABIO32) || defined(__mips_o32)
