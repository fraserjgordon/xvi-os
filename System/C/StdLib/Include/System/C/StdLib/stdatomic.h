#ifndef __SYSTEM_C_STDLIB_STDATOMIC_H
#define __SYSTEM_C_STDLIB_STDATOMIC_H


#ifndef __cplusplus
#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Types.h>


#define ATOMIC_BOOL_LOCK_FREE       __GCC_ATOMIC_BOOL_LOCK_FREE
#define ATOMIC_CHAR_LOCK_FREE       __GCC_ATOMIC_CHAR_LOCK_FREE
#define ATOMIC_CHAR8_T_LOCK_FREE    __GCC_ATOMIC_CHAR8_T_LOCK_FREE
#define ATOMIC_CHAR16_T_LOCK_FREE   __GCC_ATOMIC_CHAR16_T_LOCK_FREE
#define ATOMIC_CHAR32_T_LOCK_FREE   __GCC_ATOMIC_CHAR32_T_LOCK_FREE
#define ATOMIC_WCHAR_T_LOCK_FREE    __GCC_ATOMIC_WCHAR_T_LOCK_FREE
#define ATOMIC_SHORT_LOCK_FREE      __GCC_ATOMIC_SHORT_LOCK_FREE
#define ATOMIC_INT_LOCK_FREE        __GCC_ATOMIC_INT_LOCK_FREE
#define ATOMIC_LONG_LOCK_FREE       __GCC_ATOMIC_LONG_LOCK_FREE
#define ATOMIC_LLONG_LOCK_FREE      __GCC_ATOMIC_LLONG_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE    __GCC_ATOMIC_POINTER_LOCK_FREE

#define ATOMIC_FLAG_INIT            0
#define ATOMIC_VAR_INIT(value)      (value)

typedef enum memory_order
{
    memory_order_relaxed        = __ATOMIC_RELAXED,
    memory_order_consume        = __ATOMIC_CONSUME,
    memory_order_acquire        = __ATOMIC_ACQUIRE,
    memory_order_release        = __ATOMIC_RELEASE,
    memory_order_acq_rel        = __ATOMIC_ACQ_REL,
    memory_order_seq_cst        = __ATOMIC_SEQ_CST,
} memory_order;

typedef _Atomic int atomic_flag;


#define atomic_init(obj, value)     ((*obj) = (value))

#define kill_dependency(x)          (y)


typedef _Atomic _Bool               atomic_bool;
typedef _Atomic char                atomic_char;
typedef _Atomic signed char         atomic_schar;
typedef _Atomic unsigned char       atomic_uchar;
typedef _Atomic short               atomic_short;
typedef _Atomic unsigned short      atomic_ushort;
typedef _Atomic int                 atomic_int;
typedef _Atomic unsigned int        atomic_uint;
typedef _Atomic long                atomic_long;
typedef _Atomic unsigned long       atomic_ulong;
typedef _Atomic long long           atomic_llong;
typedef _Atomic unsigned long long  atomic_ullong;
typedef _Atomic __char16_t          atomic_char16_t;
typedef _Atomic __char32_t          atomic_char32_t;
typedef _Atomic __wchar_t           atomic_wchar_t;
typedef _Atomic __int_least8_t      atomic_int_least8_t;
typedef _Atomic __uint_least8_t     atomic_uint_least8_t;
typedef _Atomic __int_least16_t     atomic_int_least16_t;
typedef _Atomic __uint_least16_t    atomic_uint_least16_t;
typedef _Atomic __int_least32_t     atomic_int_least32_t;
typedef _Atomic __uint_least32_t    atomic_uint_least32_t;
typedef _Atomic __int_least64_t     atomic_int_least64_t;
typedef _Atomic __uint_least64_t    atomic_uint_least64_t;
typedef _Atomic __int_fast8_t       atomic_int_fast8_t;
typedef _Atomic __uint_fast8_t      atomic_uint_fast8_t;
typedef _Atomic __int_fast16_t      atomic_int_fast16_t;
typedef _Atomic __uint_fast16_t     atomic_uint_fast16_t;
typedef _Atomic __int_fast32_t      atomic_int_fast32_t;
typedef _Atomic __uint_fast32_t     atomic_uint_fast32_t;
typedef _Atomic __int_fast64_t      atomic_int_fast64_t;
typedef _Atomic __uint_fast64_t     atomic_uint_fast64_t;
typedef _Atomic __int_8_t           atomic_int_8_t;
typedef _Atomic __uint_8_t          atomic_uint_8_t;
typedef _Atomic __int_16_t          atomic_int_16_t;
typedef _Atomic __uint_16_t         atomic_uint_16_t;
typedef _Atomic __int_32_t          atomic_int_32_t;
typedef _Atomic __uint_32_t         atomic_uint_32_t;
typedef _Atomic __int_64_t          atomic_int_64_t;
typedef _Atomic __uint_64_t         atomic_uint_64_t;
typedef _Atomic __intptr_t          atomic_intptr_t;
typedef _Atomic __uintptr_t         atomic_uintptr_t;
typedef _Atomic __size_t            atomic_size_t;
typedef _Atomic __ptrdiff_t         atomic_ptrdiff_t;
typedef _Atomic __intmax_t          atomic_intmax_t;
typedef _Atomic __uintmax_t         atomic_uintmax_t;


#define atomic_thread_fence(mo)                     __atomic_thread_fence((mo))
#define atomic_signal_fence(mo)                     __atomic_signal_fence((mo))
#define atomic_is_lock_free(obj)                    __atomic_is_lock_free((obj))
#define atomic_store(obj, desire)                   __atomic_store((obj), &(desire), __ATOMIC_SEQ_CST)
#define atomic_store_explicit(obj, desire, mo)      __atomic_store((obj), &(desire), (mo))
#define atomic_load(obj)                            __atomic_load((obj), __ATOMIC_SEQ_CST)
#define atomic_load_explicit(obj, mo)               __atomic_load((obj), (mo))
#define atomic_exchange(obj, desire)                __atomic_exchange((obj), &(desire), __ATOMIC_SEQ_CST)
#define atomic_exchange_explicit(obj, desire, mo)   __atomic_exchange((obj), &(desire), (mo))
#define atomic_compare_exchange_strong(obj, exp, desire)                    __atomic_compare_exchange((obj), (exp), &(desire), 1, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#define atomic_compare_exchange_strong_explicit(obj, exp, desire, mos, mof) __atomic_compare_exchange((obj), (exp), &(desire), 1, (mos), (mof))
#define atomic_compare_exchange_weak(obj, exp, desire)                      __atomic_compare_exchange((obj), (exp), &(desire), 0, __ATOMIC_SEQ_CST, _ATOMIC_SEQ_CST)
#define atomic_compare_exchange_weak_explicit(obj, exp, desire, mos, mof)   __atomic_compare_exchange((obj), (exp), &(desire), 0, (mos), (mof))
#define atomic_fetch_add(obj, op)                   __atomic_fetch_add((obj), (op), __ATOMIC_SEQ_CST)
#define atomic_fetch_add_explicit(obj, op, mo)      __atomic_fetch_add((obj), (op), (mo))
#define atomic_fetch_sub(obj, op)                   __atomic_fetch_sub((obj), (op), __ATOMIC_SEQ_CST)
#define atomic_fetch_sub_explicit(obj, op, mo)      __atomic_fetch_sub((obj), (op), (mo))
#define atomic_fetch_and(obj, op)                   __atomic_fetch_and((obj), (op), __ATOMIC_SEQ_CST)
#define atomic_fetch_and_explicit(obj, op, mo)      __atomic_fetch_and((obj), (op), (mo))
#define atomic_fetch_or(obj, op)                    __atomic_fetch_or((obj), (op), __ATOMIC_SEQ_CST)
#define atomic_fetch_or_explicit(obj, op, mo)       __atomic_fetch_or((obj), (op), (mo))
#define atomic_fetch_xor(obj, op)                   __atomic_fetch_xor((obj), (op), __ATOMIC_SEQ_CST)
#define atomic_fetch_xor_explicit(obj, op, mo)      __atomic_fetch_xor((obj), (op), (mo))
#define atomic_flag_test_and_set(obj)               __atomic_test_and_set((obj), __ATOMIC_SEQ_CST)
#define atomic_flag_test_and_set_explicit(obj, mo)  __atomic_test_and_set((obj), (mo))
#define atomic_flag_clear(obj)                      __atomic_clear((obj), __ATOMIC_SEQ_CST)
#define atomic_flag_clear_explicit(obj, mo)         __atomic_clear((obj), (mo))


#endif // ifndef __cplusplus


#endif /* ifndef __SYSTEM_C_STDLIB_STDATOMIC_H */
