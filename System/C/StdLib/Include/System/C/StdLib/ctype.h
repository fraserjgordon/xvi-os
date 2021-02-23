#ifndef __SYSTEM_C_STDLIB_CTYPE_H
#define __SYSTEM_C_STDLIB_CTYPE_H


#include <System/C/StdLib/Private/Config.h>


#ifdef __cplusplus
extern "C"
{
#endif


__SYSTEM_C_STDLIB_EXPORT int isalnum(int);
__SYSTEM_C_STDLIB_EXPORT int isalpha(int);
__SYSTEM_C_STDLIB_EXPORT int isblank(int);
__SYSTEM_C_STDLIB_EXPORT int iscntrl(int);
__SYSTEM_C_STDLIB_EXPORT int isdigit(int);
__SYSTEM_C_STDLIB_EXPORT int isgraph(int);
__SYSTEM_C_STDLIB_EXPORT int islower(int);
__SYSTEM_C_STDLIB_EXPORT int isprint(int);
__SYSTEM_C_STDLIB_EXPORT int ispunct(int);
__SYSTEM_C_STDLIB_EXPORT int isspace(int);
__SYSTEM_C_STDLIB_EXPORT int isupper(int);
__SYSTEM_C_STDLIB_EXPORT int isxdigit(int);

__SYSTEM_C_STDLIB_EXPORT int tolower(int);
__SYSTEM_C_STDLIB_EXPORT int toupper(int);


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_CTYPE_H */
