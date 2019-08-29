#include <System/ABI/Integer/BitOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__int32_t __popcountti2(__uint128_t u)
{
    return System::ABI::Integer::popcount(u);
}
#endif
