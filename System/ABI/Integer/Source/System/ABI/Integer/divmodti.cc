#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__int128_t __divmodti4(__int128_t n, __int128_t d, __int128_t* r)
{
    auto result = System::ABI::Integer::idiv(n, d);
    *r = result.r;
    return result.q;
}
#endif
