#include <System/ABI/String/StringImpl.hh>

using namespace System::ABI::String;

int __strcmp(const char* one, const char* two)
{
    return string_compare(one, two);
}
