#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __cmpdi2(__int64_t a, __int64_t b)
{
    // Needs to return {0,1,2} instead of {-1,0,1}.
    return System::ABI::Integer::compare(a, b) + 1;
}

#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
extern "C" __SYSTEM_ABI_INTEGER_EXPORT long long __aeabi_lcmp(long long a, long long b) noexcept;

long long __aeabi_lcmp(long long a, long long b) noexcept
{
    // Returns {-1,0,1}.
    return System::ABI::Integer::compare(a, b);
}
#endif
