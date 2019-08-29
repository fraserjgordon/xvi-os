#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __ashldi3(__int64_t i, __int32_t count)
{
    return System::ABI::Integer::lshift(__uint64_t(i), __uint32_t(count));
}
