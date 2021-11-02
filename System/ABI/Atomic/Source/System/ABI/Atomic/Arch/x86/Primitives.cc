#include <System/ABI/Atomic/Arch/x86/Primitives.hh>
#include <System/ABI/Atomic/AtomicSpinlock.hh>

//#include <System/HW/CPU/CPUID/Arch/x86/CPUID.hh>


namespace System::ABI::Atomic
{


using std::uintptr_t;


#if ATOMIC_IFUNC || ATOMIC_RUNTIME_CHECKS
#  if defined(__x86_64__)
#    if !__ATOMIC_ASSUME_CMPXCHG16B
        static const bool cmpxchg16b_supported = System::HW::CPU::CPUID::HasFeature(System::HW::CPU::CPUID::Feature::CompareExchange16B);

        static bool has_cmpxchg16b()
        {
            return cmpxchg16b_supported;
        }

        static size_t max_lock_free()
        {
            return has_cmpxchg16b() ? 16 : 8;
        }
#    else
        static constexpr bool has_cmpxchg16b()
        {
            return true;
        }

        static constexpr size_t max_lock_free()
        {
            return 16;
        }
#    endif
#  else
#    if !__ATOMIC_ASSUME_CMPXCHG8B
        static bool has_cmpxchg8b()
        {
            using namespace System::HW::CPU::CPUID;
            static const bool supported = CPUID::HasFeature(CPUID::Feature::CompareExchange8B);
            return supported;
        }

        static size_t max_lock_free()
        {
            return has_cmpxchg8b() ? 8 : 4;
        }
#    else
        static constexpr bool has_cmpxchg8b()
        {
            return true;
        }

        static constexpr size_t max_lock_free()
        {
            return 4;
        }
#    endif  // !__ATOMIC_ASSUME_CMPXCHG8B
#  endif    // defined(__x86_64__)
#endif      // ATOMIC_IFUNC || ATOMIC_RUNTIME_CHECKS

#if !ATOMIC_IFUNC && !ATOMIC_RUNTIME_CHECKS
static constexpr size_t max_lock_free()
{
#  if __ATOMIC_ASSUME_CMPXCHG16B
    return 16;
#  elif defined(__x86_64__) || __ATOMIC_ASSUME_CMPXCHG8B
    return 8;
#else
    return 4;
#endif
}

static constexpr bool has_cmpxchg8b()
{
    return max_lock_free() >= 8;
}

static constexpr bool has_cmpxchg16b()
{
    return max_lock_free() >= 16;
}
#endif


bool __Atomic_is_lock_free(size_t size, const volatile void* ptr)
{
    // Check the size of the access.
    if (size > max_lock_free())
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
    if (ptr != nullptr && (uintptr_t(ptr) & (size - 1) ) != 0)
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


void __Atomic_signal_fence(int)
{
    // Does nothing; the external function call was sufficient.
    asm volatile ("" ::: "memory");
}

void __Atomic_thread_fence(int)
{
    // Insert a full memory barrier.
    asm volatile ("mfence" ::: "memory");
}


// No accesses may pass a load (in either direction) so no explicit barrier is needed.
#define ATOMIC_LOAD_N(n, type) \
type __Atomic_load_##n(const volatile void* ptr, int) \
{ \
    return *static_cast<const volatile type*>(ptr); \
}

// Later loads can pass the store so a full fence is needed.
#define ATOMIC_STORE_N(n, type) \
void __Atomic_store_##n(volatile void* ptr, type value, int) \
{ \
    *static_cast<volatile type*>(ptr) = value; \
    asm volatile ("mfence" ::: "memory"); \
}

// The xchg is implicitly locked and therefore acts as a full memory barrier.
#define ATOMIC_EXCHANGE_N(n, type, suffix) \
type __Atomic_exchange_##n(volatile void* ptr, type value, int) \
{ \
    asm volatile \
    ( \
        "xchg" suffix " %0, (%1)\n\t" \
        : "=r" (value) \
        : "r" (ptr), "0" (value) \
        : "memory" \
    ); \
    return value; \
}

// The locked cmpxchg acts as a full memory barrier.
#if !defined(__clang__)
// Use the "=@ccX" constraint if available. 
#  define CMPXCHG_RESULT_SEQ : "=@cce" (success),
#else
// Use a less efficient constraint on Clang.
#  define CMPXCHG_RESULT_SEQ "sete %0\n\t" : "=r" (success),
#endif
#define ATOMIC_CMPXCHG_N(n, type, suffix) \
bool __Atomic_compare_exchange_##n(volatile void* __restrict ptr, void* __restrict expect, type desired, int, int) \
{ \
    auto expect_ptr = static_cast<type*>(expect); \
    bool success = false; \
    asm volatile \
    ( \
        "lock cmpxchg" suffix " %2, (%3)\n\t" \
        CMPXCHG_RESULT_SEQ \
          "=a" (*expect_ptr) \
        : "r" (desired), "r" (ptr), "a" (*expect_ptr)\
        : "memory" \
    ); \
    return success; \
}

// The locked xadd acts as a full memory barrier.
#define ATOMIC_XADD_N(n, type, suffix, op, sign) \
type __Atomic_fetch_##op##_##n(volatile void* ptr, type value, int) \
{ \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wuseless-cast\"") \
    asm volatile \
    ( \
        "lock xadd" suffix " %0, (%1)\n\t" \
        : "=r" (value) \
        : "r" (ptr), "0" (type(sign value)) \
        : "memory" \
    ); \
    return value; \
    _Pragma("GCC diagnostic pop") \
} \
\
type __Atomic_##op##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    return __Atomic_fetch_##op##_##n(ptr, value, 0) sign value; \
}

// The locked cmpxchg acts as a full memory barrier.
#define ATOMIC_OP_N(n, type, suffix, name, op, op2) \
type __Atomic_fetch_##name##_##n(volatile void* ptr, type value, int) \
{ \
    type fetch = *static_cast<volatile type*>(ptr); \
    asm volatile \
    ( \
        "1: mov" suffix " %4, %1\n\t" \
        op " %3, %1\n\t" \
        op2 " %1\n\t" \
        "lock cmpxchg" suffix " %1, (%2)\n\t" \
        "jne 1b\n\t" \
        : "=a" (fetch), "=&r" (value) /* dummy so we have extra reg */ \
        : "r" (ptr), "g" (value), "a" (fetch) \
        : "memory" \
    ); \
    return fetch; \
} \
\
type __Atomic_##name##_fetch_##n(volatile void* ptr, type value, int) \
{ \
    type fetch = *static_cast<volatile type*>(ptr); \
    asm volatile \
    ( \
        "1: mov" suffix " %3, %0\n\t" \
        op " %2, %0\n\t" \
        op2 " %0\n\t" \
        "lock cmpxchg" suffix " %0, (%1)\n\t" \
        "jne 1b\n\t" \
        : "=&r" (fetch) \
        : "r" (ptr), "g" (value), "a" (fetch) \
        : "memory" \
    ); \
    return fetch; \
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

// Similar to ATOMIC_OP_N but calling __Atomic_compare_exchange_N instead of inlining it.
#define ATOMIC_OP_CMPXCHG_N(n, type, name, op1, op2) \
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
}


ATOMIC_LOAD_N(1, uint8_t)
ATOMIC_LOAD_N(2, uint16_t)
ATOMIC_LOAD_N(4, uint32_t)

ATOMIC_STORE_N(1, uint8_t)
ATOMIC_STORE_N(2, uint16_t)
ATOMIC_STORE_N(4, uint32_t)

ATOMIC_EXCHANGE_N(1, uint8_t, "b")
ATOMIC_EXCHANGE_N(2, uint16_t, "w")
ATOMIC_EXCHANGE_N(4, uint32_t, "l")

ATOMIC_CMPXCHG_N(1, uint8_t, "b")
ATOMIC_CMPXCHG_N(2, uint16_t, "w")
ATOMIC_CMPXCHG_N(4, uint32_t, "l")

ATOMIC_XADD_N(1, uint8_t, "b", add, +)
ATOMIC_XADD_N(2, uint16_t, "w", add, +)
ATOMIC_XADD_N(4, uint32_t, "l", add, +)

ATOMIC_XADD_N(1, uint8_t, "b", sub, -)
ATOMIC_XADD_N(2, uint16_t, "w", sub, -)
ATOMIC_XADD_N(4, uint32_t, "l", sub, -)

ATOMIC_OP_N(1, uint8_t, "b", and, "andb", "#")
ATOMIC_OP_N(2, uint16_t, "w", and, "andw", "#")
ATOMIC_OP_N(4, uint32_t, "l", and, "andl", "#")

ATOMIC_OP_N(1, uint8_t, "b", or, "orb", "#")
ATOMIC_OP_N(2, uint16_t, "w", or, "orw", "#")
ATOMIC_OP_N(4, uint32_t, "l", or, "orl", "#")

ATOMIC_OP_N(1, uint8_t, "b", xor, "xorb", "#")
ATOMIC_OP_N(2, uint16_t, "w", xor, "xorw", "#")
ATOMIC_OP_N(4, uint32_t, "l", xor, "xorl", "#")

ATOMIC_OP_N(1, uint8_t, "b", nand, "andb", "notb")
ATOMIC_OP_N(2, uint16_t, "w", nand, "andw", "notw")
ATOMIC_OP_N(4, uint32_t, "l", nand, "andl", "notl")

// 64-bit operations.
#ifdef __x86_64__

ATOMIC_LOAD_N(8, uint64_t)
ATOMIC_STORE_N(8, uint64_t)
ATOMIC_EXCHANGE_N(8, uint64_t, "q")
ATOMIC_CMPXCHG_N(8, uint64_t, "q")
ATOMIC_XADD_N(8, uint64_t, "q", add, +)
ATOMIC_XADD_N(8, uint64_t, "q", sub, -)
ATOMIC_OP_N(8, uint64_t, "q", and, "andq", "#")
ATOMIC_OP_N(8, uint64_t, "q", or, "orq", "#")
ATOMIC_OP_N(8, uint64_t, "q", xor, "xorq", "#")
ATOMIC_OP_N(8, uint64_t, "q", nand, "andq", "notq")

#else // ifdef __x86_64__

// No native support for 64-bit operations. They need to be emulated.

static bool cmpxchg8b(volatile void* ptr, void* expect, uint64_t value, int, int)
{
    bool result;
    auto& expect_val = *static_cast<uint64_t*>(expect);
    uint32_t expect_low = 0;
    uint32_t expect_high = 0;
    asm volatile
    (
        "lock cmpxchg8b (%3)\n\t"
        // Clang doesn't recognise "=@ccX" constraints.
    #if !defined(__GCC_ASM_FLAG_OUTPUTS__)
        "sete %0\n\t"
        : "=g" (result),
    #else
        : "=@cce" (result),
    #endif
          "=a" (expect_low),
          "=d" (expect_high)
        : "r" (ptr),
          "a" (uint32_t(expect_val)),
          "d" (uint32_t(expect_val >> 32)),
          "b" (uint32_t(value)),
          "c" (uint32_t(value >> 32))
        : "memory"
    );
    expect_val = (uint64_t(expect_high) << 32) | expect_low;
    return result;
}

static bool cmpxchg8b_emulated(volatile void* ptr, void* expect, uint64_t value, int, int)
{
    // No lockless implementation available; fall back on the locked form.
    return __Atomic_compare_exchange(8, ptr, expect, &value, 0, 0);
}


#if ATOMIC_IFUNC && !__ATOMIC_ASSUME_CMPXCHG8B
using cmpxch8b_fn = bool (*)(volatile void*, void*, uint64_t, int, int);
extern "C" cmpxchg8b_fn cmpxchg8b_resolver();
static cmpxchg8b_fn cmpxchg8b_resolver()
{
    if (has_cmpxchg8b())
        return &cmpxchg8b;
    else
        return &cmpxchg8b_emulated;
}

bool __Atomic_compare_exchange_8(volatile void*, void*, uint64_t, int, int)
    [[gnu::ifunc("cmpxchg8b_resolver")]];
#else
bool __Atomic_compare_exchange_8(volatile void* ptr, void* expect, uint64_t value, int success, int fail)
{
    if (has_cmpxchg8b())
        return cmpxchg8b(ptr, expect, value, success, fail);
    else
        return cmpxchg8b_emulated(ptr, expect, value, success, fail);
}
#endif


uint64_t __Atomic_load_8(const volatile void* ptr, int)
{
    // Emulate the load using a compare-exchange. This is technically not valid as it may cause an extraneous write but
    // it is the only way to atomically load a 64-bit quantity on x86 without using locks (assuming the cmpxchg8b
    // opcode exists... if not, it'll end up using a lock anyway).
    uint64_t value;
    __Atomic_compare_exchange_8(const_cast<volatile void*>(ptr), &value, 0, 0, 0);
    return value;
}

void __Atomic_store_8(volatile void* ptr, uint64_t value, int)
{
    // Emulate the store using a compare-exchange. This is the only way to atomically store a 64-bit quantity on x86
    // without using locks (though it'll still use a lock if the cmpxchg8b opcode is not available).
    uint64_t current = 0;
    while (!__Atomic_compare_exchange_8(ptr, &current, value, 0, 0))
        ;
}

uint64_t __Atomic_exchange_8(volatile void* ptr, uint64_t value, int)
{
    uint64_t current = 0;
    while (!__Atomic_compare_exchange_8(ptr, &current, value, 0, 0))
        ;
    return current;
}

ATOMIC_OP_CMPXCHG_N(8, uint64_t, add, +,)
ATOMIC_OP_CMPXCHG_N(8, uint64_t, sub, -,)
ATOMIC_OP_CMPXCHG_N(8, uint64_t, and, &,)
ATOMIC_OP_CMPXCHG_N(8, uint64_t, or , |,)
ATOMIC_OP_CMPXCHG_N(8, uint64_t, xor, ^,)
ATOMIC_OP_CMPXCHG_N(8, uint64_t, nand, &, ~)

#endif // ifdef __x86_64__


// 128-bit operations.
#ifdef __x86_64__
static bool cmpxchg16b(volatile void* ptr, void* expect, __uint128_t value, int, int)
{
    bool result;
    auto& expect_val = *static_cast<__uint128_t*>(expect);
    uint64_t expect_low = 0;
    uint64_t expect_high = 0;
    asm volatile
    (
        "lock cmpxchg16b (%3)\n\t"
    // Clang doesn't recognise "=@ccX" constraints.
    #if !defined(__GCC_ASM_FLAG_OUTPUTS__)
        "sete %0\n\t"
        : "=g" (result),
    #else
        : "=@cce" (result),
    #endif
          "=a" (expect_low),
          "=d" (expect_high)
        : "r" (ptr),
          "a" (uint64_t(expect_val)),
          "d" (uint64_t(expect_val >> 64)),
          "b" (uint64_t(value)),
          "c" (uint64_t(value >> 64))
        : "memory"
    );
    expect_val = (__uint128_t(expect_high) << 64) | expect_low;
    return result;
}

static bool cmpxchg16b_emulated(volatile void* ptr, void* expect, __uint128_t desire, int, int)
{
    // No lockless implementation available; fall back on the locked form.
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

#if defined(__x86_64__) && !__ATOMIC_ASSUME_CMPXCHG16B && ATOMIC_IFUNC
using cmpxchg16b_fn = bool (*)(volatile void*, void*, __uint128_t, int, int);
static cmpxchg16b_fn cmpxchg16b_resolver() asm("__cmpxchg16b_resolver");
static cmpxchg16b_fn cmpxchg16b_resolver()
{
    if (has_cmpxchg16b())
        return &cmpxchg16b;
    else
        return &cmpxchg16b_emulated;
}

bool __Atomic_compare_exchange_16(volatile void*, void*, __uint128_t, int, int)
    [[gnu::ifunc("__cmpxchg16b_resolver")]];
#else
bool __Atomic_compare_exchange_16(volatile void* ptr, void* expect, __uint128_t value, int success, int fail)
{
    if (has_cmpxchg16b())
        return cmpxchg16b(ptr, expect, value, success, fail);
    else
        return cmpxchg16b_emulated(ptr, expect, value, success, fail);
}
#endif

__uint128_t __Atomic_load_16(const volatile void* ptr, int)
{
    // Emulate the load using a compare-exchange. This is technically not valid as it may cause an extraneous write but
    // it is the only way to atomically load a 128-bit quantity on x86 without using locks.
    __uint128_t value = 0;
    __Atomic_compare_exchange_16(const_cast<volatile void*>(ptr), &value, 0, 0, 0);
    return value;
}

void __Atomic_store_16(volatile void* ptr, __uint128_t value, int)
{
    // Emulate the store using a compare-exchange. This is the only way to atomically store a 128-bit quantity on x86
    // without using locks.
    __uint128_t current = 0;
    while (!__Atomic_compare_exchange_16(ptr, &current, value, 0, 0))
        ;
}

__uint128_t __Atomic_exchange_16(volatile void* ptr, __uint128_t value, int)
{
    __uint128_t current = 0;
    while (!__Atomic_compare_exchange_16(ptr, &current, value, 0, 0))
        ;
    return current;
}

ATOMIC_OP_CMPXCHG_N(16, __uint128_t, add, +,)
ATOMIC_OP_CMPXCHG_N(16, __uint128_t, sub, -,)
ATOMIC_OP_CMPXCHG_N(16, __uint128_t, and, &,)
ATOMIC_OP_CMPXCHG_N(16, __uint128_t, or , |,)
ATOMIC_OP_CMPXCHG_N(16, __uint128_t, xor, ^,)
ATOMIC_OP_CMPXCHG_N(16, __uint128_t, nand, &, ~)

#endif // ifdef __x86_64__


void __Atomic_lock_acquire(volatile AtomicSpinlockArray::spinlock_t* lock)
{
    asm volatile
    (
        "1:         \n\t"
        "lock btsl  $0x01, (%0)\n\t"
        "pause      \n\t"
        "jnz        1b\n\t"
        :
        : "r" (lock)
        : "memory"
    );
    __Atomic_thread_fence(__memory_order_acquire);
}

void __Atomic_lock_release(volatile AtomicSpinlockArray::spinlock_t* lock)
{
    __Atomic_thread_fence(__memory_order_release);
    *reinterpret_cast<volatile uint8_t*>(lock) = 0;
    asm volatile("" ::: "memory");
}


void __Atomic_load(size_t n, const volatile void* ptr, void* result, int order)
{
    // Can we perform this load locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                *static_cast<uint8_t*>(result) = __Atomic_load_1(ptr, order);
                return;
            case 2:
                *static_cast<uint16_t*>(result) = __Atomic_load_2(ptr, order);
                return;
            case 4:
                *static_cast<uint32_t*>(result) = __Atomic_load_4(ptr, order);
                return;
            case 8:
                *static_cast<uint64_t*>(result) = __Atomic_load_8(ptr, order);
                return;
            case 16:
                *static_cast<__uint128_t*>(result) = __Atomic_load_16(ptr, order);
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

void __Atomic_store(size_t n, volatile void* ptr, void* value, int order)
{
    // Can we perform this store locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                __Atomic_store_1(ptr, *static_cast<uint8_t*>(value), order);
                return;
            case 2:
                __Atomic_store_2(ptr, *static_cast<uint16_t*>(value), order);
                return;
            case 4:
                __Atomic_store_4(ptr, *static_cast<uint32_t*>(value), order);
                return;
            case 8:
                __Atomic_store_8(ptr, *static_cast<uint64_t*>(value), order);
                return;
            case 16:
                __Atomic_store_16(ptr, *static_cast<__uint128_t*>(value), order);
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

void __Atomic_exchange(size_t n, volatile void* ptr, void* value, void* result, int order)
{
    // Can we perform this store locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                *static_cast<uint8_t*>(result) = __Atomic_exchange_1(ptr, *static_cast<uint8_t*>(value), order);
                return;
            case 2:
                *static_cast<uint16_t*>(result) = __Atomic_exchange_2(ptr, *static_cast<uint16_t*>(value), order);
                return;
            case 4:
                *static_cast<uint32_t*>(result) = __Atomic_exchange_4(ptr, *static_cast<uint32_t*>(value), order);
                return;
            case 8:
                *static_cast<uint64_t*>(result) = __Atomic_exchange_8(ptr, *static_cast<uint64_t*>(value), order);
                return;
            case 16:
                *static_cast<__uint128_t*>(result) = __Atomic_exchange_16(ptr, *static_cast<__uint128_t*>(value), order);
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

bool __Atomic_compare_exchange(size_t n, volatile void* ptr, void* expect, void* desire, int success, int fail)
{
    // Can we perform this compare-exchange locklessly?
    if (__Atomic_is_lock_free(n, ptr))
    {
        switch (n)
        {
            case 1:
                return __Atomic_compare_exchange_1(ptr, expect, *static_cast<uint8_t*>(desire), success, fail);
            case 2:
                return __Atomic_compare_exchange_2(ptr, expect, *static_cast<uint16_t*>(desire), success, fail);
            case 4:
                return __Atomic_compare_exchange_4(ptr, expect, *static_cast<uint32_t*>(desire), success, fail);
            case 8:
                return __Atomic_compare_exchange_8(ptr, expect, *static_cast<uint64_t*>(desire), success, fail);
            case 16:
                return __Atomic_compare_exchange_16(ptr, expect, *static_cast<__uint128_t*>(desire), success, fail);
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
