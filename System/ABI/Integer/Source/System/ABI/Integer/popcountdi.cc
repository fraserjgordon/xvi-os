#include <System/ABI/Integer/BitOpsImpl.hh>

__int32_t __popcountdi2(__uint64_t u)
{
    return System::ABI::Integer::popcount(u);
}
