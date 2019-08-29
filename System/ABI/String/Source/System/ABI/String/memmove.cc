#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

void* __memmove(void* dest, const void* source, std::size_t n)
{
    memory_copy(static_cast<byte_t*>(dest), static_cast<const byte_t*>(source), n);
    return dest;
}
