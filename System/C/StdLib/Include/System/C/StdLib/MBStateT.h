#ifndef __SYSTEM_C_STDLIB_MBSTATET_H
#define __SYSTEM_C_STDLIB_MBSTATET_H


#include <System/C/StdLib/stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif


struct mbstate_t
{
    uint16_t __state;
    uint8_t  __counter;
    uint8_t  __buffer[5];
};


#ifdef __cplusplus
} // extern "C"
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_MBSTATET_H */
