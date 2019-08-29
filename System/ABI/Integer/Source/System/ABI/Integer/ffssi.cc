#include <System/ABI/Integer/BitOpsImpl.hh>

__int32_t __ffssi2(__uint32_t u)
{
    return System::ABI::Integer::ffs(u);
}
