#pragma once
#ifndef __SYSTEM_CXX_LANGUAGESUPPORT_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_LANGUAGESUPPORT_PRIVATE_CONFIG_H


#include <System/C++/LanguageSupport/Private/Namespace.hh>


#if __SYSTEM_CXX_LANGUAGESUPPORT_BUILD_SHARED
#  define __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT               [[gnu::visibility("protected")]]
#  define __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE   [[gnu::weak, gnu::visibility("default")]]
#elif __SYSTEM_CXX_LANGUAGESUPPORT_BUILD_STATIC
#  define __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT               /**/
#  define __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE   [[gnu::weak]]
#else
#  define __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT               /**/
#  define __SYSTEM_CXX_LANGUAGESUPPORT_EXPORT_REPLACEABLE   /**/
#endif


#endif /* ifndef __SYSTEM_CXX_LANGUAGESUPPORT_PRIVATE_CONFIG_H */
