#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

char* __stpcpy(char* __restrict dest, const char* __restrict source)
{
    return string_copy(dest, source) - 1;
}
