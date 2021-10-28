#pragma once
#ifndef __SYSTEM_ABI_INTEGER_EXPORT_H
#define __SYSTEM_ABI_INTEGER_EXPORT_H


#if __SYSTEM_ABI_INTEGER_BUILD_SHARED
#  define __SYSTEM_ABI_INTEGER_EXPORT   [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_ABI_INTEGER_EXPORT   [[gnu::visibility("hidden")]]
#endif

#ifdef __SYSTEM_ABI_INTEGER_ARM_ABI
#  define ARM_ABI_ALIAS_FOR(func)       extern "C" [[gnu::alias(#func)]] __SYSTEM_ABI_INTEGER_EXPORT
#endif


#endif /* ifndef __SYSTEM_AB_INTEGER_EXPORT_H */
