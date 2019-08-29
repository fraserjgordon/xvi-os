#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

std::size_t __strlcat(char* __restrict dest, const char* __restrict source, std::size_t n)
{
    return string_concatenate_l(dest, source, n);
}
