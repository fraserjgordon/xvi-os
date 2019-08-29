#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

void* __memccpy(void* __restrict dest, const void* __restrict source, int terminator, std::size_t n)
{
    return memory_copy_until(static_cast<byte_t*>(dest), static_cast<const byte_t*>(source), terminator, n);
}
