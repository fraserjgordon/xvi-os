#include <System/C++/Utility/StdLib.hh>


namespace __XVI_STD_UTILITY_NS
{


div_t div(int numer, int denom)
{
    return {numer / denom, numer % denom};
}

ldiv_t ldiv(long int numer, long int denom)
{
    return {numer / denom, numer % denom};
}

lldiv_t lldiv(long long int numer, long long int denom)
{
    return {numer / denom, numer % denom};
}


} // namespace __XVI_STD_UTILITY_NS
