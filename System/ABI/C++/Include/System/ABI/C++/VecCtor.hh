#pragma once
#ifndef __SYSTEM_ABI_CXX_CTORDTOR_H
#define __SYSTEM_ABI_CXX_CTORDTOR_H


#include <System/C++/LanguageSupport/StdDef.hh>

#include <System/ABI/C++/Private/Config.hh>


namespace __SYSTEM_ABI_CXX_NS
{


extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void* __cxa_vec_new(std::size_t, std::size_t, std::size_t, void (*)(void*), void (*)(void*));
extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void* __cxa_vec_new2(std::size_t, std::size_t, std::size_t, void (*)(void*), void (*)(void*), void* (*)(std::size_t), void (*)(void*));
extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void* __cxa_vec_new3(std::size_t, std::size_t, std::size_t, void (*)(void*), void (*)(void*), void* (*)(std::size_t), void (*)(void*, std::size_t));
extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void  __cxa_vec_delete(void*, std::size_t, std::size_t, void (*)(void*));
extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void  __cxa_vec_delete2(void*, std::size_t, std::size_t, void (*)(void*), void (*)(void*));
extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void  __cxa_vec_delete3(void*, std::size_t, std::size_t, void (*)(void*), void (*)(void*, std::size_t));

extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void __cxa_throw_bad_array_new_length();

extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void __cxa_vec_ctor(void*, std::size_t, std::size_t, void (*)(void*), void (*)(void*));
extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void __cxa_vec_dtor(void*, std::size_t, std::size_t, void (*)(void*));
extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void __cxa_vec_cleanup(void*, std::size_t, std::size_t, void (*)(void*));

extern "C" __SYSTEM_ABI_CXX_CTORDTOR_EXPORT void __cxa_vec_cctor(void*, void*, std::size_t, std::size_t, void (*)(void*, void*), void (*)(void*));

} // namespace __SYSTEM_ABI_CXX_NS


#endif /* ifndef __SYSTEM_ABI_CXX_CTORDTOR_H */
