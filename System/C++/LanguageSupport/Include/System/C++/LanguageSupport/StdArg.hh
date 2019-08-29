#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDARG_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_STDARG_H


#include <System/C/BaseHeaders/VarArgs.h>

#include <System/C++/LanguageSupport/Private/Namespace.hh>


#define va_arg(va, type)        __varargs_arg(va, type)
#define va_copy(vdest, vsrc)    __varargs_copy(vdest, vsrc)
#define va_end(va)              __varargs_end(va)
#define va_start(va, param)     __varargs_start(va, param)


namespace __XVI_STD_LANGSUPPORT_NS
{


using va_list = __varargs_va_list;


} // namespace __XVI_STD_LANGSUPPORT_NS


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_STDARG_H */
