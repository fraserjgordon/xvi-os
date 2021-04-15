#include <System/C/StdLib/stdlib.h>


lldiv_t lldiv(long long n, long long d)
{
    return {.quot = n / d, .rem = n % d};
}
