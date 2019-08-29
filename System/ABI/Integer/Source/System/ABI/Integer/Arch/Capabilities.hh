#pragma once
#ifndef __SYSTEM_ABI_INTEGER_ARCH_CAPABILITIES_H
#define __SYSTEM_ABI_INTEGER_ARCH_CAPABILITIES_H


#include <System/ABI/Integer/Arch/Headers.hh>
#include __SYSTEM_ABI_INTEGER_ARCH_HDR_CAPABILITIES


#define __INTEGER_SHIFT_NONE        0
#define __INTEGER_SHIFT_32          1
#define __INTEGER_SHIFT_64          2
#define __INTEGER_SHIFT_128         3

#define __INTEGER_MUL_NONE          0
#define __INTEGER_MUL_32            1
#define __INTEGER_MUL_32_64         2
#define __INTEGER_MUL_64            3
#define __INTEGER_MUL_64_128        4
#define __INTEGER_MUL_128           5

#define __INTEGER_DIV_NONE          0
#define __INTEGER_DIV_32            1
#define __INTEGER_DIV_64_32         2
#define __INTEGER_DIV_64            3
#define __INTEGER_DIV_128_64        4
#define __INTEGER_DIV_128           5


#endif /* ifndef __SYSTEM_ABI_INTEGER_ARCH_CAPABILITIES_H */
