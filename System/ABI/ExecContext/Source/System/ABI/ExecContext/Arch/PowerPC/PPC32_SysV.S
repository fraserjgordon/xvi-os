#include <System/ABI/ExecContext/Private/Config.h>


#if _ARCH_PPC && !_ARCH_PPC64 && _CALL_SYSV


// General-purpose register saving routines.
#define DECLARE_SAVEGPR(n) \
    .global     _savegpr_##n; \
    __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD _savegpr_##n; \
    .type       _savegpr_##n, @function; \
    .size       _savegpr_##n, .L_savegpr_end - _savegpr_##n; \
    \
    .global     _restgpr_##n; \
    __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD _restgpr_##n; \
    .type       _restgpr_##n, @function; \
    .size       _restgpr_##n, .L_restgpr_end - _restgpr_##n;

DECLARE_SAVEGPR(31)
DECLARE_SAVEGPR(30)
DECLARE_SAVEGPR(29)
DECLARE_SAVEGPR(28)
DECLARE_SAVEGPR(27)
DECLARE_SAVEGPR(26)
DECLARE_SAVEGPR(25)
DECLARE_SAVEGPR(24)
DECLARE_SAVEGPR(23)
DECLARE_SAVEGPR(22)
DECLARE_SAVEGPR(21)
DECLARE_SAVEGPR(20)
DECLARE_SAVEGPR(19)
DECLARE_SAVEGPR(18)
DECLARE_SAVEGPR(17)
DECLARE_SAVEGPR(16)
DECLARE_SAVEGPR(15)
DECLARE_SAVEGPR(14)

_savegpr_14:    stw     %r14, -72(%r11)
_savegpr_15:    stw     %r15, -68(%r11)
_savegpr_16:    stw     %r16, -64(%r11)
_savegpr_17:    stw     %r17, -60(%r11)
_savegpr_18:    stw     %r18, -56(%r11)
_savegpr_19:    stw     %r19, -52(%r11)
_savegpr_20:    stw     %r20, -48(%r11)
_savegpr_21:    stw     %r21, -44(%r11)
_savegpr_22:    stw     %r22, -40(%r11)
_savegpr_23:    stw     %r23, -36(%r11)
_savegpr_24:    stw     %r24, -32(%r11)
_savegpr_25:    stw     %r25, -28(%r11)
_savegpr_26:    stw     %r26, -24(%r11)
_savegpr_27:    stw     %r27, -20(%r11)
_savegpr_28:    stw     %r28, -16(%r11)
_savegpr_29:    stw     %r29, -12(%r11)
_savegpr_30:    stw     %r30, -8(%r11)
_savegpr_31:    stw     %r31, -4(%r11) 
                blr
.L_savegpr_end = .

_restgpr_14:    lwz     %r14, -72(%r11)
_restgpr_15:    lwz     %r15, -68(%r11)
_restgpr_16:    lwz     %r16, -64(%r11)
_restgpr_17:    lwz     %r17, -60(%r11)
_restgpr_18:    lwz     %r18, -56(%r11)
_restgpr_19:    lwz     %r19, -52(%r11)
_restgpr_20:    lwz     %r20, -48(%r11)
_restgpr_21:    lwz     %r21, -44(%r11)
_restgpr_22:    lwz     %r22, -40(%r11)
_restgpr_23:    lwz     %r23, -36(%r11)
_restgpr_24:    lwz     %r24, -32(%r11)
_restgpr_25:    lwz     %r25, -28(%r11)
_restgpr_26:    lwz     %r26, -24(%r11)
_restgpr_27:    lwz     %r27, -20(%r11)
_restgpr_28:    lwz     %r28, -16(%r11)
_restgpr_29:    lwz     %r29, -12(%r11)
_restgpr_30:    lwz     %r30, -8(%r11)
_restgpr_31:    lwz     %r31, -4(%r11)
                blr
.L_restgpr_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,_1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      r3:     ppc32_sysv_frame_t* next
    //      r4:     std::uintptr_t opaque
    //
    // Returns:
    //      r3:     ppc32_sysv_frame_t* prev
    //      r4:     std::uintptr_t opaque

    // Build a normal SysV ABI stack frame that saves all non-volatile registers.
    mr      %r11, %r1
    stwu    %r1, -96(%r1)       // sizeof(ppc32_sysv_frame_t)
    mflr    %r0
    mfcr    %r12
    stw     %r0, 4(%r1)
    stw     %r12, -84(%r11)     // Condition register.
    stw     %r2,  -80(%r11)
    stw     %r13, -76(%r11)
    bl      _savegpr_14

    // Swap stacks to that of the new context.
    mr      %r1, %r5
    mr      %r3, %r1
    mr      %r5, %r3

    // Restore the registers from the new stack.
    lwz     %r11, 0(%r1)
    bl      _restgpr_14
    lwz     %r13, -72(%r11)
    lwz     %r2,  -76(%r11)
    lwz     %r12, -84(%r11)
    lwz     %r0, 4(%r1)
    mtcr    %r12
    mtlr    %r0
    mr      %r1, %r11
    blr

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,_1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      r3:     ppc32_sysv_frame_t* context
    //      r4:     std::uintptr_t opaque
    //
    // Returns:
    //      r3:     ppc32_sysv_frame_t* context     note: always null
    //      r4:     std::uintptr_t opaque

    // Restore registers from the context.
    mflr    %r5
    addi    %r11, %r3, 96       // sizeof(ppc32_sysv_frame_t)
    stw     %r5, 4(%r1)         // Need to save %lr to keep within the ABI requirements.
    bl      _restgpr_14
    lwz     %r13, -72(%r11)
    lwz     %r2,  -76(%r11)
    lwz     %r12, -84(%r11)
    lwz     %r0, 4(%r3)
    mtcr    %r12
    mtlr    %r0
    lwz     %r1, 0(%r3)
    li      %r3, 0
    blr

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,_1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      r3:     ppc32_sysv_full_frame_t* context
    //
    // Returns: <taken from the context>

    // Restore the non-volatile registers first.
    mflr    %r5
    addi    %r11, %r3, 142      // sizeof(ppc32_sysv_full_frame_t)
    stw     %r5, 4(%r1)         // Need to save %lr to keep within the ABI requirements.
    bl      _restgpr_14         // Restore the non-volatile registers.
    lwz     %r13, -72(%r11)     // SDA pointer.
    lwz     %r2,   20(%r3)      // Thread pointer / SDA2 pointer.
    lwz     %r12,  16(%r3)      // Condition register.
    lwz     %r0,    4(%r3)      // Link register.
    mtcr    %r12
    mtlr    %r0
    lwz     %r1, 0(%r3)         // Stack pointer.

    // Restore the volatile registers.
    lwz     %r12, -76(%r11)
    // Skipping %r11 until the end.
    lwz     %r10, -84(%r11)
    lwz     %r9,  -88(%r11)
    lwz     %r8,  -92(%r11)
    lwz     %r7,  -96(%r11)
    lwz     %r6, -100(%r11)
    lwz     %r5, -104(%r11)
    lwz     %r4, -108(%r11)
    lwz     %r3, -112(%r11)
    lwz     %r3, -116(%r11)
    lwz     %r2, -120(%r11)
    lwz     %r0, -124(%r11)
    lwz     %r11, -80(%r11)

    // All registers now have the requested state.
    blr

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,_1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      r3:     ppc32_sysv_frame_t* context
    //      r4:     std::uintptr_t opaque
    //
    // Returns:
    //      r3:     ppc32_sysv_frame_t* context
    //      r4:     std::uintptr_t opaque

    // Store all of the non-volatile registers into the context.
    mfcr    %r5
    mflr    %r6
    addi    %r11, %r3, 84
    stw     %r1, 0(%r3)
    stw     %r2, 4(%r3)
    stw     %r13, 8(%r3)
    stw     %r5, 84(%r3)    // Condition register.
    stw     %r6, 88(%r3)    // Link register.
    li      %r3, 0
    b       _savegpr_14

.LCaptureContext__v1_0_end = .


#endif // if _ARCH_PPC && !_ARCH_PPC64 && _CALL_SYSV
