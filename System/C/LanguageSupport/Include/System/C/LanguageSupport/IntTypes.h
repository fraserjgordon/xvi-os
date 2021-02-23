#ifndef __SYSTEM_C_LANGUAGESUPPORT_INTTYPES_H
#define __SYSTEM_C_LANGUAGESUPPORT_INTTYPES_H


#include <System/C/LanguageSupport/StdInt.h>


#define PRId8    __INT8_FMTd
#define PRIi8    __INT8_FMTi
#define PRIo8    __UINT8_FMTo
#define PRIu8    __UINT8_FMTu
#define PRIx8    __UINT8_FMTx
#define PRIX8    __UINT8_FMTX
#define PRId16   __INT16_FMTd
#define PRIi16   __INT16_FMTi
#define PRIo16   __UINT16_FMTo
#define PRIu16   __UINT16_FMTu
#define PRIx16   __UINT16_FMTx
#define PRIX16   __UINT16_FMTX
#define PRId32   __INT32_FMTd
#define PRIi32   __INT32_FMTi
#define PRIo32   __UINT32_FMTo
#define PRIu32   __UINT32_FMTu
#define PRIx32   __UINT32_FMTx
#define PRIX32   __UINT32_FMTX
#define PRId64   __INT64_FMTd
#define PRIi64   __INT64_FMTi
#define PRIo64   __UINT64_FMTo
#define PRIu64   __UINT64_FMTu
#define PRIx64   __UINT64_FMTx
#define PRIX64   __UINT64_FMTX

#define PRIdLEAST8   _ _INT8_FMTd
#define PRIiLEAST8    __INT8_FMTi
#define PRIoLEAST8    __UINT8_FMTo
#define PRIuLEAST8    __UINT8_FMTu
#define PRIxLEAST8    __UINT8_FMTx
#define PRIXLEAST8    __UINT8_FMTX
#define PRIdLEAST16   __INT16_FMTd
#define PRIiLEAST16   __INT16_FMTi
#define PRIoLEAST16   __UINT16_FMTo
#define PRIuLEAST16   __UINT16_FMTu
#define PRIxLEAST16   __UINT16_FMTx
#define PRIXLEAST16   __UINT16_FMTX
#define PRIdLEAST32   __INT32_FMTd
#define PRIiLEAST32   __INT32_FMTi
#define PRIoLEAST32   __UINT32_FMTo
#define PRIuLEAST32   __UINT32_FMTu
#define PRIxLEAST32   __UINT32_FMTx
#define PRIXLEAST32   __UINT32_FMTX
#define PRIdLEAST64   __INT64_FMTd
#define PRIiLEAST64   __INT64_FMTi
#define PRIoLEAST64   __UINT64_FMTo
#define PRIuLEAST64   __UINT64_FMTu
#define PRIxLEAST64   __UINT64_FMTx
#define PRIXLEAST64   __UINT64_FMTX

#if __UINT32_MAX >= __UINTPTR_MAX
#  define PRIdPTR     __INT32_FMTd
#  define PRIiPTR     __INT32_FMTi
#  define PRIoPTR     __UINT32_FMTo
#  define PRIuPTR     __UINT32_FMTu
#  define PRIxPTR     __UINT32_FMTx
#  define PRIXPTR     __UINT32_FMTX
#else
#  define PRIdPTR     __INT64_FMTd
#  define PRIiPTR     __INT64_FMTi
#  define PRIoPTR     __UINT64_FMTo
#  define PRIuPTR     __UINT64_FMTu
#  define PRIxPTR     __UINT64_FMTx
#  define PRIXPTR     __UINT64_FMTX
#endif

#define PRIdMAX     "lld"
#define PRIiMAX     "lli"
#define PRIoMAX     "llo"
#define PRIuMAX     "llu"
#define PRIxMAX     "llx"
#define PRIXMAX     "llX"


#define SCNd8    __INT8_FMTd
#define SCNi8    __INT8_FMTi
#define SCNo8    __UINT8_FMTo
#define SCNu8    __UINT8_FMTu
#define SCNx8    __UINT8_FMTx
#define SCNd16   __INT16_FMTd
#define SCNi16   __INT16_FMTi
#define SCNo16   __UINT16_FMTo
#define SCNu16   __UINT16_FMTu
#define SCNx16   __UINT16_FMTx
#define SCNd32   __INT32_FMTd
#define SCNi32   __INT32_FMTi
#define SCNo32   __UINT32_FMTo
#define SCNu32   __UINT32_FMTu
#define SCNx32   __UINT32_FMTx
#define SCNd64   __INT64_FMTd
#define SCNi64   __INT64_FMTi
#define SCNo64   __UINT64_FMTo
#define SCNu64   __UINT64_FMTu
#define SCNx64   __UINT64_FMTx

#define SCNdLEAST8   _ _INT8_FMTd
#define SCNiLEAST8    __INT8_FMTi
#define SCNoLEAST8    __UINT8_FMTo
#define SCNuLEAST8    __UINT8_FMTu
#define SCNxLEAST8    __UINT8_FMTx
#define SCNdLEAST16   __INT16_FMTd
#define SCNiLEAST16   __INT16_FMTi
#define SCNoLEAST16   __UINT16_FMTo
#define SCNuLEAST16   __UINT16_FMTu
#define SCNxLEAST16   __UINT16_FMTx
#define SCNdLEAST32   __INT32_FMTd
#define SCNiLEAST32   __INT32_FMTi
#define SCNoLEAST32   __UINT32_FMTo
#define SCNuLEAST32   __UINT32_FMTu
#define SCNxLEAST32   __UINT32_FMTx
#define SCNdLEAST64   __INT64_FMTd
#define SCNiLEAST64   __INT64_FMTi
#define SCNoLEAST64   __UINT64_FMTo
#define SCNuLEAST64   __UINT64_FMTu
#define SCNxLEAST64   __UINT64_FMTx

#if __UINT32_MAX >= __UINTPTR_MAX
#  define SCNdPTR     __INT32_FMTd
#  define SCNiPTR     __INT32_FMTi
#  define SCNoPTR     __UINT32_FMTo
#  define SCNuPTR     __UINT32_FMTu
#  define SCNxPTR     __UINT32_FMTx
#else
#  define SCNdPTR     __INT64_FMTd
#  define SCNiPTR     __INT64_FMTi
#  define SCNoPTR     __UINT64_FMTo
#  define SCNuPTR     __UINT64_FMTu
#  define SCNxPTR     __UINT64_FMTx
#endif

#define SCNdMAX     "lld"
#define SCNiMAX     "lli"
#define SCNoMAX     "llo"
#define SCNuMAX     "llu"
#define SCNxMAX     "llx"


//! @todo PRI?FAST? / SCN?FAST?


typedef struct imaxdiv_t
{
    intmax_t quot;
    intmax_t rem;
} imaxdiv_t;


#endif /* ifndef __SYSTEM_C_LANGUAGESUPPORT_INTTYPES_H */
