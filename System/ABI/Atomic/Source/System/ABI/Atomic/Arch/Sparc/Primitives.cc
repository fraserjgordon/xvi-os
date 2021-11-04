#include <System/ABI/Atomic/Atomic.hh>

#include <System/ABI/Atomic/AtomicSpinlock.hh>


#ifdef __sparcv9__
#  define ASSUME_V9 1
#endif


// Membar instructions are only for SPARC v9.
#if ASSUME_V9
#  define MEMBAR(order)     "membar " order "\n\t"
#else
#  define MEMBAR(order)     "stbar\n\t"
#endif


namespace System::ABI::Atomic
{


void __Atomic_signal_fence(int order)
{
    // Does nothing; the external function call to this was enough.
    (void)order;
    asm volatile ("" ::: "memory");
}

void __Atomic_thread_fence(int order)
{
    // Ignore the memory order; always use the strongest type.
    (void)order;
    asm volatile
    (
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad")
        :
        :
        : "memory"
    );
}


bool __Atomic_test_and_set(volatile void* ptr)
{
    return __Atomic_test_and_set_explicit(ptr, __memory_order_seq_cst);
}

bool __Atomic_test_and_set_explicit(volatile void* ptr, int order)
{
    return __Atomic_test_and_set_1(ptr, order);
}

void __Atomic_clear(volatile void* ptr, int order)
{
    __Atomic_store_1(ptr, 0, order);
}


bool __Atomic_test_and_set_1(volatile void* ptr, int order)
{
    // The memory order is ignored; assume sequential consistency.
    (void)order;

    uint8_t result;
    asm volatile
    (
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad")
        "ldstub     [%[ptr]], %[result]     \n\t"
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad")
        : [result] "=r" (result)
        : [ptr] "r" (ptr)
        : "memory"
    );

    return (result == 0xff);
}

bool __Atomic_test_and_set_2(volatile void* ptr, int order)
{
    return __Atomic_test_and_set_1(ptr, order);
}

bool __Atomic_test_and_set_4(volatile void* ptr, int order)
{
    return __Atomic_test_and_set_1(ptr, order);
}

bool __Atomic_test_and_set_8(volatile void* ptr, int order)
{
    return __Atomic_test_and_set_1(ptr, order);
}

bool __Atomic_test_and_set_16(volatile void* ptr, int order)
{
    return __Atomic_test_and_set_1(ptr, order);
}


#define ATOMIC_STORE_N(n, type, suffix) \
void __Atomic_store_##n(volatile void* ptr, type value, int order) \
{ \
    (void)order; \
    asm volatile \
    ( \
        MEMBAR("#LoadStore|#StoreStore") \
        "st" suffix " %[value], [%[ptr]]\n\t" \
        MEMBAR("#StoreLoad|#StoreStore") \
        : \
        : [ptr] "r" (ptr), [value] "r" (value) \
        : "memory" \
    ); \
}

#define ATOMIC_LOAD_N(n, type, suffix) \
type __Atomic_load_##n(const volatile void* ptr, int order) \
{ \
    (void)order; \
    type result; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#StoreLoad") \
        "ld" suffix " [%[ptr]], %[result]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore") \
        : [result] "=r" (result) \
        : [ptr] "r" (ptr) \
        : "memory" \
    ); \
    return result; \
}

#define ATOMIC_LOAD_STORE_N(n, type, suffix) \
    ATOMIC_LOAD_N(n, type, suffix) \
    ATOMIC_STORE_N(n, type, suffix)

ATOMIC_LOAD_STORE_N(1, uint8_t, "ub")
ATOMIC_LOAD_STORE_N(2, uint16_t, "uh")

#ifdef ASSUME_V9
ATOMIC_LOAD_STORE_N(4, uint32_t, "uw")
ATOMIC_LOAD_STORE_N(8, uint64_t, "x")
#else
ATOMIC_LOAD_STORE_N(4, uint32_t, "")
#endif

#define ATOMIC_EXCHANGE_N(n, type) \
type __Atomic_exchange_##n(volatile void * ptr, type value, int order) \
{ \
    (void)order; \
    type result; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "swap   [%[ptr]], %[value]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [value] "=r" (result) \
        : [ptr] "r" (ptr), "0" (value) \
        : "memory" \
    ); \
    return result; \
}

#define ATOMIC_COMPARE_EXCHANGE_N(n, type, suffix) \
bool __Atomic_compare_exchange_##n(volatile void* __restrict ptr, void* __restrict expect, type desire, int order_success, int order_fail) \
{ \
    (void)order_success; \
    (void)order_fail; \
    auto expect_ptr = static_cast<type*>(expect); \
    type result; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "cas" suffix " [%[ptr]], %[expect], %[desire]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [desire] "=r" (result) \
        : [ptr] "r" (ptr), [expect] "r" (*expect_ptr), "0" (desire) \
        : "memory" \
    ); \
    return result == *expect_ptr; \
}

#define ATOMIC_COMPARE_EXCHANGE_SUBWORD_N(n, type) \
bool __Atomic_compare_exchange_##n(volatile void * __restrict ptr, void * __restrict expect, type desire, int order_success, int order_fail) \
{ \
    (void)order_success; \
    (void)order_fail; \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = static_cast<__uint32_t>(desire) << shift; \
    __uint32_t previous; \
    __uint32_t expect_val = static_cast<__uint32_t>(*static_cast<type*>(expect)) << shift; \
    __uint32_t temp1; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "ld             [%[ptr]], %[previous]\n\t" \
        "andn           %[previous], %[mask], %[temp1]\n\t" \
        "or             %[desire], %[temp1], %[desire]\n\t" \
        "or             %[expect], %[temp1], %[expect]\n\t" \
        "cas            [%[ptr]], %[expect], %[desire]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [temp1] "=&r" (temp1), \
          [previous] "=&r" (previous), \
          [mask] "=&r" (mask), \
          [expect] "=&r" (expect_val), \
          [desire] "=&r" (val)  \
        : [ptr] "r" (aligned_ptr), \
          "4" (val), \
          "3" (expect_val) \
        : "memory" \
    ); \
    return (val == expect_val); \
}

#define ATOMIC_EXCHANGE_EMULATED_N(n, type, suffix, ccreg) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int order) \
{\
    (void)order; \
    type result; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "ld" suffix "   [%[ptr]], %[result]\n\t" \
        "1:\n\t" \
        "cas" suffix "  [%[ptr]], %[result], %[value]\n\t" \
        "cmp            %[result], %[value]\n\t" \
        "beq,pt         %%" ccreg ", 2f\n\t" \
        "  mov          %[value], %[result]\n\t" \
        "b              1b\n\t" \
        "  nop          \n\t" \
        "2:\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [result] "=&r" (result) \
        : [ptr] "r" (ptr), [value] "r" (value) \
        : "memory" \
    ); \
    return result; \
}

#define ATOMIC_EXCHANGE_EMULATED_SUBWORD_N(n, type) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int order) \
{ \
    (void)order; \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = static_cast<__uint32_t>(value) << shift; \
    __uint32_t previous; \
    __uint32_t temp1; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "ld             [%[ptr]], %[previous]\n\t" \
        "1:\n\t" \
        "andn           %[previous], %[mask], %[temp1]\n\t" \
        "or             %[value], %[temp1], %[value]\n\t" \
        "cas            [%[ptr]], %[previous], %[value]\n\t" \
        "cmp            %[previous], %[value]\n\t" \
        "mov            %[value], %[previous]\n\t" \
        "bne,pn         %%icc, 1b\n\t" \
        "  and          %[value], %[mask], %[value]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [temp1] "=&r" (temp1), \
          [previous] "=&r" (previous) \
        : [ptr] "r" (aligned_ptr), \
          [mask] "r" (mask), \
          [value] "r" (val) \
        : "memory" \
    ); \
    return static_cast<type>(previous >> shift); \
}

#define ATOMIC_EXCHANGE_LOCKED_N(n, type) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int order) \
{ \
    (void)order; \
    \
    /* Get the lock address for this object then lock it. */ \
    auto lock = g_atomicSpinlocks.getLock(ptr); \
    __Atomic_lock_acquire(lock); \
    \
    /* Perform the operation */ \
    auto type_ptr = static_cast<volatile type*>(ptr); \
    type previous = *type_ptr; \
    *type_ptr = value; \
    \
    /* Unlock the spinlock and return the old value */ \
    __Atomic_lock_release(lock); \
    return previous; \
}

#define ATOMIC_COMPARE_EXCHANGE_LOCKED_N(n, type) \
bool __Atomic_compare_exchange_##n(volatile void * __restrict ptr, void * __restrict expect, type desire, int order_success, int order_fail) \
{ \
    (void)order_success; \
    (void)order_fail; \
    \
    /* Get the lock address for this object then lock it. */ \
    auto lock = g_atomicSpinlocks.getLock(ptr); \
    __Atomic_lock_acquire(lock); \
    \
    /* Perform the operation */ \
    auto type_ptr = static_cast<volatile type*>(ptr); \
    auto expect_ptr = static_cast<type*>(expect); \
    type previous = *type_ptr; \
    bool eq = (previous == *expect_ptr); \
    if (previous == *expect_ptr) \
        *type_ptr = desire; \
    else \
        *expect_ptr = previous; \
    \
    /* Unlock the spinlock and return the old value */ \
    __Atomic_lock_release(lock); \
    return eq; \
}

#ifdef ASSUME_V9
ATOMIC_EXCHANGE_EMULATED_SUBWORD_N(1, uint8_t)
ATOMIC_EXCHANGE_EMULATED_SUBWORD_N(2, uint16_t)
ATOMIC_EXCHANGE_EMULATED_N(4, uint32_t, "", "icc")
ATOMIC_EXCHANGE_EMULATED_N(8, uint64_t, "x", "xcc")
ATOMIC_COMPARE_EXCHANGE_SUBWORD_N(1, uint8_t)
ATOMIC_COMPARE_EXCHANGE_SUBWORD_N(2, uint16_t)
ATOMIC_COMPARE_EXCHANGE_N(4, uint32_t, "")
ATOMIC_COMPARE_EXCHANGE_N(8, uint64_t, "x")
#else
ATOMIC_EXCHANGE_LOCKED_N(1, uint8_t)
ATOMIC_EXCHANGE_LOCKED_N(2, uint16_t)
ATOMIC_EXCHANGE_N(4, uint32_t)
ATOMIC_EXCHANGE_LOCKED_N(8, uint64_t)
ATOMIC_COMPARE_EXCHANGE_LOCKED_N(1, uint8_t)
ATOMIC_COMPARE_EXCHANGE_LOCKED_N(2, uint16_t)
ATOMIC_COMPARE_EXCHANGE_LOCKED_N(4, uint32_t)
ATOMIC_COMPARE_EXCHANGE_LOCKED_N(8, uint64_t)
#endif


#if ASSUME_V9

#define ATOMIC_OP_N(n, type, suffix, name, op1, op2, ccreg) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int order) \
{ \
    (void)order; \
    type prev; \
    type temp; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "ld" suffix "   [%[ptr]], %[prev]\n\t" \
        "1:\n\t" \
        op1 "           %[prev], %[value], %[temp]\n\t" \
        op2 "           %[temp], %[temp]\n\t" \
        "cas" suffix "  [%[ptr]], %[prev], %[temp]\n\t" \
        "cmp            %[prev], %[temp]\n\t" \
        "bne,a,pn       %%" ccreg ", 1b\n\t" \
        "  mov          %[temp], %[prev]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [prev] "=&r" (prev), [temp] "=&r" (temp) \
        : [ptr] "r" (ptr), [value] "r" (value) \
        : "memory" \
    ); \
    return prev; \
}\
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int order) \
{\
    (void)order; \
    type result; \
    type temp; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "ld" suffix "   [%[ptr]], %[temp]\n\t" \
        "1:\n\t" \
        op1 "           %[temp], %[value], %[result]\n\t" \
        op2 "           %[result], %[result]\n\t" \
        "cas" suffix "  [%[ptr]], %[temp], %[result]\n\t" \
        "cmp            %[temp], %[result]\n\t" \
        "bne,a,pn       %%" ccreg ", 1b\n\t" \
        "  mov          %[result], %[temp]\n\t" \
        op1 "           %[temp], %[value], %[result]\n\t" \
        op2 "           %[result], %[result]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [result] "=&r" (result), [temp] "=&r" (temp) \
        : [ptr] "r" (ptr), [value] "r" (value) \
        : "memory" \
    ); \
    return result; \
}

#define ATOMIC_OP_SUBWORD_N(n, type, name, op1, op2) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int order) \
{ \
    (void)order; \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = static_cast<__uint32_t>(value << shift); \
    __uint32_t previous; \
    __uint32_t result; \
    __uint32_t temp1, temp2; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "ld             [%[ptr]], %[previous]\n\t" \
        "1:\n\t" \
        "and            %[previous], %[mask], %[temp1]\n\t" \
        "andn           %[previous], %[mask], %[temp2]\n\t" \
        op1 "           %[temp1], %[value], %[result]\n\t" \
        op2 "           %[result], %[result]\n\t" \
        "and            %[result], %[mask], %[temp1]\n\t" \
        "or             %[temp1], %[temp2], %[temp1]\n\t" \
        "cas            [%[ptr]], %[previous], %[temp1]\n\t" \
        "cmp            %[previous], %[temp1]\n\t" \
        "bne,a,pn       %%icc, 1b\n\t" \
        "  mov          %[temp1], %[previous]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [result] "=&r" (result), \
          [temp1] "=&r" (temp1), \
          [temp2] "=&r" (temp2), \
          [previous] "=&r" (previous), \
          [mask] "=&r" (mask) \
        : [ptr] "r" (aligned_ptr), \
          [value] "r" (val), \
          "4" (mask) \
        : "memory" \
    ); \
    return static_cast<type>((previous & mask) >> shift); \
}\
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int order) \
{\
    (void)order; \
    __uintptr_t aligned_ptr = __uintptr_t(ptr) & ~(sizeof(__uint32_t) - 1); \
    __size_t shift = __uintptr_t(ptr) - aligned_ptr; \
    __uint32_t mask = static_cast<__uint32_t>(~(((1 << (8 * n)) - 1) << shift)); \
    __uint32_t val = static_cast<__uint32_t>(value << shift); \
    __uint32_t previous; \
    __uint32_t result; \
    __uint32_t temp1, temp2; \
    asm volatile \
    ( \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        "ld             [%[ptr]], %[previous]\n\t" \
        "1:\n\t" \
        "and            %[previous], %[mask], %[temp1]\n\t" \
        "andn           %[previous], %[mask], %[temp2]\n\t" \
        op1 "           %[temp1], %[value], %[result]\n\t" \
        op2 "           %[result], %[result]\n\t" \
        "and            %[result], %[mask], %[temp1]\n\t" \
        "or             %[temp1], %[temp2], %[temp1]\n\t" \
        "cas            [%[ptr]], %[previous], %[temp1]\n\t" \
        "cmp            %[previous], %[temp1]\n\t" \
        "bne,a,pn       %%icc, 1b\n\t" \
        "  mov          %[temp1], %[previous]\n\t" \
        MEMBAR("#LoadLoad|#LoadStore|#StoreStore|#StoreLoad") \
        : [result] "=&r" (result), \
          [temp1] "=&r" (temp1), \
          [temp2] "=&r" (temp2), \
          [previous] "=&r" (previous), \
          [mask] "=&r" (mask) \
        : [ptr] "r" (aligned_ptr), \
          [value] "r" (val), \
          "4" (mask) \
        : "memory" \
    ); \
    return static_cast<type>((result & mask) >> shift); \
}

#define ATOMIC_OPS_SIZE(n, type, suffix, ccreg) \
    ATOMIC_OP_N(n, type, suffix, add, "add", "#", ccreg) \
    ATOMIC_OP_N(n, type, suffix, sub, "sub", "#", ccreg) \
    ATOMIC_OP_N(n, type, suffix, and, "and", "#", ccreg) \
    ATOMIC_OP_N(n, type, suffix, or,  "or",  "#", ccreg) \
    ATOMIC_OP_N(n, type, suffix, xor, "xor", "#", ccreg) \
    ATOMIC_OP_N(n, type, suffix, nand, "and", "not", ccreg)

#define ATOMIC_OPS_SUBWORD_SIZE(n, type) \
    ATOMIC_OP_SUBWORD_N(n, type, add, "add", "#") \
    ATOMIC_OP_SUBWORD_N(n, type, sub, "sub", "#") \
    ATOMIC_OP_SUBWORD_N(n, type, and, "and", "#") \
    ATOMIC_OP_SUBWORD_N(n, type, or,  "or",  "#") \
    ATOMIC_OP_SUBWORD_N(n, type, xor, "xor", "#") \
    ATOMIC_OP_SUBWORD_N(n, type, nand, "and", "not")

ATOMIC_OPS_SUBWORD_SIZE(1, uint8_t)
ATOMIC_OPS_SUBWORD_SIZE(2, uint16_t)
ATOMIC_OPS_SIZE(4, uint32_t, "", "icc")
ATOMIC_OPS_SIZE(8, uint64_t, "x", "xcc")

#else // if ASSUME_V9

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

#define ATOMIC_OPS_LOCKED_N(n, type) \
    ATOMIC_OP_LOCKED_N(n, type, add, +, ) \
    ATOMIC_OP_LOCKED_N(n, type, sub, -, ) \
    ATOMIC_OP_LOCKED_N(n, type, and, &, ) \
    ATOMIC_OP_LOCKED_N(n, type, or,  |, ) \
    ATOMIC_OP_LOCKED_N(n, type, xor, ^, ) \
    ATOMIC_OP_LOCKED_N(n, type, nand, &, ~)

ATOMIC_OPS_LOCKED_N(1, uint8_t)
ATOMIC_OPS_LOCKED_N(2, uint16_t)
ATOMIC_OPS_LOCKED_N(4, uint32_t)
ATOMIC_OPS_LOCKED_N(8, uint64_t)

#endif // if ASSUME_V9


void __Atomic_lock_acquire(volatile __uintptr_t* lock)
{
    while (!__Atomic_test_and_set_4(lock, __memory_order_seq_cst))
        ;
}

void __Atomic_lock_release(volatile __uintptr_t* lock)
{
    __Atomic_clear(lock, __memory_order_seq_cst);
}


}
