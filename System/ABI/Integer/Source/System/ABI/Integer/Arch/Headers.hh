#pragma once
#ifndef __SYSTEM_ABI_INTEGER_ARCH_HEADERS_H
#define __SYSTEM_ABI_INTEGER_ARCH_HEADERS_H


#if defined(__x86_64__) || defined(__i386__)
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_BITOPS         <System/ABI/Integer/Arch/x86/BitOps.hh>
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_CAPABILITIES   <System/ABI/Integer/Arch/x86/Capabilities.hh>
#elif defined(__arm__)
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_BITOPS         <System/ABI/Integer/Arch/arm/BitOps.hh>
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_CAPABILITIES   <System/ABI/Integer/Arch/arm/Capabilities.hh>
#elif defined(__mips__)
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_BITOPS          <System/ABI/Integer/Arch/mips/BitOps.hh>
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_CAPABILITIES    <System/ABI/Integer/Arch/mips/Capabilities.hh>
#elif defined(_ARCH_PPC)
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_BITOPS          <System/ABI/Integer/Arch/powerpc/BitOps.hh>
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_CAPABILITIES    <System/ABI/Integer/Arch/powerpc/Capabilities.hh>
#elif defined(__sparc__)
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_BITOPS          <System/ABI/Integer/Arch/sparc/BitOps.hh>
#  define __SYSTEM_ABI_INTEGER_ARCH_HDR_CAPABILITIES    <System/ABI/Integer/Arch/sparc/Capabilities.hh>
#else
#  error Unknown architecture
#endif


#endif /* ifndef __SYSTEM_ABI_INTEGER_ARCH_HEADERS_H */
