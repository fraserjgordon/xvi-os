#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

int __memcmp(const void* one, const void* two, std::size_t n)
{
    return memory_compare(static_cast<const byte_t*>(one), static_cast<const byte_t*>(two), n);
}
