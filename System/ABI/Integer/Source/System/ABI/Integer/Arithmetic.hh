#pragma once
#ifndef __SYSTEM_ABI_INTEGER_ARITHMETIC_H
#define __SYSTEM_ABI_INTEGER_ARITHMETIC_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/Integer/Export.hh>
#include <System/ABI/Integer/Types.hh>


extern "C"
{


// Suppress a false-positive warning from clang.
#ifdef __llvm__
#  pragma GCC diagnostic ignored "-Wreturn-type-c-linkage"
#endif


__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ashlsi3(std::int32_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __ashldi3(std::int64_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __ashlti3(__int128_t, std::int32_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ashrsi3(std::int32_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __ashrdi3(std::int64_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __ashrti3(__int128_t, std::int32_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __lshrsi3(std::int32_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __lshrdi3(std::int64_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __lshrti3(__int128_t, std::int32_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __divsi3(std::int32_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __divdi3(std::int64_t, std::int64_t);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __divti3(__int128_t, __int128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __modsi3(std::int32_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __moddi3(std::int64_t, std::int64_t);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __modti3(__int128_t, __int128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __divmodsi4(std::int32_t, std::int32_t, std::int32_t*);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __divmoddi4(std::int64_t, std::int64_t, std::int64_t*);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __divmodti4(__int128_t, __int128_t, __int128_t*);

__SYSTEM_ABI_INTEGER_EXPORT std::uint32_t __udivsi3(std::uint32_t, std::uint32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::uint64_t __udivdi3(std::uint64_t, std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT __uint128_t __udivti3(__uint128_t, __uint128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::uint32_t __umodsi3(std::uint32_t, std::uint32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::uint64_t __umoddi3(std::uint64_t, std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT __uint128_t __umodti3(__uint128_t, __uint128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::uint32_t __udivmodsi4(std::uint32_t, std::uint32_t, std::uint32_t*);
__SYSTEM_ABI_INTEGER_EXPORT std::uint64_t __udivmoddi4(std::uint64_t, std::uint64_t, std::uint64_t*);
__SYSTEM_ABI_INTEGER_EXPORT __uint128_t __udivmodti4(__uint128_t, __uint128_t, __uint128_t*);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __mulsi3(std::int32_t, std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __muldi3(std::int64_t, std::int64_t);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __multi3(__int128_t, __int128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __negdi2(std::int64_t);
__SYSTEM_ABI_INTEGER_EXPORT __int128_t __negti2(__int128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __cmpdi2(std::int64_t, std::int64_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __cmpti2(__int128_t, __int128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ucmpdi2(std::uint64_t, std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ucmpti2(__uint128_t, __uint128_t);


} // extern "C"


#endif /* ifndef __SYSTEM_ABI_INTEGER_ARITHMETIC_H */
