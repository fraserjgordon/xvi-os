#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

void* __memrchr(const void* memory, int value, std::size_t n)
{
    return const_cast<byte_t*>(memory_find_last(static_cast<const byte_t*>(memory), value, n));
}
