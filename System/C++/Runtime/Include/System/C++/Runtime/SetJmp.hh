#pragma once
#ifndef __SYSTEM_CXX_RUNTIME_SETJMP_H
#define __SYSTEM_CXX_RUNTIME_SETJMP_H


#include <System/C++/Runtime/Private/Config.hh>


namespace __XVI_STD_RUNTIME_NS
{


namespace __detail
{


//! @TODO: this struct belongs in a C-compat header.
struct __jmp_buf_t {};

[[gnu::returns_twice]]
int __setjmp(__jmp_buf_t* __env);


} // namespace __detail


using jmp_buf = __detail::__jmp_buf_t[1];

[[noreturn]] void longjmp(jmp_buf __env, int __val);
#define setjmp(env) __detail::__setjmp(env)


} // namespace __XVI_STD_RUNTIME_NS


#endif /* ifndef __SYSTEM_CXX_RUNTIME_SETJMP_H */
