#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __mulsi3(__int32_t a, __int32_t b)
{
    return static_cast<__int32_t>(System::ABI::Integer::mul(__uint32_t(a), __uint32_t(b)));
}
