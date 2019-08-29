#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__uint64_t __umoddi3(__uint64_t n, __uint64_t d)
{
    return System::ABI::Integer::udiv(n, d).r;
}
