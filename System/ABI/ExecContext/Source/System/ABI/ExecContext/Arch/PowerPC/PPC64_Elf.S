#include <System/ABI/ExecContext/Private/Config.hh>


#if _ARCH_PPC64


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


#endif // if _ARCH_PPC64
