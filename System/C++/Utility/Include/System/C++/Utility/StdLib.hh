#pragma once
#ifndef __SYSTEM_CXX_UTILITY_STDLIB_H
#define __SYSTEM_CXX_UTILITY_STDLIB_H


#include <System/C++/Utility/Private/Config.hh>

#include <System/C/BaseHeaders/Null.h>
#include <System/C++/LanguageSupport/StdDef.hh>


namespace __XVI_STD_UTILITY_NS
{


struct div_t
{
    int quot, rem;
};

struct ldiv_t
{
    long int quot, rem;
};

struct lldiv_t
{
    long long int quot, rem;
};


namespace __detail
{

extern "C" using __c_compare_pred = int(const void*, const void*);
extern "C++" using __cxx_compare_pred = int(const void*, const void*);

} // namespace __detail


void* aligned_alloc(size_t __alignment, size_t __size);
void* calloc(size_t __nmemb, size_t __size);
void  free(void* __ptr);
void* malloc(size_t __size);
void* realloc(void* __ptr, size_t __size);

double atof(const char* __nptr);
int atoi(const char* __nptr);
long int atol(const char* __nptr);
long long int atoll(const char* __nptr);
double strtod(const char* __nptr, char** __endptr);
float strtof(const char* __nptr, char** __endptr);
long int strtol(const char* __nptr, char** __endptr);
long long int strtoll(const char* __nptr, char** __endptr);
unsigned long int strtoul(const char* __nptr, char** __endptr);
unsigned long long int strtoull(const char* __nptr, char** __endptr);

__XVI_CXX_UTILITY_EXPORT void* bsearch(const void* __key, const void* __base, size_t __nmemb, size_t __size, __detail::__c_compare_pred* __pred) __XVI_CXX_UTILITY_SYMBOL(bsearch);
__XVI_CXX_UTILITY_EXPORT void  qsort(void* __base, size_t __nmemb, size_t __size, __detail::__c_compare_pred* __predicate) __XVI_CXX_UTILITY_SYMBOL(qsort);

__XVI_CXX_UTILITY_EXPORT int __abs(int __j) __XVI_CXX_UTILITY_SYMBOL(abs);
__XVI_CXX_UTILITY_EXPORT long int labs(long int __j) __XVI_CXX_UTILITY_SYMBOL(labs);
__XVI_CXX_UTILITY_EXPORT long long int llabs(long long int __j) __XVI_CXX_UTILITY_SYMBOL(llabs);

__XVI_CXX_UTILITY_EXPORT div_t div(int __numer, int __denom) __XVI_CXX_UTILITY_SYMBOL(div);
__XVI_CXX_UTILITY_EXPORT ldiv_t ldiv(long int __numer, long int __denom) __XVI_CXX_UTILITY_SYMBOL(ldiv);
__XVI_CXX_UTILITY_EXPORT lldiv_t lldiv(long long int __numer, long long int __denom) __XVI_CXX_UTILITY_SYMBOL(lldiv);

inline ldiv_t div(long int __numer, long int __denom)
{
    return ldiv(__numer, __denom);
}

inline lldiv_t div(long long int __numer, long long int __denom)
{
    return lldiv(__numer, __denom);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_STDLIB_H */
