#include <System/C/StdLib/stdlib.h>

#include <System/C/StdLib/errno.h>
#include <System/C/StdLib/StrToNum.hh>


long strtol(const char* __restrict str, char** __restrict end, int base)
{
    return System::C::StdLib::StrToNum<long>(str, end, base);
}
