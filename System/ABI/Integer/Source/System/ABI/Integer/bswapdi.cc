#include <System/ABI/Integer/BitOpsImpl.hh>

__int64_t __bswapdi2(__int64_t u)
{
    return static_cast<__int64_t>(System::ABI::Integer::bswap(static_cast<__uint64_t>(u)));
}
