#ifndef __SYSTEM_C_STDLIB_STRING_H
#define __SYSTEM_C_STDLIB_STRING_H


#include <System/C/StdLib/Private/Config.h>

#include <System/C/BaseHeaders/Null.h>
#include <System/C/LanguageSupport/StdDef.h>

#include <System/C/StdLib/Private/AnnexK.h>


#ifdef __cplusplus
extern "C"
{
#endif


__SYSTEM_C_STDLIB_EXPORT void* memcpy(void* __restrict, const void* __restrict, size_t);
__SYSTEM_C_STDLIB_EXPORT void* memccpy(void* __restrict, const void* __restrict, int, size_t);
__SYSTEM_C_STDLIB_EXPORT void* memmove(void*, const void*, size_t);
__SYSTEM_C_STDLIB_EXPORT char* strcpy(char* __restrict, const char* __restrict);
__SYSTEM_C_STDLIB_EXPORT char* strncpy(char* __restrict, const char* __restrict, size_t);
__SYSTEM_C_STDLIB_EXPORT char* strcat(char* __restrict, const char* __restrict);
__SYSTEM_C_STDLIB_EXPORT char* strncat(char* __restrict, const char* __restrict, size_t);
__SYSTEM_C_STDLIB_EXPORT int memcmp(const void*, const void*, size_t);
__SYSTEM_C_STDLIB_EXPORT int strcmp(const char*, const char*);
__SYSTEM_C_STDLIB_EXPORT int strcoll(const char*, const char*);
__SYSTEM_C_STDLIB_EXPORT int strncmp(const char*, const char*, size_t);
__SYSTEM_C_STDLIB_EXPORT size_t strxfrm(char* __restrict, const char* __restrict, size_t);
__SYSTEM_C_STDLIB_EXPORT void* memchr(const void*, int, size_t);
__SYSTEM_C_STDLIB_EXPORT char* strchr(const char*, int);
__SYSTEM_C_STDLIB_EXPORT size_t strcspn(const char*, const char*);
__SYSTEM_C_STDLIB_EXPORT char* strpbrk(const char*, const char*);
__SYSTEM_C_STDLIB_EXPORT char* strrchr(const char*, int);
__SYSTEM_C_STDLIB_EXPORT size_t strspn(const char*, const char*);
__SYSTEM_C_STDLIB_EXPORT char* strstr(const char*, const char*);
__SYSTEM_C_STDLIB_EXPORT char* strtok(char* __restrict, const char* __restrict);
__SYSTEM_C_STDLIB_EXPORT void* memset(void*, int, size_t);
__SYSTEM_C_STDLIB_EXPORT char* strerror(int);
__SYSTEM_C_STDLIB_EXPORT size_t strlen(const char*);
__SYSTEM_C_STDLIB_EXPORT char* strdup(const char*);
__SYSTEM_C_STDLIB_EXPORT char* strndup(const char*, size_t);


#if __SYSTEM_C_STDLIB_ANNEXK_ENABLED


__SYSTEM_C_STDLIB_EXPORT errno_t memcpy_s(void* __restrict, rsize_t, const void* __restrict, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t memmove_s(void*, rsize_t, const void*, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t strcpy_s(char* __restrict, rsize_t, const char* __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t strncpy_s(char* __restrict, rsize_t, const char* __restrict, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t strcat_s(char* __restrict, rsize_t, const char* __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t strncat_s(char* __restrict, rsize_t, const char* __restrict, rsize_t);
__SYSTEM_C_STDLIB_EXPORT char* strtok_s(char* __restrict, rsize_t* __restrict, const char* __restrict, char** __restrict);
__SYSTEM_C_STDLIB_EXPORT errno_t memset_s(void*, rsize_t, int, rsize_t);
__SYSTEM_C_STDLIB_EXPORT errno_t strerror_s(char*, rsize_t, errno_t);
__SYSTEM_C_STDLIB_EXPORT errno_t strerrorlen_s(errno_t);
__SYSTEM_C_STDLIB_EXPORT size_t strnlen_s(const char*, size_t);


#endif // __SYSTEM_C_STDLIB_ANNEXK_ENABLED


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_STRING_H */
