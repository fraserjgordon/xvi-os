#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_NEW_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_NEW_H


#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/Private/Config.hh>

#if defined(__XVI_HOSTED) && __has_include_next(<new>)
#  include_next <new>
#endif


// Some of these types must be in std to trigger special compiler behaviour.
#if !__XVI_HOSTED
namespace std
#else
namespace __XVI_STD_LANGSUPPORT_NS
#endif
{

struct destroying_delete_t { explicit destroying_delete_t() = default; };

enum class align_val_t : __XVI_STD_NS::size_t {};

struct nothrow_t { explicit nothrow_t() = default; };


} // namespace std OR namespace __XVI_STD_LANGSUPPORT_NS


namespace __XVI_STD_LANGSUPPORT_NS
{


#if !__XVI_HOSTED
using std::destroying_delete_t;
using std::align_val_t;
using std::nothrow_t;
#endif


inline constexpr destroying_delete_t destroying_delete {};
inline constexpr nothrow_t nothrow {};

using new_handler = void (*)();


__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT new_handler get_new_handler() noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT new_handler set_new_handler(new_handler) noexcept;


template <class _T>
[[nodiscard]] constexpr _T* launder(_T* __p) noexcept
{
    static_assert(!__XVI_STD_NS::is_function_v<_T> && !__XVI_STD_NS::is_void_v<_T>);
    return __builtin_launder(__p);
}

//inline constexpr size_t hardware_constructive_interference_size = ?
//inline constexpr size_t hardware_destructive_interference_size = ?


} // namespace __XVI_STD_LANGSUPPORT_NS


#ifndef __XVI_HOSTED

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new(__XVI_STD_NS::size_t);
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new(__XVI_STD_NS::size_t, __XVI_STD_NS::align_val_t);

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new[](__XVI_STD_NS::size_t);
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new[](__XVI_STD_NS::size_t, __XVI_STD_NS::align_val_t);

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete(void*) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete(void*, __XVI_STD_NS::size_t) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete(void*, __XVI_STD_NS::align_val_t) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete(void*, __XVI_STD_NS::size_t, __XVI_STD_NS::align_val_t) noexcept;

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete[](void*) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete[](void*, __XVI_STD_NS::size_t) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete[](void*, __XVI_STD_NS::align_val_t) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete[](void*, __XVI_STD_NS::size_t, __XVI_STD_NS::align_val_t) noexcept;


[[nodiscard]] constexpr void* operator new(__XVI_STD_NS::size_t, void* __ptr) noexcept
{
    return __ptr;
}

[[nodiscard]] constexpr void* operator new[](__XVI_STD_NS::size_t, void* __ptr) noexcept
{
    return __ptr;
}

constexpr void operator delete(void*, void*) noexcept
{
}

constexpr void operator delete[](void*, void*) noexcept
{
}


__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new(__XVI_STD_NS::size_t, const __XVI_STD_NS::nothrow_t&) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new(__XVI_STD_NS::size_t, __XVI_STD_NS::align_val_t, const __XVI_STD_NS::nothrow_t&) noexcept;

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new[](__XVI_STD_NS::size_t, const __XVI_STD_NS::nothrow_t&) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE [[nodiscard]] void* operator new[](__XVI_STD_NS::size_t, __XVI_STD_NS::align_val_t, const __XVI_STD_NS::nothrow_t&) noexcept;

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete(void*, const __XVI_STD_NS::nothrow_t&) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete(void*, __XVI_STD_NS::align_val_t, const __XVI_STD_NS::nothrow_t&) noexcept;

__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete[](void*, const __XVI_STD_NS::nothrow_t&) noexcept;
__SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE void operator delete[](void*, __XVI_STD_NS::align_val_t, const __XVI_STD_NS::nothrow_t&) noexcept;

#endif // ifndef __XVI_HOSTED


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_NEW_H */
