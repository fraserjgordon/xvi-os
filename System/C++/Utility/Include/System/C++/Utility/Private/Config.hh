#pragma once
#ifndef __SYSTEM_CXX_UTILITY_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_UTILITY_PRIVATE_CONFIG_H


#include <System/C++/Utility/Private/Namespace.hh>


#if !defined(__XVI_CXX_UTILITY_NO_EXCEPTIONS)
#  if !defined(__EXCEPTIONS) && !defined(__cpp_exceptions)
#    define __XVI_CXX_UTILITY_NO_EXCEPTIONS 1
#  endif
#endif

#if __XVI_CXX_UTILITY_NO_EXCEPTIONS
#  define __XVI_CXX_UTILITY_FN_TRY                      /* empty */
#  define __XVI_CXX_UTILITY_FN_CATCH(pattern, ...)      /* empty */
#  define __XVI_CXX_UTILITY_TRY                         if (true)
#  define __XVI_CXX_UTILITY_CATCH(pattern)              else
#  define __XVI_CXX_UTILITY_THROW(expr)                 terminate()
#  define __XVI_CXX_UTILITY_RETHROW                     do {} while(false)
#  define __XVI_CXX_UTILITY_CHECK_NEW_RESULT(ptr)       do { if (!ptr) { terminate(); } } while(false)
#else
#  define __XVI_CXX_UTILITY_FN_TRY                      try
#  define __XVI_CXX_UTILITY_FN_CATCH(pattern, ...)      catch (pattern) __VA_ARGS__
#  define __XVI_CXX_UTILITY_TRY                         try
#  define __XVI_CXX_UTILITY_CATCH(pattern)              catch (pattern)
#  define __XVI_CXX_UTILITY_THROW(expr)                 throw expr
#  define __XVI_CXX_UTILITY_RETHROW                     throw
#  define __XVI_CXX_UTILITY_CHECK_NEW_RESULT(ptr)       /* new throws; no check needed */
#endif


#endif /* ifndef __SYSTEM_CXX_UTILITY_PRIVATE_CONFIG_H */
