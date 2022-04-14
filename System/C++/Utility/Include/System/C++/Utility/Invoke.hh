#pragma once
#ifndef __SYSTEM_CXX_UTILITY_INVOKE_H
#define __SYSTEM_CXX_UTILITY_INVOKE_H


#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Utility/Private/Config.hh>


namespace __XVI_STD_UTILITY_NS
{


template <class _F, class... _Args>
    requires std::is_invocable_v<_F, _Args...>
constexpr invoke_result_t<_F, _Args...> invoke(_F&& __f, _Args&&... __args)
    noexcept(std::is_nothrow_invocable_v<_F, _Args...>)
{
    return __detail::_INVOKE(std::forward<_F>(__f), std::forward<_Args>(__args)...);
}


template <class _R, class _F, class... _Args>
    requires std::is_invocable_r_v<_R, _F, _Args...>
constexpr _R invoke_r(_F&& __f, _Args&&... __args)
    noexcept(std::is_nothrow_invocable_r_v<_R, _F, _Args...>)
{
    return __detail::_INVOKE_R<_R>(std::forward<_F>(__f), std::forward<_Args>(__args)...);
}


} // namespace __XVI_STD_UTILITY_NS


#endif /* ifndef __SYSTEM_CXX_UTILITY_INVOKE_H */
