#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__uint32_t __umodsi3(__uint32_t n, __uint32_t d)
{
    return System::ABI::Integer::udiv(n, d).r;
}
