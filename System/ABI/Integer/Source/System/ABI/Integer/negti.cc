#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__int128_t __negti2(__int128_t i)
{
    return System::ABI::Integer::negate(i);
}
#endif
