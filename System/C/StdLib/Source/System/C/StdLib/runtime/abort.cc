#include <System/C/StdLib/stdlib.h>


[[noreturn]] void __abort() asm("System.ABI.Runtime.Abort");


void abort()
{
    __abort();
}
