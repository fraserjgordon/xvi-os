#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __lshrdi3(__int64_t i, __int32_t count)
{
    return static_cast<__int64_t>(System::ABI::Integer::rshiftl(__uint64_t(i), __uint32_t(count)));
}

#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
ARM_ABI_ALIAS_FOR(__lshrdi3) long long __aeabi_llsr(long long, int) noexcept;
#endif
