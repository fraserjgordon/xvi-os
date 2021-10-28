#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int64_t __muldi3(__int64_t a, __int64_t b)
{
    return static_cast<__int64_t>(System::ABI::Integer::mul(__uint64_t(a), __uint64_t(b)));
}

#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
ARM_ABI_ALIAS_FOR(__muldi3) long long __aeabi_lmul(long long, long long) noexcept;
#endif
