#pragma once
#ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_INTEGRALCONSTANT_H
#define __SYSTEM_CXX_TYPETRAITS_PRIVATE_INTEGRALCONSTANT_H


#include <System/C++/TypeTraits/Private/Namespace.hh>


namespace __XVI_STD_TYPETRAITS_NS_DECL
{


template <class _T, _T _Value>
struct integral_constant
{
    static constexpr _T value = _Value;

    using value_type = _T;
    using type = integral_constant<_T, _Value>;

    constexpr operator value_type() const noexcept
    {
        return value;
    }

    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};

template <bool _B>
using bool_constant = integral_constant<bool, _B>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;


} // namespace __XVI_STD_TYPETRAITS_NS_DECL


#endif /* ifndef __SYSTEM_CXX_TYPETRAITS_PRIVATE_INTEGRALCONSTANT_H */
