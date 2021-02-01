#pragma once
#ifndef __SYSTEM_ABI_ATOMIC_ATOMIC_H
#define __SYSTEM_ABI_ATOMIC_ATOMIC_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


/* In order to avoid name clashes with compiler built-in functions, the names of the functions in this file are
 * capitalised in the source code and re-named to the non-capitalised forms using attributes. They are required to have
 * the non-capitalised forms in object code as the compiler automatically generates references to them.
 */
#if 0
#  define __ATOMIC_NAME(n) __ATOMIC_NAME_(n)
#  define __ATOMIC_NAME_(n) __attribute__((__alias__(#n)))
#else
#  define __ATOMIC_NAME(n) __ATOMIC_NAME_(n)
#  define __ATOMIC_NAME_(n) __asm__(#n)
#endif

#define __memory_order_relaxed  0
#define __memory_order_consume  1
#define __memory_order_acquire  2
#define __memory_order_release  3
#define __memory_order_acq_rel  4
#define __memory_order_seq_cst  5


#if __SYSTEM_ABI_ATOMIC_BUILD_SHARED
#  define __SYSTEM_ABI_ATOMIC_EXPORT [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_ABI_ATOMIC_EXPORT [[gnu::visibility("hidden")]]
#endif


namespace System::ABI::Atomic
{


using std::size_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;


#if !defined(__LP64__)
struct __uint128_t
{
    uint64_t __elem[2];
};
#endif


__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_is_lock_free(size_t, const volatile void*) __ATOMIC_NAME(__atomic_is_lock_free);

__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_load(size_t __size, const volatile void* __ptr, void* __ret, int __order) __ATOMIC_NAME(__atomic_load);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_store(size_t __size, volatile void* __ptr, void* __val, int __order) __ATOMIC_NAME(__atomic_store);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_exchange(size_t __size, volatile void* __ptr, void* __val, void* __ret, int __order) __ATOMIC_NAME(__atomic_exchange);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_compare_exchange(size_t __size, volatile void* __ptr, void* __expect, void* __desire, int __success, int __fail) __ATOMIC_NAME(__atomic_compare_exchange);

__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_test_and_set(volatile void*) __ATOMIC_NAME(atomic_flag_test_and_set);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_test_and_set_explicit(volatile void*, int) __ATOMIC_NAME(atomic_flag_test_and_set_explicit);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_clear(volatile void*, int) __ATOMIC_NAME(atomic_flag_clear);

__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_signal_fence(int) __ATOMIC_NAME(atomic_signal_fence);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_thread_fence(int) __ATOMIC_NAME(atomic_thread_fence);

__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_test_and_set_1(volatile void*, int) __ATOMIC_NAME(__atomic_test_and_set_1);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_test_and_set_2(volatile void*, int) __ATOMIC_NAME(__atomic_test_and_set_2);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_test_and_set_4(volatile void*, int) __ATOMIC_NAME(__atomic_test_and_set_4);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_test_and_set_8(volatile void*, int) __ATOMIC_NAME(__atomic_test_and_set_8);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_test_and_set_16(volatile void*, int) __ATOMIC_NAME(__atomic_test_and_set_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t   __Atomic_load_1(const volatile void*, int) __ATOMIC_NAME(__atomic_load_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t  __Atomic_load_2(const volatile void*, int) __ATOMIC_NAME(__atomic_load_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t  __Atomic_load_4(const volatile void*, int) __ATOMIC_NAME(__atomic_load_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t  __Atomic_load_8(const volatile void*, int) __ATOMIC_NAME(__atomic_load_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_load_16(const volatile void*, int) __ATOMIC_NAME(__atomic_load_16);

__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_store_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_store_1);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_store_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_store_2);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_store_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_store_4);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_store_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_store_8);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_store_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_store_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t   __Atomic_exchange_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_exchange_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t  __Atomic_exchange_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_exchange_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t  __Atomic_exchange_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_exchange_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t  __Atomic_exchange_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_exchange_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_exchange_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_exchange_16);

__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_compare_exchange_1(volatile void*, void*, uint8_t, int, int) __ATOMIC_NAME(__atomic_compare_exchange_1);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_compare_exchange_2(volatile void*, void*, uint16_t, int, int) __ATOMIC_NAME(__atomic_compare_exchange_2);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_compare_exchange_4(volatile void*, void*, uint32_t, int, int) __ATOMIC_NAME(__atomic_compare_exchange_4);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_compare_exchange_8(volatile void*, void*, uint64_t, int, int) __ATOMIC_NAME(__atomic_compare_exchange_8);
__SYSTEM_ABI_ATOMIC_EXPORT bool __Atomic_compare_exchange_16(volatile void*, void*, __uint128_t, int, int) __ATOMIC_NAME(__atomic_compare_exchange_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_fetch_add_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_fetch_add_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_fetch_add_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_fetch_add_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_fetch_add_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_fetch_add_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_fetch_add_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_fetch_add_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_fetch_add_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_fetch_add_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_add_fetch_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_add_fetch_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_add_fetch_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_add_fetch_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_add_fetch_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_add_fetch_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_add_fetch_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_add_fetch_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_add_fetch_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_add_fetch_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_fetch_sub_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_fetch_sub_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_fetch_sub_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_fetch_sub_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_fetch_sub_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_fetch_sub_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_fetch_sub_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_fetch_sub_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_fetch_sub_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_fetch_sub_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_sub_fetch_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_sub_fetch_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_sub_fetch_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_sub_fetch_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_sub_fetch_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_sub_fetch_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_sub_fetch_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_sub_fetch_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_sub_fetch_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_sub_fetch_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_fetch_and_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_fetch_and_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_fetch_and_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_fetch_and_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_fetch_and_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_fetch_and_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_fetch_and_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_fetch_and_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_fetch_and_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_fetch_and_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_and_fetch_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_and_fetch_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_and_fetch_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_and_fetch_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_and_fetch_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_and_fetch_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_and_fetch_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_and_fetch_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_and_fetch_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_and_fetch_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_fetch_or_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_fetch_or_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_fetch_or_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_fetch_or_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_fetch_or_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_fetch_or_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_fetch_or_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_fetch_or_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_fetch_or_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_fetch_or_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_or_fetch_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_or_fetch_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_or_fetch_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_or_fetch_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_or_fetch_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_or_fetch_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_or_fetch_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_or_fetch_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_or_fetch_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_or_fetch_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_fetch_xor_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_fetch_xor_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_fetch_xor_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_fetch_xor_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_fetch_xor_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_fetch_xor_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_fetch_xor_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_fetch_xor_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_fetch_xor_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_fetch_xor_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_xor_fetch_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_xor_fetch_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_xor_fetch_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_xor_fetch_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_xor_fetch_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_xor_fetch_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_xor_fetch_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_xor_fetch_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_xor_fetch_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_xor_fetch_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_fetch_nand_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_fetch_nand_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_fetch_nand_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_fetch_nand_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_fetch_nand_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_fetch_nand_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_fetch_nand_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_fetch_nand_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_fetch_nand_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_fetch_nand_16);

__SYSTEM_ABI_ATOMIC_EXPORT uint8_t __Atomic_nand_fetch_1(volatile void*, uint8_t, int) __ATOMIC_NAME(__atomic_nand_fetch_1);
__SYSTEM_ABI_ATOMIC_EXPORT uint16_t __Atomic_nand_fetch_2(volatile void*, uint16_t, int) __ATOMIC_NAME(__atomic_nand_fetch_2);
__SYSTEM_ABI_ATOMIC_EXPORT uint32_t __Atomic_nand_fetch_4(volatile void*, uint32_t, int) __ATOMIC_NAME(__atomic_nand_fetch_4);
__SYSTEM_ABI_ATOMIC_EXPORT uint64_t __Atomic_nand_fetch_8(volatile void*, uint64_t, int) __ATOMIC_NAME(__atomic_nand_fetch_8);
__SYSTEM_ABI_ATOMIC_EXPORT __uint128_t __Atomic_nand_fetch_16(volatile void*, __uint128_t, int) __ATOMIC_NAME(__atomic_nand_fetch_16);

#if 0
// Probably not appropriate as part of the ABI.Atomic library.
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_wait_1(const volatile uint8_t*) __ATOMIC_NAME(__atomic_wait_1);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_wait_2(const volatile uint16_t*) __ATOMIC_NAME(__atomic_wait_2);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_wait_4(const volatile uint32_t*) __ATOMIC_NAME(__atomic_wait_4);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_wait_8(const volatile uint64_t*) __ATOMIC_NAME(__atomic_wait_8);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_wait_16(const volatile __uint128_t*) __ATOMIC_NAME(__atomic_wait_16);

__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_one_1(const volatile uint8_t*) __ATOMIC_NAME(__atomic_notify_one_1);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_one_2(const volatile uint16_t*) __ATOMIC_NAME(__atomic_notify_one_2);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_one_4(const volatile uint32_t*) __ATOMIC_NAME(__atomic_notify_one_4);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_one_8(const volatile uint64_t*) __ATOMIC_NAME(__atomic_notify_one_8);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_one_16(const volatile __uint128_t*) __ATOMIC_NAME(__atomic_notify_one_16);

__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_all_1(const volatile uint8_t*) __ATOMIC_NAME(__atomic_notify_all_1);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_all_2(const volatile uint16_t*) __ATOMIC_NAME(__atomic_notify_all_2);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_all_4(const volatile uint32_t*) __ATOMIC_NAME(__atomic_notify_all_4);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_all_8(const volatile uint64_t*) __ATOMIC_NAME(__atomic_notify_all_8);
__SYSTEM_ABI_ATOMIC_EXPORT void __Atomic_notify_all_16(const volatile __uint128_t*) __ATOMIC_NAME(__atomic_notify_all_16);
#endif


} // namespace System::ABI::Atomic


#endif /* ifndef __SYSTEM_ABI_ATOMIC_ATOMIC_H */
