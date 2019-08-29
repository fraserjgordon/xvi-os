#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

char* __strrchr(const char* string, int value)
{
    return const_cast<char*>(string_find_last(string, value));
}
