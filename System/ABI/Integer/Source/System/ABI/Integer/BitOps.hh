#pragma once
#ifndef __SYSTEM_ABI_INTEGER_BITOPS_H
#define __SYSTEM_ABI_INTEGER_BITOPS_H


#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/Integer/Export.hh>
#include <System/ABI/Integer/Types.hh>


__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __clzsi2(std::uint32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __clzdi2(std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __clzti2(__uint128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ctzsi2(std::uint32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ctzdi2(std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ctzti2(__uint128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ffssi2(std::uint32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ffsdi2(std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __ffsti2(__uint128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __paritysi2(std::uint32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __paritydi2(std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __parityti2(__uint128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __popcountsi2(std::uint32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __popcountdi2(std::uint64_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __popcountti2(__uint128_t);

__SYSTEM_ABI_INTEGER_EXPORT std::int32_t __bswapsi2(std::int32_t);
__SYSTEM_ABI_INTEGER_EXPORT std::int64_t __bswapdi2(std::int64_t);


#endif /* ifndef __SYSTEM_ABI_INTEGER_BITOPS_H */
