#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

void* __memmem(const void* memory, std::size_t memory_len, const void* pattern, std::size_t pattern_len)
{
    return const_cast<byte_t*>(memory_search(static_cast<const byte_t*>(memory), memory_len, static_cast<const byte_t*>(pattern), pattern_len));
}
