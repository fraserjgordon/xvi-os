#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__int128_t __ashlti3(__int128_t i, __int32_t count)
{
    return System::ABI::Integer::lshift(__uint128_t(i), __uint32_t(count));
}
#endif
