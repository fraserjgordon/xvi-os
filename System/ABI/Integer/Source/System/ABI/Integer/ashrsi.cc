#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __ashrsi3(__int32_t i, __int32_t count)
{
    return System::ABI::Integer::rshifta(i, __uint32_t(count));
}
