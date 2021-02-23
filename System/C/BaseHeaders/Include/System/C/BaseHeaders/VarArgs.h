#ifndef __SYSTEM_C_BASEHEADERS_VARARGS_H
#define __SYSTEM_C_BASEHEADERS_VARARGS_H


#define __varargs_va_list               __builtin_va_list
#define __varargs_start(va, param)      __builtin_va_start(va, param)
#define __varargs_end(va)               __builtin_va_end(va)
#define __varargs_arg(va, type)         __builtin_va_arg(va, type)
#define __varargs_copy(dest, src)       __builtin_va_copy(dest, src)


#endif /* ifndef __SYSTEM_C_BASEHEADERS_VARARGS_H */
