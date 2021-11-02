#include <System/ABI/Atomic/Atomic.hh>

#include <System/ABI/Atomic/AtomicSpinlock.hh>


namespace System::ABI::Atomic
{


// Auto-detect various architecture options.
#ifdef _ARCH_PPC64
#  define __CRT_ASSUME_LDARX 1
#endif
#if 0
// LL/SC operations are available on byte and halfword values.
#  define __CRT_ASSUME_LBARX 1
#endif


// Values for the atomic access type hint. These are not supported before PowerPC 2.00.
#if 1
#  define EH_LOCK   1
#  define EH_ATOMIC 0
#endif

// Elide the lwarx hints with Clang - its integrated assembler doesn't like them.
#if defined(__llvm__)
#  define __PPC_LWARX_HINT(x)
#else
#  define __PPC_LWARX_HINT(x)   ", " x
#endif 


// Condition register flags.
#define CR_EQ 0b0010


#if __CRT_ASSUME_LDARX
// We have doubleword atomic accesses.
static constexpr __size_t MaxAtomicSize = sizeof(__uint64_t);
#else
// Only 32-bit atomic accesses.
static constexpr __size_t MaxAtomicSize = sizeof(__uint32_t);
#endif


bool __Atomic_is_lock_free(__size_t size, const volatile void* ptr)
{
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
}


bool __Atomic_test_and_set(volatile void* ptr)
{
    return __Atomic_test_and_set_explicit(ptr, __memory_order_seq_cst);
}

bool __Atomic_test_and_set_explicit(volatile void* ptr, int)
{
    return __Atomic_exchange_1(ptr, 1, 0);
}

void __Atomic_clear(volatile void* ptr, int)
{
    __Atomic_store_1(ptr, 0, 0);
}


bool __Atomic_test_and_set_1(volatile void* ptr, int order)
{
    // Use an exchange operation to set the flag to 0x01.
    return __Atomic_exchange_1(ptr, 1, order);
}

bool __Atomic_test_and_set_2(volatile void* ptr, int order)
{
    // Just use the byte implementation.
    return __Atomic_test_and_set_1(ptr, order);
}

bool __Atomic_test_and_set_4(volatile void* ptr, int order)
{
    // Use an lwarx/stwcx pair.
    __uint32_t previous = __Atomic_exchange_4(ptr, 1, order);
    return (previous == 0);
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
    // Insert a heavyweight memory barrier.
    asm volatile("sync 0" ::: "memory");
}


// Appropriately aligned loads are always atomic.
#define ATOMIC_LOAD_N(n, type, suffix) \
type __Atomic_load_##n(const volatile void* ptr, int) \
{ \
    type val; \
    asm volatile \
    ( \
        "l" suffix " %[value], 0(%[ptr])\n\t" \
        "cmpw       cr0, %[value], %[value]\n\t" \
        "bne-       1f\n\t" \
        "1: isync   \n\t" \
        : [value] "=r" (val) \
        : [ptr] "b" (ptr) \
        : "memory", \
          "cc" \
    ); \
    return val; \
}

// Appropriately aligned stores are always atomic.
#define ATOMIC_STORE_N(n, type) \
void __Atomic_store_##n(volatile void* ptr, type value, int) \
{ \
    asm volatile("sync 0" ::: "memory"); \
    *static_cast<volatile type*>(ptr) = value; \
}

// Use an lwarx/stwcx sequence for exchanges.
#define ATOMIC_EXCHANGE_N(n, type, suffix, hint_val) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int) \
{ \
    type previous; \
    __uint32_t temp; \
    asm volatile \
    ( \
        "sync 0 \n\t" \
        "1: l" suffix "arx  %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        "st" suffix "cx.    %[value], 0, %[ptr]\n\t" \
        "bne-   1b\n\t" \
        "isync  \n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp) \
        : [value] "r" (value), \
          [ptr] "b" (ptr), \
          [hint] "I" (hint_val) \
        : "memory" \
    ); \
    return previous; \
}
// Like ATOMIC_EXCHANGE_N but uses a word-sized access to emulate smaller operations.
#define ATOMIC_EXCHANGE_WORD_N(n, type, hint_val) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = value << shift; \
    type previous; \
    __uint32_t temp; \
    asm volatile \
    ( \
        "sync 0     \n\t" \
        "1: lwarx   %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        "and        %[temp], %[previous], %[mask]\n\t" \
        "or         %[temp], %[temp], %[value]\n\t" \
        "stwcx.     %[temp], 0, %[ptr]\n\t" \
        "bne-       1b\n\t" \
        "isync      \n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp) \
        : [ptr] "b" (aligned_ptr), \
          [mask] "r" (mask), \
          [value] "r" (val), \
          [hint] "I" (hint_val) \
        : "memory" \
    ); \
    return (previous & ~mask) >> shift; \
}


#  define ATOMIC_CMPXCHG_N(n, type, suffix) \
bool __Atomic_compare_exchange_##n(volatile void* __restrict ptr, void* __restrict expect, type desired, int, int) \
{ \
    auto expect_ptr = static_cast<type*>(expect); \
    __uint32_t cr; \
    asm volatile \
    ( \
        "crclr      4*cr0+eq\n\t" \
        "sync 0     \n\t" \
        "1: l" suffix "arx %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        "cmp        cr1, %[l], %[previous], %[expect]\n\t" \
        "bne-       cr1, 2f\n\t" \
        "st" suffix "cx. %[desire], 0, %[ptr]\n\t" \
        "bne-       1b\n\t" \
        "isync      \n\t" \
        "2: crand   4*cr0+eq, 4*cr1+eq, 4*cr0+eq\n\t" \
        "mfcr       %[cr]\n\t" \
        : [previous] "=&r" (*expect_ptr), \
          [cr] "=r" (cr) \
        : [desire] "r" (desired), \
          [ptr] "b" (ptr), \
          [expect] "r" (*expect_ptr), \
          [hint] "I" (EH_ATOMIC), \
          [l] "I" (sizeof(type) == sizeof(__uint64_t) ? 1 : 0) \
        : "memory", \
          "cc" \
    ); \
    return (cr & CR_EQ); \
}

#  define ATOMIC_CMPXCHG_WORD_N(n, type) \
bool __Atomic_compare_exchange_##n(volatile void* ptr, void* expect, type desire, int, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = desire << shift; \
    __uint32_t exp = *static_cast<type*>(expect) << shift; \
    __uint32_t temp; \
    type previous; \
    __uint32_t cr; \
    asm volatile \
    ( \
        "crclr      4*cr0+eq\n\t" \
        "sync 0     \n\t" \
        "1: lwarx   %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        "and        %[temp], %[previous], %[mask]\n\t" \
        "or         %[cr], %[temp], %[expect]\n\t" \
        "or         %[temp], %[temp], %[desire]\n\t" \
        "cmpw       cr1, %[previous], %[cr]\n\t" \
        "bne-       cr1, 2f\n\t" \
        "stwcx.     %[desire], 0, %[ptr]\n\t" \
        "bne-       1b\n\t" \
        "isync      \n\t" \
        "2: crand   4*cr0+eq, 4*cr1+eq, 4*cr0+eq\n\t" \
        "mfcr       %[cr]\n\t" \
        : [previous] "=&r" (previous), \
          [cr] "=&r" (cr), \
          [temp] "=&r" (temp) \
        : [desire] "r" (val), \
          [ptr] "b" (aligned_ptr), \
          [expect] "r" (exp), \
          [mask] "r" (mask), \
          [hint] "I" (EH_ATOMIC), \
          [l] "I" (sizeof(type) == sizeof(__uint64_t) ? 1 : 0) \
        : "memory", \
          "cc" \
    ); \
    *static_cast<type*>(expect) = (previous & ~mask) >> shift; \
    return (cr & CR_EQ); \
}

// Inline LDREX/STREX for arithmetic operations.
#  define ATOMIC_OP_N(n, type, suffix, name, op) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int) \
{ \
    type previous; \
    type result; \
    asm volatile \
    ( \
        "sync 0     \n\t" \
        "1: l" suffix "arx %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        op "        %[result], %[previous], %[value]\n\t" \
        "st" suffix "cx. %[result], 0, %[ptr]\n\t" \
        "bne-       1b\n\t" \
        "isync      \n\t" \
        : [previous] "=&r" (previous), \
          [result] "=&r" (result) \
        : [value] "r" (value), \
          [ptr] "b" (ptr), \
          [hint] "I" (EH_ATOMIC) \
        : "memory", \
          "cc" \
    ); \
    return previous; \
} \
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    type previous; \
    type result; \
    asm volatile \
    ( \
        "sync 0     \n\t" \
        "1: l" suffix "arx %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        op "        %[result], %[previous], %[value]\n\t" \
        "st" suffix "cx. %[result], 0, %[ptr]\n\t" \
        "bne-       1b\n\t" \
        "isync      \n\t" \
        : [previous] "=&r" (previous), \
          [result] "=&r" (result) \
        : [value] "r" (value), \
          [ptr] "b" (ptr), \
          [hint] "I" (EH_ATOMIC) \
        : "memory", \
          "cc" \
    ); \
    return result; \
}

#  define ATOMIC_OP_WORD_N(n, type, name, op) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = value << shift; \
    __uint32_t temp;\
    type previous; \
    type result; \
    asm volatile \
    ( \
        "sync 0     \n\t" \
        "1: lwarx   %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        "and        %[temp], %[previous], %[mask]\n\t" \
        op "        %[result], %[previous], %[value]\n\t" \
        "nand       %[result], %[result], %[mask]\n\t" \
        "or         %[result], %[temp], %[result]\n\t" \
        "stwcx.     %[result], 0, %[ptr]\n\t" \
        "bne-       1b\n\t" \
        "isync      \n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp), \
          [result] "=&r" (result) \
        : [ptr] "r" (aligned_ptr), \
          [mask] "r" (mask), \
          [value] "r" (val), \
          [hint] "I" (EH_ATOMIC) \
        : "memory", \
           "cc" \
    ); \
    return (previous & ~mask) >> shift; \
} \
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = value << shift; \
    __uint32_t temp; \
    type previous; \
    type result; \
    asm volatile \
    ( \
        "sync 0     \n\t" \
        "1: lwarx   %[previous], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t" \
        "and        %[temp], %[previous], %[mask]\n\t" \
        op "        %[result], %[previous], %[value]\n\t" \
        "nand       %[result], %[result], %[mask]\n\t" \
        "or         %[result], %[temp], %[result]\n\t" \
        "stwcx.     %[result], 0, %[ptr]\n\t" \
        "bne-       1b\n\t" \
        "isync      \n\t" \
        : [previous] "=&r" (previous), \
          [temp] "=&r" (temp), \
          [result] "=&r" (result) \
        : [ptr] "r" (aligned_ptr), \
          [mask] "r" (mask), \
          [value] "r" (val), \
          [hint] "I" (EH_ATOMIC) \
        : "memory", \
          "cc" \
    ); \
    return (result & ~mask) >> shift; \
}

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


ATOMIC_LOAD_N(1, __uint8_t, "bz")
ATOMIC_LOAD_N(2, __uint16_t, "hz")
ATOMIC_LOAD_N(4, __uint32_t, "wz")

ATOMIC_STORE_N(1, __uint8_t)
ATOMIC_STORE_N(2, __uint16_t)
ATOMIC_STORE_N(4, __uint32_t)


// Atomic exchanges.
#if __CRT_ASSUME_LBARX
ATOMIC_EXCHANGE_N(1, __uint8_t, "b", EH_ATOMIC)
ATOMIC_EXCHANGE_N(2, __uint16_t, "h", EH_ATOMIC)
#else
ATOMIC_EXCHANGE_WORD_N(1, __uint8_t, EH_ATOMIC)
ATOMIC_EXCHANGE_WORD_N(2, __uint16_t, EH_ATOMIC)
#endif

ATOMIC_EXCHANGE_N(4, __uint32_t, "w", EH_ATOMIC)


// Atomic compare-exchange.
#if __CRT_ASSUME_LBARX
ATOMIC_CMPXCHG_N(1, __uint8_t, "b")
ATOMIC_CMPXCHG_N(2, __uint16_t, "h")
#else
ATOMIC_CMPXCHG_WORD_N(1, __uint8_t)
ATOMIC_CMPXCHG_WORD_N(2, __uint16_t)
#endif

ATOMIC_CMPXCHG_N(4, __uint32_t, "w")


// Atomic 32-bit arithmetic.
ATOMIC_OP_N(4, __uint32_t, "w", add, "add")
ATOMIC_OP_N(4, __uint32_t, "w", sub, "sub")
ATOMIC_OP_N(4, __uint32_t, "w", and, "and")
ATOMIC_OP_N(4, __uint32_t, "w", or,  "or")
ATOMIC_OP_N(4, __uint32_t, "w", xor, "xor")
ATOMIC_OP_N(4, __uint32_t, "w", nand, "nand")

#if __CRT_ASSUME_LBARX
// Atomic 16-bit arithmetic.
ATOMIC_OP_N(2, __uint16_t, "h", add, "add")
ATOMIC_OP_N(2, __uint16_t, "h", sub, "sub")
ATOMIC_OP_N(2, __uint16_t, "h", and, "and")
ATOMIC_OP_N(2, __uint16_t, "h", or,  "or")
ATOMIC_OP_N(2, __uint16_t, "h", xor, "xor")
ATOMIC_OP_N(2, __uint16_t, "h", nand, "nand")

// Atomic 8-bit arithmetic.
ATOMIC_OP_N(1, __uint8_t, "b", add, "add")
ATOMIC_OP_N(1, __uint8_t, "b", sub, "sub")
ATOMIC_OP_N(1, __uint8_t, "b", and, "and")
ATOMIC_OP_N(1, __uint8_t, "b", or,  "or")
ATOMIC_OP_N(1, __uint8_t, "b", xor, "xor")
ATOMIC_OP_N(1, __uint8_t, "b", nand, "nand")
#else
// Atomic 16-bit arithmetic.
ATOMIC_OP_WORD_N(2, __uint16_t, add, "add")
ATOMIC_OP_WORD_N(2, __uint16_t, sub, "sub")
ATOMIC_OP_WORD_N(2, __uint16_t, and, "and")
ATOMIC_OP_WORD_N(2, __uint16_t, or,  "or")
ATOMIC_OP_WORD_N(2, __uint16_t, xor, "xor")
ATOMIC_OP_WORD_N(2, __uint16_t, nand, "nand")

// Atomic 8-bit arithmetic.
ATOMIC_OP_WORD_N(1, __uint8_t, add, "add")
ATOMIC_OP_WORD_N(1, __uint8_t, sub, "sub")
ATOMIC_OP_WORD_N(1, __uint8_t, and, "and")
ATOMIC_OP_WORD_N(1, __uint8_t, or,  "or")
ATOMIC_OP_WORD_N(1, __uint8_t, xor, "xor")
ATOMIC_OP_WORD_N(1, __uint8_t, nand, "nand")
#endif


// Native 64-bit operations (if available).
#if __CRT_ASSUME_LDARX
ATOMIC_LOAD_N(8, __uint64_t, "d")
ATOMIC_STORE_N(8, __uint64_t)
ATOMIC_EXCHANGE_N(8, __uint64_t, "d", EH_ATOMIC)
ATOMIC_CMPXCHG_N(8, __uint64_t, "d")

ATOMIC_OP_N(8, __uint64_t, "d", add, "add")
ATOMIC_OP_N(8, __uint64_t, "d", sub, "sub")
ATOMIC_OP_N(8, __uint64_t, "d", and, "and")
ATOMIC_OP_N(8, __uint64_t, "d", or,  "or")
ATOMIC_OP_N(8, __uint64_t, "d", xor, "xor")
ATOMIC_OP_N(8, __uint64_t, "d", nand, "nand")
#else
// Use locks to implement 64-bit loads.
__uint64_t __Atomic_load_8(const volatile void* ptr, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    __Atomic_lock_acquire(lock);
    __uint64_t value = *static_cast<const __uint64_t*>(const_cast<const void*>(ptr));
    __Atomic_lock_release(lock);
    return value;
}

// Use locks to implement 64-bit stores.
void __Atomic_store_8(volatile void* ptr, __uint64_t value, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    __Atomic_lock_acquire(lock);
    *static_cast<__uint64_t*>(const_cast<void*>(ptr)) = value;
    __Atomic_lock_release(lock);
}

// Use locks to implement 64-bit exchanges.
__uint64_t __Atomic_exchange_8(volatile void* ptr, __uint64_t value, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    auto uint64_ptr = static_cast<__uint64_t*>(const_cast<void*>(ptr));
    __Atomic_lock_acquire(lock);
    __uint64_t old = *uint64_ptr;
    *uint64_ptr = value;
    __Atomic_lock_release(lock);
    return old;
}

// Use locks to implement 64-bit compare-exchange.
bool __Atomic_compare_exchange_8(volatile void* ptr, void* expect, __uint64_t desire, int, int)
{
    auto lock = g_atomicSpinlocks.getLock(ptr);
    auto expect_ptr = static_cast<__uint64_t*>(expect);
    auto uint64_ptr = static_cast<__uint64_t*>(const_cast<void*>(ptr));
    bool result;
    __Atomic_lock_acquire(lock);
    if (auto current = *uint64_ptr; current == *expect_ptr)
    {
        *uint64_ptr = desire;
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

ATOMIC_OP_LOCKED_N(8, __uint64_t, add, +, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, sub, -, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, and, &, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, or,  |, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, xor, ^, )
ATOMIC_OP_LOCKED_N(8, __uint64_t, nand, &, ~)
#endif


// Utility for 128-bit comparisons.
#ifndef __LP64__
static constexpr bool operator==(const __uint128_t& a, const __uint128_t& b)
{
    return a.__elem[0] == b.__elem[0] && a.__elem[1] == b.__elem[1];
}
#endif

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

//! @TODO: 128-bit arithmetic operations


void __Atomic_lock_acquire(volatile __uintptr_t* lock)
{
    __uint32_t temp;
    asm volatile
    (
        "1:     \n\t"
        "lwarx  %[temp], 0, %[ptr]" __PPC_LWARX_HINT("%[hint]") "\n\t"
        "cmpw   %[temp], %[value]\n\t"
        "beq-   1b\n\t"
        "stwcx. %[value], 0, %[ptr]\n\t"
        "bne-   1b\n\t"
        "isync  \n\t"
        : [temp] "=&r" (temp)
        : [value] "r" (1),
          [ptr] "b" (lock),
          [hint] "I" (EH_LOCK)
        : "memory",
          "cc"
    );
}
void __Atomic_lock_release(volatile __uintptr_t* lock)
{
    asm volatile
    (
        "sync 0     \n\t"
        "stw        %[zero], 0(%[ptr])\n\t"
        :
        : [zero] "r" (0),
          [ptr] "b" (lock)
        : "memory"
    );
}


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
