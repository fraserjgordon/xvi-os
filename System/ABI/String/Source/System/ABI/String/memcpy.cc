#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

void* __memcpy(void* __restrict dest, const void* __restrict source, std::size_t n)
{
    memory_copy(static_cast<byte_t*>(dest), static_cast<const byte_t*>(source), n);
    return dest;
}
