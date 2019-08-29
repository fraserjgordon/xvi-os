#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __muldi3(__int64_t a, __int64_t b)
{
    return System::ABI::Integer::mul(__uint64_t(a), __uint64_t(b));
}
