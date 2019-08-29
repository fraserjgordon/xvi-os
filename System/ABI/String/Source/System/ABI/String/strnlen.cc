#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

std::size_t __strnlen(const char* string, std::size_t n)
{
    return string_length(string, n);
}
