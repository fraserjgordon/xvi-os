#include <System/C/StdLib/stdlib.h>

#include <System/C/StdLib/StrToNum.hh>


long atol(const char* str)
{
    return System::C::StdLib::StrToNum<long>(str, nullptr, 10);
}
