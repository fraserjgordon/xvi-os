#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDDEF_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_STDDEF_H


#include <System/C/BaseHeaders/OffsetOf.h>
#include <System/C/BaseHeaders/Null.h>
#include <System/C/BaseHeaders/Types.h>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/LanguageSupport/Private/Namespace.hh>



namespace __XVI_STD_LANGSUPPORT_NS_DECL
{


// Some hosts define __size_t
#if __XVI_HOSTED && defined(__size_t)
#  define __XVI_REDEF_SIZE_T
#  undef __size_t
#endif

using ptrdiff_t = __ptrdiff_t;
using size_t = __size_t;
using max_align_t = __max_align_t;
using nullptr_t = decltype(nullptr);

#ifdef __XVI_REDEF_SIZE_T
#  undef __XVI_REDEF_SIZE_T
#  define __size_t
#endif


enum class byte : unsigned char {};

template <class _IntType> requires __XVI_STD_NS::is_integral_v<_IntType>
    constexpr byte& operator<<=(byte& __b, _IntType __shift) noexcept
        { return __b = __b << __shift; }

template <class _IntType> requires __XVI_STD_NS::is_integral_v<_IntType>
    constexpr byte& operator>>=(byte& __b, _IntType __shift) noexcept
        { return __b = __b >> __shift; }

template <class _IntType> requires __XVI_STD_NS::is_integral_v<_IntType>
    constexpr byte operator<<(byte __b, _IntType __shift) noexcept
        { return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(__b) << __shift)); }

template <class _IntType> requires __XVI_STD_NS::is_integral_v<_IntType>
    constexpr byte operator>>(byte __b, _IntType __shift) noexcept
        { return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(__b) >> __shift)); }

constexpr byte operator|(byte __l, byte __r) noexcept
    { return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(__l) | static_cast<unsigned int>(__r))); }

constexpr byte operator&(byte __l, byte __r) noexcept
    { return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(__l) & static_cast<unsigned int>(__r))); }

constexpr byte operator^(byte __l, byte __r) noexcept
    { return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(__l) ^ static_cast<unsigned int>(__r))); }

constexpr byte operator~(byte __b) noexcept
    {return static_cast<byte>(static_cast<unsigned char>(~static_cast<unsigned int>(__b))); }

constexpr byte& operator|=(byte& __b, byte __r) noexcept
    { return __b = __b | __r; }

constexpr byte& operator&=(byte& __b, byte __r) noexcept
    { return __b = __b & __r; }

constexpr byte& operator^=(byte& __b, byte __r) noexcept
    { return __b = __b ^ __r; };

template <class _IntType>
    requires __XVI_STD_NS::is_integral_v<_IntType>
constexpr _IntType to_integer(byte __b) noexcept
    { return static_cast<_IntType>(__b); }



} // namespace __XVI_STD_LANGSUPPORT_NS_DECL


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDDEF_H */
