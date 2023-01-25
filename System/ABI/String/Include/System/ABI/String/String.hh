#ifndef __SYSTEM_ABI_STRING_STRING_H
#define __SYSTEM_ABI_STRING_STRING_H


#if __XVI_HOSTED_TEST
#  include <cstddef>
#else
#  define __XVI_STD_NS std
#  include <System/C++/LanguageSupport/StdDef.hh>
#endif


// Symbols are weak to prevent clashes with C library symbols.
#if __SYSTEM_ABI_STRING_BUILD_SHARED
#  define __SYSTEM_ABI_STRING_EXPORT [[gnu::visibility("protected"), gnu::weak]]
#elif __SYSTEM_ABI_STRING_BUILD
#  define __SYSTEM_ABI_STRING_EXPORT [[gnu::weak]]
#else
#  define __SYSTEM_ABI_STRING_EXPORT /**/
#endif


__SYSTEM_ABI_STRING_EXPORT void* __memccpy(void* __restrict, const void* __restrict, int, std::size_t) asm("memccpy");
__SYSTEM_ABI_STRING_EXPORT void* __memchr(const void*, int, std::size_t) asm("memchr");
__SYSTEM_ABI_STRING_EXPORT int   __memcmp(const void*, const void*, std::size_t) asm("memcmp");
__SYSTEM_ABI_STRING_EXPORT void* __memcpy(void* __restrict, const void* __restrict, std::size_t) asm("memcpy");
__SYSTEM_ABI_STRING_EXPORT void* __memmem(const void*, std::size_t, const void*, std::size_t) asm("memmem");
__SYSTEM_ABI_STRING_EXPORT void* __memmove(void*, const void*, std::size_t) asm("memmove");
__SYSTEM_ABI_STRING_EXPORT void* __memrchr(const void*, int, std::size_t) asm("memrchr");
__SYSTEM_ABI_STRING_EXPORT void* __memset(void*, int, std::size_t) asm("memset");

__SYSTEM_ABI_STRING_EXPORT char*    __stpcpy(char* __restrict, const char* __restrict) asm("stpcpy");
__SYSTEM_ABI_STRING_EXPORT char*    __stpncpy(char* __restrict, const char* __restrict, std::size_t) asm("stpncpy");

__SYSTEM_ABI_STRING_EXPORT char*    __strcat(char* __restrict, const char* __restrict) asm("strcat");
__SYSTEM_ABI_STRING_EXPORT char*    __strchr(const char*, int) asm("strchr");
__SYSTEM_ABI_STRING_EXPORT int      __strcmp(const char*, const char*) asm("strcmp");
__SYSTEM_ABI_STRING_EXPORT char*    __strcpy(char* __restrict, const char* __restrict) asm("strcpy");
__SYSTEM_ABI_STRING_EXPORT std::size_t __strlen(const char*) asm("strlen");
__SYSTEM_ABI_STRING_EXPORT char*    __strrchr(const char*, int) asm("strrchr");
__SYSTEM_ABI_STRING_EXPORT char*    __strstr(const char*, const char*) asm("strstr");

__SYSTEM_ABI_STRING_EXPORT char*    __strncat(char* __restrict, const char* __restrict, std::size_t) asm("strncat");
__SYSTEM_ABI_STRING_EXPORT int      __strncmp(const char*, const char*, std::size_t) asm("strncmp");
__SYSTEM_ABI_STRING_EXPORT char*    __strncpy(char* __restrict, const char* __restrict, std::size_t) asm("strncpy");
__SYSTEM_ABI_STRING_EXPORT std::size_t __strnlen(const char*, std::size_t) asm("strnlen");

__SYSTEM_ABI_STRING_EXPORT std::size_t __strlcat(char* __restrict, const char* __restrict, std::size_t n) asm("strlcat");
__SYSTEM_ABI_STRING_EXPORT std::size_t __strlcpy(char* __restrict, const char* __restrict, std::size_t n) asm ("strlcpy");


#endif /* ifndef __SYSTEM_ABI_STRING_STRING_H */
