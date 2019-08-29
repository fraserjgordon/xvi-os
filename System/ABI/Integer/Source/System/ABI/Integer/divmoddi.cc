#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __divmoddi4(__int64_t n, __int64_t d, __int64_t* r)
{
    auto result = System::ABI::Integer::idiv(n, d);
    *r = result.r;
    return result.q;
}
