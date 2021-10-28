#pragma once
#ifndef __SYSTEM_ABI_INTEGER_ARITHMETICOPSIMPL_H
#define __SYSTEM_ABI_INTEGER_ARITHMETICOPSIMPL_H


#include <System/ABI/Integer/Arch/Capabilities.hh>
#include <System/ABI/Integer/Arithmetic.hh>
#include <System/ABI/Integer/Types.hh>


namespace System::ABI::Integer
{


// Forward declarations.
static constexpr uint32_t  add(uint32_t, uint32_t);
static constexpr uint64_t  add(uint64_t, uint64_t);
static constexpr uint128_t add(uint128_t, uint128_t);
static constexpr uint32_t  sub(uint32_t, uint32_t);
static constexpr uint64_t  sub(uint64_t, uint64_t);
static constexpr uint128_t sub(uint128_t, uint128_t);
static constexpr uint32_t  lshift(uint32_t, uint32_t);
static constexpr uint64_t  lshift(uint64_t, uint32_t);
static constexpr uint128_t lshift(uint128_t, uint32_t);
static constexpr uint32_t  rshiftl(uint32_t, uint32_t);
static constexpr uint64_t  rshiftl(uint64_t, uint32_t);
static constexpr int32_t   rshifta(int32_t, uint32_t);
static constexpr int64_t   rshifta(int64_t, uint32_t);
static constexpr uint32_t  mul(uint32_t, uint32_t);
static constexpr uint64_t  mul(uint64_t, uint64_t);
static constexpr uint64_t  mulx(uint32_t, uint32_t);
static constexpr int64_t   mulx(int32_t, int32_t);
static constexpr int32_t   negate(int32_t);
static constexpr int64_t   negate(int64_t);
static constexpr int128_t  negate(int128_t);
static constexpr int       compare(int32_t, int32_t);
static constexpr int       compare(int64_t, int64_t);
static constexpr int       compare(int128_t, int128_t);
static constexpr int       compare(uint32_t, uint32_t);
static constexpr int       compare(uint64_t, uint64_t);
static constexpr int       compare(uint128_t, uint128_t);
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr uint256_t add(uint256_t, uint256_t);
static constexpr uint256_t sub(uint256_t, uint256_t);
static constexpr uint256_t lshift(uint256_t, uint32_t);
static constexpr uint128_t rshiftl(uint128_t, uint32_t);
static constexpr int128_t  rshifta(int128_t, uint32_t);
static constexpr uint128_t mul(uint128_t, uint128_t);
static constexpr uint128_t mulx(uint64_t, uint64_t);
static constexpr int128_t  mulx(int64_t, int64_t);
static constexpr int256_t negate(int256_t);
static constexpr int       compare(int256_t, int256_t);
static constexpr int       compare(uint256_t, uint256_t);
#endif


template <class T>
static constexpr T add_split_emul(T a, T b)
{
    // Do a piecewise addition with carry.
    split_int_t<T> as = a;
    split_int_t<T> bs = b;
    split_int_t<T> result {0, 0};
    result.split.low = add(as.split.low, bs.split.low);
    result.split.high = add(as.split.high, bs.split.high);
    if (as.split.low > result.split.low)
        result.split.high = add(result.split.high, 1);
    return result;
}


template <class T>
static constexpr T sub_neg_emul(T a, T b)
{
    // Subtraction == addition with second argument negated.
    return add(a, negate(b));
}


template <class T>
static constexpr T lshift_add_emul(T t, uint32_t count)
{
    // Avoid pathological O(count) behaviour for excessive count values.
    uint32_t Tbits = sizeof(T) * 8;
    if (count >= Tbits)
        return 0;

    // For each bit to be shifted left, double the value (without using multiplies).
    for (uint32_t i = 0; i < count; ++i)
        t += t;

    return t;
}

template <class T>
static constexpr T lshift_split_emul(T t, uint32_t count)
{
    // Use two half-size left-shifts to emulate the full-size shift.
    constexpr int Tbits = sizeof(T) * 8;
    constexpr int HalfTbits = Tbits / 2;
    split_int_t<T> ts(t);
    if (count < HalfTbits)
    {
        ts.split.high = lshift(ts.split.uhigh, count) | rshiftl(ts.split.low, HalfTbits - count);
        ts.split.low = lshift(ts.split.low, count);
    }
    else
    {
        ts.split.high = lshift(ts.split.low, count - HalfTbits);
        ts.split.low = 0;
    }

    return ts;
}


template <class T>
static constexpr T rshiftl_split_emul(T t, uint32_t count)
{
    // Use two half-size right-shifts to emulate the full-size shift.
    constexpr int Tbits = sizeof(T) * 8;
    constexpr int HalfTbits = Tbits / 2;
    split_int_t<T> ts(t);
    if (count < HalfTbits)
    {
        ts.split.low = rshiftl(ts.split.low, count) | lshift(ts.split.high, HalfTbits - count);
        ts.split.high = rshiftl(ts.split.high, count);
    }
    else
    {
        ts.split.low = rshiftl(ts.split.high, count - HalfTbits);
        ts.split.high = 0;
    }

    return ts;
}


template <class T>
static constexpr T mul_add_emul(T a, T b)
{
    // Use a repeated shift-and-add sequence to emulate the multiplication.
    constexpr int Tbits = sizeof(T) * 8;
    T result = 0;
    for (int i = 0; i < Tbits; ++i)
    {
        if (a & 1)
            result += b;
        a = rshiftl(a, 1);
        b = lshift(b, 1);
    }

    return result;
}

template <class T>
static constexpr double_bits_t<T> mulx_add_emul(T a, T b)
{
    // Use a repeated shift-and-add sequence to emulate the multiplication.
    constexpr int Tbits = sizeof(T) * 8;
    double_bits_t<T> result = 0;
    double_bits_t<T> db = b;
    for (int i = 0; i < Tbits; ++i)
    {
        if (a & 1)
            result += db;
        a = rshiftl(a, 1);
        db = lshift(db, 1);
    }

    return result;
}

template <class T>
static constexpr T mul_split_emul(T a, T b)
{
    // Perform a piecewise multiplication of the operands. Because this is a non-expanding multiplication, the product
    // of the two high halfs does not contribute to the result (it will always overflow if non-zero).
    constexpr int Tbits = sizeof(T) * 8;
    constexpr int HalfTbits = Tbits / 2;
    split_int_t<T> as = a;
    split_int_t<T> bs = b;
    T lls = mulx(as.split.low, bs.split.low);
    T lhs = mul(as.split.low, bs.split.uhigh);
    T hls = mul(as.split.uhigh, bs.split.low);
    
    // Combine the piecewise results.
    T result = lls + lshift(lhs, HalfTbits) + lshift(hls, HalfTbits);
    return result;
}

template <class T>
static constexpr double_bits_t<T> mulx_split_emul(T a, T b)
{
    // Perform a piecewise multiplication of the operands.
    constexpr int Tbits = sizeof(T) * 8;
    constexpr int HalfTbits = Tbits / 2;
    split_int_t<T> as = a;
    split_int_t<T> bs = b;
    T lls = mulx(as.split.low, bs.split.low);
    T lhs = mulx(as.split.low, bs.split.uhigh);
    T hls = mulx(as.split.uhigh, bs.split.low);
    T hhs = mulx(as.split.high, bs.split.high);

    // Combine the piecewise results.
    using result_t = double_bits_t<T>;
    using uresult_t = make_unsigned_t<result_t>;
    result_t result = lls
                      + lshift(uresult_t(lhs), HalfTbits)
                      + lshift(uresult_t(hls), HalfTbits)
                      + lshift(uresult_t(hhs), Tbits);
    return result;
}


template <class T>
static constexpr T rshifta_split_emul(T t, uint32_t count)
{
    // Use two half-size right-shifts to emulate the full-size shift.
    constexpr int Tbits = sizeof(T) * 8;
    constexpr int HalfTbits = Tbits / 2;
    split_int_t<T> ts(t);
    if (count < HalfTbits)
    {
        ts.split.low = rshiftl(ts.split.low, count) | lshift(ts.split.uhigh, HalfTbits - count);
        ts.split.high = rshifta(ts.split.high, count);
    }
    else
    {
        ts.split.low = rshifta(ts.split.high, count - HalfTbits);
        ts.split.high = rshifta(ts.split.high, HalfTbits - 1);
    }

    return ts;
}


template <class T>
static constexpr T negate_emul(T t)
{
    // Two's-complement negation is the same as bitwise-not then incrementing.
    split_int_t<T> ts = t;
    ts.split.low = ~ts.split.low;
    ts.split.high = ~ts.split.high;
    return add(T(ts), T(split_int_t<T>{0, 1}));
}


template <class T>
static constexpr int compare_split_emul(T a, T b)
{
    // Perform a piecewise compare of the two values.
    split_int_t<T> as = a;
    split_int_t<T> bs = b;
    int high = compare(as.split.high, bs.split.high);
    if (high != 0)
        return high;
    else 
        return compare(as.split.low, bs.split.low);
}


template <class T>
struct divide_result { T q, r; };

template <class T>
static constexpr divide_result<T> udiv_nonrestoring_emul(T num, T denom)
{
    // Temporary partial remainder (r) and denominator (d) need to be twice the input size.
    constexpr int Tbits = sizeof(T) * 8;
    using DT = double_bits_t<T>;
    using SDT = make_signed_t<DT>;
    
    // Some behaviour needs to change if emulating the double-wide type.
    constexpr bool UsingEmulatedInt = is_same<DT, emulated_integer<T>>;

    SDT r = {};
    DT d = {};
    if constexpr (UsingEmulatedInt)
    {
        // Initialise in an emulated_integer-compatible fashion.
        r = split_int_t<SDT>{0, num};
        d = split_int_t<DT>{denom, 0};
    }
    else
    {
        r = num;
        d = lshift(DT(denom), Tbits);
    }
    T q = 0;

    // Perform the divide from the high to low bits.
    constexpr SDT zero = zero_value<SDT>;
    for (int i = Tbits - 1; i >= 0; --i)
    {
        if (compare(r, zero) >= 0)
        {
            q |= T(1) << i;
            r = static_cast<SDT>(sub(lshift(DT(r), 1), d));
        }
        else
        {
            q |= T(0) << i;
            r = static_cast<SDT>(add(lshift(DT(r), 1), d));
        }
    }

    // Convert q to {0,1} instead of {-1,1}.
    q -= ~q;

    // Correct the result if the remainder is negative.
    if (compare(r, zero) < 0)
    {
        q -= 1;
        r = static_cast<SDT>(add(DT(r), DT(d)));
    }

    if constexpr (UsingEmulatedInt)
        return {q, T(split_int_t<SDT>(r).split.high)};
    else
        return {q, T(rshiftl(DT(r), Tbits))};
}

template <class T>
static constexpr divide_result<T> idiv_nonrestoring_emul(T num, T denom)
{
    // Capture the signs of both members and make them positive and unsigned.
    using UT = make_unsigned_t<T>;
    bool nnum = (num < 0);
    bool ndenom = (denom < 0);
    UT unum = nnum ? UT(negate(num)) : UT(num);
    UT udenom = ndenom ? UT(negate(denom)) : UT(denom);

    // Perform the division as if the numbers were unsigned.
    auto [q, r] = udiv_nonrestoring_emul(unum, udenom);

    // Signed division: q is negative if exactly one of the numerator or denomenator is negative. The sign of r matches
    // the sign of the numerator.
    T sq = (nnum != ndenom) ? -T(q) : T(q);
    T sr = (nnum) ? -T(r) : T(r);

    return {sq, sr};
}


static constexpr uint32_t add(uint32_t a, uint32_t b)
{
    return a + b;
}

static constexpr uint64_t add(uint64_t a, uint64_t b)
{
    return a + b;
}

static constexpr uint128_t add(uint128_t a, uint128_t b)
{
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
    return a + b;
#else
    return add_split_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr uint256_t add(uint256_t a, uint256_t b)
{
    return add_split_emul(a, b);
}
#endif


static constexpr uint32_t sub(uint32_t a, uint32_t b)
{
    return a - b;
}

static constexpr uint64_t sub(uint64_t a, uint64_t b)
{
    return a - b;
}

static constexpr uint128_t sub(uint128_t a, uint128_t b)
{
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
    return a - b;
#else
    return sub_neg_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr uint256_t sub(uint256_t a, uint256_t b)
{
    return sub_neg_emul(a, b);
}
#endif


static constexpr uint32_t lshift(uint32_t u, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_32
    return (u << count);
#else
    return lshift_add_emul(u, count);
#endif
}

static constexpr uint64_t lshift(uint64_t u, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_64
    return (u << count);
#elif __INTEGER_ARCH_SHIFT_CAP > __INTEGER_SHIFT_NONE
    return lshift_split_emul(u, count);
#else
    return lshift_add_emul(u, count);
#endif
}

static constexpr uint128_t lshift(uint128_t u, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_128
    return (u << count);
#elif __INTEGER_ARCH_SHIFT_CAP > __INTEGER_SHIFT_NONE
    return lshift_split_emul(u, count);
#else
    return lshift_add_emul(u, count);
#endif
}

#ifdef __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr uint256_t lshift(uint256_t u, uint32_t count)
{
    // Always uses the split emulation as native addition is not available.
    return lshift_split_emul(u, count);
}
#endif


static constexpr uint32_t rshiftl(uint32_t u, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_32
    return (u >> count);
#else
#  error No emulation of right shift available.
#endif
}

static constexpr uint64_t rshiftl(uint64_t u, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_64
    return (u >> count);
#else
    return rshiftl_split_emul(u, count);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr uint128_t rshiftl(uint128_t u, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_128
    return (u >> count);
#else
    return rshiftl_split_emul(u, count);
#endif
}
#endif


static constexpr int32_t rshifta(int32_t i, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_32
    return (i >> count);
#else
#  error No emulation of right shift available.
#endif
}

static constexpr int64_t rshifta(int64_t i, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_64
    return (i >> count);
#else
    return rshifta_split_emul(i, count);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr int128_t rshifta(int128_t i, uint32_t count)
{
#if __INTEGER_ARCH_SHIFT_CAP >= __INTEGER_SHIFT_128
    return (i >> count);
#else
    return rshifta_split_emul(i, count);
#endif
}
#endif


static constexpr uint32_t mul(uint32_t a, uint32_t b)
{
#if __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_32
    return (a * b);
#else
    return mul_add_emul(a, b);
#endif
}

static constexpr uint64_t mul(uint64_t a, uint64_t b)
{
#if __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_64
    return (a * b);
#elif __INTEGER_ARCH_MUL_CAP > __INTEGER_MUL_NONE
    return mul_split_emul(a, b);
#else
    return mul_add_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr uint128_t mul(uint128_t a, uint128_t b)
{
#if __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_128
    return (a * b);
#elif __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_NONE
    return mul_split_emul(a, b);
#else
    return mul_add_emul(a, b);
#endif
}
#endif

static constexpr uint64_t mulx(uint32_t a, uint32_t b)
{
#if __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_32_64
    return (uint64_t(a) * b);
#elif __INTEGER_ARCH_MUL_CAP > __INTEGER_MUL_NONE
    return mulx_split_emul(a, b);
#else
    return mulx_add_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr uint128_t mulx(uint64_t a, uint64_t b)
{
#if __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_64_128
    return (uint128_t(a) * b);
#elif __INTEGER_ARCH_MUL_CAP > __INTEGER_MUL_NONE
    return mulx_split_emul(a, b);
#else
    return mulx_add_emul(a, b);
#endif
}
#endif

static constexpr int64_t mulx(int32_t a, int32_t b)
{
#if __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_32_64
    return (int64_t(a) * b);
#elif __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_NONE
    return mulx_split_emul(a, b);
#else
    return mulx_add_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr int128_t mulx(int64_t a, int64_t b)
{
#if __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_64_128
    return (int128_t(a) * b);
#elif __INTEGER_ARCH_MUL_CAP >= __INTEGER_MUL_NONE
    return mulx_split_emul(a, b);
#else
    return mulx_add_emul(a, b);
#endif
}
#endif


static constexpr int32_t negate(int32_t u)
{
    return -u;
}

static constexpr int64_t negate(int64_t u)
{
    return -u;
}

static constexpr int128_t negate(int128_t u)
{
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
    return -u;
#else
    return negate_emul(u);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr int256_t negate(int256_t u)
{
    return negate_emul(u);
}
#endif


static constexpr int compare(int32_t a, int32_t b)
{
    return (a == b) ? 0 : (a < b) ? -1 : +1;
}

static constexpr int compare(int64_t a, int64_t b)
{
    return (a == b) ? 0 : (a < b) ? -1 : +1;
}

static constexpr int compare(int128_t a, int128_t b)
{
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
    return (a == b) ? 0 : (a < b) ? -1 : +1;
#else
    return compare_split_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr int compare(int256_t a, int256_t b)
{
    return compare_split_emul(a, b);
}
#endif

static constexpr int compare(uint32_t a, uint32_t b)
{
    return (a == b) ? 0 : (a < b) ? -1 : +1;
}

static constexpr int compare(uint64_t a, uint64_t b)
{
    return (a == b) ? 0 : (a < b) ? -1 : +1;
}

static constexpr int compare(uint128_t a, uint128_t b)
{
#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
    return (a == b) ? 0 : (a < b) ? -1 : +1;
#else
    return compare_split_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr int compare(uint256_t a, uint256_t b)
{
    return compare_split_emul(a, b);
}
#endif


static constexpr divide_result<uint32_t> udiv(uint32_t a, uint32_t b)
{
#if __INTEGER_ARCH_DIV_CAP >= __INTEGER_DIV_32
    return { a / b, a % b };
#else
    return udiv_nonrestoring_emul(a, b);
#endif
}

static constexpr divide_result<uint64_t> udiv(uint64_t a, uint64_t b)
{
#if __INTEGER_ARCH_DIV_CAP >= __INTEGER_DIV_64
    return { a / b, a % b };
#else
    return udiv_nonrestoring_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr divide_result<uint128_t> udiv(uint128_t a, uint128_t b)
{
    return udiv_nonrestoring_emul(a, b);
}
#endif

static constexpr divide_result<int32_t> idiv(int32_t a, int32_t b)
{
#if __INTEGER_ARCH_DIV_CAP >= __INTEGER_DIV_32
    return { a / b, a % b };
#else
    return idiv_nonrestoring_emul(a, b);
#endif
}

static constexpr divide_result<int64_t> idiv(int64_t a, int64_t b)
{
#if __INTEGER_ARCH_DIV_CAP >= __INTEGER_DIV_64
    return { a / b, a % b };
#else
    return idiv_nonrestoring_emul(a, b);
#endif
}

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
static constexpr divide_result<int128_t> idiv(int128_t a, int128_t b)
{
    return idiv_nonrestoring_emul(a, b);
}
#endif


//static_assert(udiv_nonrestoring_emul<uint32_t>(0xFFFFFFFF, 2).q == 0x7FFFFFFF);


} // namespace System::ABI::Integer


#endif /* __SYSTEM_ABI_INTEGER_ARITHMETICOPSIMPL_H */
