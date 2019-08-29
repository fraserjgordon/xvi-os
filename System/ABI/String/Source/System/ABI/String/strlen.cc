#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

std::size_t __strlen(const char* string)
{
    return string_length(string);
}
