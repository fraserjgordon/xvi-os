#ifndef __SYSTEM_C_LANGUAGESUPPORT_STDDEF_H
#define __SYSTEM_C_LANGUAGESUPPORT_STDDEF_H


#include <System/C/BaseHeaders/Null.h>
#include <System/C/BaseHeaders/OffsetOf.h>
#include <System/C/BaseHeaders/Types.h>


typedef __ptrdiff_t     ptrdiff_t;
typedef __size_t        size_t;
typedef __max_align_t   max_align_t;

#ifndef __cplusplus
typedef  __wchar_t       wchar_t;
#endif


#endif /* ifndef __SYSTEM_C_LANGUAGESUPPORT_STDDEF_H */
