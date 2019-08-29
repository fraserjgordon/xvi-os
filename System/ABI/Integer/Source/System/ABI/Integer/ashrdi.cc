#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __ashrdi3(__int64_t i, __int32_t count)
{
    return System::ABI::Integer::rshifta(i, __uint32_t(count));
}
