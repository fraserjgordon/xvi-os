#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__uint32_t __udivmodsi4(__uint32_t n, __uint32_t d, __uint32_t* r)
{
    auto result = System::ABI::Integer::udiv(n, d);
    *r = result.r;
    return result.q;
}
