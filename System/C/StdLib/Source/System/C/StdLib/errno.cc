#include <System/C/StdLib/errno.h>


static thread_local int s_tls_errno;


int* __cstdlib_get_errno()
{
    return &s_tls_errno;
}
