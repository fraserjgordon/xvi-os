#include <System/C/StdLib/stdlib.h>

#include <System/C/StdLib/StrToNum.hh>


int atoi(const char* str)
{
    return System::C::StdLib::StrToNum<int>(str, nullptr, 10);
}
