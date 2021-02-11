#if defined(_ABIO32)

#include <System/ABI/ExecContext/Private/Config.hh>


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
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
    addiu   $sp, $sp, -44   // sizeof(mips_o32_frame_t)
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
      nop

.LSwapContexts__v1_0_end = .

#endif // if defined(_ABIO32)
