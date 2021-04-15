#include <System/C/StdLib/inttypes.h>

#include <System/C/StdLib/errno.h>
#include <System/C/StdLib/StrToNum.hh>


intmax_t strtoimax(const char* __restrict str, char** __restrict end, int base)
{
    return System::C::StdLib::StrToNum<intmax_t>(str, end, base);
}
