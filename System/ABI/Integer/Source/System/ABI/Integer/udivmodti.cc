#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__uint128_t __udivmodti4(__uint128_t n, __uint128_t d, __uint128_t* r)
{
    auto result = System::ABI::Integer::udiv(n, d);
    *r = result.r;
    return result.q;
}
#endif
