#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __divsi3(__int32_t n, __int32_t d)
{
    return System::ABI::Integer::idiv(n, d).q;
}
