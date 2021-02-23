#include <System/ABI/ExecContext/Private/Config.h>


#if _ARCH_PPC64 && _CALL_ELF


// General-purpose register saving routines.
#define DECLARE_SAVEGPR(n) \
    .global     _savegpr0_##n; \
    __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD _savegpr0_##n; \
    .type       _savegpr0_##n, @function; \
    .size       _savegpr0_##n, .L_savegpr0_end - _savegpr0_##n; \
    \
    .global     _restgpr0_##n; \
    __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD _restgpr0_##n; \
    .type       _restgpr0_##n, @function; \
    .size       _restgpr0_##n, .L_restgpr0_end - _restgpr0_##n; \
    \
    .global     _savegpr1_##n; \
    __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD _savegpr1_##n; \
    .type       _savegpr1_##n, @function; \
    .size       _savegpr1_##n, .L_savegpr1_end - _savegpr1_##n; \
    \
    .global     _restgpr1_##n; \
    __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD _restgpr1_##n; \
    .type       _restgpr1_##n, @function; \
    .size       _restgpr1_##n, .L_restgpr1_end - _restgpr1_##n;

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

_savegpr0_14:   std     %r14, -144(%r1)
_savegpr0_15:   std     %r15, -136(%r1)
_savegpr0_16:   std     %r16, -128(%r1)
_savegpr0_17:   std     %r17, -120(%r1)
_savegpr0_18:   std     %r18, -112(%r1)
_savegpr0_19:   std     %r19, -104(%r1)
_savegpr0_20:   std     %r20, -96(%r1)
_savegpr0_21:   std     %r21, -88(%r1)
_savegpr0_22:   std     %r22, -80(%r1)
_savegpr0_23:   std     %r23, -72(%r1)
_savegpr0_24:   std     %r24, -64(%r1)
_savegpr0_25:   std     %r25, -56(%r1)
_savegpr0_26:   std     %r26, -48(%r1)
_savegpr0_27:   std     %r27, -40(%r1)
_savegpr0_28:   std     %r28, -32(%r1)
_savegpr0_29:   std     %r29, -24(%r1)
_savegpr0_30:   std     %r30, -16(%r1)
_savegpr0_31:   std     %r31, -8(%r1)
                std     %r0, 16(%r1)
                blr
.L_savegpr0_end = .

_savegpr1_14:   std     %r14, -144(%r12)
_savegpr1_15:   std     %r15, -136(%r12)
_savegpr1_16:   std     %r16, -128(%r12)
_savegpr1_17:   std     %r17, -120(%r12)
_savegpr1_18:   std     %r18, -112(%r12)
_savegpr1_19:   std     %r19, -104(%r12)
_savegpr1_20:   std     %r20, -96(%r12)
_savegpr1_21:   std     %r21, -88(%r12)
_savegpr1_22:   std     %r22, -80(%r12)
_savegpr1_23:   std     %r23, -72(%r12)
_savegpr1_24:   std     %r24, -64(%r12)
_savegpr1_25:   std     %r25, -56(%r12)
_savegpr1_26:   std     %r26, -48(%r12)
_savegpr1_27:   std     %r27, -40(%r12)
_savegpr1_28:   std     %r28, -32(%r12)
_savegpr1_29:   std     %r29, -24(%r12)
_savegpr1_30:   std     %r30, -16(%r12)
_savegpr1_31:   std     %r31, -8(%r12)
                blr
.L_savegpr1_end = .

_restgpr0_14:   ld      %r14, -144(%r1)
_restgpr0_15:   ld      %r15, -136(%r1)
_restgpr0_16:   ld      %r16, -128(%r1)
_restgpr0_17:   ld      %r17, -120(%r1)
_restgpr0_18:   ld      %r18, -112(%r1)
_restgpr0_19:   ld      %r19, -104(%r1)
_restgpr0_20:   ld      %r20, -96(%r1)
_restgpr0_21:   ld      %r21, -88(%r1)
_restgpr0_22:   ld      %r22, -80(%r1)
_restgpr0_23:   ld      %r23, -72(%r1)
_restgpr0_24:   ld      %r24, -64(%r1)
_restgpr0_25:   ld      %r25, -56(%r1)
_restgpr0_26:   ld      %r26, -48(%r1)
_restgpr0_27:   ld      %r27, -40(%r1)
_restgpr0_28:   ld      %r28, -32(%r1)
_restgpr0_29:   ld      %r0, 16(%r1)
                ld      %r29, -24(%r1)
                mtlr    %r0
                ld      %r30, -16(%r1)
                ld      %r31, -8(%r1)
                blr
_restgpr0_30:   ld      %r30, -16(%r1)
_restgpr0_31:   ld      %r0, 16(%r1)
                ld      %r31, -8(%r1)
                mtlr    %r0
                blr
.L_restgpr0_end = .


_restgpr1_14:   ld      %r14, -144(%r12)
_restgpr1_15:   ld      %r15, -136(%r12)
_restgpr1_16:   ld      %r16, -128(%r12)
_restgpr1_17:   ld      %r17, -120(%r12)
_restgpr1_18:   ld      %r18, -112(%r12)
_restgpr1_19:   ld      %r19, -104(%r12)
_restgpr1_20:   ld      %r20, -96(%r12)
_restgpr1_21:   ld      %r21, -88(%r12)
_restgpr1_22:   ld      %r22, -80(%r12)
_restgpr1_23:   ld      %r23, -72(%r12)
_restgpr1_24:   ld      %r24, -64(%r12)
_restgpr1_25:   ld      %r25, -56(%r12)
_restgpr1_26:   ld      %r26, -48(%r12)
_restgpr1_27:   ld      %r27, -40(%r12)
_restgpr1_28:   ld      %r28, -32(%r12)
_restgpr1_29:   ld      %r29, -24(%r12)
_restgpr1_30:   ld      %r30, -16(%r12)
_restgpr1_31:   ld      %r31, -8(%r12)
                blr
.L_restgpr1_end = .


// Offsets for some frame fields differ between ELFv1 and ELFv2 ABIs.
#if _CALL_ELF == 2
#  define FRAME_SIZE            192
#  define FULL_FRAME_SIZE       272
#  define BACKCHAIN_OFFSET      0
#  define CR_OFFSET             8
#  define LR_OFFSET             16
#  define TOC_OFFSET            24
#  define TP_OFFSET             152
#else
#  define FRAME_SIZE            208
#  define FULL_FRAME_SIZE       288
#  define BACKCHAIN_OFFSET      0
#  define CR_OFFSET             8
#  define LR_OFFSET             16
#  define TOC_OFFSET            40
#  define TP_OFFSET             152
#endif


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(SwapContexts__v1_0,SwapContexts,1.0)
.LSwapContexts__v1_0:
    // Parameters:
    //      r3:     ppc64_elfvX_frame_t* next
    //      r4:     std::uintptr_t opaque
    //
    // Returns:
    //      r3:     ppc64_elfvX_frame_t* prev
    //      r4:     std::uintptr_t opaque

    // Build a normal ELF ABI stack frame that saves all non-volatile registers.
    mr      %r12, %r1
    mflr    %r0
    bl      _savegpr0_14            // Save link register to prev frame, too.
    stdu    %r1, -FRAME_SIZE(%r1)   // sizeof(ppc64_elfvX_frame_t)
    mfcr    %r11
    std     %r0, LR_OFFSET(%r1)     // Link register.
    stw     %r11, CR_OFFSET(%r1)    // Condition register (32 bits!).
    std     %r2, TOC_OFFSET(%r1)    // TOC pointer.
    std     %r13, -TP_OFFSET(%r12)  // Thread pointer.

    // Swap stacks to that of the new context.
    mr      %r1, %r5
    mr      %r3, %r1
    mr      %r5, %r3

    // Restore the registers from the new stack.
    ld      %r12, BACKCHAIN_OFFSET(%r1)
    bl      _restgpr1_14            // We will restore LR from this frame, not previous frame.
    ld      %r13, -TP_OFFSET(%r12)  // Thread pointer.
    ld      %r2, TOC_OFFSET(%r1)    // TOC pointer.
    lwz     %r11, CR_OFFSET(%r1)    // Condition register (32 bits!).
    ld      %r0, LR_OFFSET(%r1)     // Link register.
    mtcr    %r11
    mtlr    %r0
    mr      %r1, %r12               // Pop stack frame.
    blr

.LSwapContexts__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContext__v1_0,ResumeContext,1.0)
.LResumeContext__v1_0:
    // Parameters:
    //      r3:     ppc64_elfvX_frame_t* context
    //      r4:     std::uintptr_t opaque
    //
    // Returns:
    //      r3:     ppc64_elfvX_frame_t* context     note: always null
    //      r4:     std::uintptr_t opaque

    // Restore registers from the context.
    mflr    %r0
    addi    %r12, %r3, FRAME_SIZE   // sizeof(ppc64_elfvX_frame_t)
    std     %r0, LR_OFFSET(%r1)     // Need to save %lr to keep within the ABI requirements.
    bl      _restgpr1_14
    ld      %r13, -TP_OFFSET(%r12)  // Thread pointer.
    ld      %r2, TOC_OFFSET(%r3)    // TOC pointer.
    lwz     %r11, CR_OFFSET(%r3)    // Condition register (32 bits!).
    ld      %r0, LR_OFFSET(%r3)     // Link register.
    mtcr    %r11
    mtlr    %r0
    ld      %r1, BACKCHAIN_OFFSET(%r3)
    li      %r3, 0
    blr

.LResumeContext__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(ResumeContextFull__v1_0,ResumeContextFull,1.0)
.LResumeContextFull__v1_0:
    // Parameters:
    //      r3:     ppc64_elfvX_full_frame_t* context
    //
    // Returns: <taken from the context>

    // Restore the non-volatile registers first (including the stack pointer).
    mflr    %r0
    addi    %r12, %r3, FULL_FRAME_SIZE
    std     %r0, LR_OFFSET(%r1)     // Need to save %lr to keep within the ABI requirements.
    bl      _restgpr1_14
    ld      %r13, -TP_OFFSET(%r12)  // Thread pointer.
    ld      %r2, TOC_OFFSET(%r3)    // TOC pointer.
    lwz     %r11, CR_OFFSET(%r3)    // Condition register (32 bits!).
    ld      %r0, LR_OFFSET(%r3)     // Link register.
    mtcr    %r11
    mtlr    %r0
    ld      %r1, BACKCHAIN_OFFSET(%r3)

    // Restore the volatile registers.
    // Skipping %r12 until the end.
    ld      %r11, -160(%r12)
    ld      %r10, -168(%r12)
    ld      %r9,  -176(%r12)
    ld      %r8,  -184(%r12)
    ld      %r7,  -192(%r12)
    ld      %r6,  -200(%r12)
    ld      %r5,  -208(%r12)
    ld      %r4,  -216(%r12)
    ld      %r3,  -224(%r12)
    ld      %r3,  -232(%r12)
    ld      %r2,  -240(%r12)
    ld      %r0,  -248(%r12)
    ld      %r12, -256(%r12)

    // All registers now have the requested state.
    blr

.LResumeContextFull__v1_0_end = .


__SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(CaptureContext__v1_0,CaptureContext,1.0)
.LCaptureContext__v1_0:
    // Parameters:
    //      r3:     ppc64_elfvX_frame_t* context
    //      r4:     std::uintptr_t opaque
    //
    // Returns:
    //      r3:     ppc64_elfvX_frame_t* context
    //      r4:     std::uintptr_t opaque

    // Store all of the non-volatile registers into the context.
    mfcr    %r11
    mflr    %r0
    addi    %r12, %r3, FRAME_SIZE           // sizeof(ppc64_elfvX_frame_t)
    std     %r1, BACKCHAIN_OFFSET(%r3)      // Stack pointer.
    std     %r2, TOC_OFFSET(%r3)            // TOC pointer.
    std     %r13, -TP_OFFSET(%r12)          // Thread pointer.
    stw     %r11, CR_OFFSET(%r3)            // Condition register (32 bits!).
    std     %r0, LR_OFFSET(%r3)             // Link register.
    li      %r3, 0
    b       _savegpr1_14                    // Non-volatile registers.

.LCaptureContext__v1_0_end = .


#endif // if _ARCH_PPC64
