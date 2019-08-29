#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __ucmpdi2(__uint64_t a, __uint64_t b)
{
    // Needs to return {0,1,2} instead of {-1,0,1}.
    return System::ABI::Integer::compare(a, b) + 1;
}
