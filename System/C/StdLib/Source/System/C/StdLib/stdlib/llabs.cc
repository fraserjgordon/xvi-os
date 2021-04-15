#include <System/C/StdLib/stdlib.h>


long long llabs(long long i)
{
    // Will return LLONG_MIN if i == LLONG_MIN due to two's-complement limitations.
    return (i < 0) ? -i : i;
}
