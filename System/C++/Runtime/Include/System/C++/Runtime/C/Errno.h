#pragma once
#ifndef __SYSTEM_CXX_RUNTIME_ERRNO_H
#define __SYSTEM_CXX_RUNTIME_ERRNO_H


#include <System/C++/Runtime/Private/Config.hh>

#include <System/C/LanguageSupport/ErrorCodes.h>


#define errno (*__get_errno_ptr())
extern "C" int* __get_errno_ptr(void);


#endif /* ifndef __SYSTEM_CXX_RUNTIME_ERRNO_H */
