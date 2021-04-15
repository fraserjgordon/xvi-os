#include <System/C/StdLib/stdlib.h>


long labs(long i)
{
    // Will return LONG_MIN if i == LONG_MIN due to two's-complement limitations.
    return (i < 0) ? -i : i;
}
