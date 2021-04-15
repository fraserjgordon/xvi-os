#include <System/C/StdLib/stdlib.h>

#include <System/C/StdLib/StrToNum.hh>


long long atoll(const char* str)
{
    return System::C::StdLib::StrToNum<long long>(str, nullptr, 10);
}
