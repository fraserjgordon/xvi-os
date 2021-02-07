#include <System/ABI/Atomic/Atomic.hh>

#include <System/ABI/Atomic/AtomicSpinlock.hh>


#if !defined(__ARM_ARCH)
#  error unknown ARM architecture, cannot build
#endif

// This file is only for ARMv8 and later.
#if __ARM_ARCH >= 8


#if __ATOMIC_ASSUME_LSE_ATOMICS

#define __AARCH64_CAS(reg, size, size_name, order, size_suffix, order_suffix) \
extern "C" __uint##size##_t __aarch64_cas##size_name##_##order(__uint##size##_t expect, __uint##size##_t desire, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_cas##size_name##_##order(__uint##size##_t expect, __uint##size##_t desire, __uint##size##_t* ptr) \
{ \
    asm volatile \
    ( \
        "CAS" order_suffix size_suffix " %" reg "[expect], %" reg "[desire], [%[ptr]]" \
        :   [expect] "+r" (expect)\
        :   "0" (expect), \
            [desire] "r" (desire), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return expect; \
}

#define __AARCH64_SWP(reg, size, size_name, order, size_suffix, order_suffix) \
extern "C" __uint##size##_t __aarch64_swp##size_name##_##order(__uint##size##_t desire, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_swp##size_name##_##order(__uint##size##_t desire, __uint##size##_t* ptr) \
{ \
    __uint##size##_t previous; \
    asm volatile \
    ( \
        "SWP" order_suffix size_suffix " %" reg "[desire], %" reg "[previous], [%[ptr]]" \
        :   [previous] "=r" (previous) \
        :   [desire] "r" (desire), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}

#define __AARCH64_LDOP(op, opcode, reg, size, size_name, order, size_suffix, order_suffix) \
extern "C" __uint##size##_t __aarch64_ld##op##size_name##_##order(__uint##size##_t operand, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_ld##op##size_name##_##order(__uint##size##_t operand, __uint##size##_t* ptr) \
{ \
    __uint##size##_t previous; \
    asm volatile \
    ( \
        "LD" opcode order_suffix size_suffix " %" reg "[operand], %" reg "[previous], [%[ptr]]" \
        :   [previous] "=r" (previous) \
        :   [operand] "r" (operand), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}

#define __AARCH64_LDADD(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(add, "ADD", reg, size, size_name, order, size_suffix, order_suffix)

#define __AARCH64_LDEOR(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(eor, "EOR", reg, size, size_name, order, size_suffix, order_suffix)

#define __AARCH64_LDCLR(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(clr, "CLR", reg, size, size_name, order, size_suffix, order_suffix)

#define __AARCH64_LDSET(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDOP(set, "SET", reg, size, size_name, order, size_suffix, order_suffix)

#define __AARCH64_LSE_OPS_0(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_CAS(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_SWP(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDADD(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDEOR(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDCLR(reg, size, size_name, order, size_suffix, order_suffix) \
    __AARCH64_LDSET(reg, size, size_name, order, size_suffix, order_suffix)

#define __AARCH64_LSE_OPS_1(reg, size, size_name, size_suffix) \
    __AARCH64_LSE_OPS_0(reg, size, size_name, relax, size_suffix, "") \
    __AARCH64_LSE_OPS_0(reg, size, size_name, acq, size_suffix, "A") \
    __AARCH64_LSE_OPS_0(reg, size, size_name, rel, size_suffix, "L") \
    __AARCH64_LSE_OPS_0(reg, size, size_name, acq_rel, size_suffix, "AL")

#define __AARCH64_LSE_OPS \
    __AARCH64_LSE_OPS_1("w", 8, 1, "B") \
    __AARCH64_LSE_OPS_1("w", 16, 2, "H") \
    __AARCH64_LSE_OPS_1("w", 32, 4, "") \
    __AARCH64_LSE_OPS_1("x", 64, 8, "") \


#else // __ATOMIC_ASSUME_LSE_ATOMICS

#define __AARCH64_CAS(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
extern "C" __uint##size##_t __aarch64_cas##size_name##_##order(__uint##size##_t expect, __uint##size##_t desire, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_cas##size_name##_##order(__uint##size##_t expect, __uint##size##_t desire, __uint##size##_t* ptr) \
{ \
    __uint##size##_t previous; \
    __uint##size##_t temp; \
    asm volatile \
    ( \
        "1:\n" \
        "LD" acq_suffix "XR" size_suffix " %" reg "[previous], [%[ptr]]\n" \
        "CMP %" reg "[previous], %" reg "[expect]\n" \
        "B.NE 2f\n" \
        "ST" rel_suffix "XR" size_suffix " %w[temp], %" reg "[desire], [%[ptr]]\n" \
        "CMP %w[temp], #1\n" \
        "B.NE 1b\n" \
        "2:\n" \
        :   [previous] "=&r" (previous), \
            [temp] "=&r" (temp) \
        :   [expect] "r" (expect), \
            [desire] "r" (desire), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}

#define __AARCH64_SWP(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
extern "C" __uint##size##_t __aarch64_swp##size_name##_##order(__uint##size##_t desire, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_swp##size_name##_##order(__uint##size##_t desire, __uint##size##_t* ptr) \
{ \
    __uint##size##_t previous; \
    __uint##size##_t temp; \
    asm volatile \
    ( \
        "1:\n" \
        "LD" acq_suffix "XR" size_suffix " %" reg "[previous], [%[ptr]]\n" \
        "ST" rel_suffix "XR" size_suffix " %w[temp], %" reg "[desire], [%[ptr]]\n" \
        "CMP %w[temp], #1\n" \
        "B.NE 1b\n" \
        "2:\n" \
        :   [previous] "=&r" (previous), \
            [temp] "=&r" (temp) \
        :   [desire] "r" (desire), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}

#define __AARCH64_LDOP(op_name, op, reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
extern "C" __uint##size##_t __aarch64_ld##op_name##size_name##_##order(__uint##size##_t operand, __uint##size##_t* ptr); \
__uint##size##_t __aarch64_ld##op_name##size_name##_##order(__uint##size##_t operand, __uint##size##_t* ptr) \
{ \
    __uint##size##_t previous; \
    __uint##size##_t temp1, temp2; \
    asm volatile \
    ( \
        "1:\n" \
        "LD" acq_suffix "XR" size_suffix " %" reg "[previous], [%[ptr]]\n" \
        op " %" reg "[temp1], %" reg "[previous], %" reg "[operand]\n" \
        "ST" rel_suffix "XR" size_suffix " %w[temp2], %" reg "[temp1], [%[ptr]]\n" \
        "CMP %w[temp2], #1\n" \
        "B.NE 1b\n" \
        "2:\n" \
        :   [previous] "=&r" (previous), \
            [temp1] "=&r" (temp1), \
            [temp2] "=&r" (temp2) \
        :   [operand] "r" (operand), \
            [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}

#define __AARCH64_LDADD(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDOP(add, "ADD", reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix)

#define __AARCH64_LDEOR(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDOP(eor, "EOR", reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix)

#define __AARCH64_LDCLR(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDOP(clr, "BIC", reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix)

#define __AARCH64_LDSET(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDOP(set, "ORR", reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix)

#define __AARCH64_LSE_OPS_0(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_CAS(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_SWP(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDADD(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDEOR(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDCLR(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix) \
    __AARCH64_LDSET(reg, size, size_name, order, size_suffix, acq_suffix, rel_suffix)

#define __AARCH64_LSE_OPS_1(reg, size, size_name, size_suffix) \
    __AARCH64_LSE_OPS_0(reg, size, size_name, relax, size_suffix, "", "") \
    __AARCH64_LSE_OPS_0(reg, size, size_name, acq, size_suffix, "A", "") \
    __AARCH64_LSE_OPS_0(reg, size, size_name, rel, size_suffix, "", "L") \
    __AARCH64_LSE_OPS_0(reg, size, size_name, acq_rel, size_suffix, "A", "L")

#define __AARCH64_LSE_OPS \
    __AARCH64_LSE_OPS_1("w", 8, 1, "B") \
    __AARCH64_LSE_OPS_1("w", 16, 2, "H") \
    __AARCH64_LSE_OPS_1("w", 32, 4, "") \
    __AARCH64_LSE_OPS_1("x", 64, 8, "")


#endif // __ATOMIC_ASSUME_LSE_ATOMICS


__AARCH64_LSE_OPS


namespace System::ABI::Atomic
{



}
#endif
