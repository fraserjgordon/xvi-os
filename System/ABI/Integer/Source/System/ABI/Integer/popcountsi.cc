#include <System/ABI/Integer/BitOpsImpl.hh>

__int32_t __popcountsi2(__uint32_t u)
{
    return System::ABI::Integer::popcount(u);
}
