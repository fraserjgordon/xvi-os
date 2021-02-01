#pragma once
#ifndef __SYSTEM_CXX_ATOMIC_ATOMIC_H
#define __SYSTEM_CXX_ATOMIC_ATOMIC_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Atomic/Private/Config.hh>


namespace __XVI_STD_ATOMIC_NS
{


enum class memory_order : int
{
    relaxed = __ATOMIC_RELAXED,
    consume = __ATOMIC_CONSUME,
    acquire = __ATOMIC_ACQUIRE,
    release = __ATOMIC_RELEASE,
    acq_rel = __ATOMIC_ACQ_REL,
    seq_cst = __ATOMIC_SEQ_CST,
};

inline constexpr memory_order memory_order_relaxed = memory_order::relaxed;
inline constexpr memory_order memory_order_consume = memory_order::consume;
inline constexpr memory_order memory_order_acquire = memory_order::acquire;
inline constexpr memory_order memory_order_release = memory_order::release;
inline constexpr memory_order memory_order_acq_rel = memory_order::acq_rel;
inline constexpr memory_order memory_order_seq_cst = memory_order::seq_cst;


template <class _T>
_T kill_dependency(_T __y) noexcept
{
    return __y;
}


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


#define ATOMIC_VAR_INIT(__value)    __value


namespace __detail
{

template <class _T> using __atomic_storage_equivalent = conditional_t
<
    sizeof(_T) <= sizeof(uint8_t) && alignof(_T) <= alignof(uint8_t),
    uint8_t,
    conditional_t
    <
        sizeof(_T) <= sizeof(uint16_t) && alignof(_T) <= alignof(uint16_t),
        uint16_t,
        conditional_t
        <
            sizeof(_T) <= sizeof(uint32_t) && alignof(_T) <= alignof(uint32_t),
            uint32_t,
            conditional_t
            <
                sizeof(_T) <= sizeof(uint64_t) && alignof(_T) <= alignof(uint64_t),
                uint64_t,
                uintptr_t[(sizeof(_T) + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)]
            >
        >
    >
>;

template <class _T> using __aligned_atomic_storage_t = aligned_union_t<sizeof(_T), _T, __atomic_storage_equivalent<_T>>;

template <class _T, class _U>
union __atomic_storage_union
{
    _T __init;
    _U __value;

    //! @TODO: check GCC/Clang for necessity for e.g. 3-byte objects.   
    //__aligned_atomic_storage_t<_T> __force_size_and_alignment;
};

template <size_t> struct __integral_atomic_ops; // Not defined.

template <> struct __integral_atomic_ops<sizeof(uint8_t)>
{
    using __type = uint8_t;

    static uint8_t load(const volatile uint8_t* __ptr, memory_order __mo)
    {
        return __atomic_load_n(__ptr, int(__mo));
    }

    static void store(volatile uint8_t* __ptr, uint8_t __val, memory_order __mo)
    {
        __atomic_store_n(__ptr, __val, int(__mo));
    }

    static uint8_t exchange(volatile uint8_t* __ptr, uint8_t __val, memory_order __mo)
    {
        return __atomic_exchange_n(__ptr, __val, int(__mo));
    }

    static bool compare_exchange_weak(volatile uint8_t* __ptr, uint8_t* __expect, uint8_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, true, int(__s), int(__f));
    }

    static bool compare_exchange_strong(volatile uint8_t* __ptr, uint8_t* __expect, uint8_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, false, int(__s), int(__f));
    }

    static uint8_t fetch_add(volatile uint8_t* __ptr, uint8_t __op, memory_order __mo)
    {
        return __atomic_fetch_add(__ptr, __op, int(__mo));
    }

    static uint8_t fetch_sub(volatile uint8_t* __ptr, uint8_t __op, memory_order __mo)
    {
        return __atomic_fetch_sub(__ptr, __op, int(__mo));
    }

    static uint8_t fetch_and(volatile uint8_t* __ptr, uint8_t __op, memory_order __mo)
    {
        return __atomic_fetch_and(__ptr, __op, int(__mo));
    }

    static uint8_t fetch_or(volatile uint8_t* __ptr, uint8_t __op, memory_order __mo)
    {
        return __atomic_fetch_or(__ptr, __op, int(__mo));
    }

    static uint8_t fetch_xor(volatile uint8_t* __ptr, uint8_t __op, memory_order __mo)
    {
        return __atomic_fetch_xor(__ptr, __op, int(__mo));
    }
};

template <> struct __integral_atomic_ops<sizeof(uint16_t)>
{
    using __type = uint16_t;

    static uint16_t load(const volatile uint16_t* __ptr, memory_order __mo)
    {
        return __atomic_load_n(__ptr, int(__mo));
    }

    static void store(volatile uint16_t* __ptr, uint16_t __val, memory_order __mo)
    {
        __atomic_store_n(__ptr, __val, int(__mo));
    }

    static uint16_t exchange(volatile uint16_t* __ptr, uint16_t __val, memory_order __mo)
    {
        return __atomic_exchange_n(__ptr, __val, int(__mo));
    }

    static bool compare_exchange_weak(volatile uint16_t* __ptr, uint16_t* __expect, uint16_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, true, int(__s), int(__f));
    }

    static bool compare_exchange_strong(volatile uint16_t* __ptr, uint16_t* __expect, uint16_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, false, int(__s), int(__f));
    }

    static uint16_t fetch_add(volatile uint16_t* __ptr, uint16_t __op, memory_order __mo)
    {
        return __atomic_fetch_add(__ptr, __op, int(__mo));
    }

    static uint16_t fetch_sub(volatile uint16_t* __ptr, uint16_t __op, memory_order __mo)
    {
        return __atomic_fetch_sub(__ptr, __op, int(__mo));
    }

    static uint16_t fetch_and(volatile uint16_t* __ptr, uint16_t __op, memory_order __mo)
    {
        return __atomic_fetch_and(__ptr, __op, int(__mo));
    }

    static uint16_t fetch_or(volatile uint16_t* __ptr, uint16_t __op, memory_order __mo)
    {
        return __atomic_fetch_or(__ptr, __op, int(__mo));
    }

    static uint16_t fetch_xor(volatile uint16_t* __ptr, uint16_t __op, memory_order __mo)
    {
        return __atomic_fetch_xor(__ptr, __op, int(__mo));
    }
};

template <> struct __integral_atomic_ops<sizeof(uint32_t)>
{
    using __type = uint32_t;

    static uint32_t load(const volatile uint32_t* __ptr, memory_order __mo)
    {
        return __atomic_load_n(__ptr, int(__mo));
    }

    static void store(volatile uint32_t* __ptr, uint32_t __val, memory_order __mo)
    {
        __atomic_store_n(__ptr, __val, int(__mo));
    }

    static uint32_t exchange(volatile uint32_t* __ptr, uint32_t __val, memory_order __mo)
    {
        return __atomic_exchange_n(__ptr, __val, int(__mo));
    }

    static bool compare_exchange_weak(volatile uint32_t* __ptr, uint32_t* __expect, uint32_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, true, int(__s), int(__f));
    }

    static bool compare_exchange_strong(volatile uint32_t* __ptr, uint32_t* __expect, uint32_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, false, int(__s), int(__f));
    }

    static uint32_t fetch_add(volatile uint32_t* __ptr, uint32_t __op, memory_order __mo)
    {
        return __atomic_fetch_add(__ptr, __op, int(__mo));
    }

    static uint32_t fetch_sub(volatile uint32_t* __ptr, uint32_t __op, memory_order __mo)
    {
        return __atomic_fetch_sub(__ptr, __op, int(__mo));
    }

    static uint32_t fetch_and(volatile uint32_t* __ptr, uint32_t __op, memory_order __mo)
    {
        return __atomic_fetch_and(__ptr, __op, int(__mo));
    }

    static uint32_t fetch_or(volatile uint32_t* __ptr, uint32_t __op, memory_order __mo)
    {
        return __atomic_fetch_or(__ptr, __op, int(__mo));
    }

    static uint32_t fetch_xor(volatile uint32_t* __ptr, uint32_t __op, memory_order __mo)
    {
        return __atomic_fetch_xor(__ptr, __op, int(__mo));
    }
};

template <> struct __integral_atomic_ops<sizeof(uint64_t)>
{
    using __type = uint64_t;

    static uint64_t load(const volatile uint64_t* __ptr, memory_order __mo)
    {
        return __atomic_load_n(__ptr, int(__mo));
    }

    static void store(volatile uint64_t* __ptr, uint64_t __val, memory_order __mo)
    {
        __atomic_store_n(__ptr, __val, int(__mo));
    }

    static uint64_t exchange(volatile uint64_t* __ptr, uint64_t __val, memory_order __mo)
    {
        return __atomic_exchange_n(__ptr, __val, int(__mo));
    }

    static bool compare_exchange_weak(volatile uint64_t* __ptr, uint64_t* __expect, uint64_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, true, int(__s), int(__f));
    }

    static bool compare_exchange_strong(volatile uint64_t* __ptr, uint64_t* __expect, uint64_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, false, int(__s), int(__f));
    }

    static uint64_t fetch_add(volatile uint64_t* __ptr, uint64_t __op, memory_order __mo)
    {
        return __atomic_fetch_add(__ptr, __op, int(__mo));
    }

    static uint64_t fetch_sub(volatile uint64_t* __ptr, uint64_t __op, memory_order __mo)
    {
        return __atomic_fetch_sub(__ptr, __op, int(__mo));
    }

    static uint64_t fetch_and(volatile uint64_t* __ptr, uint64_t __op, memory_order __mo)
    {
        return __atomic_fetch_and(__ptr, __op, int(__mo));
    }

    static uint64_t fetch_or(volatile uint64_t* __ptr, uint64_t __op, memory_order __mo)
    {
        return __atomic_fetch_or(__ptr, __op, int(__mo));
    }

    static uint64_t fetch_xor(volatile uint64_t* __ptr, uint64_t __op, memory_order __mo)
    {
        return __atomic_fetch_xor(__ptr, __op, int(__mo));
    }
};

/*template <> struct __integral_atomic_ops<sizeof(__uint128_t)>
{
    using __type = __uint128_t;

    static __uint128_t load(const volatile __uint128_t* __ptr, memory_order __mo)
    {
        return __atomic_load_n(__ptr, int(__mo));
    }

    static void store(volatile __uint128_t* __ptr, __uint128_t __val, memory_order __mo)
    {
        __atomic_store_n(__ptr, __val, int(__mo));
    }

    static __uint128_t exchange(volatile __uint128_t* __ptr, __uint128_t __val, memory_order __mo)
    {
        return __atomic_exchange_n(__ptr, __val, int(__mo));
    }

    static bool compare_exchange_weak(volatile __uint128_t* __ptr, __uint128_t* __expect, __uint128_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, true, int(__s), int(__f));
    }

    static bool compare_exchange_strong(volatile __uint128_t* __ptr, __uint128_t* __expect, __uint128_t __desire, memory_order __s, memory_order __f)
    {
        return __atomic_compare_exchange_n(__ptr, __expect, __desire, false, int(__s), int(__f));
    }

    static __uint128_t fetch_add(volatile __uint128_t* __ptr, __uint128_t __op, memory_order __mo)
    {
        return __atomic_fetch_add(__ptr, __op, int(__mo));
    }

    static __uint128_t fetch_sub(volatile __uint128_t* __ptr, __uint128_t __op, memory_order __mo)
    {
        return __atomic_fetch_sub(__ptr, __op, int(__mo));
    }

    static __uint128_t fetch_and(volatile __uint128_t* __ptr, __uint128_t __op, memory_order __mo)
    {
        return __atomic_fetch_and(__ptr, __op, int(__mo));
    }

    static __uint128_t fetch_or(volatile __uint128_t* __ptr, __uint128_t __op, memory_order __mo)
    {
        return __atomic_fetch_or(__ptr, __op, int(__mo));
    }

    static __uint128_t fetch_xor(volatile __uint128_t* __ptr, __uint128_t __op, memory_order __mo)
    {
        return __atomic_fetch_xor(__ptr, __op, int(__mo));
    }
};*/


template <class _T> struct __integral_atomic_ref
{
private:

    using __ops = __integral_atomic_ops<sizeof(_T)>;
    using __type = typename __ops::__type;

public:

    static_assert(is_trivially_copyable_v<_T>);

    using value_type = _T;

    static constexpr bool is_always_lock_free   = __atomic_always_lock_free(sizeof(_T), 0);
    static constexpr size_t required_alignment  = alignof(_T);

    __integral_atomic_ref& operator=(const __integral_atomic_ref&) = delete;

    explicit __integral_atomic_ref(_T& __t)
        : _M_ptr(reinterpret_cast<__type*>(addressof(__t)))
    {
    }

    __integral_atomic_ref(const __integral_atomic_ref& __r) noexcept
        : _M_ptr(__r._M_ptr)
    {
    }

    _T operator=(_T __desired) const noexcept
    {
        store(__desired);
        return __desired;
    }

    operator _T() const noexcept
    {
        return load();
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), _M_ptr);
    }

    _T load(memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::load(_M_ptr, __mo));
    }

    void store(_T __val, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        __ops::store(_M_ptr, static_cast<__type>(__val), __mo);
    }

    _T exchange(_T __val, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::exchange(_M_ptr, static_cast<__type>(__val), __mo));
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) const noexcept
    {
        return __ops::compare_exchange_weak(_M_ptr, reinterpret_cast<__type*>(&__expect), static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) const noexcept
    {
        return __ops::compare_exchange_strong(_M_ptr, reinterpret_cast<__type*>(&__expect), static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return __ops::compare_exchange_weak(_M_ptr, reinterpret_cast<__type*>(&__expect), static_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return __ops::compare_exchange_strong(_M_ptr, reinterpret_cast<__type*>(&__expect), static_cast<__type>(__desire), __mo, __mo);
    }

    _T fetch_add(_T __op, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::fetch_add(_M_ptr, static_cast<__type>(__op), __mo));
    }

    _T fetch_sub(_T __op, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::fetch_sub(_M_ptr, static_cast<__type>(__op), __mo));
    }

    _T fetch_and(_T __op, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::fetch_and(_M_ptr, static_cast<__type>(__op), __mo));
    }

    _T fetch_or(_T __op, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::fetch_or(_M_ptr, static_cast<__type>(__op), __mo));
    }

    _T fetch_xor(_T __op, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::fetch_xor(_M_ptr, static_cast<__type>(__op), __mo));
    }

    _T operator++(int) const noexcept
    {
        return fetch_add(1);
    }

    _T operator--(int) const noexcept
    {
        return fetch_sub(1);
    }

    _T operator++() const noexcept
    {
        return fetch_add(1) + 1;
    }

    _T operator--() const noexcept
    {
        return fetch_sub(1) - 1;
    }

    _T operator+=(_T __val) const noexcept
    {
        return fetch_add(__val) + __val;
    }

    _T operator-=(_T __val) const noexcept
    {
        return fetch_sub(__val) - __val;
    }

    _T operator&=(_T __val) const noexcept
    {
        return fetch_and(__val) & __val;
    }

    _T operator|=(_T __val) const noexcept
    {
        return fetch_or(__val) | __val;
    }

    _T operator^=(_T __val) const noexcept
    {
        return fetch_xor(__val) ^ __val;
    }

private:

    __type* _M_ptr;
};


template <class _T> struct __pointer_atomic_ref
{
private:

    using __ops = __integral_atomic_ops<sizeof(uintptr_t)>;
    using __type = typename __ops::__type;

public:

    static_assert(is_pointer_v<_T>);

    using value_type = _T;
    using difference_type = ptrdiff_t;

    static constexpr bool is_always_lock_free   = __atomic_always_lock_free(sizeof(uintptr_t), nullptr);
    static constexpr size_t required_alignment  = alignof(uintptr_t);

    __pointer_atomic_ref& operator=(const __pointer_atomic_ref&) = delete;

    explicit __pointer_atomic_ref(_T& __t)
        : _M_ptr(addressof(__t))
    {
    }

    __pointer_atomic_ref(const __pointer_atomic_ref& __r) noexcept
        : _M_ptr(__r._M_ptr)
    {
    }

    _T operator=(_T __desired) const noexcept
    {
        store(__desired);
        return __desired;
    }

    operator _T() const noexcept
    {
        return load();
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), _M_ptr);
    }

    _T load(memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return reinterpret_cast<_T>(__ops::load(_M_ptr, __mo));
    }

    void store(_T __val, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        __ops::store(_M_ptr, static_cast<__type>(__val), __mo);
    }

    _T exchange(_T __val, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return reinterpret_cast<_T>(__ops::exchange(_M_ptr, static_cast<__type>(__val), __mo));
    }

    bool compare_exchange_weak(_T* __expect, _T __desire, memory_order __s, memory_order __f) const noexcept
    {
        return __ops::compare_exchange_weak(_M_ptr, __expect, static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_strong(_T* __expect, _T __desire, memory_order __s, memory_order __f) const noexcept
    {
        return __ops::compare_exchange_strong(_M_ptr, __expect, static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_weak(_T* __expect, _T __desire, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return __ops::compare_exchange_weak(_M_ptr, __expect, static_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_strong(_T* __expect, _T __desire, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return __ops::compare_exchange_strong(_M_ptr, __expect, static_cast<__type>(__desire), __mo, __mo);
    }

    _T fetch_add(difference_type __op, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return reinterpret_cast<_T>(__ops::fetch_add(_M_ptr, static_cast<__type>(__op), __mo));
    }

    _T fetch_sub(difference_type __op, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return reinterpret_cast<_T>(__ops::fetch_sub(_M_ptr, static_cast<__type>(__op), __mo));
    }

    _T operator++(int) const noexcept
    {
        return fetch_add(1);
    }

    _T operator--(int) const noexcept
    {
        return fetch_sub(1);
    }

    _T operator++() const noexcept
    {
        return fetch_add(1) + 1;
    }

    _T operator--() const noexcept
    {
        return fetch_sub(1) - 1;
    }

    _T operator+=(difference_type __val) const noexcept
    {
        return fetch_add(__val) + __val;
    }

    _T operator-=(difference_type __val) const noexcept
    {
        return fetch_sub(__val) - __val;
    }

private:

    __type* _M_ptr;
};


template <class _T> struct __atomic_integer
{
private:

    using __ops = __integral_atomic_ops<sizeof(_T)>;
    using __type = typename __ops::__type;

public:

    static_assert(is_trivially_copyable_v<_T>);

    using value_type = _T;

    static constexpr bool is_always_lock_free = __atomic_always_lock_free(sizeof(_T), 0);

    __atomic_integer() noexcept = default;

    constexpr __atomic_integer(_T __t) noexcept
        : _M_storage{.__value = static_cast<__type>(__t)}
    {
    }

    __atomic_integer(const __atomic_integer&) = delete;

    __atomic_integer& operator=(const __atomic_integer&) = delete;
    __atomic_integer& operator=(const __atomic_integer&) volatile = delete;

    bool is_lock_free() const volatile noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), &_M_storage.__value);
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), &_M_storage.__value);
    }

    void store(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        __ops::store(&_M_storage.__value, static_cast<__type>(__t), __mo);
    }

    void store(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        __ops::store(&_M_storage.__value, static_cast<__type>(__t), __mo);
    }

    _T operator=(_T __t) volatile noexcept
    {
        store(__t);
        return __t;
    }

    _T operator=(_T __t) noexcept
    {
        store(__t);
        return __t;
    }

    _T load(memory_order __mo = memory_order::seq_cst) const volatile noexcept
    {
        return static_cast<_T>(__ops::load(&_M_storage.__value, __mo));
    }

    _T load(memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return static_cast<_T>(__ops::load(&_M_storage.__value, __mo));
    }

    operator _T() const volatile noexcept
    {
        return load();
    }

    operator _T() const noexcept
    {
        return load();
    }

    _T exchange(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return static_cast<_T>(__ops::exchange(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T exchange(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return static_cast<_T>(__ops::exchange(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) volatile noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) volatile noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), static_cast<__type>(__desire), __mo, __mo);
    }

    _T fetch_add(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return static_cast<_T>(__ops::fetch_add(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_add(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return static_cast<_T>(__ops::fetch_add(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_sub(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return static_cast<_T>(__ops::fetch_sub(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_sub(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return static_cast<_T>(__ops::fetch_sub(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_and(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return static_cast<_T>(__ops::fetch_and(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_and(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return static_cast<_T>(__ops::fetch_and(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_or(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return static_cast<_T>(__ops::fetch_or(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_or(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return static_cast<_T>(__ops::fetch_or(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_xor(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return static_cast<_T>(__ops::fetch_xor(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T fetch_xor(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return static_cast<_T>(__ops::fetch_xor(&_M_storage.__value, static_cast<__type>(__t), __mo));
    }

    _T operator++(int) volatile noexcept
    {
        return fetch_add(1);
    }

    _T operator++(int) noexcept
    {
        return fetch_add(1);
    }

    _T operator--(int) volatile noexcept
    {
        return fetch_sub(1);
    }

    _T operator--(int) noexcept
    {
        return fetch_sub(1);
    }

    _T operator++() volatile noexcept
    {
        return fetch_add(1) + 1;
    }

    _T operator++() noexcept
    {
        return fetch_add(1) + 1;
    }

    _T operator--() volatile noexcept
    {
        return fetch_sub(1) - 1;
    }

    _T operator--() noexcept
    {
        return fetch_sub(1) - 1;
    }

    _T operator+=(_T __t) volatile noexcept
    {
        return fetch_add(__t) + __t;
    }

    _T operator+=(_T __t) noexcept
    {
        return fetch_add(__t) + __t;
    }

    _T operator-=(_T __t) volatile noexcept
    {
        return fetch_sub(__t) - __t;
    }

    _T operator-=(_T __t) noexcept
    {
        return fetch_sub(__t) - __t;
    }

    _T operator&=(_T __t) volatile noexcept
    {
        return fetch_and(__t) & __t;
    }

    _T operator&=(_T __t) noexcept
    {
        return fetch_and(__t) & __t;
    }

    _T operator|=(_T __t) volatile noexcept
    {
        return fetch_or(__t) | __t;
    }

    _T operator|=(_T __t) noexcept
    {
        return fetch_or(__t) | __t;
    }

    _T operator^=(_T __t) volatile noexcept
    {
        return fetch_xor(__t) ^ __t;
    }

    _T operator^=(_T __t) noexcept
    {
        return fetch_xor(__t) ^ __t;
    }

    using __storage_t = __detail::__atomic_storage_union<_T, __type>;
    __storage_t _M_storage;
};

template <class _T> struct __atomic_pointer
{
private:

    using __ops = __integral_atomic_ops<sizeof(uintptr_t)>;
    using __type = typename __ops::__type;

public:

    static_assert(is_trivially_copyable_v<_T>);

    using value_type = _T;
    using difference_type = ptrdiff_t;

    static constexpr bool is_always_lock_free = __atomic_always_lock_free(sizeof(_T), 0);

    __atomic_pointer() noexcept = default;

    constexpr __atomic_pointer(_T __t) noexcept
        : _M_storage{.__value = reinterpret_cast<__type>(__t)}
    {
    }

    __atomic_pointer(const __atomic_pointer&) = delete;

    __atomic_pointer& operator=(const __atomic_pointer&) = delete;
    __atomic_pointer& operator=(const __atomic_pointer&) volatile = delete;

    bool is_lock_free() const volatile noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), &_M_storage.__value);
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), &_M_storage.__value);
    }

    void store(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        __ops::store(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo);
    }

    void store(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        __ops::store(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo);
    }

    _T operator=(_T __t) volatile noexcept
    {
        store(__t);
        return __t;
    }

    _T operator=(_T __t) noexcept
    {
        store(__t);
        return __t;
    }

    _T load(memory_order __mo = memory_order::seq_cst) const volatile noexcept
    {
        return reinterpret_cast<_T>(__ops::load(&_M_storage.__value, __mo));
    }

    _T load(memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return reinterpret_cast<_T>(__ops::load(&_M_storage.__value, __mo));
    }

    operator _T() const volatile noexcept
    {
        return load();
    }

    operator _T() const noexcept
    {
        return load();
    }

    _T exchange(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return reinterpret_cast<_T>(__ops::exchange(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo));
    }

    _T exchange(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return reinterpret_cast<_T>(__ops::exchange(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo));
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) volatile noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) volatile noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __s, __f);
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return __ops::compare_exchange_weak(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __mo, __mo);
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return __ops::compare_exchange_strong(&_M_storage.__value, &reinterpret_cast<__type&>(__expect), reinterpret_cast<__type>(__desire), __mo, __mo);
    }

    _T fetch_add(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return reinterpret_cast<_T>(__ops::fetch_add(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo));
    }

    _T fetch_add(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return reinterpret_cast<_T>(__ops::fetch_add(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo));
    }

    _T fetch_sub(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return reinterpret_cast<_T>(__ops::fetch_sub(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo));
    }

    _T fetch_sub(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return reinterpret_cast<_T>(__ops::fetch_sub(&_M_storage.__value, reinterpret_cast<__type>(__t), __mo));
    }

    _T operator++(int) volatile noexcept
    {
        return fetch_add(1);
    }

    _T operator++(int) noexcept
    {
        return fetch_add(1);
    }

    _T operator--(int) volatile noexcept
    {
        return fetch_sub(1);
    }

    _T operator--(int) noexcept
    {
        return fetch_sub(1);
    }

    _T operator++() volatile noexcept
    {
        return fetch_add(1) + 1;
    }

    _T operator++() noexcept
    {
        return fetch_add(1) + 1;
    }

    _T operator--() volatile noexcept
    {
        return fetch_sub(1) - 1;
    }

    _T operator--() noexcept
    {
        return fetch_sub(1) - 1;
    }

    _T operator+=(_T __t) volatile noexcept
    {
        return fetch_add(__t) + __t;
    }

    _T operator+=(_T __t) noexcept
    {
        return fetch_add(__t) + __t;
    }

    _T operator-=(_T __t) volatile noexcept
    {
        return fetch_sub(__t) - __t;
    }

    _T operator-=(_T __t) noexcept
    {
        return fetch_sub(__t) - __t;
    }

    using __storage_t = __detail::__atomic_storage_union<_T, __type>;
    __storage_t _M_storage;
};


} // namespace __detail


template <class _T>
struct atomic_ref
{
public:

    static_assert(is_trivially_copyable_v<_T>);

    using value_type = _T;

    static constexpr bool is_always_lock_free   = __atomic_always_lock_free(sizeof(_T), 0);
    static constexpr size_t required_alignment  = alignof(_T);

    atomic_ref& operator=(const atomic_ref&) = delete;

    explicit atomic_ref(_T& __t)
        : _M_ptr(addressof(__t))
    {
    }

    atomic_ref(const atomic_ref& __r) noexcept
        : _M_ptr(__r._M_ptr)
    {
    }

    _T operator=(_T __desired) const noexcept
    {
        store(__desired);
        return __desired;
    }

    operator _T() const noexcept
    {
        return load();
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), _M_ptr);
    }

    void store(_T __desired, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        __atomic_store(_M_ptr, &__desired, int(__mo));
    }

    _T load(memory_order __mo = memory_order::seq_cst) const noexcept
    {
        _T __val;
        __atomic_load(_M_ptr, &__val, int(__mo));
        return __val;
    }

    _T exchange(_T __desired, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        _T __val;
        __atomic_exchange(_M_ptr, &__desired, &__val, int(__mo));
        return __val;
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) const noexcept
    {
        return __atomic_compare_exchange(_M_ptr, &__expect, &__desire, true, int(__s), int(__f));
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) const noexcept
    {
        return __atomic_compare_exchange(_M_ptr, &__expect, &__desire, false, int(__s), int(__f));
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return __atomic_compare_exchange(_M_ptr, &__expect, &__desire, true, int(__mo), int(__mo));
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) const noexcept
    {
        return __atomic_compare_exchange(_M_ptr, &__expect, &__desire, false, int(__mo), int(__mo));
    }

    void wait(_T, memory_order __mo = memory_order::seq_cst) const noexcept;

    void notify_one() const noexcept;

    void notify_all() const noexcept;

private:

    _T* _M_ptr;
};


template <> struct atomic_ref<char>
    : __detail::__integral_atomic_ref<char>
        { using __detail::__integral_atomic_ref<char>::__integral_atomic_ref; };
template <> struct atomic_ref<signed char>
    : __detail::__integral_atomic_ref<signed char>
        { using __detail::__integral_atomic_ref<signed char>::__integral_atomic_ref; };
template <> struct atomic_ref<unsigned char>
    : __detail::__integral_atomic_ref<unsigned char>
        { using __detail::__integral_atomic_ref<unsigned char>::__integral_atomic_ref; };
template <> struct atomic_ref<short>
    : __detail::__integral_atomic_ref<short>
        { using __detail::__integral_atomic_ref<short>::__integral_atomic_ref; };
template <> struct atomic_ref<unsigned short>
    : __detail::__integral_atomic_ref<unsigned short>
        { using __detail::__integral_atomic_ref<unsigned short>::__integral_atomic_ref; };
template <> struct atomic_ref<int>
    : __detail::__integral_atomic_ref<int>
        { using __detail::__integral_atomic_ref<int>::__integral_atomic_ref; };
template <> struct atomic_ref<unsigned int>
    : __detail::__integral_atomic_ref<unsigned int>
        { using __detail::__integral_atomic_ref<unsigned int>::__integral_atomic_ref; };
template <> struct atomic_ref<long>
    : __detail::__integral_atomic_ref<long>
        { using __detail::__integral_atomic_ref<long>::__integral_atomic_ref; };
template <> struct atomic_ref<unsigned long>
    : __detail::__integral_atomic_ref<unsigned long>
        { using __detail::__integral_atomic_ref<unsigned long>::__integral_atomic_ref; };
template <> struct atomic_ref<long long>
    : __detail::__integral_atomic_ref<long long>
        { using __detail::__integral_atomic_ref<long long>::__integral_atomic_ref; };
template <> struct atomic_ref<unsigned long long>
    : __detail::__integral_atomic_ref<unsigned long long>
        { using __detail::__integral_atomic_ref<unsigned long long>::__integral_atomic_ref; };
template <> struct atomic_ref<char8_t>
    : __detail::__integral_atomic_ref<char8_t>
        { using __detail::__integral_atomic_ref<char8_t>::__integral_atomic_ref; };
template <> struct atomic_ref<char16_t>
    : __detail::__integral_atomic_ref<char16_t>
        { using __detail::__integral_atomic_ref<char16_t>::__integral_atomic_ref; };
template <> struct atomic_ref<char32_t>
    : __detail::__integral_atomic_ref<char32_t>
        { using __detail::__integral_atomic_ref<char32_t>::__integral_atomic_ref; };
template <> struct atomic_ref<wchar_t>
    : __detail::__integral_atomic_ref<wchar_t>
        { using __detail::__integral_atomic_ref<wchar_t>::__integral_atomic_ref; };


//! @TODO: specialisations for floating-point atomic_ref types.
template <> struct atomic_ref<float> {};
template <> struct atomic_ref<double> {};
template <> struct atomic_ref<long double> {};


template <class _T> struct atomic_ref<_T*>
    : __detail::__pointer_atomic_ref<_T*>
        { using __detail::__pointer_atomic_ref<_T*>::__pointer_atomic_ref; };


template <class _T>
struct atomic
{
public:

    static_assert(is_trivially_copyable_v<_T>);

    using value_type = _T;

    static constexpr bool is_always_lock_free = __atomic_always_lock_free(sizeof(_T), 0);

    atomic() noexcept = default;

    constexpr atomic(_T __t) noexcept
        : _M_storage{.__value = __t}
    {
    }

    atomic(const atomic&) = delete;

    atomic& operator=(const atomic&) = delete;
    atomic& operator=(const atomic&) volatile = delete;

    bool is_lock_free() const volatile noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), &_M_storage.__value);
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_T), &_M_storage.__value);
    }

    void store(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        __atomic_store(&_M_storage.__value, &__t, int(__mo));
    }

    void store(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        __atomic_store(&_M_storage.__value, &__t, int(__mo));
    }

    _T operator=(_T __t) volatile noexcept
    {
        store(__t);
        return __t;
    }

    _T operator=(_T __t) noexcept
    {
        store(__t);
        return __t;
    }

    _T load(memory_order __mo = memory_order::seq_cst) const volatile noexcept
    {
        _T __ret;
        __atomic_load(&_M_storage.__value, &__ret, int(__mo));
        return __ret;
    }

    _T load(memory_order __mo = memory_order::seq_cst) const noexcept
    {
        _T __ret;
        __atomic_load(&_M_storage.__value, &__ret, int(__mo));
        return __ret;
    }

    operator _T() const volatile noexcept
    {
        return load();
    }

    operator _T() const noexcept
    {
        return load();
    }

    _T exchange(_T __t, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        _T __ret;
        __atomic_exchange(&_M_storage.__value, &__t, &__ret, int(__mo));
        return __ret;
    }

    _T exchange(_T __t, memory_order __mo = memory_order::seq_cst) noexcept
    {
        _T __ret;
        __atomic_exchange(&_M_storage.__value, &__t, &__ret, int(__mo));
        return __ret;
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, true, int(__s), int(__f));
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __s, memory_order __f) noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, true, int(__s), int(__f));
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, false, int(__s), int(__f));
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __s, memory_order __f) noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, false, int(__s), int(__f));
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, true, int(__mo), int(__mo));
    }

    bool compare_exchange_weak(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, true, int(__mo), int(__mo));
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, false, int(__mo), int(__mo));
    }

    bool compare_exchange_strong(_T& __expect, _T __desire, memory_order __mo = memory_order::seq_cst) noexcept
    {
        return __atomic_compare_exchange(&_M_storage.__value, &__expect, &__desire, false, int(__mo), int(__mo));
    }

    void wait(_T, memory_order __mo = memory_order::seq_cst) const noexcept;

    void notify_one() const noexcept;

    void notify_all() const noexcept;


    using __storage_t = __detail::__atomic_storage_union<_T, _T>;
    __storage_t _M_storage;
};


template <> struct atomic<char>
    : __detail::__atomic_integer<char>
        { using __detail::__atomic_integer<char>::__atomic_integer; };
template <> struct atomic<signed char>
    : __detail::__atomic_integer<signed char>
        { using __detail::__atomic_integer<signed char>::__atomic_integer; };
template <> struct atomic<unsigned char>
    : __detail::__atomic_integer<unsigned char>
        { using __detail::__atomic_integer<unsigned char>::__atomic_integer; };
template <> struct atomic<short>
    : __detail::__atomic_integer<short>
        { using __detail::__atomic_integer<short>::__atomic_integer; };
template <> struct atomic<unsigned short>
    : __detail::__atomic_integer<unsigned short>
        { using __detail::__atomic_integer<unsigned short>::__atomic_integer; };
template <> struct atomic<int>
    : __detail::__atomic_integer<int>
        { using __detail::__atomic_integer<int>::__atomic_integer; };
template <> struct atomic<unsigned int>
    : __detail::__atomic_integer<unsigned int>
        { using __detail::__atomic_integer<unsigned int>::__atomic_integer; };
template <> struct atomic<long>
    : __detail::__atomic_integer<long>
        { using __detail::__atomic_integer<long>::__atomic_integer; };
template <> struct atomic<unsigned long>
    : __detail::__atomic_integer<unsigned long>
        { using __detail::__atomic_integer<unsigned long>::__atomic_integer; };
template <> struct atomic<long long>
    : __detail::__atomic_integer<long long>
        { using __detail::__atomic_integer<long long>::__atomic_integer; };
template <> struct atomic<unsigned long long>
    : __detail::__atomic_integer<unsigned long long>
        { using __detail::__atomic_integer<unsigned long long>::__atomic_integer; };
template <> struct atomic<char8_t>
    : __detail::__atomic_integer<char8_t>
        { using __detail::__atomic_integer<char8_t>::__atomic_integer; };
template <> struct atomic<char16_t>
    : __detail::__atomic_integer<char16_t>
        { using __detail::__atomic_integer<char16_t>::__atomic_integer; };
template <> struct atomic<char32_t>
    : __detail::__atomic_integer<char32_t>
        { using __detail::__atomic_integer<char32_t>::__atomic_integer; };
template <> struct atomic<wchar_t>
    : __detail::__atomic_integer<wchar_t>
        { using __detail::__atomic_integer<wchar_t>::__atomic_integer; };


template <class _T> struct atomic<_T*>
    : __detail::__atomic_pointer<_T*>
        { using __detail::__atomic_pointer<_T*>::__atomic_pointer; };


//! @TODO: specialisations of atomic<> for floating-point types.
template <> struct atomic<float>;
template <> struct atomic<double>;
template <> struct atomic<long double>;


template <class _T>
void atomic_init(volatile atomic<_T>* __object, typename atomic<_T>::value_type __desired) noexcept
{
    __object->_M_storage.__init = __desired;
}

template <class _T>
void atomic_init(atomic<_T*>* __object, typename atomic<_T>::value_type __desired) noexcept
{
    __object->_M_storage.__init = __desired;
}

template <class _T>
bool atomic_is_lock_free(const volatile atomic<_T>* __object) noexcept
{
    return __object->is_lock_free();
}

template <class _T>
bool atomic_is_lock_free(const atomic<_T>* __object) noexcept
{
    return __object->is_lock_free();
}

template <class _T>
void atomic_store(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    __object->store(__t);
}

template <class _T>
void atomic_store(atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    __object->store(__t);
}

template <class _T>
void atomic_store_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    __object->store(__t, __mo);
}

template <class _T>
void atomic_store_explicit(atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    __object->store(__t, __mo);
}

template <class _T>
_T atomic_load(const volatile atomic<_T>* __object) noexcept
{
    return __object->load();
}

template <class _T>
_T atomic_load(const atomic<_T>* __object) noexcept
{
    return __object->load();
}

template <class _T>
_T atomic_load_explicit(const volatile atomic<_T>* __object, memory_order __mo) noexcept
{
    return __object->load(__mo);
}

template <class _T>
_T atomic_load_explicit(const atomic<_T>* __object, memory_order __mo) noexcept
{
    return __object->load(__mo);
}

template <class _T>
_T atomic_exchange(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    return __object->exchange(__t);
}

template <class _T>
_T atomic_exchange(atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    return __object->exchange(__t);
}

template <class _T>
_T atomic_exchange_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->exchange(__t, __mo);
}

template <class _T>
_T atomic_exchange_explicit(atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->exchange(__t, __mo);
}

template <class _T>
bool atomic_compare_exchange_weak(volatile atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire) noexcept
{
    return __object->compare_exchange_weak(*__expect, __desire);
}

template <class _T>
bool atomic_compare_exchange_weak(atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire) noexcept
{
    return __object->compare_exchange_weak(*__expect, __desire);
}

template <class _T>
bool atomic_compare_exchange_weak_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire, memory_order __s, memory_order __f) noexcept
{
    return __object->compare_exchange_weak(*__expect, __desire, __s, __f);
}

template <class _T>
bool atomic_compare_exchange_weak_explicit(atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire, memory_order __s, memory_order __f) noexcept
{
    return __object->compare_exchange_weak(*__expect, __desire, __s, __f);
}

template <class _T>
bool atomic_compare_exchange_strong(volatile atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire) noexcept
{
    return __object->compare_exchange_strong(*__expect, __desire);
}

template <class _T>
bool atomic_compare_exchange_strong(atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire) noexcept
{
    return __object->compare_exchange_strong(*__expect, __desire);
}

template <class _T>
bool atomic_compare_exchange_strong_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire, memory_order __s, memory_order __f) noexcept
{
    return __object->compare_exchange_strong(*__expect, __desire, __s, __f);
}

template <class _T>
bool atomic_compare_exchange_strong_explicit(atomic<_T>* __object, typename atomic<_T>::value_type* __expect, typename atomic<_T>::value_type __desire, memory_order __s, memory_order __f) noexcept
{
    return __object->compare_exchange_strong(*__expect, __desire, __s, __f);
}

template <class _T>
_T atomic_fetch_add(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    return __object->fetch_add(__t);
}

template <class _T>
_T atomic_fetch_add(atomic<_T>* __object, typename atomic<_T>::value __t) noexcept
{
    return __object->fetch_add(__t);
}

template <class _T>
_T atomic_fetch_add_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_add(__t, __mo);
}

template <class _T>
_T atomic_fetch_add_explicit(atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_add(__t, __mo);
}

template <class _T>
_T atomic_fetch_sub(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    return __object->fetch_sub(__t);
}

template <class _T>
_T atomic_fetch_sub(atomic<_T>* __object, typename atomic<_T>::value __t) noexcept
{
    return __object->fetch_sub(__t);
}

template <class _T>
_T atomic_fetch_sub_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_sub(__t, __mo);
}

template <class _T>
_T atomic_fetch_sub_explicit(atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_sub(__t, __mo);
}

template <class _T>
_T atomic_fetch_and(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    return __object->fetch_and(__t);
}

template <class _T>
_T atomic_fetch_and(atomic<_T>* __object, typename atomic<_T>::value __t) noexcept
{
    return __object->fetch_and(__t);
}

template <class _T>
_T atomic_fetch_and_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_and(__t, __mo);
}

template <class _T>
_T atomic_fetch_and_explicit(atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_and(__t, __mo);
}

template <class _T>
_T atomic_fetch_or(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    return __object->fetch_or(__t);
}

template <class _T>
_T atomic_fetch_or(atomic<_T>* __object, typename atomic<_T>::value __t) noexcept
{
    return __object->fetch_or(__t);
}

template <class _T>
_T atomic_fetch_or_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_or(__t, __mo);
}

template <class _T>
_T atomic_fetch_or_explicit(atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_or(__t, __mo);
}

template <class _T>
_T atomic_fetch_xor(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t) noexcept
{
    return __object->fetch_xor(__t);
}

template <class _T>
_T atomic_fetch_xor(atomic<_T>* __object, typename atomic<_T>::value __t) noexcept
{
    return __object->fetch_xor(__t);
}

template <class _T>
_T atomic_fetch_xor_explicit(volatile atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_xor(__t, __mo);
}

template <class _T>
_T atomic_fetch_xor_explicit(atomic<_T>* __object, typename atomic<_T>::value_type __t, memory_order __mo) noexcept
{
    return __object->fetch_xor(__t, __mo);
}


struct atomic_flag
{
    atomic_flag() noexcept = default;

    atomic_flag(const atomic_flag&) = delete;
    atomic_flag& operator=(const atomic_flag&) = delete;
    atomic_flag& operator=(const atomic_flag&) volatile = delete;

    bool test_and_set(memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        return __atomic_test_and_set(&_M_flag, int(__mo));
    }

    bool test_and_set(memory_order __mo = memory_order::seq_cst) noexcept
    {
        return __atomic_test_and_set(&_M_flag, int(__mo));
    }

    void clear(memory_order __mo = memory_order::seq_cst) volatile noexcept
    {
        __atomic_clear(&_M_flag, int(__mo));
    }

    void clear(memory_order __mo = memory_order::seq_cst) noexcept
    {
        __atomic_clear(&_M_flag, int(__mo));
    }

    uint8_t _M_flag;
};

#define ATOMIC_FLAG_INIT {0}

inline bool atomic_flag_test_and_set(volatile atomic_flag* __f) noexcept
{
    return __f->test_and_set();
}

inline bool atomic_flag_test_and_set(atomic_flag* __f) noexcept
{
    return __f->test_and_set();
}

inline bool atomic_flag_test_and_set_explicit(volatile atomic_flag* __f, memory_order __mo) noexcept
{
    return __f->test_and_set(__mo);
}

inline bool atomic_flag_test_and_set_explicit(atomic_flag* __f, memory_order __mo) noexcept
{
    return __f->test_and_set(__mo);
}

inline void atomic_flag_clear(volatile atomic_flag* __f) noexcept
{
    __f->clear();
}

inline void atomic_flag_clear(atomic_flag* __f) noexcept
{
    __f->clear();
}

inline void atomic_flag_clear_explicit(volatile atomic_flag* __f, memory_order __mo) noexcept
{
    __f->clear(__mo);
}

inline void atomic_flag_clear_explicit(atomic_flag* __f, memory_order __mo) noexcept
{
    __f->clear(__mo);
}


extern "C" inline void atomic_thread_fence(memory_order __mo) noexcept
{
    __atomic_thread_fence(int(__mo));
}

extern "C" inline void atomic_signal_fence(memory_order __mo) noexcept
{
    __atomic_signal_fence(int(__mo));
}


using atomic_bool       = atomic<bool>;
using atomic_char       = atomic<char>;
using atomic_schar      = atomic<signed char>;
using atomic_uchar      = atomic<unsigned char>;
using atomic_short      = atomic<short>;
using atomic_ushort     = atomic<unsigned short>;
using atomic_int        = atomic<int>;
using atomic_uint       = atomic<unsigned int>;
using atomic_long       = atomic<long>;
using atomic_ulong      = atomic<unsigned long>;
using atomic_llong      = atomic<long long>;
using atomic_ullong     = atomic<unsigned long long>;
using atomic_char8_t    = atomic<char8_t>;
using atomic_char16_t   = atomic<char16_t>;
using atomic_char32_t   = atomic<char32_t>;
using atomic_wchar_t    = atomic<wchar_t>;

using atomic_int8_t     = atomic<int8_t>;
using atomic_uint8_t    = atomic<uint8_t>;
using atomic_int16_t    = atomic<int16_t>;
using atomic_uint16_t   = atomic<uint16_t>;
using atomic_int32_t    = atomic<int32_t>;
using atomic_uint32_t   = atomic<uint32_t>;
using atomic_int64_t    = atomic<int64_t>;
using atomic_uint64_t   = atomic<uint64_t>;

using atomic_int_least8_t   = atomic<int_least8_t>;
using atomic_uint_least8_t  = atomic<uint_least8_t>;
using atomic_int_least16_t  = atomic<int_least16_t>;
using atomic_uint_least16_t = atomic<uint_least16_t>;
using atomic_int_least32_t  = atomic<int_least32_t>;
using atomic_uint_least32_t = atomic<uint_least32_t>;
using atomic_int_least64_t  = atomic<int_least64_t>;
using atomic_uint_least64_t = atomic<uint_least64_t>;

using atomic_int_fast8_t    = atomic<int_fast8_t>;
using atomic_uint_fast8_t   = atomic<uint_fast8_t>;
using atomic_int_fast16_t   = atomic<int_fast16_t>;
using atomic_uint_fast16_t  = atomic<uint_fast16_t>;
using atomic_int_fast32_t   = atomic<int_fast32_t>;
using atomic_uint_fast32_t  = atomic<uint_fast32_t>;
using atomic_int_fast64_t   = atomic<int_fast64_t>;
using atomic_uint_fast64_t  = atomic<uint_fast64_t>;

using atomic_intptr_t   = atomic<intptr_t>;
using atomic_uintptr_t  = atomic<uintptr_t>;
using atomic_size_t     = atomic<size_t>;
using atomic_ptrdiff_t  = atomic<ptrdiff_t>;
using atomic_intmax_t   = atomic<intmax_t>;
using atomic_uintmax_t  = atomic<uintmax_t>;


} // namespace __XVI_STD_ATOMIC_NS


#endif /* ifndef __SYSTEM_CXX_ATOMIC_ATOMIC_H */
