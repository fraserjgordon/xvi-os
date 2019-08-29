#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

char* __strstr(const char* string, const char* pattern)
{
    return const_cast<char*>(string_search(string, pattern));
}
