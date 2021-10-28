#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __lshrsi3(__int32_t i, __int32_t count)
{
    return static_cast<__int32_t>(System::ABI::Integer::rshiftl(__uint32_t(i), __uint32_t(count)));
}
