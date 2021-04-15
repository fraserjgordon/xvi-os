#include <System/C/StdLib/stdlib.h>


int abs(int i)
{
    // Will return INT_MIN if i == INT_MIN due to two's-complement limitations.
    return (i < 0) ? -i : i;
}
