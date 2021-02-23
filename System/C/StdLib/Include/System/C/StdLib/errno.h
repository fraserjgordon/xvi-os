#ifndef __SYSTEM_C_STDLIB_ERRNO_H
#define __SYSTEM_C_STDLIB_ERRNO_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/LanguageSupport/ErrorCodes.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define errno (*__cstdlib_get_errno())
__SYSTEM_C_STDLIB_EXPORT int* __cstdlib_get_errno();


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_ERRNO_H */
