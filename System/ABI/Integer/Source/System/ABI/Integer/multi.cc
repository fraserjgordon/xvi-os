#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__int128_t __multi3(__int128_t a, __int128_t b)
{
    return System::ABI::Integer::mul(__uint128_t(a), __uint128_t(b));
}
#endif
