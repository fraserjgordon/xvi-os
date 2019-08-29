#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

void* __memchr(const void* memory, int value, std::size_t n)
{
    return const_cast<byte_t*>(memory_find(static_cast<const byte_t*>(memory), value, n));
}
