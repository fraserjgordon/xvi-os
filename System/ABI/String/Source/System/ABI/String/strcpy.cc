#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

char* __strcpy(char* dest, const char* source)
{
    string_copy(dest, source);
    return dest;
}
