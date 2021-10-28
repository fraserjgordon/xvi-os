#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __ashrdi3(__int64_t i, __int32_t count)
{
    return System::ABI::Integer::rshifta(i, __uint32_t(count));
}

#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
ARM_ABI_ALIAS_FOR(__ashrdi3) long long __aeabi_lasr(long long, int) noexcept;
#endif
