#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__uint32_t __udivsi3(__uint32_t n, __uint32_t d)
{
    return System::ABI::Integer::udiv(n, d).q;
}
