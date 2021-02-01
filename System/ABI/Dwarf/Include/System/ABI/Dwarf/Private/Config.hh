#pragma once
#ifndef __SYSTEM_ABI_DWARF_PRIVATE_CONFIG_H
#define __SYSTEM_ABI_DWARF_PRIVATE_CONFIG_H


#if __SYSTEM_ABI_DWARF_BUILD_SHARED
#  define __SYSTEM_ABI_DWARF_EXPORT     [[gnu::visibility("protected")]]
#else
#  define __SYSTEM_ABI_DWARF_EXPORT     /**/
#endif

#define __SYSTEM_ABI_DWARF_SYMBOL_(x)   asm("System.ABI.Dwarf." #x)
#define __SYSTEM_ABI_DWARF_SYMBOL(x)    __SYSTEM_ABI_DWARF_SYMBOL_(x)


#endif /* ifndef __SYSTEM_ABI_CXX_PRIVATE_CONFIG_H */
