#ifndef __SYSTEM_C_STDLIB_SETJMP_H
#define __SYSTEM_C_STDLIB_SETJMP_H


#include <System/C/StdLib/Private/Config.h>

#include <System/ABI/ExecContext/ExecContext.h>
#include <System/C/BaseHeaders/Types.h>


#ifdef __cplusplus
extern "C"
{
#endif


struct __jmp_buf
{
    __SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORD_T __state[__SYSTEM_ABI_EXECCONTEXT_JMPBUF_WORDS];
};

typedef struct __jmp_buf jmp_buf[1];


__SYSTEM_C_STDLIB_EXPORT [[gnu::returns_twice]] int setjmp(jmp_buf);
__SYSTEM_C_STDLIB_EXPORT __noreturn void longjmp(jmp_buf, int);

#define setjmp setjmp


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_SETJMP_H */
