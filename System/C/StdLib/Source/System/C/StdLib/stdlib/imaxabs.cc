#include <System/C/StdLib/inttypes.h>


intmax_t imaxabs(intmax_t i)
{
    // Will return INTMAX_MIN if i == INTMAX_MIN due to two's-complement limitations.
    return (i < 0) ? -i : i;
}
