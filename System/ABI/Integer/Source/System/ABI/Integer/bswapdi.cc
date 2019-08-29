#include <System/ABI/Integer/BitOpsImpl.hh>

__int64_t __bswapdi2(__int64_t u)
{
    return System::ABI::Integer::bswap(u);
}
