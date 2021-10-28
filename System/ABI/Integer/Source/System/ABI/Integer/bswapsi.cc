#include <System/ABI/Integer/BitOpsImpl.hh>

__int32_t __bswapsi2(__int32_t u)
{
    return static_cast<__int32_t>(System::ABI::Integer::bswap(static_cast<__uint32_t>(u)));
}
