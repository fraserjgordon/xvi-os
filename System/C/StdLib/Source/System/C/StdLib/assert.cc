#include <System/C/StdLib/assert.h>

#include <System/C/StdLib/stdio.h>
#include <System/C/StdLib/stdlib.h>


void __cstdlib_assert(int cond, const char* message)
{
    if (!cond) [[likely]]
        return;

    fprintf(stderr, "%s", message);
    abort();
}
