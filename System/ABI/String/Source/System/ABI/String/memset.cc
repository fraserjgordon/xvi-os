#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

// The attribute prevents GCC from turning this function into a call to memset...
#ifdef __GNUC__
[[gnu::optimize("-fno-tree-loop-distribute-patterns")]]
#endif
void* __memset(void* memory, int value, std::size_t n)
{
    memory_set(static_cast<byte_t*>(memory), value, n);
    return memory;
}
