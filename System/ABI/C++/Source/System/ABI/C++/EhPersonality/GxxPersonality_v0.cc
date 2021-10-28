#include <System/ABI/C++/Private/Config.hh>

#include <System/ABI/C++/Exception.hh>
#include <System/ABI/C++/TypeInfo.hh>
#include <System/ABI/C++/Unwinder.hh>
#include <System/ABI/Dwarf/Decode.hh>
#include <System/ABI/Dwarf/LEB128.hh>
#include <System/C++/Utility/Optional.hh>

#include <System/ABI/C++/EhPersonality/Utils.hh>


using namespace __cxxabiv1;
using namespace System::ABI::CXX;

namespace Dwarf = System::ABI::Dwarf;


// This is the function that we need to export as the compiler automatically generates references to it.
#ifdef __SYSTEM_ABI_CXX_AEABI
extern "C" __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT _Unwind_Reason_Code __gxx_personality_v0(_Unwind_State, _Unwind_Control_Block*, _Unwind_Context*);
#else
extern "C" __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT _Unwind_Reason_Code __gxx_personality_v0(int, _Unwind_Action, std::uint64_t, _Unwind_Exception*, _Unwind_Context*);
#endif


// Holds the information parsed from the header of the LSDA.
struct lsda_header_info
{
    std::uintptr_t      text_rel_base       = 0;        // Base address to use for text-relative values.
    std::uintptr_t      data_rel_base       = 0;        // Base address to use for data-relative values.
    std::uintptr_t      lsda_rel_base       = 0;        // Base address to use for LSDA-relative values.
    const std::byte*    typetable_ptr       = nullptr;  // Pointer to the type table.
    const std::byte*    calltable_ptr       = nullptr;  // Pointer to the call site table.
    std::size_t         calltable_size      = 0;        // Size (in bytes) of the call site table.
    std::uint8_t        typetable_encoding = Dwarf::kDwarfPtrOmit;  // Encoding of entries in the type table.
    std::uint8_t        calltable_encoding = Dwarf::kDwarfPtrOmit;  // Encoding of entries in the call site table.
};

// Holds the information parsed from the call-site table.
struct callsite_info
{
    std::uintptr_t      range_start     = 0;    // Start of code range covered by this entry.
    std::size_t         range_length    = 0;    // Length of code covered by this entry.
    std::uintptr_t      landingpad      = 0;    // Address of landingpad code for this call site.
    std::size_t         action_id       = 0;    // Offset into action table (plus 1) or zero for no action.
};

// Holds the information parsed from an action table entry.
struct action_info
{
    std::ptrdiff_t      type_filter = 0;        // Zero is cleanup, positive are handlers, negative are specifications.
    const std::byte*    next        = nullptr;  // Next action in the chain to check.  
};

// Information needed when handling an exception.
struct handler_info
{
    std::ptrdiff_t      switch_value    = 0;
    const char*         action_record   = nullptr;
    void*               adjusted_ptr    = nullptr;
};


// Parses the LSDA header to locate the various data tables.
static lsda_header_info parseLSDAHeader(_Unwind_Context* context)
{
    // Get the address of the LSDA as we need to parse it.
    auto lsda_address = _Unwind_GetLanguageSpecificData(context);
    auto lsda = reinterpret_cast<const std::byte*>(lsda_address);
    auto ptr = lsda;

    // If there is no associated LSDA, we don't need to do anything.
    if (!lsda)
        return {};

    // The LSDA contains various pointers encoded using the DWARF encoding scheme so set up a decoder.
    auto text_rel_base = _Unwind_GetTextRelBase(context);
    auto data_rel_base = _Unwind_GetDataRelBase(context);
    auto func_rel_base = _Unwind_GetRegionStart(context);
    Dwarf::DwarfPointerDecoderNative decoder = {{.text = text_rel_base, .data = data_rel_base, .func = func_rel_base}};

    // The first byte of the LSDA is the DWARF encoding of the base address to use for landing pad offsets. It is
    // followed by the encoded base address.
    auto lsda_rel_base = func_rel_base;
    auto lp_base_encoding = static_cast<std::uint8_t>(*ptr++);
    if (lp_base_encoding != Dwarf::kDwarfPtrOmit)
    {
        lsda_rel_base = decoder.decode(lp_base_encoding, ptr);
        decoder = {{.text = text_rel_base, .data = data_rel_base, .func = lsda_rel_base}};
    }

    // The next byte is the encoding for the type table entries.
    auto typetable_encoding = static_cast<std::uint8_t>(*ptr++);

    // If there is a type table, it's ULEB128 encoded self-relative offset comes next.
    const std::byte* typetable_ptr = nullptr;
    if (typetable_encoding != Dwarf::kDwarfPtrOmit)
    {
        // Note that this is relative to the address *after* the encoded offset.
        auto typetable_offset = Dwarf::DecodeULEB128(ptr);
        typetable_ptr = ptr + typetable_offset;
    }

    // Next, the encoding of entries in the call site table.
    auto calltable_encoding = static_cast<std::uint8_t>(*ptr++);

    // And finally the encoded length of the call site table.
    auto calltable_size = Dwarf::DecodeULEB128(ptr);

    // The after the LSDA is the callsite table itself.
    auto calltable_ptr = ptr;

    return
    {
        .text_rel_base = text_rel_base,
        .data_rel_base = data_rel_base,
        .lsda_rel_base = lsda_rel_base,
        .typetable_ptr = typetable_ptr,
        .calltable_ptr = calltable_ptr,
        .calltable_size = calltable_size,
        .typetable_encoding = typetable_encoding,
        .calltable_encoding = calltable_encoding,
    };
}

// Parses a single entry in the call site table.
static callsite_info parseCallSiteInfo(const lsda_header_info& info, const std::byte*& ptr)
{
    // Create a pointer decoder. Relative addresses are not supported in this table.
    Dwarf::DwarfPointerDecoderNative decoder = {{}};

    // Decode the start of the covered code. It is relative to the LSDA reference address.
    auto region_start = info.lsda_rel_base + decoder.decode(info.calltable_encoding, ptr);

    // Decode the number of bytes of code that is covered by this entry.
    auto region_length = decoder.decode(info.calltable_encoding, ptr);

    // Decode the landingpad offset. If non-zero, it is relative to the LSDA reference address.
    auto landingpad = decoder.decode(info.calltable_encoding, ptr);
    if (landingpad != 0)
        landingpad += info.lsda_rel_base;

    // Finally, the ID of the action to take.
    auto action = Dwarf::DecodeULEB128(ptr);

    return
    {
        .range_start = region_start,
        .range_length = region_length,
        .landingpad = landingpad,
        .action_id = action,
    };
}

// Finds the call site info (if any) for the given PC.
static callsite_info findCallSiteInfo(const lsda_header_info& info, std::uintptr_t pc)
{
    // Parse the call site info entries until we pass PC or reach the end of the table.
    auto calltable_start = info.calltable_ptr;
    auto calltable_ptr = calltable_start;
    while ((calltable_ptr - calltable_start) < info.calltable_size)
    {
        // Parse the current entry.
        auto csinfo = parseCallSiteInfo(info, calltable_ptr);

        // Does this entry cover the desired PC?
        if (csinfo.range_start <= pc && pc < (csinfo.range_start + csinfo.range_length))
            return csinfo;

        // The table is ordered so if we've gone past the current PC then we can stop looking.
        if (csinfo.range_start > pc)
            break;
    }

    // Failed to find any matching call site info.
    return {};
}

// Returns a pointer to the given action table entry.
static const std::byte* getActionTableEntry(const lsda_header_info& info, std::size_t offset)
{
    // The action table follows the call site table in the LSDA.
    return info.calltable_ptr + info.calltable_size + offset;
}

// Parses an entry in the action table.
static action_info parseActionTableEntry(const std::byte* ptr)
{
    // The action table is a pair of SLEB128 values: the type filter ID and the offset to the next action.
    auto type_filter = Dwarf::DecodeSLEB128(ptr);
    auto base = ptr;
    auto offset = Dwarf::DecodeSLEB128(ptr);

    const std::byte* next = nullptr;
    if (offset != 0)
        next = base + offset;

    return
    {
        .type_filter = type_filter,
        .next = next,
    };
}

// Returns the type info for a positive type_filter value.
static const std::type_info* getCatchTypeInfo(const lsda_header_info& info, std::ptrdiff_t index)
{
    // The LSDA header tells us now big the table entries are.
    using dw_ptr_type = Dwarf::dw_ptr_type;
    std::size_t entry_size = 0;
    switch (static_cast<Dwarf::dw_ptr_type>(info.typetable_encoding & Dwarf::kDwarfPtrTypeMask))
    {
        case dw_ptr_type::sdata2:
        case dw_ptr_type::udata2:
            entry_size = 2;
            break;

        case dw_ptr_type::sdata4:
        case dw_ptr_type::udata4:
            entry_size = 4;
            break;

        case dw_ptr_type::sdata8:
        case dw_ptr_type::udata8:
            entry_size = 8;
            break;

        default:
            // Not a fixed-size type; not valid for the type table.
            return nullptr;
    }

    // Decode the entry. Catch entries are at negative offsets from the type table base address.
    Dwarf::DwarfPointerDecoderNative decoder = {{.text = info.text_rel_base, .data = info.data_rel_base, .func = info.lsda_rel_base}};
    auto offset = entry_size * index;
    auto ptr = info.typetable_ptr - offset;
    auto typeinfo_address = decoder.decode(info.typetable_encoding, ptr, true);

    return reinterpret_cast<const std::type_info*>(typeinfo_address);
}

// Finds the first action that matches the given exception type.
static std::optional<handler_info> findMatchingAction(const lsda_header_info& info, std::ptrdiff_t action_id, const std::type_info* exception_type, void* exception_object)
{
    // Get the first action table entry. It's offset is the action ID minus 1.
    auto action_ptr = getActionTableEntry(info, action_id) - 1;
    auto action = parseActionTableEntry(action_ptr);
    while (true)
    {
        // Is this a catch action or an exception specification action?
        if (action.type_filter == 0)
        {
            // This is a cleanup action. It matches regardless of type.
            return handler_info
            {
                .switch_value = 0,
                .action_record = reinterpret_cast<const char*>(action_ptr),
                .adjusted_ptr = exception_object,
            };
        }
        else if (action.type_filter > 0)
        {
            // This is a catch action. Look up the type info for this catch clause.
            auto catch_type = getCatchTypeInfo(info, action.type_filter);
            if (!catch_type)
            {
                // No type info means this is a catch(...) clause.
                return handler_info
                {
                    .switch_value = action.type_filter,
                    .action_record = reinterpret_cast<const char*>(action_ptr),
                    .adjusted_ptr = exception_object,
                };
            }
            else
            {
                // Catching a specific type.

                // Pointer to the caught object.
                void* catch_object = exception_object;
                auto type = getTypeInfoType(*catch_type);

                // For derived-to-base conversions of thrown pointers, we need to adjust the value of the thrown
                // pointer (and not the pointer to the thrown pointer!).
                if (type == TypeInfoType::Pointer)
                    catch_object = *reinterpret_cast<void**>(catch_object);

                // Does this catch clause match the thrown type?
                auto match_result = __cxa_type_match(toExceptionT(exception_object), catch_type, false, &catch_object);
                if (match_result != __ctm_failed)
                {
                    // Adjust the pointer to the exception object. This needs special handling if the exception type is
                    // itself a pointer: we need to adjust the pointer itself, not the pointer to the pointer.
                    auto adjusted_ptr = catch_object;
                    if (type == TypeInfoType::Pointer)
                    {
                        *reinterpret_cast<void**>(exception_object) = catch_object;
                        adjusted_ptr = exception_object;
                    }

                    return handler_info
                    {
                        .switch_value = action.type_filter,
                        .action_record = reinterpret_cast<const char*>(action_ptr),
                        .adjusted_ptr = adjusted_ptr,
                    };
                }
            }
        }
        else if (action.type_filter < 0)
        {
            // This is an exception specification.
            //! @TODO: support exception specifications.
        }

        // Move on to the next action.
        action_ptr = action.next;
        if (action_ptr)
            action = parseActionTableEntry(action_ptr);
        else
            break;
    }

    // Reached the end of the action table without finding a matching action.
    return {};
}


static _Unwind_Reason_Code unwindPhase1(_Unwind_T* exception, _Unwind_Context* context)
{
    // This is the first time we're seeing this exception. We need to consult the LSDA (if it exists) to find out what
    // we're supposed to do with it.
    //
    // If there's no LSDA, we don't need to do anything with the exception.
    auto lsda_address = _Unwind_GetLanguageSpecificData(context);
    auto lsda_ptr = reinterpret_cast<const std::byte*>(lsda_address);
    if (!lsda_ptr)
        return _URC_CONTINUE_UNWIND;

    // Parse the LSDA header so we know how to read the call site table.
    auto lsda_info = parseLSDAHeader(context);

    // Find the address of the faulting call. The PC normally points to the instruction after the faulting one so we
    // need to adjust it backwards in that case.
    int ip_before;
    auto pc = _Unwind_GetIPInfo(context, &ip_before);
    if (!ip_before)
        pc -= 1;

    // Find the information for the call site that triggered this exception.
    auto call_site_info = findCallSiteInfo(lsda_info, pc);

    // If the call site was not found, we cannot deal with this exception.
    if (call_site_info.range_start == 0)
        __cxa_call_terminate(exception);

    // If the call site has no landingpad, we have no actions to perform; the unwind can continue through this function
    // without requiring any cleanup.
    if (call_site_info.landingpad == 0)
        return _URC_CONTINUE_UNWIND;

    // If the action ID is zero, we have cleanup actions but no catch handlers - we will need to do some work during
    // phase 2 but there's nothing we need to do right now.
    if (call_site_info.action_id == 0)
        return _URC_CONTINUE_UNWIND;

    // We need to find which action (if any) matches the exception. To do that, we need to get the exception's type.
    const std::type_info* exception_type;
    bool native_exception = isNativeException(exception);
    if (native_exception)
    {
        // This is a native C++ exception. We can get the type info from the exception's header.
        auto* cxa = getCxaException(exception);
        exception_type = cxa->exceptionType;
    }
    else
    {
        // This is a foreign exception. Use a synthetic type for it so that it might be caught.
        exception_type = &typeid(__foreign_exception);
    }

    // If the type info is invalid, we can't deal with this exception.
    if (!exception_type)
        __cxa_call_terminate(exception);

    // Look up the action that will handle this.
    void* exception_object = exception + 1;
    auto action = findMatchingAction(lsda_info, call_site_info.action_id, exception_type, exception_object);

    // If we found no action (or if the action is cleanup-only), the exception will not be handled here.
    if (!action || action->switch_value == 0)
        return _URC_CONTINUE_UNWIND;

    // We're going to handle this exception. If this is a native exception, cache the information that we looked up in
    // the exception.
    if (native_exception)
    {
        // Note: the accessed fields are at the same location for normal and dependant exceptions.
        auto* cxa = getCxaException(exception);
#ifdef __SYSTEM_ABI_CXX_AEABI
        cxa->ucb.barrier_cache.sp = _Unwind_GetCFA(context);
        cxa->ucb.barrier_cache.bitpattern[0] = action->switch_value;
        cxa->ucb.barrier_cache.bitpattern[1] = reinterpret_cast<std::uint32_t>(action->action_record);
        cxa->ucb.barrier_cache.bitpattern[2] = reinterpret_cast<std::uint32_t>(lsda_ptr);
        cxa->ucb.barrier_cache.bitpattern[3] = call_site_info.landingpad;
        cxa->ucb.barrier_cache.bitpattern[4] = reinterpret_cast<std::uint32_t>(action->adjusted_ptr);
#else
        cxa->handlerSwitchValue = action->switch_value;
        cxa->actionRecord = action->action_record;
        cxa->languageSpecificDataArea = reinterpret_cast<const char*>(lsda_ptr);
        cxa->catchTemp = reinterpret_cast<void*>(call_site_info.landingpad);
        cxa->adjustedPointer = action->adjusted_ptr;
#endif
    }

    return _URC_HANDLER_FOUND;
}

static _Unwind_Reason_Code unwindPhase2Cleanup(_Unwind_T* exception, _Unwind_Context* context)
{
    // We're not the handling frame so all information needs to be looked up.

    // If there's no LSDA, we don't need to do anything with the exception.
    auto lsda_address = _Unwind_GetLanguageSpecificData(context);
    auto lsda_ptr = reinterpret_cast<const std::byte*>(lsda_address);
    if (!lsda_ptr)
        return _URC_CONTINUE_UNWIND;

    // Parse the LSDA header so we know how to read the call site table.
    auto lsda_info = parseLSDAHeader(context);

    // Find the address of the faulting call. The PC normally points to the instruction after the faulting one so we
    // need to adjust it backwards in that case.
    int ip_before;
    auto pc = _Unwind_GetIPInfo(context, &ip_before);
    if (!ip_before)
        pc -= 1;

    // Find the information for the call site that triggered this exception.
    auto call_site_info = findCallSiteInfo(lsda_info, pc);

    // If the call site was not found, we cannot deal with this exception.
    if (call_site_info.range_start == 0)
        __cxa_call_terminate(exception);

    // If the call site has no landingpad, we have no actions to perform; the unwind can continue through this function
    // without requiring any cleanup.
    if (call_site_info.landingpad == 0)
        return _URC_CONTINUE_UNWIND;

    // If the action ID is zero, we have cleanup actions to perform. If non-zero, we might have some to perform but we
    // need to check the action chain.
    if (call_site_info.action_id != 0)
    {
        auto action_offset = call_site_info.action_id - 1;
        auto action_ptr = getActionTableEntry(lsda_info, action_offset);
        auto action = parseActionTableEntry(action_ptr);
        while (true)
        {
            // Is this a cleanup action?
            if (action.type_filter == 0)
                break;

            // Move on to the next action. If there are no more, we have no cleanup to do.
            action_ptr = action.next;
            if (action_ptr)
                action = parseActionTableEntry(action_ptr);
            else
                return _URC_CONTINUE_UNWIND;
        }
    }

    // We found a cleanup action so we need to install the landingpad. The switch value for cleanups is always zero.
    _Unwind_SetGR(context, __builtin_eh_return_data_regno(0), reinterpret_cast<std::uintptr_t>(exception));
    _Unwind_SetGR(context, __builtin_eh_return_data_regno(1), 0);
    _Unwind_SetIP(context, call_site_info.landingpad);
    return _URC_INSTALL_CONTEXT;
}

static _Unwind_Reason_Code unwindPhase2Handler(_Unwind_T* exception, _Unwind_Context* context)
{
    // If this is a native exception, we have all the info we need cached in the exception header already.
    bool native_exception = isNativeException(exception);
    if (native_exception) [[likely]]
    {
        auto cxa = getCxaException(exception);
#ifdef __SYSTEM_ABI_CXX_AEABI
        _Unwind_VRS_Set(context, _UVRSC_CORE, __builtin_eh_return_data_regno(0), _UVRSD_UINT32, &cxa->ucb.barrier_cache.bitpattern[4]);
        _Unwind_VRS_Set(context, _UVRSC_CORE, __builtin_eh_return_data_regno(1), _UVRSD_UINT32, &cxa->ucb.barrier_cache.bitpattern[0]);
        _Unwind_VRS_Set(context, _UVRSC_CORE, 15, _UVRSD_UINT32, &cxa->ucb.barrier_cache.bitpattern[3]);
#else
        _Unwind_SetGR(context, __builtin_eh_return_data_regno(0), reinterpret_cast<std::uintptr_t>(cxa->adjustedPointer));
        _Unwind_SetGR(context, __builtin_eh_return_data_regno(1), static_cast<std::uintptr_t>(cxa->handlerSwitchValue));
        _Unwind_SetIP(context, reinterpret_cast<std::uintptr_t>(cxa->catchTemp));
#endif
        return _URC_INSTALL_CONTEXT;
    }

    // For non-native exceptions, we need to repeat the phase 1 work and find all of the information again.

    // Get the LSDA. We know it exists.
    auto lsda_address = _Unwind_GetLanguageSpecificData(context);
    auto lsda_ptr = reinterpret_cast<const std::byte*>(lsda_address);
    auto lsda_info = parseLSDAHeader(context);

    // Get the instruction pointer and find the call site information.
    int ip_before;
    auto pc = _Unwind_GetIPInfo(context, &ip_before);
    if (!ip_before)
        pc -= 1;
    auto call_site_info = findCallSiteInfo(lsda_info, pc);

    // Find the appropriate action to give us the handler.
    auto exception_type = &typeid(__foreign_exception);
    void* exception_object = exception + 1;
    auto action = findMatchingAction(lsda_info, call_site_info.action_id, exception_type, exception_object);

    // We now have all the required information to set up the landingpad.
    _Unwind_SetGR(context, __builtin_eh_return_data_regno(0), reinterpret_cast<std::uintptr_t>(exception));
    _Unwind_SetGR(context, __builtin_eh_return_data_regno(1), static_cast<std::uintptr_t>(action->switch_value));
    _Unwind_SetIP(context, call_site_info.landingpad);
    return _URC_INSTALL_CONTEXT;
}

//! @brief      Personality routine for GCC-compatible exception handling.
//!
//! @note       This routine conforms to the personality routine specification
//!             as given in the Itanium ABI.
//!
//! @implements _Unwind_Personality_Fn
//!
#ifdef __SYSTEM_ABI_CXX_AEABI
/*_Unwind_Reason_Code __gxx_personality_v0(_Unwind_State state, _Unwind_Control_Block* exception, _Unwind_Context* context)
{

}*/
#else
_Unwind_Reason_Code __gxx_personality_v0(int version, _Unwind_Action action, std::uint64_t exception_class, _Unwind_Exception* exception, _Unwind_Context* context)
{
    // Note: this function performs minimal error checking as the LSDA is generated by the compiler and is expected to
    //       be valid.

    // We can't handle unwind ABIs other than version 1.
    if (version != 1)
        return _URC_FATAL_PHASE1_ERROR;

    if (action & _UA_SEARCH_PHASE)
        return unwindPhase1(exception, context);
    else if (action & _UA_HANDLER_FRAME)
        return unwindPhase2Handler(exception, context);
    else if (action & (_UA_FORCE_UNWIND | _UA_CLEANUP_PHASE))
        return unwindPhase2Cleanup(exception, context);
    
    return _URC_FATAL_PHASE1_ERROR;
}
#endif
