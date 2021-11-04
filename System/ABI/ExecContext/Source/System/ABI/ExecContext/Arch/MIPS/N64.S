#if defined(_ABI64) || defined(__mips_n64)

#include <System/ABI/ExecContext/Private/Config.h>


.set noat
.set noreorder


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,_1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      $a0:    mips_n64_frame_t* next
    //      $a1:    std::uintptr_t opaque
    //
    // Returns:
    //      $v0:    mips_n64_frame_t* prev
    //      $v1:    std::uintptr_t opaque

    // Save the current context to the stack.
    move    $t0, $sp
    daddiu  $sp, $sp, -96   // sizeof(mips_n64_frame_t)
    sd      $s0, 0($sp)
    sd      $s1, 8($sp)
    sd      $s2, 16($sp)
    sd      $s3, 24($sp)
    sd      $s4, 32($sp)
    sd      $s5, 40($sp)
    sd      $s6, 48($sp)
    sd      $s7, 56($sp)
    sd      $s8, 64($sp)
    sd      $gp, 72($sp)
    sd      $t0, 80($sp)    // Stack pointer.
    sd      $ra, 88($sp)

    // Swap stacks.
    move    $v0, $sp
    move    $sp, $a0

    // Restore the new context from the stack.
    ld      $ra, 88($sp)
    ld      $gp, 72($sp)
    ld      $s8, 64($sp)
    ld      $s7, 56($sp)
    ld      $s6, 48($sp)
    ld      $s5, 40($sp)
    ld      $s4, 32($sp)
    ld      $s3, 24($sp)
    ld      $s2, 16($sp)
    ld      $s1, 8($sp)
    ld      $s0, 0($sp)
    ld      $sp, 80($sp)
    jr      $ra
      move  $v1, $a1

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,_1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      $a0:    mips_n64_frame_t* context
    //      $a1:    std::uintptr_t opaque
    //
    // Returns:
    //      $v0:    mips_n64_frame_t* context     note: always null
    //      $v1:    std::uintptr_t opaque

    // Restore the non-volatile registers from the context.
    ld      $ra, 88($a0)
    ld      $gp, 72($a0)
    ld      $s8, 64($a0)
    ld      $s7, 56($a0)
    ld      $s6, 48($a0)
    ld      $s5, 40($a0)
    ld      $s4, 32($sp)
    ld      $s3, 24($a0)
    ld      $s2, 16($a0)
    ld      $s1, 8($a0)
    ld      $s0, 0($a0)
    ld      $sp, 80($a0)

    // Set the return values and return.
    move    $v0, $zero
    jr      $ra
      move  $v1, $a1

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,_1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      $a0:    mips_n64_full_frame_t* context
    //
    // Returns: <taken from the context>

    // Restore all of the registers.
    ld      $ra, 240($a0)       // Restored first to avoid pipeline delays.
    ld      $at, 0($a0)
    ld      $v0, 8($a0)
    ld      $v1, 16($a0)
    // $a0 is reloaded last.
    ld      $a1, 32($a0)
    ld      $a2, 40($a0)
    ld      $a3, 48($a0)
    ld      $a4, 56($a0)
    ld      $a5, 64($a0)
    ld      $a6, 72($a0)
    ld      $a7, 80($a0)
    ld      $t0, 88($a0)
    ld      $t1, 96($a0)
    ld      $t2, 104($a0)
    ld      $t3, 112($a0)
    ld      $t8, 120($a0)
    ld      $t9, 128($a0)
    ld      $s0, 136($a0)
    ld      $s1, 144($a0)
    ld      $s2, 152($a0)
    ld      $s3, 160($a0)
    ld      $s4, 168($a0)
    ld      $s5, 176($a0)
    ld      $s6, 184($a0)
    ld      $s7, 192($a0)
    ld      $s8, 200($a0)
    ld      $k0, 208($a0)
    ld      $k1, 216($a0)
    ld      $gp, 224($a0)
    ld      $sp, 232($a0)

    // Restore $a0 and return.
    jr      $ra
      lw    $a0, 24($a0)

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,_1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      $a0:    mips_n64_frame_t* context
    //      $a1:    std::uintptr_t opaque
    //
    // Returns:
    //      $v0:    mips_n64_frame_t* context
    //      $v1:    std::uintptr_t opaque

    // Store all of the non-volatile registers into the context.
    sd      $s0, 0($a0)
    sd      $s1, 8($a0)
    sd      $s2, 16($a0)
    sd      $s3, 24($a0)
    sd      $s4, 32($a0)
    sd      $s5, 40($a0)
    sd      $s6, 48($a0)
    sd      $s7, 56($a0)
    sd      $s8, 64($a0)
    sd      $gp, 72($a0)
    sd      $sp, 80($a0)
    sd      $ra, 88($a0)

    // Set the return values and return.
    move    $v0, $zero
    jr      $ra
      move  $v1, $a1

.LCaptureContext__v1_0_end = .


#endif // if defined(_ABI64) || defined(__mips_n64)
