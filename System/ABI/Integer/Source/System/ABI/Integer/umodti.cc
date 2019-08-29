#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__uint128_t __umodti3(__uint128_t n, __uint128_t d)
{
    return System::ABI::Integer::udiv(n, d).r;
}
#endif
