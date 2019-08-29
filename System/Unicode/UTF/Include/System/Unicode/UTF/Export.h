#pragma once
#ifndef __SYSTEM_UNICODE_UTF_EXPORT_H
#define __SYSTEM_UNICODE_UTF_EXPORT_H


// Are we building a shared library?
#if defined(System_Unicode_UTF_EXPORTS)
   // Shared library; export the symbols but enforce local binding.
#  if defined(__cplusplus)
#    define __SYSTEM_UNICODE_UTF_EXPORT                  [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_UNICODE_UTF_EXPORT                  __attribute__((__visibility__("protected")))
#  endif
#elif defined(System_Unicode_UTF_BUILD)
   // Static library; do not export the symbols.
#  if defined(__cplusplus)
#    define __SYSTEM_UNICODE_UTF_EXPORT                  [[gnu::visibility("hidden")]]
#  else
#    define __SYSTEM_UNICODE_UTF_EXPORT                  __attribute__((__visibility__("hidden")))
#  endif
#else
   // Library client; no visibility defined.
#  define __SYSTEM_UNICODE_UTF_EXPORT                     /* nothing */
#endif


// Linker symbol declaration.
#define __SYSTEM_UNICODE_UTF_SYMBOL(name)                __SYSTEM_UNICODE_UTF_SYMBOL_(name)
#define __SYSTEM_UNICODE_UTF_SYMBOL_(name)               __asm__("$System$Unicode$UTF$" #name)


#endif /* ifndef __SYSTEM_UNICODE_UTF_EXPORT_H */
