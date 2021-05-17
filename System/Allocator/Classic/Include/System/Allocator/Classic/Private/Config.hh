#ifndef __SYSTEM_ALLOCATOR_CLASSIC_PRIVATE_CONFIG_H
#define __SYSTEM_ALLOCATOR_CLASSIC_PRIVATE_CONFIG_H


#if defined(__SYSTEM_ALLOCATOR_CLASSIC_BUILD_SHARED)
#  define __XVI_ALLOCATOR_CLASSIC_EXPORT            [[gnu::visibility("protected")]]
#else
#  define __XVI_ALLOCATOR_CLASSIC_EXPORT            /**/
#endif

#define __XVI_ALLOCATOR_CLASSIC_SYMBOL(x)           asm("System.Allocator.Classic." #x)



#endif /* ifndef __SYSTEM_ALLOCATOR_CLASSIC_PRIVATE_CONFIG_H */
