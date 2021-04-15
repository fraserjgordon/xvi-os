#include <System/C/StdLib/stdlib.h>


div_t div(int n, int d)
{
    return {.quot = n / d, .rem = n % d};
}
