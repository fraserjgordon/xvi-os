#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

char* __strcat(char* __restrict dest, const char* __restrict source)
{
    string_concatenate(dest, source);
    return dest;
}
