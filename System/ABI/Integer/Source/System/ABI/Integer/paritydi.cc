#include <System/ABI/Integer/BitOpsImpl.hh>

__int32_t __paritydi2(__uint64_t u)
{
    return System::ABI::Integer::parity(u);
}
