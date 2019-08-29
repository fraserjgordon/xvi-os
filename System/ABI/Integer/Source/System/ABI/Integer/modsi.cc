#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __modsi3(__int32_t n, __int32_t d)
{
    return System::ABI::Integer::idiv(n, d).r;
}
