#pragma once
#ifndef __SYSTEM_ABI_INTEGER_BITOPSIMPL_H
#define __SYSTEM_ABI_INTEGER_BITOPSIMPL_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/Integer/BitOps.hh>
#include <System/ABI/Integer/Arch/BitOps.hh>
#include <System/ABI/Integer/Types.hh>


namespace System::ABI::Integer
{


// Forward declarations.
static inline std::int32_t clz(std::uint32_t);
static inline std::int32_t clz(std::uint64_t);
static inline std::int32_t ctz(std::uint32_t);
static inline std::int32_t ctz(std::uint64_t);
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static inline std::int32_t clz(__uint128_t);
static inline std::int32_t ctz(__uint128_t);
#endif

template <class T>
constexpr int clz_emul(T t)
{
    int count = 0;
    int Tbits = sizeof(T) * 8;

    // Ensure we returned a defined result for zero.
    if (t == 0)
        return Tbits;

    for (int shift = Tbits / 2; shift > 0; shift /= 2)
    {
        if (t < (T(1) << shift))
            count += shift;
        else
            t >>= shift;
    }

    return count;
}

template <class T>
constexpr int clz_split_emul(T t)
{
    constexpr int Tbits = sizeof(T) * 8;
    constexpr int HalfTbits = Tbits / 2;
    
    // Count the leading zeroes in the top half and then the bottom half if the top is zero.
    split_int_t<T> ts = t;
    return (ts.split.high == 0) ? clz(ts.split.low) + HalfTbits : clz(ts.split.high);
}

template <class T>
constexpr int ctz_emul(T t)
{
    int count = 0;
    int Tbits = sizeof(T) * 8;

    // Ensure we return a defined result for zero.
    if (t == 0)
        return Tbits;

    for (int shift = Tbits / 2; shift > 0; shift /= 2)
    {
        auto mask = (T(1) << shift) - 1;
        if ((t & mask) == 0)
        {
            count += shift;
            t >>= shift;
        }
    }

    return count;
}

template <class T>
constexpr int ctz_split_emul(T t)
{
    constexpr int Tbits = sizeof(T) * 8;
    constexpr int HalfTbits = Tbits / 2;
    
    // Count the trailing zeroes in the bottom half and then the top half if the bottom is zero.
    split_int_t<T> ts = t;
    return (ts.split.low == 0) ? ctz(ts.split.high) + HalfTbits : ctz(ts.split.low);
}

template <class T>
constexpr int ffs_emul(T t)
{
    // Returns zero if the argument is zero, else returns the index of the least-significant 1 bit.
    if (t == 0)
        return 0;

    return ctz(t) + 1;
}

template <class T>
constexpr int parity_emul(T t)
{
    // Fold the argument until only one bit remains.
    int Tbits = sizeof(T) * 8;
    for (int shift = Tbits / 2; shift > 0; shift /= 2)
    {
        auto mask = (T(1) << shift) - 1;
        t = (t & mask) ^ (t >> shift);
    }
    return t;
}


// Constants for population count calculation.
static constexpr std::uint64_t PopcountM1  = 0x5555'5555'5555'5555;
static constexpr std::uint64_t PopcountM2  = 0x3333'3333'3333'3333;
static constexpr std::uint64_t PopcountM4  = 0x0f0f'0f0f'0f0f'0f0f;
static constexpr std::uint64_t PopcountH01 = 0x0101'0101'0101'0101;

template <class T>
constexpr int popcount_nomul_emul(T t)
{
    // Uses fewer arithmetic instructions than a naive implementation but without using multiplication.
    static_assert(sizeof(t) <= sizeof(std::uint64_t));
    t -= (t >> 1) & T(PopcountM1);
    t = (t & T(PopcountM2)) + ((t >> 2) & T(PopcountM2));
    t = (t + (t >> 4)) & T(PopcountM4);
    t += t >> 8;
    t += t >> 16;
    if /*constexpr*/ (sizeof(t) > sizeof(std::uint32_t))
        t += t >> 32;
    return t & 0x7f;
}

template <class T>
constexpr int popcount_mul_emul(T t)
{
    // Uses fewer arithmetic instructions but assumes multiplication is fast.
    static_assert(sizeof(t) <= sizeof(std::uint64_t));
    t -= (t >> 1) & T(PopcountM1);
    t = (t & T(PopcountM2)) + ((t >> 2) & T(PopcountM2));
    t = (t + (t >> 4)) & T(PopcountM4);
    return (t * T(PopcountH01)) >> ((8 * sizeof(T)) - 8);
}


static inline std::int32_t clz(std::uint32_t u)
{
#if defined(__INTEGER_CLZ32_INTRINSIC)
    return __INTEGER_CLZ32_INTRINSIC(u);
#else
    return clz_emul(u);
#endif
}

static inline std::int32_t clz(std::uint64_t u)
{
#if defined(__INTEGER_CLZ64_INTRINSIC)
    return __INTEGER_CLZ64_INTRINSIC(u);
#elif defined(__INTEGER_CLZ32_INTRINSIC)
    return clz_split_emul(u);
#else
    return clz_emul(u);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static inline std::int32_t clz(__uint128_t u)
{
#if defined(__INTEGER_CLZ128_INTRINSIC)
    return __INTEGER_CLZ128_INTRINSIC(u);
#elif defined(__INTEGER_CLZ32_INTRINSIC) || defined(__INTEGER_CLZ64_INTRINSIC)
    return clz_split_emul(u);
#else
    return clz_emul(u);
#endif
}
#endif


static inline std::int32_t ctz(std::uint32_t u)
{
#if defined(__INTEGER_CTZ32_INTRINSIC)
    return __INTEGER_CTZ32_INTRINSIC(u);
#else
    return ctz_emul(u);
#endif
}

static inline std::int32_t ctz(std::uint64_t u)
{
#if defined(__INTEGER_CTZ64_INTRINSIC)
    return __INTEGER_CTZ64_INTRINSIC(u);
#elif defined(__INTEGER_CTZ32_INTRINSIC)
    return ctz_split_emul(u);
#else
    return ctz_emul(u);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static inline std::int32_t ctz(__uint128_t u)
{
#if defined(__INTEGER_CTZ128_INTRINSIC)
    return __INTEGER_CTZ128_INTRINSIC(u);
#elif defined(__INTEGER_CTZ32_INTRINSIC) || defined(__INTEGER_CTZ64_INTRINSIC)
    return ctz_split_emul(u);
#else
    return ctz_emul(u);
#endif
}
#endif


static inline std::int32_t ffs(std::uint32_t u)
{
#if defined(__INTEGER_FFS32_INTRINSIC)
    return __INTEGER_FFS32_INTRINSIC(u);
#else
    return ffs_emul(u);
#endif
}

static inline std::int32_t ffs(std::uint64_t u)
{
#if defined(__INTEGER_FFS64_INTRINSIC)
    return __INTEGER_FFS64_INTRINSIC(u);
#else
    return ffs_emul(u);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static inline std::int32_t ffs(__uint128_t u)
{
#if defined(__INTEGER_FFS128_INTRINSIC)
    return __INTEGER_FFS128_INTRINSIC(u);
#else
    return ffs_emul(u);
#endif
}
#endif


static inline std::int32_t parity(std::uint32_t u)
{
#if defined(__INTEGER_PARITY32_INTRINSIC)
    return __INTEGER_PARITY32_INTRINSIC(u);
#else
    return parity_emul(u);
#endif
}

static inline std::int32_t parity(std::uint64_t u)
{
#if defined(__INTEGER_PARITY64_INTRINSIC)
    return __INTEGER_PARITY64_INTRINSIC(u);
#else
    return parity_emul(u);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static inline std::int32_t parity(__uint128_t u)
{
#if defined(__INTEGER_PARITY128_INTRINSIC)
    return __INTEGER_PARITY128_INTRINSIC(u);
#else
    return parity_emul(u);
#endif
}
#endif


static inline std::int32_t popcount(std::uint32_t u)
{
#if defined(__INTEGER_POPCOUNT32_INTRINSIC)
    return __INTEGER_POPCOUNT32_INTRINSIC(u);
#else
    //! @TODO: identify cases where the no-mul implementation is faster.
    return popcount_mul_emul(u);
#endif
}

static inline std::int32_t popcount(std::uint64_t u)
{
#if defined(__INTEGER_POPCOUNT64_INTRINSIC)
    return __INTEGER_POPCOUNT64_INTRINSIC(u);
#else
    //! @TODO: identify cases where the no-mul implementation is faster.
    return popcount_mul_emul(u);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static inline std::int32_t popcount(__uint128_t u)
{
#if defined(__INTEGER_POPCOUNT128_INTRINSIC)
    return __INTEGER_POPCOUNT128_INTRINSIC(u);
#else
    // No native 128-bit machines so use two 64-bit population counts.
    auto low = std::uint64_t(u);
    auto high = std::uint64_t(u >> 64);
    return popcount(low) + popcount(high);
#endif
}
#endif


static inline std::int32_t bswap(std::int32_t i)
{
#if defined(__INTEGER_BSWAP32_INTRINSIC)
    return __INTEGER_BSWAP32_INTRINSIC(i);
#else
    return bswap_emul(i);
#endif
}

static inline std::int64_t bswap(std::int64_t i)
{
#if defined (__INTEGER_BSWAP64_INTRINSIC)
    return __INTEGER_BSWAP64_INTRINSIC(i);
#else
    return bswap_emul(i);
#endif
}


} // namespace System::ABI::Integer


#endif /* ifndef __SYSTEM_ABI_INTEGER_BITOPSIMPL_H */
