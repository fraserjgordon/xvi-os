#include <System/C/StdLib/inttypes.h>

#include <System/C/StdLib/errno.h>
#include <System/C/StdLib/StrToNum.hh>


uintmax_t strtoumax(const char* __restrict str, char** __restrict end, int base)
{
    return System::C::StdLib::StrToNum<uintmax_t>(str, end, base);
}
