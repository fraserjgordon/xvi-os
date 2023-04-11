#ifndef __SYSTEM_CXX_ALLOCATOR_PRIVATE_CONFIG_H
#define __SYSTEM_CXX_ALLOCATOR_PRIVATE_CONFIG_H


#if defined(__XVI_HOSTED) && !defined(__XVI_STD_NS)
#  define __XVI_STD_NS _xvi_std
#endif

#if !defined(__XVI_STD_STRING_NS)
#  if defined (__XVI_STD_NS)
#   define __XVI_STD_ALLOCATOR_NS       __XVI_STD_NS
#  else
#    define __XVI_STD_NS                std
#    define __XVI_STD_ALLOCATOR_NS      __XVI_STD_NS
#  endif
#endif

#define __XVI_STD_ALLOCATOR_NS_CXX_VER  __cxx23
#define __XVI_STD_ALLOCATOR_ABI_TAG     [[gnu::abi_tag("cxx23")]]
#define __XVI_STD_ALLOCATOR_NS_DECL     __XVI_STD_ALLOCATOR_NS::inline __XVI_STD_ALLOCATOR_NS_CXX_VER


namespace __XVI_STD_ALLOCATOR_NS
{

inline namespace __XVI_STD_ALLOCATOR_ABI_TAG __XVI_STD_ALLOCATOR_NS_CXX_VER {}

}


#if __SYSTEM_CXX_ALLOCATOR_BUILD_SHARED
#  define __SYSTEM_CXX_ALLOCATOR_EXPORT                     [[gnu::visibility("protected")]]
#  define __SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE         [[gnu::weak, gnu::visibility("default")]]
#else
#  define __SYSTEM_CXX_ALLOCATOR_EXPORT                     /**/
#  define __SYSTEM_CXX_ALLOCATOR_EXPORT_REPLACEABLE         [[gnu::weak]]
#endif


#endif /* ifndef __SYSTEM_CXX_ALLOCATOR_PRIVATE_CONFIG_H */
