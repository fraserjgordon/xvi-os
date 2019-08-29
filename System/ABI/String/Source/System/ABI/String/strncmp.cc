#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

int __strncmp(const char* one, const char* two, std::size_t n)
{
    using T = const unsigned char*;
    return string_compare<unsigned char>(reinterpret_cast<T>(one), reinterpret_cast<T>(two), n);
}
