#ifndef __SYSTEM_C_STDLIB_STDIO_H
#define __SYSTEM_C_STDLIB_STDIO_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Null.h>
#include <System/C/BaseHeaders/Types.h>
#include <System/C/BaseHeaders/VarArgs.h>

#include <System/C/StdLib/MBStateT.h>
#include <System/C/StdLib/Private/AnnexK.h>


#ifdef __cplusplus
extern "C"
{
#endif


#define EOF         (-1)

#define _IOFBF      1
#define _IOLBF      2
#define _IONBF      3

#define FOPEN_MAX   256

#define FILENAME_MAX 1024

#define L_tmpnam    1024
#define TMP_MAX     __INT32_MAX

#define _PRINTF_NAN_LEN_MAX     (8+(__SIZEOF_LONG_DOUBLE__ * 2))

#define SEEK_CUR    1
#define SEEK_SET    0
#define SEEK_END    -1

#define stdin       (__get_stdin())
#define stdout      (__get_stdout())
#define stderr      (__get_stderr())


struct FILE;
typedef struct fpos_t
{
    mbstate_t       __state;
    __uint64_t      __pos;
} fpos_t;


__SYSTEM_C_STDLIB_EXPORT FILE* __get_stdin(void);
__SYSTEM_C_STDLIB_EXPORT FILE* __get_stdout(void);
__SYSTEM_C_STDLIB_EXPORT FILE* __get_stderr(void);

__SYSTEM_C_STDLIB_EXPORT int remove(const char*);
__SYSTEM_C_STDLIB_EXPORT int rename(const char*, const char*);
__SYSTEM_C_STDLIB_EXPORT FILE* tmpfile(void);
__SYSTEM_C_STDLIB_EXPORT char* tmpnam(char*);
__SYSTEM_C_STDLIB_EXPORT int fclose(FILE*);
__SYSTEM_C_STDLIB_EXPORT int fflush(FILE*);
__SYSTEM_C_STDLIB_EXPORT FILE* fopen(const char* __restrict, const char* __restrict);
__SYSTEM_C_STDLIB_EXPORT FILE* freopen(const char* __restrict, const char* __restrict, FILE* __restrict);
__SYSTEM_C_STDLIB_EXPORT void setbuf(FILE* __restrict, char* __restrict);
__SYSTEM_C_STDLIB_EXPORT int setvbuf(FILE* __restrict, char* __restrict, int, __size_t);
__SYSTEM_C_STDLIB_EXPORT int fprintf(FILE* __restrict, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int fscanf(FILE* __restrict, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int printf(const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int scanf(const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int vfprintf(FILE* __restrict, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vfscanf(FILE* __restrict, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vprintf(const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vscanf(const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int fgetc(FILE*);
__SYSTEM_C_STDLIB_EXPORT char* fgets(char* __restrict, int, FILE* __restrict);
__SYSTEM_C_STDLIB_EXPORT int fputc(int, FILE*);
__SYSTEM_C_STDLIB_EXPORT int fputs(const char* __restrict, FILE* __restrict);
__SYSTEM_C_STDLIB_EXPORT int getc(FILE*);
__SYSTEM_C_STDLIB_EXPORT int getchar(void);
__SYSTEM_C_STDLIB_EXPORT int putc(int, FILE*);
__SYSTEM_C_STDLIB_EXPORT int putchar(int);
__SYSTEM_C_STDLIB_EXPORT int puts(const char*);
__SYSTEM_C_STDLIB_EXPORT int ungetc(int, FILE*);
__SYSTEM_C_STDLIB_EXPORT __size_t fread(void* __restrict, __size_t, __size_t, FILE* __restrict);
__SYSTEM_C_STDLIB_EXPORT __size_t fwrite(const void* __restrict, __size_t, __size_t, FILE* __restrict);
__SYSTEM_C_STDLIB_EXPORT int fgetpos(FILE* __restrict, fpos_t* __restrict);
__SYSTEM_C_STDLIB_EXPORT int fseek(FILE*, long int, int);
__SYSTEM_C_STDLIB_EXPORT int fsetpos(FILE*, const fpos_t*);
__SYSTEM_C_STDLIB_EXPORT long int ftell(FILE*);
__SYSTEM_C_STDLIB_EXPORT void rewind(FILE*);
__SYSTEM_C_STDLIB_EXPORT void clearerr(FILE*);
__SYSTEM_C_STDLIB_EXPORT int feof(FILE*);
__SYSTEM_C_STDLIB_EXPORT int ferror(FILE*);
__SYSTEM_C_STDLIB_EXPORT void perror(const char*);

__SYSTEM_C_STDLIB_EXPORT int snprintf(char* __restrict, __size_t, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int sprinf(char* __restrict, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int sscanf(const char* __restrict, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int vsnprintf(char* __restrict, __size_t, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vsprintf(char* __restrict, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vsscanf(const char* __restrict, const char* __restrict, __varargs_va_list);


#if __SYSTEM_C_STDLIB_ANNEXK_ENABLED


#define L_tmpnam_s      L_tmpnam
#define TMP_MAX_S       TMP_MAX


__SYSTEM_C_STDLIB_EXPORT errno_t tmpfile_s(FILE * __restrict * __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t tmpnam_s(char*, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t fopen_s(FILE * __restrict * __restrict, const char* __restrict, const char* __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t freopen_s(FILE * __restrict * __restrict, const char * __restrict, const char* __restrict, FILE * __restrict);
__SYSTEM_C_STDLIB_EXPORT int fprintf_s(FILE * __restrict, const char * __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int fscanf_s(FILE * __restrict, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int printf_s(const char * __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int scanf_s(const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int snprintf_s(char* __restrict, rsize_t, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int sprintf_s(char*  __restrict, rsize_t, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int sscanf_s(const char * __restrict, const char* __restrict, ...);
__SYSTEM_C_STDLIB_EXPORT int vfprintf_s(FILE* __restrict, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vfscanf_s(FILE* __restrict, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vprintf_s(const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vscanf_s(const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vsnprintf_s(char* __restrict, rsize_t, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vsprintf_s(char* __restrict, rsize_t, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT int vsscanf_s(const char* __restrict, const char* __restrict, __varargs_va_list);
__SYSTEM_C_STDLIB_EXPORT char* gets_s(char*, rsize_t);


#endif // __SYSTEM_C_STDLIB_ANNEXK_ENABLED


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_STDIO_H */
