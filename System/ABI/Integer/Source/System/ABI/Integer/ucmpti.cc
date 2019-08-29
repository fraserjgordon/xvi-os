#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__int32_t __ucmpti2(__uint128_t a, __uint128_t b)
{
    // Needs to return {0,1,2} instead of {-1,0,1}.
    return System::ABI::Integer::compare(a, b) + 1;
}
#endif
