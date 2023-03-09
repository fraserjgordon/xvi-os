#ifndef __SYSTEM_CXX_CORE_INVOKE_H
#define __SYSTEM_CXX_CORE_INVOKE_H


#include <System/C++/LanguageSupport/Utility.hh>
#include <System/C++/TypeTraits/TypeTraits.hh>

#include <System/C++/Core/Private/Config.hh>


namespace __XVI_STD_CORE_NS_DECL
{


template <class _F, class... _Args>
    requires __XVI_STD_NS::is_invocable_v<_F, _Args...>
constexpr invoke_result_t<_F, _Args...> invoke(_F&& __f, _Args&&... __args)
    noexcept(__XVI_STD_NS::is_nothrow_invocable_v<_F, _Args...>)
{
    return __detail::_INVOKE(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::forward<_Args>(__args)...);
}


template <class _R, class _F, class... _Args>
    requires __XVI_STD_NS::is_invocable_r_v<_R, _F, _Args...>
constexpr _R invoke_r(_F&& __f, _Args&&... __args)
    noexcept(__XVI_STD_NS::is_nothrow_invocable_r_v<_R, _F, _Args...>)
{
    return __detail::_INVOKE_R<_R>(__XVI_STD_NS::forward<_F>(__f), __XVI_STD_NS::forward<_Args>(__args)...);
}


} // namespace __XVI_STD_CORE_NS_DECL


#endif /* ifndef __SYSTEM_CXX_CORE_INVOKE_H */
