#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __moddi3(__int64_t n, __int64_t d)
{
    return System::ABI::Integer::idiv(n, d).r;
}
