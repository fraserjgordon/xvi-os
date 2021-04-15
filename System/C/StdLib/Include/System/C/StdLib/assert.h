// No include guard on this file; it may change depending on the NDEBUG macro.


#include <System/C/StdLib/Private/Config.h>


#ifdef __cplusplus
extern "C"
{
#endif


#undef assert
#undef __assert
__SYSTEM_C_STDLIB_EXPORT void __cstdlib_assert(int, const char*);


#ifdef NDEBUG
#  define assert(ignored)   ((void)0)
#else
#  define assert(expr)      __assert(expr)
#  define __assert(expr)    __cstdlib_assert(!((expr) == 0), \
    __FILE__ ":" __LINE__ ": [" __func__ "] assertion failed: " #expr);
#endif


#ifndef __cplusplus
#  define static_assert     _Static_assert
#endif


#ifdef __cplusplus
} // extern "C"
#endif
