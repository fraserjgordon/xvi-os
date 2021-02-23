#ifndef __SYSTEM_C_STDLIB_STDINT_H
#define __SYSTEM_C_STDLIB_STDINT_H


#include <System/C/BaseHeaders/Limits.h>
#include <System/C/LanguageSupport/StdInt.h>

#include <System/C/StdLib/Private/AnnexK.h>


#define SIG_ATOMIC_WIDTH    __SIG_ATOMIC_WIDTH


#if __SYSTEM_C_STDLIB_ANNEXK_ENABLED
#  define RSIZE_MAX     (__SIZE_MAX >> 1)
#endif


#endif /* ifndef __SYSTEM_C_STDLIB_STDINT_H */
