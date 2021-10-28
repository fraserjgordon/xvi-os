#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__int32_t __divsi3(__int32_t n, __int32_t d)
{
    return System::ABI::Integer::idiv(n, d).q;
}

#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
ARM_ABI_ALIAS_FOR(__divsi3) int __aeabi_idiv(int, int) noexcept;
#endif
