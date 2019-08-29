#include <System/ABI/Integer/ArithmeticOpsImpl.hh>

#if __SYSTEM_ABI_INTEGER_PROVIDE_128BIT
__int128_t __divti3(__int128_t n, __int128_t d)
{
    return System::ABI::Integer::idiv(n, d).q;
}
#endif
