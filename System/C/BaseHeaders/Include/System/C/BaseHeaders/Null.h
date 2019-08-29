#pragma once
#ifndef __SYSTEM_C_BASEHEADERS_NULL_H
#define __SYSTEM_C_BASEHEADERS_NULL_H


#undef NULL
#if defined(__cplusplus)
#  define NULL      nullptr
#elif defined(__GNUC__)
#  define NULL      __null
#else
#  define NULL      ((void*)0)
#endif


#endif /* ifndef __SYSTEM_C_BASEHEADERS_NULL_H */
