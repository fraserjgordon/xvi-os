#pragma once
#ifndef __SYSTEM_C_LANGUAGESUPPORT_STDARG_H
#define __SYSTEM_C_LANGUAGESUPPORT_STDARG_H


#include <System/C/BaseHeaders/VarArgs.h>


typedef __varargs_va_list       va_list;

#define va_start(va, param)     __varargs_start(va, param)
#define va_arg(va, type)        __varargs_arg(va, type)
#define va_end(va)              __varargs_end(va)
#define va_copy(dest, src)      __varargs_copy(dest, src)


#endif /* ifndef __SYSTEM_C_LANGUAGESUPPORT_STDARG_H */
