#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDDEF_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_STDDEF_H


#include <System/C/BaseHeaders/OffsetOf.h>
#include <System/C/BaseHeaders/Null.h>
#include <System/C/BaseHeaders/Types.h>
#include <System/C++/LanguageSupport/Private/Namespace.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>


namespace __XVI_STD_LANGSUPPORT_NS
{


using ptrdiff_t = __ptrdiff_t;
using size_t = __size_t;
using max_align_t = __max_align_t;
using nullptr_t = decltype(nullptr);


enum class byte : unsigned char {};

template <class _IntType, class = enable_if_t<is_integral_v<_IntType>, void>>
    constexpr byte& operator<<=(byte& __b, _IntType __shift) noexcept
        { return __b = __b << __shift; }

template <class _IntType, class = enable_if_t<is_integral_v<_IntType>, void>>
    constexpr byte& operator>>=(byte& __b, _IntType __shift) noexcept
        { return __b = __b >> __shift; }

template <class _IntType, class = enable_if_t<is_integral_v<_IntType>, void>>
    constexpr byte operator<<(byte __b, _IntType __shift) noexcept
        { return static_cast<byte>(static_cast<unsigned char>(static_cast<unsigned int>(__b) << __shift)); }

template <class _IntType, class = enable_if_t<is_integral_v<_IntType>, void>>
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

template <class _IntType, class = enable_if_t<is_integral_v<_IntType>, void>>
    constexpr _IntType to_integer(byte __b) noexcept
        { return static_cast<_IntType>(__b); }



} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDDEF_H */
