#include <System/ABI/Atomic/Atomic.hh>

#include <System/ABI/Atomic/AtomicSpinlock.hh>


#if !defined(__ARM_ARCH)
#  error unknown ARM architecture, cannot build
#endif

// This file is only for ARMv8 and later.
#if __ARM_ARCH >= 8


#if 1 || __ATOMIC_ASSUME_LSE_ATOMICS

#define __AARCH64_CAS(reg, size, order, size_suffix, order_suffix) \
extern "C" __uint##size##_t __aarch64_cas##size##_##order(__uint##size##_t expect, __uint##size##_t desire, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_cas##size##_##order(__uint##size##_t expect, __uint##size##_t desire, __uint##size##_t* ptr) \
{ \
    asm volatile \
    ( \
        "cas" order_suffix size_suffix " %" reg "[expect], %" reg "[desire], [%[ptr]]" \
        :   [expect] "+r" (expect)\
        :   "0" (expect), \
            [desire] "r" (desire), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return expect; \
}

#define __AARCH64_SWP(reg, size, order, size_suffix, order_suffix) \
extern "C" __uint##size##_t __aarch64_swp##size##_##order(__uint##size##_t desire, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_swp##size##_##order(__uint##size##_t desire, __uint##size##_t* ptr) \
{ \
    __uint##size##_t previous; \
    asm volatile \
    ( \
        "swp" order_suffix size_suffix " %" reg "[desire], %" reg "[previous], [%[ptr]]" \
        :   [previous] "=r" (previous) \
        :   [desire] "r" (desire), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}

#define __AARCH64_LDOP(op, opcode, reg, size, order, size_suffix, order_suffix) \
extern "C" __uint##size##_t __aarch64_ld##op##size##_##order(__uint##size##_t operand, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_ld##op##size##_##order(__uint##size##_t operand, __uint##size##_t* ptr) \
{ \
    __uint##size##_t previous; \
    asm volatile \
    ( \
        "ld" opcode order_suffix size_suffix " %" reg "[operand], %" reg "[previous], [%[ptr]]" \
        :   [previous] "=r" (previous) \
        :   [operand] "r" (operand), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}

#define __AARCH64_LDADD(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(add, "add", reg, size, order, size_suffix, order_suffix)

#define __AARCH64_LDEOR(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(eor, "eor", reg, size, order, size_suffix, order_suffix)

#define __AARCH64_LDCLR(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(clr, "clr", reg, size, order, size_suffix, order_suffix)

#define __AARCH64_LDSET(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(set, "set", reg, size, order, size_suffix, order_suffix)

#else

#endif


#define __AARCH64_OPS_0(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_CAS(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_SWP(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDADD(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDEOR(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDCLR(reg, size, order, size_suffix, order_suffix) \
    __AARCH64_LDSET(reg, size, order, size_suffix, order_suffix)

#define __AARCH64_OPS_1(reg, size, size_suffix) \
    __AARCH64_OPS_0(reg, size, relax, size_suffix, "") \
    __AARCH64_OPS_0(reg, size, acq, size_suffix, "a") \
    __AARCH64_OPS_0(reg, size, rel, size_suffix, "l") \
    __AARCH64_OPS_0(reg, size, acq_rel, size_suffix, "al")

#define __AARCH64_OPS \
    __AARCH64_OPS_1("w", 8, "b") \
    __AARCH64_OPS_1("w", 16, "h") \
    __AARCH64_OPS_1("w", 32, "") \
    __AARCH64_OPS_1("x", 64, "") \


__AARCH64_OPS


namespace System::ABI::Atomic
{



}
#endif
