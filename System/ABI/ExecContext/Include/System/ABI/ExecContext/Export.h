#pragma once
#ifndef __SYSTEM_ABI_EXECCONTEXT_EXPORT_H
#define __SYSTEM_ABI_EXECCONTEXT_EXPORT_H


#ifdef __cplusplus
#  include <System/C++/TypeTraits/TypeTraits.hh>
#endif


// Are we building a shared library?
#if defined(System_ABI_ExecContext_EXPORTS)
   // Shared library; export the symbols but enforce local binding.
#  if defined(__cplusplus)
#    define __SYSTEM_ABI_EXECCONTEXT_EXPORT              [[gnu::visibility("protected")]]
#  else
#    define __SYSTEM_ABI_EXECCONTEXT_EXPORT              __attribute__((__visibility__("protected")))
#  endif
#  define __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD       .protected
#elif defined(System_ABI_ExecContext_BUILD)
   // Static library; do not export the symbols.
#  if defined(__cplusplus)
#    define __SYSTEM_ABI_EXECCONTEXT_EXPORT              [[gnu::visibility("hidden")]]
#  else
#    define __SYSTEM_ABI_EXECCONTEXT_EXPORT              __attribute__((__visibility__("hidden")))
#  endif
#  define __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD       .hidden
#else
   // Library client; no visibility defined.
#  define __SYSTEM_ABI_EXECCONTEXT_EXPORT                /* nothing */
#endif


// Linker symbol declaration.
#define __SYSTEM_ABI_EXECCONTEXT_SYMBOL(name)            __SYSTEM_ABI_EXECCONTEXT_SYMBOL_(name)
#define __SYSTEM_ABI_EXECCONTEXT_SYMBOL_(name)           __asm__("_$System$ABI$ExecContext$" #name)


// Symbol versioning maps. Only needed in implementation code.
#ifdef __cplusplus
// Attributes we declare:
//   visibility=hidden -- we're not exporting the implementation symbol directly
//   no_reorder        -- the ASM statements in the SYMVER macro must not be re-ordered before the function definition
#  define __SYSTEM_ABI_EXECCONTEXT_VERSIONED_SYMBOL \
     extern "C" [[gnu::visibility("hidden")]] [[gnu::no_reorder]]

// This macro does the following things:
//  - re-defines the implementation symbol as local even though we said `extern "C"` (which we needed for a non-mangled name)
//  - declares a named-version symbol as an alias of the implementation symbol with proper linkage, visibility and namespacing
//  - defines a version that links the named-version symbol to its version and plain symbol
#  define __SYSTEM_ABI_EXECCONTEXT_SYMVER(internal, external, ver)    __SYSTEM_ABI_EXECCONTEXT_SYMVER_(internal, external, ver, __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD)
#  define __SYSTEM_ABI_EXECCONTEXT_SYMVER_(internal, external, ver, vis) __SYSTEM_ABI_EXECCONTEXT_SYMVER__(internal, external, ver, vis)
#  define __SYSTEM_ABI_EXECCONTEXT_SYMVER__(internal, external, ver, vis)   \
     __asm__(".local  " #internal); \
     __asm__(".set    _$System$ABI$ExecContext$" #internal ", " #internal); \
     __asm__(".global _$System$ABI$ExecContext$" #internal); \
     __asm__(#vis   " _$System$ABI$ExecContext$" #internal); \
     __asm__(".symver _$System$ABI$ExecContext$" #internal ", _$System$ABI$ExecContext$" #external "@SYSTEM_ABI_EXECCONTEXT_" #ver); \
     static_assert(is_same_v<decltype(internal), decltype(external)>)

// Like __SYSTEM_ABI_EXECCONTEXT_SYMVER but also marks this as being the default implementation when no particular
// version of the symbol is requested.
#  define __SYSTEM_ABI_EXECCONTEXT_SYMVER_LATEST(internal, external, ver)    __SYSTEM_ABI_EXECCONTEXT_SYMVER_LATEST_(internal, external, ver, __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD)
#  define __SYSTEM_ABI_EXECCONTEXT_SYMVER_LATEST_(internal, external, ver, vis) __SYSTEM_ABI_EXECCONTEXT_SYMVER_LATEST__(internal, external, ver, vis)
#  define __SYSTEM_ABI_EXECCONTEXT_SYMVER_LATEST__(internal, external, ver, vis)   \
     __asm__(".local  " #internal); \
     __asm__(".set    _$System$ABI$ExecContext$" #internal ", " #internal); \
     __asm__(".global _$System$ABI$ExecContext$" #internal); \
     __asm__(#vis   " _$System$ABI$ExecContext$" #internal); \
     __asm__(".symver _$System$ABI$ExecContext$" #internal ", _$System$ABI$ExecContext$" #external "@@SYSTEM_ABI_EXECCONTEXT_" #ver); \
     static_assert(is_same_v<decltype(internal), decltype(external)>)
#endif


// ASM versioned symbol declaration.
#define __SYSTEM_ABI_EXECCONTEXT_ASM_FN_LATEST(internal,external,ver) \
   .global _$System$ABI$ExecContext$##internal; \
   __SYSTEM_ABI_EXECCONTEXT_EXPORT_ASM_WORD _$System$ABI$ExecContext$##internal; \
   .type   _$System$ABI$ExecContext$##internal, STT_FUNC; \
   .size   .L##internal, .L##internal##_end - .L##internal; \
   .equ    _$System$ABI$ExecContext$##internal, .L##internal; \
   .symver _$System$ABI$ExecContext$##internal, _$System$ABI$ExecContext$##external##@@SYSTEM_ABI_EXECCONTEXT_##ver;


#endif /* ifndef __SYSTEM_ABI_EXECCONTEXT_H */