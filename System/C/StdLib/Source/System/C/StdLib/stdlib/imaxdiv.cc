#include <System/C/StdLib/inttypes.h>


imaxdiv_t imaxdiv(intmax_t n, intmax_t d)
{
    return {.quot = n / d, .rem = n % d};
}
