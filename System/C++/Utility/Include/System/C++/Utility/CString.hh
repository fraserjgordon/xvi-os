#ifndef __SYSTEM_CXX_UTILITY_CSTRING_H
#define __SYSTEM_CXX_UTILITY_CSTRING_H


#include <System/C++/Utility/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>


namespace __XVI_STD_UTILITY_NS
{


// The functions defined here are implemented in System.ABI.String.
//
// Double-underscored functions are non-standard.


void* __memccpy(void* __restrict, const void* __restrict, int, std::size_t) asm("memccpy");
void* memchr(void*, int, std::size_t) asm("memchr");
int   memcmp(const void*, const void*, std::size_t) asm("memcmp");
void* memcpy(void* __restrict, const void* __restrict, std::size_t) asm("memcpy");
void* __memmem(void*, std::size_t, const void*, std::size_t) asm("memmem");
void* memmove(void*, const void*, std::size_t) asm("memmove");
void* __memrchr(void*, int, std::size_t) asm("memrchr");
void* memset(void*, int, std::size_t) asm("memset");

char*    __stpcpy(char* __restrict, const char* __restrict) asm("stpcpy");
char*    __stpncpy(char* __restrict, const char* __restrict, std::size_t) asm("stpncpy");

char*    strcat(char* __restrict, const char* __restrict) asm("strcat");
char*    strchr(char*, int) asm("strchr");
int      strcmp(const char*, const char*) asm("strcmp");
char*    strcpy(char* __restrict, const char* __restrict) asm("strcpy");
std::size_t strlen(const char*) asm("strlen");
char*    strrchr(char*, int) asm("strrchr");
char*    strstr(char*, const char*) asm("strstr");

char*    strncat(char* __restrict, const char* __restrict, std::size_t) asm("strncat");
int      strncmp(const char*, const char*, std::size_t) asm("strncmp");
char*    strncpy(char* __restrict, const char* __restrict, std::size_t) asm("strncpy");
std::size_t __strnlen(const char*, std::size_t) asm("strnlen");

std::size_t __strlcat(char* __restrict, const char* __restrict, std::size_t n) asm("strlcat");
std::size_t __strlcpy(char* __restrict, const char* __restrict, std::size_t n) asm ("strlcpy");


// Extra overloads for const correctness.
const void* memchr(const void*, int, std::size_t) asm("memchr");
const char* strchr(const char*, int) asm("strchr");
const char* strrchr(const char*, int) asm("strrchr");
const char* strstr(const char*, const char*) asm("strstr");
const void* __memmem(const void*, std::size_t, const void*, std::size_t) asm("memmem");
const void* __memrchr(const void*, int, std::size_t) asm("memrchr");



} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_CSTRING_H */
