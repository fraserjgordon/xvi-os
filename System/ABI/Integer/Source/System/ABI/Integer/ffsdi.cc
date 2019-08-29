#include <System/ABI/Integer/BitOpsImpl.hh>

__int32_t __ffsdi2(__uint64_t u)
{
    return System::ABI::Integer::ffs(u);
}
