#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __lshrdi3(__int64_t i, __int32_t count)
{
    return System::ABI::Integer::rshiftl(__uint64_t(i), __uint32_t(count));
}
