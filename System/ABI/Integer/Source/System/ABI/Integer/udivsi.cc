#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

__uint32_t __udivsi3(__uint32_t n, __uint32_t d)
{
    return System::ABI::Integer::udiv(n, d).q;
}

#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
ARM_ABI_ALIAS_FOR(__udivsi3) unsigned int __aeabi_uidiv(unsigned int, unsigned int) noexcept;
#endif
