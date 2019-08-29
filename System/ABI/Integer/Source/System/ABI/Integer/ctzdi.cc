#include <System/ABI/Integer/BitOpsImpl.hh>

__int32_t __ctzdi2(__uint64_t u)
{
    return System::ABI::Integer::ctz(u);
}
