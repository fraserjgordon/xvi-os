#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__uint64_t __udivmoddi4(__uint64_t n, __uint64_t d, __uint64_t* r)
{
    auto result = System::ABI::Integer::udiv(n, d);
    *r = result.r;
    return result.q;
}
