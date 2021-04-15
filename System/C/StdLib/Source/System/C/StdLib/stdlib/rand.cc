#include <System/C/StdLib/stdlib.h>

#include <System/C/StdLib/stdint.h>


// Seed for the random number generator.
//
// The C standard requires that the initial seed value is 1.
//
// Using the random number generator from multiple threads causes a data race as there is no protection of this value.
static uint32_t s_seed = 1;


// Simple non-secure pseudorandom generator using the "minstd" algorithm.
int rand()
{
    constexpr uint64_t A = 48271;

    // 32x32 -> 64 bit multiplication of the seed.
    auto product = s_seed * A;

    // Reduce the result modulo 2^31-1 (0x7fffffff). To avoid a division, take advantage of the following identity:
    //  2^31 mod (2^31 - 1) == 1
    //  hence (x * 2^31 + y) mod (2^31 - 1) == x + y  [where y < 2^31 - 1]
    //
    // The maximum product of the seed and A (== 48271 * UINT32_MAX) is <<2^62 so we can discard the upper two bits as
    // they're always zero; i.e it is safe to treat the result as a pair of 31-bit quantities.
    auto upper = static_cast<uint32_t>(product >> 31);
    auto lower = static_cast<uint32_t>(product & 0x7fffffff) + upper;
    auto result = upper + lower;

    // It is possible that the sum in the reduction step above caused us to exceed 2^31-1 so perform the step one final
    // time. The arithmetic is idempotent so we can do this unconditionally.
    //
    // The only difference compared to the first reduction is that we can only overflow by a single bit (not that this
    // changes the arithmetic).
    upper = result >> 31;
    lower = result & 0x7fffffff;
    result = upper + lower;

    // Update the seed to the newly-calculated value.
    s_seed = result;

    // This function is expected to return a signed number between zero and RAND_MAX. As this library defines RAND_MAX
    // to be the same as INT_MAX, which happens to equal 2^31-1, we can return the result unchanged.
    return static_cast<int>(result);
}

void srand(unsigned int seed)
{
    s_seed = seed;
}
