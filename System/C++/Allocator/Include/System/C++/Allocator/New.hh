#ifndef __SYSTEM_CXX_ALLOCATOR_NEW_H
#define __SYSTEM_CXX_ALLOCATOR_NEW_H


#if defined(__XVI_HOSTED)
#  include <new>
#endif

#include <System/C++/LanguageSupport/New.hh>

#include <System/C++/Allocator/Private/Config.hh>


namespace __XVI_STD_ALLOCATOR_NS_DECL
{


namespace __detail
{


#if defined(__XVI_HOSTED)
using __align_val_t = std::align_val_t;
using __nothrow_t = std::nothrow_t;
#else
using __align_val_t = __XVI_STD_NS::align_val_t;
using __nothrow_t = __XVI_STD_NS::nothrow_t;
#endif

extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void* __default_operator_new(size_t);
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void* __default_operator_new_array(size_t);
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void* __default_operator_new_nothrow(size_t, const __nothrow_t&) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void* __default_operator_new_array_nothrow(size_t, const __nothrow_t&) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void* __default_operator_new_aligned(size_t, __align_val_t);

extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void __default_operator_delete(void*) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void __default_operator_delete_sized(void*, size_t) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void __default_operator_delete_array(void*) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void __default_operator_delete_array_sized(void*, size_t) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void __default_operator_delete_nothrow(void*, const __nothrow_t&) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void __default_operator_delete_array_nothrow(void*, const __nothrow_t&) noexcept;
extern "C" __SYSTEM_CXX_ALLOCATOR_EXPORT void __default_operator_delete_aligned(void*, __align_val_t) noexcept;


} // namespace __detail


} // namespace __XVI_STD_ALLOCATOR_NS_DECL


#endif /* ifndef __SYSTEM_CXX_ALLOCATOR_NEW_H */
