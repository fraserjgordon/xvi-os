#pragma once
#ifndef __SYSTEM_ABI_CXX_UNWINDER_H
#define __SYSTEM_ABI_CXX_UNWINDER_H


#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>

#include <System/ABI/C++/Private/Config.hh>


extern "C"
{


enum _Unwind_Reason_Code
{
    _URC_NO_REASON                  = 0,                // Missing from ARM EHABI.
    _URC_OK                         = _URC_NO_REASON,   // ARM EHABI addition.
    _URC_FOREIGN_EXCEPTION_CAUGHT   = 1,
    _URC_FATAL_PHASE2_ERROR         = 2,                // Missing from ARM EHABI.
    _URC_FATAL_PHASE1_ERROR         = 3,                // Missing from ARM EHABI.
    _URC_NORMAL_STOP                = 4,                // Missing from ARM EHABI.
    _URC_END_OF_STACK               = 5,                // Missing from ARM EHABI.
    _URC_HANDLER_FOUND              = 6,
    _URC_INSTALL_CONTEXT            = 7,
    _URC_CONTINUE_UNWIND            = 8,
    _URC_FAILURE                    = 9,                // ARM EHABI addition.
};

#if defined(__SYSTEM_ABI_CXX_AEABI)
using _Unwind_State = std::uint32_t;

inline constexpr _Unwind_State _US_VIRTUAL_UNWIND_FRAME     = 0;
inline constexpr _Unwind_State _US_UNWIND_FRAME_STARTING    = 1;
inline constexpr _Unwind_State _US_UNWIND_FRAME_RESUME      = 2;
inline constexpr _Unwind_State _US_FORCE_UNWIND             = 8;    // Flag - XVI extension.

using _Unwind_EHT_Header = std::uint32_t;
using _Unwind_Exception_Cleanup_Fn  = void (*)(_Unwind_Reason_Code, struct _Unwind_Control_Block*);

struct _Unwind_Context;


// GCC extension.
using _Unwind_Trace_Fn              = _Unwind_Reason_Code (*)(_Unwind_Context*, void*);

// XVI extension (based on the SysV ABI).
using _Unwind_Stop_Fn               = _Unwind_Reason_Code (*)(int, _Unwind_State, std::uint64_t, struct _Unwind_Control_Block*, struct _Unwind_Context*, void*);

struct alignas(8) _Unwind_Control_Block
{
    union
    {
        char                        exception_class[8];
        std::uint64_t               __exception_class = 0;
    };

    _Unwind_Exception_Cleanup_Fn    exception_cleanup = nullptr;

    // Unwinder cache.
    struct
    {
        std::uint32_t   reserved1   = 0;
        std::uint32_t   reserved2   = 0;
        std::uint32_t   reserved3   = 0;
        std::uint32_t   reserved4   = 0;
        std::uint32_t   reserved5   = 0;
    } unwinder_cache;

    // The "propogation barrier" is the frame that handles the exception.
    struct
    {
        std::uint32_t   sp = 0;
        std::uint32_t   bitpattern[5] = {};
    } barrier_cache;

    struct
    {
        std::uint32_t   bitpattern[4] = {};
    } cleanup_cache;

    // Personality routine cache.
    struct
    {
        std::uint32_t       fnstart = 0;
        _Unwind_EHT_Header* ehtp = nullptr;
        std::uint32_t       additional = 0;
        std::uint32_t       reserved = 0;
    } pr_cache;
};

enum _Unwind_VRS_RegClass
{
    _UVRSC_CORE     = 0,
    _UVRSC_VFP      = 1,
    _UVRSC_WMMXD    = 3,
    _UVRSC_WMMXC    = 4,
};

enum _Unwind_VRS_DataRepresentation
{
    _UVRSD_UINT32   = 0,
    _UVRSD_VFPX     = 1,
    _UVRSD_UINT64   = 3,
    _UVRSD_FLOAT    = 4,
    _UVRSD_DOUBLE   = 5,
};

enum _Unwind_VRS_Result
{
    _UVRSR_OK               = 0,
    _UVRSR_NOT_IMPLEMENTED  = 1,
    _UVRSR_FAILED           = 2,
};


__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_Reason_Code _Unwind_RaiseException(_Unwind_Control_Block*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void _Unwind_Resume(_Unwind_Control_Block*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void _Unwind_Complete(_Unwind_Control_Block*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void _Unwind_DeleteException(_Unwind_Control_Block*);

__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_VRS_Result _Unwind_VRS_Set(_Unwind_Context*, _Unwind_VRS_RegClass, std::uint32_t, _Unwind_VRS_DataRepresentation, void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_VRS_Result _Unwind_VRS_Get(_Unwind_Context*, _Unwind_VRS_RegClass, std::uint32_t, _Unwind_VRS_DataRepresentation, void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_VRS_Result _Unwind_VRS_Pop(_Unwind_Context*, _Unwind_VRS_RegClass, std::uint32_t, _Unwind_VRS_DataRepresentation);

// GCC extension.
__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn, void*);

// XVI extensions (based on the SysV ABI).
__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_Reason_Code _Unwind_ForcedUnwind(_Unwind_Control_Block*, _Unwind_Stop_Fn, void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetGR(_Unwind_Context*, int);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                _Unwind_SetGR(_Unwind_Context*, int, std::uintptr_t);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetIP(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                _Unwind_SetIP(_Unwind_Context*, std::uintptr_t);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetLanguageSpecificData(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetRegionStart(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetDataRelBase(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetTextRelBase(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void*               _Unwind_FindEnclosingFunction(void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetCFA(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetIPInfo(_Unwind_Context*, int*);



#else // if defined(__SYSTEM_ABI_CXX_AEABI)

using _Unwind_Action = int;
inline constexpr _Unwind_Action _UA_SEARCH_PHASE       = 0x00000001;
inline constexpr _Unwind_Action _UA_CLEANUP_PHASE      = 0x00000002;
inline constexpr _Unwind_Action _UA_HANDLER_FRAME      = 0x00000004;
inline constexpr _Unwind_Action _UA_FORCE_UNWIND       = 0x00000008;

using _Unwind_Exception_Cleanup_Fn  = void (*)(_Unwind_Reason_Code, struct _Unwind_Exception*);
using _Unwind_Stop_Fn               = _Unwind_Reason_Code (*)(int, _Unwind_Action, std::uint64_t, struct _Unwind_Exception*, struct _Unwind_Context*, void*);

// GCC extension.
using _Unwind_Trace_Fn              = _Unwind_Reason_Code (*)(_Unwind_Context*, void*);

// The Itanium ABI specfies this signature but not the name for a type representing it.
//
//! @brief      Function signature for personality routines.
//!
//! @returns    A status code indicating on how the personality routine would
//!             like to handle this exception.
//!
//! @param  version     The version of the ABI that the calling unwinder
//!                     conforms to (currently always @c 1).
//! @param  action      A bitmask of action flags indicating the type of unwind
//!                     and current unwind phase.
//! @param  exception_class An identifier for the runtime and language that
//!                         raised this exception.
//! @param  exception   The exception object.
//! @param  context     The current unwind context. The language-specific data
//!                     for the current frame can be retrieved from it.
//!
using _Unwind_Personality_Fn        = _Unwind_Reason_Code (*)(int version, _Unwind_Action action, std::uint64_t exception_class, _Unwind_Exception* exception, _Unwind_Context* context);

struct _Unwind_Exception
{
    std::uint64_t                   exception_class     = 0;
    _Unwind_Exception_Cleanup_Fn    exception_cleanup   = nullptr;
    std::uint64_t                   private1            = 0;
    std::uint64_t                   private2            = 0;
};

struct _Unwind_Context;


__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_Reason_Code _Unwind_RaiseException(_Unwind_Exception*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_Reason_Code _Unwind_ForcedUnwind(_Unwind_Exception*, _Unwind_Stop_Fn, void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT [[noreturn]] void   _Unwind_Resume(_Unwind_Exception*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                _Unwind_DeleteException(_Unwind_Exception*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetGR(_Unwind_Context*, int);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                _Unwind_SetGR(_Unwind_Context*, int, std::uintptr_t);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetIP(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                _Unwind_SetIP(_Unwind_Context*, std::uintptr_t);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetLanguageSpecificData(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetRegionStart(_Unwind_Context*);

// GCC extensions.
__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_Reason_Code _Unwind_Resume_or_Rethrow(_Unwind_Exception*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetDataRelBase(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetTextRelBase(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void*               _Unwind_FindEnclosingFunction(void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT _Unwind_Reason_Code _Unwind_Backtrace(_Unwind_Trace_Fn, void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT const void*         _Unwind_Find_FDE(const void*, struct dwarf_eh_bases*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetCFA(_Unwind_Context*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT std::uintptr_t      _Unwind_GetIPInfo(_Unwind_Context*, int*);

// GCC extensions - should be moved to DWARF unwinder.
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                __register_frame(const void*);
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                __deregister_frame(const void*);

// XVI extensions.
__SYSTEM_ABI_CXX_UNWIND_EXPORT void                 _Unwind_XVI_RegisterEhFrameHdr(const std::byte*, std::uintptr_t text, std::uintptr_t data);

#endif // if defined(__SYSTEM_ABI_CXX_AEABI)


} // extern "C"


#endif /* ifndef __SYSTEM_ABI_CXX_UNWINDER_H */
