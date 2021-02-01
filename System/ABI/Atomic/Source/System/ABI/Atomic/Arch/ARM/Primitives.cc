#include <System/ABI/Atomic/Atomic.hh>

#include <System/ABI/Atomic/AtomicSpinlock.hh>


namespace System::ABI::Atomic
{


// Auto-detect various architecture options.
//
//! @TODO: these should be refactored into __CRT_HAS_LDREXB etc.
#if !defined(__ARM_ARCH)
#  error unknown ARM architecture, cannot build
#endif
#if !defined(__CRT_ASSUME_ARMV7) && __ARM_ARCH >= 7
#  define __CRT_ASSUME_ARMV7 1
#endif
#if !defined(__CRT_ASSUME_ARMV6K) && (__ARM_ARCH >= 7 || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6KZ__))
#  define __CRT_ASSUME_ARMV6K 1
#endif
#if !defined(__CRT_ASSUME_ARMV6) && __ARM_ARCH >= 6
#  define __CRT_ASSUME_ARMV6 1
#endif


// Pick the correct form of memory barrier.
#ifdef __CRT_ASSUME_ARMV7
// ARMv7 defines a dedicated opcode for barriers.
#  define ARM_BARRIER "DMB ISH"
#elif __CRT_ASSUME_ARMV6
// ARMv6 uses a coprocessor write to create an inter-CPU barrier.
#  define ARM_BARRIER "MCR p15,0,r0,c7,c10,5"
#else
// Earlier ARM variants only define a very heavyweight barrier (blocks until memory accesses complete!).
#  define ARM_BARRIER "MCR p15,0,%[zero],c7,c10,4"
#endif


#if __CRT_ASSUME_ARMV6K
// We have ldrexb/strexb, ldrexh/strexh, ldrex/strex and ldrexd/strexd available.
static constexpr __size_t MaxAtomicSize = sizeof(__uint64_t);
#elif __CRT_ASSUME_ARMV6
// Only ldrex/strex instructions are available.
static constexpr __size_t MaxAtomicSize = sizeof(__uint32_t);
#else
// Only swp and swpb are available. Atomics are emulated using locks.
static constexpr __size_t MaxAtomicSize = 0;
#endif


bool __Atomic_is_lock_free(__size_t size, const volatile void* ptr)
{
#if __CRT_ASSUME_ARMV6
    // Check the size of the access.
    if (size > MaxAtomicSize)
    {
        // Unsupported size for lock-free operation.
        return false;
    }
    
    // Non power-of-two sizes are not lock-free.
    if (size & (size - 1))
    {
        return false;
    }
    
    // Check that the alignment is appropriate for the size.
    if (ptr != nullptr && (__uintptr_t(ptr) & (size - 1) ) != 0)
    {
        // The pointer is misaligned so is not lock-free.
        return false;
    }

    return true;
#else
    // Nothing has lock-free atomic access.
    return false;
#endif
}


bool __Atomic_test_and_set(volatile void* ptr)
{
    return __Atomic_test_and_set_explicit(ptr, __memory_order_seq_cst);
}

bool __Atomic_test_and_set_explicit(volatile void* ptr, int)
{
#if __CRT_ASSUME_ARMV6
    // Use an atomic exchange in preference to the deprecated "swp" instruction.
    return __Atomic_exchange_1(ptr, 1, 0);
#else
    __uint8_t previous;
    asm volatile
    (
        ARM_BARRIER "\n\t"
        "SWPB   %[previous], %[value], [%[ptr]]\n\t"
        ARM_BARRIER "\n\t"
        : [previous] "=&r" (previous)
        : [value] "r" (1),
          [ptr] "r" (ptr),
          [zero] "r" (0)
        : "memory"
    );
    return previous;
#endif
}

void __Atomic_clear(volatile void* ptr, int)
{
    __Atomic_store_1(ptr, 0, 0);
}


bool __Atomic_test_and_set_1(volatile void* ptr, int order)
{
#if __CRT_ASSUME_ARMV6K
    // Use an exchange (via ldrexb/strexb) operation to set the flag to 0x01.
    return __Atomic_exchange_1(ptr, 1, order);
#elif __CRT_ASSUME_ARMV6
    // Although not strictly conforming (because it writes to more than the specified byte) we need to use a word access
    // as it is the only (non-deprecated) method available on ARMv6.
    auto aligned_ptr = reinterpret_cast<volatile void*>(__uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1));
    __uint32_t value = 1 << 8 * (__uintptr_t(ptr) - __uintptr_t(aligned_ptr));
    __uint32_t previous;
    asm volatile
    (
        ARM_BARRIER "\n\t"
        "1:         \n\t"
        "LDREX      %[previous], [%[ptr]]\n\t"
        "ANDS       %[previous], %[previous], %[value]\n\t"
        "BNE        2f\n\t"
        "STREX      %[previous], %[value], [%[ptr]]\n\t"
        "TEQ        %[previous], #0\n\t"
        "BNE        1b\n\t"
        "2:         \n\t"
        ARM_BARRIER "\n\t"
        : [previous] "=&r" (previous)
        : [value] "r" (value),
          [ptr] "r" (aligned_ptr)
        : "memory"
    );
    return previous;
#else
    // Use the swpb instruction to implement the operation.
    return __Atomic_exchange_1(ptr, 1, order);
#endif
}

bool __Atomic_test_and_set_2(volatile void* ptr, int order)
{
    // Just use the byte implementation.
    return __Atomic_test_and_set_1(ptr, order);
}

bool __Atomic_test_and_set_4(volatile void* ptr, int order)
{
#if __CRT_ASSUME_ARMV6
    // Use an ldrex/strex pair.
    __uint32_t expect = 0;
    __uint32_t desire = 1;
    return !__Atomic_compare_exchange_4(ptr, &expect, desire, order, order);
#else
    // Use swpb.
    return __Atomic_test_and_set_1(ptr, order);
#endif
}

bool __Atomic_test_and_set_8(volatile void* ptr, int order)
{
    return __Atomic_test_and_set_4(ptr, order);
}

bool __Atomic_test_and_set_16(volatile void* ptr, int order)
{
    return __Atomic_test_and_set_4(ptr, order);
}


void __Atomic_signal_fence(int)
{
    // Does nothing; the external function call was sufficient.
    asm volatile ("" ::: "memory");
}

void __Atomic_thread_fence(int)
{
    // Insert a full memory barrier.
#if __CRT_ASSUME_ARMV6
    asm volatile(ARM_BARRIER ::: "memory");
#else
    asm volatile(ARM_BARRIER :: [zero] "r" (0) : "memory");
#endif
}


// Appropriately aligned loads are always atomic.
#define ATOMIC_LOAD_N(n, type) \
type __Atomic_load_##n(const volatile void* ptr, int) \
{ \
    auto val = *static_cast<const volatile type*>(ptr); \
    __Atomic_thread_fence(0); \
    return val; \
}

// Appropriately aligned stores are always atomic.
#define ATOMIC_STORE_N(n, type) \
void __Atomic_store_##n(volatile void* ptr, type value, int) \
{ \
    __Atomic_thread_fence(0); \
    *static_cast<volatile type*>(ptr) = value; \
    __Atomic_thread_fence(0); \
}

// Use an LDREX/STREX sequence on ARMv6+ but SWP on earlier architectures.
#if __CRT_ASSUME_ARMV6
#define ATOMIC_EXCHANGE_N(n, type, suffix) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int) \
{ \
    type previous; \
    __uint32_t temp; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX" suffix " %[previous], [%[ptr]]\n\t" \
        "STREX" suffix " %[temp], %[value], [%[ptr]]\n\t" \
        "TEQ    %[temp], #0\n\t" \
        "BNE    1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp) \
        : [value] "r" (value), \
          [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
}
// Like ATOMIC_EXCHANGE_N but uses a word-sized access to emulate smaller operations.
#define ATOMIC_EXCHANGE_WORD_N(n, type) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = ~(((1 << (8 * n)) - 1) << shift); \
    __uint32_t val = value << shift; \
    type previous; \
    __uint32_t temp, temp2; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX   %[previous], [%[ptr]]\n\t" \
        "AND        %[temp2], %[previous], %[mask]\n\t" \
        "ORR        %[temp2], %[temp2], %[value]\n\t" \
        "STREX      %[temp], %[temp2], [%[ptr]]\n\t" \
        "TEQ        %[temp], #0\n\t" \
        "BNE        1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp), \
          [temp2] "=&r" (temp2) \
        : [ptr] "r" (aligned_ptr), \
          [mask] "r" (mask), \
          [value] "r" (val) \
        : "memory" \
    ); \
    return (previous & ~mask) >> shift; \
}
#else // __CRT_ASSUME_ARMV6
#define ATOMIC_EXCHANGE_N(n, type, suffix) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int) \
{ \
    type previous; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "SWP" suffix "  %[previous], %[value], [%[ptr]]\n\t" \
        ARM_BARRIER "\n\t" \
        : [previous] "=&r" (previous) \
        : [value] "r" (value), \
          [ptr] "r" (ptr), \
          [zero] "r" (0) \
        : "memory" \
    ); \
    return previous; \
}
#endif // __CRT_ASSUME_ARMV6

// Only ARMv6 and above have lockless cmpxchg.
#if __CRT_ASSUME_ARMV6
#  define ATOMIC_CMPXCHG_N(n, type, suffix) \
bool __Atomic_compare_exchange_##n(volatile void* ptr, void* expect, type desired, int, int) \
{ \
    auto expect_ptr = static_cast<type*>(expect); \
    __uint32_t fail; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX" suffix " %[expect_out], [%[ptr]]\n\t" \
        "MOV    %[result], #-1\n\t" \
        "TEQ    %[expect_out], %[expect_in]\n\t" \
        "STREX" suffix "EQ %[result], %[desired], [%[ptr]]\n\t" \
        "TEQ    %[result], #1\n\t" \
        "BEQ    1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [result] "=&r" (fail), \
          [expect_out] "=&r" (*expect_ptr) \
        : [desired] "r" (desired), \
          [ptr] "r" (ptr), \
          [expect_in] "r" (*expect_ptr) \
        : "memory" \
    ); \
    return !fail; \
}

#  define ATOMIC_CMPXCHG_WORD_N(n, type) \
bool __Atomic_compare_exchange_##n(volatile void* ptr, void* expect, type desire, int, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = ~(((1 << (8 * n)) - 1) << shift); \
    __uint32_t val = desire << shift; \
    __uint32_t exp = *static_cast<type*>(expect) << shift; \
    __uint32_t temp; \
    type previous; \
    __uint32_t fail; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX   %[previous], [%[ptr]]\n\t" \
        "AND        %[temp], %[previous], %[mask]\n\t" \
        "ORR        %[result], %[temp], %[expect]\n\t" \
        "ORR        %[temp], %[temp], %[desire]\n\t" \
        "TEQ        %[result], %[previous]\n\t" \
        "MOV        %[result], #-1\n\t" \
        "STREXEQ    %[result], %[temp], [%[ptr]]\n\t" \
        "TEQ        %[result], #1\n\t" \
        "BEQ        1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [result] "=&r" (fail), \
          [previous] "=&r" (previous), \
          [temp] "=&r" (temp) \
        : [ptr] "r" (aligned_ptr), \
          [mask] "r" (mask), \
          [desire] "r" (val), \
          [expect] "r" (exp) \
        : "memory" \
    ); \
    *static_cast<type*>(expect) = (previous & ~mask) >> shift; \
    return !fail; \
}

// Inline LDREX/STREX for arithmetic operations.
#  define ATOMIC_OP_N(n, type, suffix, name, op, op2) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int) \
{ \
    type previous; \
    type result; \
    __uint32_t temp; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX" suffix " %[previous], [%[ptr]]\n\t" \
        op "    %[result], %[previous], %[value]\n\t" \
        op2 "   %[result], %[result]\n\t" \
        "STREX" suffix "eq %[temp], %[result], [%[ptr]]\n\t" \
        "TEQ    %[temp], #0\n\t" \
        "BNE    1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp), \
          [result] "=&r" (result) \
        : [value] "r" (value), \
          [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return previous; \
} \
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    type previous; \
    type result; \
    __uint32_t temp; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX" suffix " %[previous], [%[ptr]]\n\t" \
        op "    %[result], %[previous], %[value]\n\t" \
        op2 "   %[result], %[result]\n\t" \
        "STREX" suffix "eq %[temp], %[result], [%[ptr]]\n\t" \
        "TEQ    %[temp], #0\n\t" \
        "BNE    1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp), \
          [result] "=&r" (result) \
        : [value] "r" (value), \
          [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return result; \
}

#  define ATOMIC_OP_WORD_N(n, type, name, op, op2) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = ~(((1 << (8 * n)) - 1) << shift); \
    __uint32_t val = value << shift; \
    __uint32_t temp;\
    type previous; \
    type result; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX   %[previous], [%[ptr]]\n\t" \
        "AND        %[temp], %[previous], %[mask]\n\t" \
        op "        %[result], %[previous], %[value]\n\t" \
        op2 "       %[result], %[result]\n\t" \
        "BIC        %[result], %[result], %[mask]\n\t" \
        "ORR        %[temp], %[temp], %[result]\n\t" \
        "STREX      %[result], %[temp], [%[ptr]]\n\t" \
        "TEQ        %[result], #0\n\t" \
        "BNE        1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp), \
          [result] "=&r" (result) \
        : [ptr] "r" (aligned_ptr), \
          [mask] "r" (mask), \
          [value] "r" (val) \
        : "memory" \
    ); \
    return (previous & ~mask) >> shift; \
} \
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = ~(((1 << (8 * n)) - 1) << shift); \
    __uint32_t val = value << shift; \
    type updated; \
    type previous; \
    type result; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREX   %[previous], [%[ptr]]\n\t" \
        "AND        %[updated], %[previous], %[mask]\n\t" \
        op "        %[result], %[previous], %[value]\n\t" \
        op2 "       %[result], %[result]\n\t" \
        "BIC        %[result], %[result], %[mask]\n\t" \
        "ORR        %[updated], %[updated], %[result]\n\t" \
        "STREX      %[result], %[updated], [%[ptr]]\n\t" \
        "TEQ        %[result], #0\n\t" \
        "BNE        1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [previous] "=&r" (previous), \
          [updated] "=&r" (updated), \
          [result] "=&r" (result) \
        : [ptr] "r" (aligned_ptr), \
          [mask] "r" (mask), \
          [value] "r" (val) \
        : "memory" \
    ); \
    return (updated & ~mask) >> shift; \
}
#endif // __CRT_ASSUME_ARMV6

// Similar to ATOMIC_OP_N but using locked accesses instead of atomic accesses.
#define ATOMIC_OP_LOCKED_N(n, type, name, op1, op2) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int) \
{ \
    /* Get the lock address for this object then lock it. */ \
    auto lock = g_atomicSpinlocks.getLock(ptr); \
    __Atomic_lock_acquire(lock); \
    \
    /* Perform the operation */ \
    auto type_ptr = static_cast<volatile type*>(ptr); \
    type previous = *type_ptr; \
    *type_ptr = op2 (previous op1 value); \
    \
    /* Unlock the spinlock and return the old value */ \
    __Atomic_lock_release(lock); \
    return previous; \
} \
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    /* Get the lock address for this object then lock it. */ \
    auto lock = g_atomicSpinlocks.getLock(ptr); \
    __Atomic_lock_acquire(lock); \
    \
    /* Perform the operation */ \
    auto type_ptr = static_cast<volatile type*>(ptr); \
    auto new_value = op2 (*type_ptr op1 value); \
    *type_ptr = new_value; \
    \
    /* Unlock the spinlock and return the new value */ \
    __Atomic_lock_release(lock); \
    return new_value; \
}

// Similar to ATOMIC_OP_N but calling __Atomic_compare_exchange_N instead of inlining it.
/*#define ATOMIC_OP_CMPXCHG_N(n, type, name, op1, op2) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int) \
{ \
    type fetch = *static_cast<volatile type*>(ptr); \
    while (!__Atomic_compare_exchange_##n(ptr, &fetch, op2 (fetch op1 value), 0, 0)) \
        ; \
    return fetch; \
} \
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    type fetch = *static_cast<volatile type*>(ptr); \
    type desire = op2 (fetch op1 value); \
    while (!__Atomic_compare_exchange_##n(ptr, &fetch, desire, 0, 0)) \
        desire = op2 (fetch op1 value); \
    return desire; \
}*/


ATOMIC_LOAD_N(1, __uint8_t)
ATOMIC_LOAD_N(2, __uint16_t)
ATOMIC_LOAD_N(4, __uint32_t)

ATOMIC_STORE_N(1, __uint8_t)
ATOMIC_STORE_N(2, __uint16_t)
ATOMIC_STORE_N(4, __uint32_t)


// Atomic exchanges.
#if __CRT_ASSUME_ARMV6K
// ARMv6k has all operations natively.
ATOMIC_EXCHANGE_N(1, __uint8_t, "B")
ATOMIC_EXCHANGE_N(2, __uint16_t, "H")
ATOMIC_EXCHANGE_N(4, __uint32_t, "")
#elif __CRT_ASSUME_ARMV6
// Emulate byte and halfword exchange using ldrex/strex.
ATOMIC_EXCHANGE_WORD_N(1, __uint8_t)
ATOMIC_EXCHANGE_WORD_N(2, __uint16_t)
ATOMIC_EXCHANGE_N(4, __uint32_t, "")
#else
// Byte exchange is directly available but halfword needs to be emulated.
ATOMIC_EXCHANGE_N(1, __uint8_t, "B")
__uint16_t __Atomic_exchange_2(volatile void* ptr, __uint16_t value, int order)
{
    __Atomic_exchange(2, ptr, &value, &value, order);
    return value;
}
ATOMIC_EXCHANGE_N(4, __uint32_t, "")
#endif


// Atomic compare-exchange.
#if __CRT_ASSUME_ARMV6K
// ARMv6k has all operations natively.
ATOMIC_CMPXCHG_N(1, __uint8_t, "B")
ATOMIC_CMPXCHG_N(2, __uint16_t, "H")
ATOMIC_CMPXCHG_N(4, __uint32_t, "")
#elif __CRT_ASSUME_ARMV6
// Emulate byte and halfword compare-exchange using ldrex/strex
ATOMIC_CMPXCHG_WORD_N(1, __uint8_t)
ATOMIC_CMPXCHG_WORD_N(2, __uint16_t)
ATOMIC_CMPXCHG_N(4, __uint32_t, "")
#else
// All of the compare-exchange operations need to be emulated.
bool __Atomic_compare_exchange_1(volatile void* ptr, void* expect, __uint8_t desire, int, int)
{
    return __Atomic_compare_exchange(1, ptr, expect, &desire, 0, 0);
}
bool __Atomic_compare_exchange_2(volatile void* ptr, void* expect, __uint16_t desire, int, int)
{
    return __Atomic_compare_exchange(2, ptr, expect, &desire, 0, 0);
}
bool __Atomic_compare_exchange_4(volatile void* ptr, void* expect, __uint32_t desire, int, int)
{
    return __Atomic_compare_exchange(4, ptr, expect, &desire, 0, 0);
}
#endif // __CRT_ASSUME_ARMV6K


// 32-bit arithmetic operations.
#if __CRT_ASSUME_ARMV6
// We have arithmetic ops with the LDREX/STREX inlined for ARMv6+.
ATOMIC_OP_N(4, __uint32_t, "", add, "ADD", "@")
ATOMIC_OP_N(4, __uint32_t, "", sub, "SUB", "@")
ATOMIC_OP_N(4, __uint32_t, "", and, "AND", "@")
ATOMIC_OP_N(4, __uint32_t, "", or,  "ORR", "@")
ATOMIC_OP_N(4, __uint32_t, "", xor, "EOR", "@")
ATOMIC_OP_N(4, __uint32_t, "", nand, "AND", "MVN")
#else
// Earlier ARMs need locking so do not have inlined operations.
ATOMIC_OP_LOCKED_N(4, __uint32_t, add, +,)
ATOMIC_OP_LOCKED_N(4, __uint32_t, sub, -,)
ATOMIC_OP_LOCKED_N(4, __uint32_t, and, &,)
ATOMIC_OP_LOCKED_N(4, __uint32_t, or,  |,)
ATOMIC_OP_LOCKED_N(4, __uint32_t, xor, ^,)
ATOMIC_OP_LOCKED_N(4, __uint32_t, nand, &, ~)
#endif


// 16-bit arithmetic operations.
#if __CRT_ASSUME_ARMV6K
// We have halfword arithmetic ops with the LDREX/STREX inlined only from ARMv6k.
ATOMIC_OP_N(2, __uint16_t, "H", add, "ADD", "@")
ATOMIC_OP_N(2, __uint16_t, "H", sub, "SUB", "@")
ATOMIC_OP_N(2, __uint16_t, "H", and, "AND", "@")
ATOMIC_OP_N(2, __uint16_t, "H", or,  "ORR", "@")
ATOMIC_OP_N(2, __uint16_t, "H", xor, "EOR", "@")
ATOMIC_OP_N(2, __uint16_t, "H", nand, "AND", "MVN")
#elif __CRT_ASSUME_ARMV6
// We can emulate halfword arithmetic using LDREX/STREX on ARMv6.
ATOMIC_OP_WORD_N(2, __uint16_t, add, "ADD", "@")
ATOMIC_OP_WORD_N(2, __uint16_t, sub, "SUB", "@")
ATOMIC_OP_WORD_N(2, __uint16_t, and, "AND", "@")
ATOMIC_OP_WORD_N(2, __uint16_t, or,  "ORR", "@")
ATOMIC_OP_WORD_N(2, __uint16_t, xor, "EOR", "@")
ATOMIC_OP_WORD_N(2, __uint16_t, nand, "AND", "MVN")
#else
// Earlier ARMs need locking for atomic halfword access so do not have inlined operations.
ATOMIC_OP_LOCKED_N(2, __uint16_t, add, +,)
ATOMIC_OP_LOCKED_N(2, __uint16_t, sub, -,)
ATOMIC_OP_LOCKED_N(2, __uint16_t, and, &,)
ATOMIC_OP_LOCKED_N(2, __uint16_t, or,  |,)
ATOMIC_OP_LOCKED_N(2, __uint16_t, xor, ^,)
ATOMIC_OP_LOCKED_N(2, __uint16_t, nand, &, ~)
#endif


// 8-bit arithmetic operations.
#if __CRT_ASSUME_ARMV6K
// We have byte arithmetic ops with the LDREX/STREX inlined only from ARMv6K.
ATOMIC_OP_N(1, __uint8_t, "B", add, "ADD", "@")
ATOMIC_OP_N(1, __uint8_t, "B", sub, "SUB", "@")
ATOMIC_OP_N(1, __uint8_t, "B", and, "AND", "@")
ATOMIC_OP_N(1, __uint8_t, "B", or,  "ORR", "@")
ATOMIC_OP_N(1, __uint8_t, "B", xor, "EOR", "@")
ATOMIC_OP_N(1, __uint8_t, "B", nand, "AND", "MVN")
#elif __CRT_ASSUME_ARMV6
// We can emulate byte arithmetic using LDREX/STREX on ARMv6.
ATOMIC_OP_WORD_N(1, __uint8_t, add, "ADD", "@")
ATOMIC_OP_WORD_N(1, __uint8_t, sub, "SUB", "@")
ATOMIC_OP_WORD_N(1, __uint8_t, and, "AND", "@")
ATOMIC_OP_WORD_N(1, __uint8_t, or,  "ORR", "@")
ATOMIC_OP_WORD_N(1, __uint8_t, xor, "EOR", "@")
ATOMIC_OP_WORD_N(1, __uint8_t, nand, "AND", "MVN")
#else
// Earlier ARMs need locking for atomic byte access so do not have inlined operations.
ATOMIC_OP_LOCKED_N(1, __uint8_t, add, +,)
ATOMIC_OP_LOCKED_N(1, __uint8_t, sub, -,)
ATOMIC_OP_LOCKED_N(1, __uint8_t, and, &,)
ATOMIC_OP_LOCKED_N(1, __uint8_t, or,  |,)
ATOMIC_OP_LOCKED_N(1, __uint8_t, xor, ^,)
ATOMIC_OP_LOCKED_N(1, __uint8_t, nand, &, ~)
#endif


// ARMv6K can do 64-bit atomic loads and stores using LDREXD/STREXD pairs.
#if __CRT_ASSUME_ARMV6K
__uint64_t __Atomic_load_8(const volatile void* ptr, int)
{
    register __uint32_t hi asm("r3");
    register __uint32_t lo asm("r2");
    __uint32_t temp;
    asm volatile
    (
        ARM_BARRIER "\n\t"
        "1: LDREXD  %[lo], %[hi], [%[ptr]]\n\t"
        "STREXD     %[temp], %[lo], %[hi], [%[ptr]]\n\t"
        "TEQ        %[temp], #0\n\t"
        "BNE        1b\n\t"
        ARM_BARRIER "\n\t"
        : [lo] "=&r" (lo),
          [hi] "=&r" (hi),
          [temp] "=&r" (temp)
        : [ptr] "r" (ptr)
        : "memory"
    );
    return (__uint64_t(hi) << 32) | lo;
}

void __Atomic_store_8(volatile void* ptr, __uint64_t value, int)
{
    register __uint32_t hi asm("r3") = value >> 32;
    register __uint32_t lo asm("r2") = __uint32_t(value);
    register __uint32_t old_hi asm("r5");
    register __uint32_t old_lo asm("r4");
    register __uint32_t new_ptr asm("r0") = __uint32_t(ptr);
    __uint32_t temp;
    asm volatile
    (
        ARM_BARRIER "\n\t"
        "1: LDREXD  %[old_lo], %[old_hi], [%[ptr]]\n\t"
        "STREXD     %[temp], %[lo], %[hi], [%[ptr]]\n\t"
        "TEQ        %[temp], #0\n\t"
        "BNE        1b\n\t"
        ARM_BARRIER "\n\t"
        : [old_lo] "=&r" (old_lo),
          [old_hi] "=&r" (old_hi),
          [temp] "=&r" (temp)
        : [lo] "r" (lo),
          [hi] "r" (hi),
          [ptr] "r" (new_ptr)
        : "memory"
    );
}

__uint64_t __Atomic_exchange_8(volatile void* ptr, __uint64_t value, int)
{
    register __uint32_t hi asm("r3") = value >> 32;
    register __uint32_t lo asm("r2") = __uint32_t(value);
    register __uint32_t old_hi asm("r5");
    register __uint32_t old_lo asm("r4");
    __uint32_t temp;
    asm volatile
    (
        ARM_BARRIER "\n\t"
        "1: LDREXD  %[old_lo], %[old_hi], [%[ptr]]\n\t"
        "STREXD     %[temp], %[lo], %[hi], [%[ptr]]\n\t"
        "TEQ        %[temp], #0\n\t"
        "BNE        1b\n\t"
        ARM_BARRIER "\n\t"
        : [old_lo] "=&r" (old_lo),
          [old_hi] "=&r" (old_hi),
          [temp] "=&r" (temp)
        : [lo] "r" (lo),
          [hi] "r" (hi),
          [ptr] "r" (ptr)
        : "memory"
    );
    return (__uint64_t(old_hi) << 32) | old_lo;
}

bool __Atomic_compare_exchange_8(volatile void* ptr, void* expect, __uint64_t desire, int, int)
{
    auto expect_ptr = static_cast<__uint64_t*>(expect);
    register __uint32_t hi asm("r3") = desire >> 32;
    register __uint32_t lo asm("r2") = __uint32_t(desire);
    register __uint32_t old_hi asm("r5");
    register __uint32_t old_lo asm("r4");
    register __uint32_t expect_hi asm("r7") = *expect_ptr >> 32;
    register __uint32_t expect_lo asm("r6") = __uint32_t(*expect_ptr);
    __uint32_t fail;
    asm volatile
    (
        ARM_BARRIER "\n\t"
        "1: LDREXD  %[old_lo], %[old_hi], [%[ptr]]\n\t"
        "MOV        %[fail], #-1\n\t"
        "TEQ        %[old_lo], %[expect_lo]\n\t"
        "TEQEQ      %[old_hi], %[expect_hi]\n\t"
        "STREXDEQ   %[fail], %[lo], %[hi], [%[ptr]]\n\t"
        "TEQ        %[fail], #1\n\t"
        "BEQ        1b\n\t"
        ARM_BARRIER "\n\t"
        : [old_lo] "=&r" (old_lo),
          [old_hi] "=&r" (old_hi),
          [fail] "=&r" (fail)
        : [lo] "r" (lo),
          [hi] "r" (hi),
          [ptr] "r" (ptr),
          [expect_lo] "r" (expect_lo),
          [expect_hi] "r" (expect_hi)
        : "memory"
    );
    return !fail;
}

#  define ATOMIC_OP_8(name, op_lo, op_hi, op2) \
__uint64_t __Atomic_fetch_##name##_8(volatile void* ptr, __uint64_t value, int) \
{ \
    register __uint32_t hi asm("r3") = value >> 32; \
    register __uint32_t lo asm("r2") = __uint32_t(value); \
    register __uint32_t old_hi asm("r5"); \
    register __uint32_t old_lo asm("r4"); \
    register __uint32_t new_hi asm("r7"); \
    register __uint32_t new_lo asm("r6"); \
    __uint32_t fail; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREXD  %[old_lo], %[old_hi], [%[ptr]]\n\t" \
        op_lo "     %[new_lo], %[old_lo], %[lo]\n\t" \
        op_hi "     %[new_hi], %[old_hi], %[hi]\n\t" \
        op2 "       %[new_lo], %[new_lo]\n\t" \
        op2 "       %[new_hi], %[new_hi]\n\t" \
        "STREXD     %[fail], %[new_lo], %[new_hi], [%[ptr]]\n\t" \
        "TEQ        %[fail], #0\n\t" \
        "BNE        1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [old_lo] "=&r" (old_lo), \
          [old_hi] "=&r" (old_hi), \
          [new_lo] "=&r" (new_lo), \
          [new_hi] "=&r" (new_hi), \
          [fail] "=&r" (fail) \
        : [lo] "r" (lo), \
          [hi] "r" (hi), \
          [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return (__uint64_t(old_hi) << 32) | old_lo; \
} \
\
__uint64_t __Atomic_##name##_fetch_8(volatile void* ptr, __uint64_t value, int) \
{ \
    register __uint32_t hi asm("r3") = value >> 32; \
    register __uint32_t lo asm("r2") = __uint32_t(value); \
    register __uint32_t old_hi asm("r5"); \
    register __uint32_t old_lo asm("r4"); \
    register __uint32_t new_hi asm("r7"); \
    register __uint32_t new_lo asm("r6"); \
    __uint32_t fail; \
    asm volatile \
    ( \
        ARM_BARRIER "\n\t" \
        "1: LDREXD  %[old_lo], %[old_hi], [%[ptr]]\n\t" \
        op_lo "     %[new_lo], %[old_lo], %[lo]\n\t" \
        op_hi "     %[new_hi], %[old_hi], %[hi]\n\t" \
        op2 "       %[new_lo], %[new_lo]\n\t" \
        op2 "       %[new_hi], %[new_hi]\n\t" \
        "STREXD     %[fail], %[new_lo], %[new_hi], [%[ptr]]\n\t" \
        "TEQ        %[fail], #0\n\t" \
        "BNE        1b\n\t" \
        ARM_BARRIER "\n\t" \
        : [old_lo] "=&r" (old_lo), \
          [old_hi] "=&r" (old_hi), \
          [new_lo] "=&r" (new_lo), \
          [new_hi] "=&r" (new_hi), \
          [fail] "=&r" (fail) \
        : [lo] "r" (lo), \
          [hi] "r" (hi), \
          [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return (__uint64_t(new_hi) << 32) | new_lo; \
}

ATOMIC_OP_8(add, "ADDS", "ADC", "@")
ATOMIC_OP_8(sub, "SUBS", "SBC", "@")
ATOMIC_OP_8(and, "AND",  "AND", "@")
ATOMIC_OP_8(or,  "ORR",  "ORR", "@")
ATOMIC_OP_8(xor, "EOR",  "EOR", "@")
ATOMIC_OP_8(nand, "AND", "AND", "MVN")
#else // __CRT_ASSUME_ARMV6K
// Locks need to be used for 64-bit accesses.
__uint64_t __Atomic_load_8(const volatile void* ptr, int order)
{
    __uint64_t result;
    __Atomic_load(8, ptr, &result, order);
    return result;
}

void __Atomic_store_8(volatile void* ptr, __uint64_t value, int order)
{
    __Atomic_store(8, ptr, &value, order);
}

__uint64_t __Atomic_exchange_8(volatile void* ptr, __uint64_t value, int order)
{
    __uint64_t old;
    __Atomic_exchange(8, ptr, &value, &old, order);
    return old;
}

bool __Atomic_compare_exchange_8(volatile void* ptr, void* expect, __uint64_t desire, int succeed, int fail)
{
    return __Atomic_compare_exchange(8, ptr, expect, &desire, succeed, fail);
}

ATOMIC_OP_LOCKED_N(8, __uint64_t, add, +, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, sub, -, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, and, &, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, or,  |, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, xor, ^, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, nand, &, ~)
#endif // __CRT_ASSUME_ARMV6K


// Utility for 128-bit comparisons.
static constexpr bool operator==(const __uint128_t& a, const __uint128_t& b)
{
    return a.__elem[0] == b.__elem[0] && a.__elem[1] == b.__elem[1];
}

// Use locks to implement 128-bit loads.
__uint128_t __Atomic_load_16(const volatile void* ptr, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    __Atomic_lock_acquire(lock);
    __uint128_t value = *static_cast<const __uint128_t*>(const_cast<const void*>(ptr));
    __Atomic_lock_release(lock);
    return value;
}

// Use locks to implement 128-bit stores.
void __Atomic_store_16(volatile void* ptr, __uint128_t value, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    __Atomic_lock_acquire(lock);
    *static_cast<__uint128_t*>(const_cast<void*>(ptr)) = value;
    __Atomic_lock_release(lock);
}

// Use locks to implement 128-bit exchanges.
__uint128_t __Atomic_exchange_16(volatile void* ptr, __uint128_t value, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    auto uint128_ptr = static_cast<__uint128_t*>(const_cast<void*>(ptr));
    __Atomic_lock_acquire(lock);
    __uint128_t old = *uint128_ptr;
    *uint128_ptr = value;
    __Atomic_lock_release(lock);
    return old;
}

// Use locks to implement 128-bit compare-exchange.
bool __Atomic_compare_exchange_16(volatile void* ptr, void* expect, __uint128_t desire, int, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    auto expect_ptr = static_cast<__uint128_t*>(expect);
    auto uint128_ptr = static_cast<__uint128_t*>(const_cast<void*>(ptr));
    bool result;
    __Atomic_lock_acquire(lock);
    if (auto current = *uint128_ptr; current == *expect_ptr)
    {
        *uint128_ptr = desire;
        result = true;
    }
    else
    {
        *expect_ptr = current;
        result = false;
    }
    __Atomic_lock_release(lock);
    return result;
}

//! @TODO: 128-bit arithmetic operations (if any compiler actually calls them in 32-bit mode...)


#if __CRT_ASSUME_ARMV6K
void __Atomic_lock_acquire(volatile __uintptr_t* lock)
{
    __uint32_t temp;
    asm volatile
    (
        "1:         \n\t"
        "LDREX      %[temp], [%[ptr]]\n\t"
        "TEQ        %[temp], #0\n\t"
        "WFENE      \n\t"
        "STREXEQ    %[temp], %[value], [%[ptr]]\n\t"
        "TEQEQ      %[temp], #0\n\t"
        "BNE    1b"
        : [temp] "=&r" (temp)
        : [value] "r" (1),
          [ptr] "r" (lock)
        : "memory"
    );
    __Atomic_thread_fence(__memory_order_acquire);
}
void __Atomic_lock_release(volatile __uintptr_t* lock)
{
    __Atomic_thread_fence(__memory_order_release);
    asm volatile
    (
        "STR    %[zero], [%[ptr]]\n\t"
        "SEV    \n\t"
        :
        : [zero] "r" (0),
          [ptr] "r" (lock)
        : "memory"
    );
}
#elif __CRT_ASSUME_ARMV6
void __Atomic_lock_acquire(volatile __uintptr_t* lock)
{
    __uint32_t temp;
    asm volatile
    (
        "1:         \n\t"
        "LDREX      %[temp], [%[ptr]]\n\t"
        "TEQ        %[temp], #0\n\t"
        "STREXEQ    %[temp], %[value], [%[ptr]]\n\t"
        "TEQEQ      %[temp], #0\n\t"
        "BNE    1b"
        : [temp] "=&r" (temp)
        : [value] "r" (1),
          [ptr] "r" (lock)
        : "memory"
    );
    __Atomic_thread_fence(__memory_order_acquire);
}
void __Atomic_lock_release(volatile __uintptr_t* lock)
{
    __Atomic_thread_fence(__memory_order_release);
    asm volatile
    (
        "STR    %[zero], [%[ptr]]\n\t"
        :
        : [zero] "r" (0),
          [ptr] "r" (lock)
        : "memory"
    );
}
#else
void __Atomic_lock_acquire(volatile __uintptr_t* lock)
{
    asm volatile
    (
        "1:     \n\t"
        "SWP    %[value], %[value], [%[ptr]]\n\t"
        "TEQ    %[value], #0\n\t"
        "BNE    1b"
        :
        : [value] "r" (1),
          [ptr] "r" (lock)
        : "memory"
    );
    __Atomic_thread_fence(__memory_order_acquire);
}
void __Atomic_lock_release(volatile __uintptr_t* lock)
{
    __Atomic_thread_fence(__memory_order_release);
    asm volatile
    (
        "STR    %[zero], [%[ptr]]\n\t"
        :
        : [zero] "r" (0),
          [ptr] "r" (lock)
        : "memory"
    );
}
#endif


void __Atomic_load(__size_t n, const volatile void* ptr, void* result, int order)
{
    // Can we perform this load locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                *static_cast<__uint8_t*>(result) = __Atomic_load_1(ptr, order);
                return;
            case 2:
                *static_cast<__uint16_t*>(result) = __Atomic_load_2(ptr, order);
                return;
            case 4:
                *static_cast<__uint32_t*>(result) = __Atomic_load_4(ptr, order);
                return;
            case 8:
                *static_cast<__uint64_t*>(result) = __Atomic_load_8(ptr, order);
                return;
        }

        // No other sizes are possible.
        __builtin_unreachable();
    }

    // Get a lock and perform the load.
    auto lock = g_atomicSpinlocks.getLock(ptr);
    __Atomic_lock_acquire(lock);
    __builtin_memcpy(result, const_cast<const void*>(ptr), n);
    __Atomic_lock_release(lock);
}

void __Atomic_store(__size_t n, volatile void* ptr, void* value, int order)
{
    // Can we perform this store locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                __Atomic_store_1(ptr, *static_cast<__uint8_t*>(value), order);
                return;
            case 2:
                __Atomic_store_2(ptr, *static_cast<__uint16_t*>(value), order);
                return;
            case 4:
                __Atomic_store_4(ptr, *static_cast<__uint32_t*>(value), order);
                return;
            case 8:
                __Atomic_store_8(ptr, *static_cast<__uint64_t*>(value), order);
                return;
        }

        // No other sizes are possible.
        __builtin_unreachable();
    }

    // Get a lock and perform the store.
    auto lock = g_atomicSpinlocks.getLock(ptr);
    __Atomic_lock_acquire(lock);
    __builtin_memcpy(const_cast<void*>(ptr), value, n);
    __Atomic_lock_release(lock);
}

void __Atomic_exchange(__size_t n, volatile void* ptr, void* value, void* result, int order)
{
    // Can we perform this store locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                *static_cast<__uint8_t*>(result) = __Atomic_exchange_1(ptr, *static_cast<__uint8_t*>(value), order);
                return;
            case 2:
                *static_cast<__uint16_t*>(result) = __Atomic_exchange_2(ptr, *static_cast<__uint16_t*>(value), order);
                return;
            case 4:
                *static_cast<__uint32_t*>(result) = __Atomic_exchange_4(ptr, *static_cast<__uint32_t*>(value), order);
                return;
            case 8:
                *static_cast<__uint64_t*>(result) = __Atomic_exchange_8(ptr, *static_cast<__uint64_t*>(value), order);
                return;
        }

        // No other sizes are possible.
        __builtin_unreachable();
    }

    // Get a lock and perform the exchange.
    auto lock = g_atomicSpinlocks.getLock(ptr);
    __Atomic_lock_acquire(lock);
    __builtin_memcpy(result, const_cast<void*>(ptr), n);
    __builtin_memcpy(const_cast<void*>(ptr), value, n);
    __Atomic_lock_release(lock);
}

bool __Atomic_compare_exchange(__size_t n, volatile void* ptr, void* expect, void* desire, int success, int fail)
{
    // Can we perform this compare-exchange locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                return __Atomic_compare_exchange_1(ptr, expect, *static_cast<__uint8_t*>(desire), success, fail);
            case 2:
                return __Atomic_compare_exchange_2(ptr, expect, *static_cast<__uint16_t*>(desire), success, fail);
            case 4:
                return __Atomic_compare_exchange_4(ptr, expect, *static_cast<__uint32_t*>(desire), success, fail);
            case 8:
                return __Atomic_compare_exchange_8(ptr, expect, *static_cast<__uint64_t*>(desire), success, fail);
        }

        // No other sizes are possible.
        __builtin_unreachable();
    }

    // Get a lock and perform the compare and exchange.
    auto lock = g_atomicSpinlocks.getLock(ptr);
    bool equal;
    __Atomic_lock_acquire(lock);
    if (__builtin_memcmp(const_cast<void*>(ptr), expect, n) == 0)
    {
        // Objects were equal.
        __builtin_memcpy(const_cast<void*>(ptr), desire, n);
        equal = true;
    }
    else
    {
        // Objects were not equal.
        __builtin_memcpy(expect, const_cast<void*>(ptr), n);
        equal = false;
    }
    __Atomic_lock_release(lock);
    return equal;
}

} // namespace System::ABI::Atomic
