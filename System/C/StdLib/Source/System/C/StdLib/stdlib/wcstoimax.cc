#include <System/C/StdLib/inttypes.h>

#include <System/C/StdLib/errno.h>
#include <System/C/StdLib/StrToNum.hh>


intmax_t wcstoimax(const wchar_t* __restrict str, wchar_t** __restrict end, int base)
{
    return System::C::StdLib::StrToNum<intmax_t, wchar_t>(str, end, base);
}
