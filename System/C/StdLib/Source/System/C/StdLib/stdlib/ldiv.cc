#include <System/C/StdLib/stdlib.h>


ldiv_t ldiv(long n, long d)
{
    return {.quot = n / d, .rem = n % d};
}
