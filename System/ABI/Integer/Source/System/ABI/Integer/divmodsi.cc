#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __divmodsi4(__int32_t n, __int32_t d, __int32_t* r)
{
    auto result = System::ABI::Integer::idiv(n, d);
    *r = result.r;
    return result.q;
}
