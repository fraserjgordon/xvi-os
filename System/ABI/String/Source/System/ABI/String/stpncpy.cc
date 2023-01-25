#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

char* __stpncpy(char* __restrict dest, const char* __restrict source, std::size_t n)
{
    return string_copy_n(dest, source, n);
}
