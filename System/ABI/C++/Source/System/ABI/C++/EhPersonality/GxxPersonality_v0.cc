#include <System/ABI/C++/Private/Config.hh>

#include <System/ABI/C++/Exception.hh>
#include <System/ABI/C++/TypeInfo.hh>
#include <System/ABI/C++/Unwinder.hh>
#include <System/ABI/Dwarf/Decode.hh>
#include <System/ABI/Dwarf/LEB128.hh>
#include <System/C++/Utility/Optional.hh>


using namespace __cxxabiv1;
using namespace System::ABI::CXX;

namespace Dwarf = System::ABI::Dwarf;


// This is the function that we need to export as the compiler automatically generates references to it.
extern "C" __SYSTEM_ABI_CXX_EHPERSONALITY_EXPORT _Unwind_Reason_Code __gxx_personality_v0(int, _Unwind_Action, std::uint64_t, _Unwind_Exception*, _Unwind_Context*);


// Type category for checking catch clauses.
enum class type_category
{
    unknown,
    fundamental,
    array,
    function,
    enumeration,
    class_type,
    pointer,
    pointer_to_member,
};

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


// Determines the type category for a type_info.
static type_category getTypeCategory(const std::type_info* ti)
{
    // Get the type info of the type into. Because type_info is polymorphic, we can use that to find the type category.
    auto& titi = typeid(*ti);
    if (titi == typeid(__fundamental_type_info))
        return type_category::fundamental;
    else if (titi == typeid(__array_type_info))
        return type_category::array;
    else if (titi == typeid(__function_type_info))
        return type_category::function;
    else if (titi == typeid(__enum_type_info))
        return type_category::enumeration;
    else if (titi == typeid(__class_type_info) || titi == typeid(__si_class_type_info) || titi == typeid(__vmi_class_type_info))
        return type_category::class_type;
    else if (titi == typeid(__pointer_type_info))
        return type_category::pointer;
    else if (titi == typeid(__pointer_to_member_type_info))
        return type_category::pointer_to_member;
    else
        return type_category::unknown;
}

// Gets a __cxa_exception header from an _Unwind_Exception header.
static __cxa_exception* getCxaException(_Unwind_Exception* e)
{
    // The _Unwind_Exception header is the last field in the __cxa_exception header.
    return reinterpret_cast<__cxa_exception*>(e + 1) - 1;
}

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

struct ti_path_info
{
    void*           object_ptr          = nullptr;
    bool            is_public           = true;
    bool            is_virtual          = false;
    bool            no_repeated_bases   = false;
};

static std::optional<ti_path_info> findPathToBase(const std::type_info* base, const std::type_info* derived, void* obj_pointer)
{
    std::optional<ti_path_info> candidate = std::nullopt;    
    ti_path_info initial_info = {.object_ptr = obj_pointer};

    walkTypeInfoTree(*derived, [&](ti_path_info& info, const std::type_info& ti, const std::type_info* parent, unsigned int index) -> TypeInfoWalkResult
    {
        std::optional<ti_path_info> new_candidate = std::nullopt;   

        // If the parent type had complicated inheritance, some adjustment may be needed.
        if (parent && getTypeInfoType(*parent) == TypeInfoType::VmiClass)
        {
            // Get the information on how this class was inherited.
            auto p = static_cast<const __vmi_class_type_info*>(parent);
            auto& base_info = p->__base_info[index];

            // Is this inheritance non-public?
            if (!(base_info.__get_flags() & base_info.__public_mask))
            {
                info.is_public = false;
            }

            // Is this inheritance virtual?
            if (base_info.__get_flags() & base_info.__virtual_mask)
            {
                // The path to this base includes at least one virtual link.
                info.is_virtual = true;

                // We need to access the vtable of the object in order to adjust the object pointer.
                if (info.object_ptr != nullptr)
                {
                    auto vtbl = VTable::fromObject(info.object_ptr);
                    auto offset = vtbl->virtualBaseOffset(base_info.__get_offset());
                    info.object_ptr = reinterpret_cast<void*>(reinterpret_cast<std::intptr_t>(info.object_ptr) + offset);
                }
            }
            else
            {
                // Adjust the object pointer.
                if (info.object_ptr != nullptr)
                {
                    auto offset = base_info.__get_offset();
                    info.object_ptr = reinterpret_cast<void*>(reinterpret_cast<std::intptr_t>(info.object_ptr) + offset);
                }
            }
        }

        // Is this the destination type?
        if (ti == *base)
        {
            // We have a new candidate. Is there an existing candidate?
            new_candidate = info;
            if (!candidate)
            {
                // No existing candidate; take this one and continue searching in case we find a better one or until we
                // determinte that the inheritance is ambiguous.
                candidate = new_candidate;
            }
            else
            {
                // There is an existing candidate. This ambiguates the inheritance unless both are virtual and refer to
                // the same subobject.
                if (!new_candidate->is_virtual || !candidate->is_virtual || new_candidate->object_ptr != candidate->object_ptr)
                {
                    // We've found an ambiguity; no need to keep searching.
                    candidate = std::nullopt;
                    return TypeInfoWalkResult::Done;
                }

                // Upgrade the access type if at least one of the candidates is public.
                if (new_candidate->is_public)
                    candidate->is_public;
            }

            // If we know there are no repeated bases of the next-highest class with multiple inheritance, we can skip
            // checking any remaining siblings.
            if (info.no_repeated_bases)
                return TypeInfoWalkResult::SkipSiblings;

            // No need to recurse below the base.
            return TypeInfoWalkResult::NoRecurse;
        }

        // Can we guarantee any special properties below this point?
        if (auto type = getTypeInfoType(ti); type == TypeInfoType::VmiClass)
        {
            auto p = static_cast<const __vmi_class_type_info*>(&ti);
            
            // If there are no non-virtual repeated bases, pass that information down.
            if (!(p->__flags & p->__non_diamond_repeat_mask))
                info.no_repeated_bases = true;
            else
                info.no_repeated_bases = false;
        }
        
        // Recurse (if possible).
        return TypeInfoWalkResult::Recurse;
    }, initial_info);

    return candidate;
}

// Checks whether one type is a public, unambiguous base of another. If it is, returns the pointer adjustment necessary.
static std::optional<std::ptrdiff_t> isUnambiguousPublicBase(const std::type_info* base, const std::type_info* derived, void* object)
{
    auto path = findPathToBase(base, derived, object);
    if (!path || !path->is_public)
        return std::nullopt;

    return (reinterpret_cast<std::intptr_t>(path->object_ptr) - reinterpret_cast<std::intptr_t>(object));
}

// Checks whether one type is a public, unambiguous, non-virtual base of another.
static bool isUnambiguousNonVirtualPublicBase(const std::type_info* base, const std::type_info* derived)
{
    auto path = findPathToBase(base, derived, nullptr);
    if (!path || path->is_virtual || !path->is_public)
        return false;

    return true;
}

// Checks whether two sets of data pointers have compatible qualifiers.
static bool checkDataPointerQualifiers(const __pbase_type_info* exception_ptr_type, const __pbase_type_info* catch_ptr_type)
{
    //! @TODO: rules for restrict qualifiers?    

    // The catch type can add (but not remove) const and volatile qualifiers.
    auto cv_mask = __pbase_type_info::__const_mask | __pbase_type_info::__volatile_mask;
    auto throw_cv = exception_ptr_type->__flags & cv_mask;
    auto catch_cv = catch_ptr_type->__flags & cv_mask;
    if ((throw_cv & catch_cv) != throw_cv)
        return false;

    // All the qualifier checks have passed.
    return true;
}

// Checks whether two sets of function pointers have compatible qualifiers.
static bool checkFunctionPointerQualifiers(const __pbase_type_info* exception_ptr_type, const __pbase_type_info* catch_ptr_type)
{
    //! @TODO: rules for restrict qualifiers?    

    // The CV qualifiers must be the same on both types.
    auto cv_mask = __pbase_type_info::__const_mask | __pbase_type_info::__volatile_mask;
    auto throw_cv = exception_ptr_type->__flags & cv_mask;
    auto catch_cv = catch_ptr_type->__flags & cv_mask;
    if (catch_cv != throw_cv)
        return false;
    
    // The catch type can remove (but not add) the noexcept function qualifier.
    auto noexcept_mask = __pbase_type_info::__noexcept_mask;
    auto throw_noexcept = exception_ptr_type->__flags & noexcept_mask;
    auto catch_noexcept = catch_ptr_type->__flags & noexcept_mask;
    if ((throw_noexcept | catch_noexcept) != catch_noexcept)
        return false;

    // All the qualifier checks have passed.
    return true;
}

// Checks whether two function pointers are compatible.
static bool checkPointerToNonMemberFunctionConversion(const std::type_info* exception_ptr_type, const std::type_info* catch_ptr_type)
{
    // Get the class types being pointed to.
    auto* exception_pti = static_cast<const __pointer_type_info*>(exception_ptr_type);
    auto* catch_pti = static_cast<const __pointer_type_info*>(catch_ptr_type);
    auto* exception_function_type = exception_pti->__pointee;
    auto* catch_function_type = catch_pti->__pointee;
    
    // The pointer types have to match exactly except for the possible removal of noexcept in the catch clause.
    if (!checkFunctionPointerQualifiers(exception_pti, catch_pti))
        return false;

    return (*exception_function_type == *catch_function_type);
}

static bool checkPointerVoidConversion(const std::type_info* exception_ptr_type, const std::type_info* catch_ptr_type)
{
    // A catch of void-pointer type catches everything with compatible CV qualifiers.
    auto* exception_pti = static_cast<const __pointer_type_info*>(exception_ptr_type);
    auto* catch_pti = static_cast<const __pointer_type_info*>(catch_ptr_type);
    
    return checkDataPointerQualifiers(exception_pti, catch_pti);
}

static bool checkPointerToNonClassConversion(const std::type_info* exception_ptr_type, const std::type_info* catch_ptr_type)
{
    // Get the class types being pointed to.
    auto* exception_pti = static_cast<const __pointer_type_info*>(exception_ptr_type);
    auto* catch_pti = static_cast<const __pointer_type_info*>(catch_ptr_type);
    auto* exception_pointee_type = exception_pti->__pointee;
    auto* catch_pointee_type = catch_pti->__pointee;
    
    // The pointer types have to match exactly except for the possible addition of CV in the catch clause.
    if (!checkDataPointerQualifiers(exception_pti, catch_pti))
        return false;

    return (*exception_pointee_type == *catch_pointee_type);
}

// Checks for valid non-member pointer conversions. Returns the object pointer adjustment if allowed.
static std::optional<std::ptrdiff_t> checkNonMemberPointerConversion(const std::type_info* exception_ptr_type, const std::type_info* catch_ptr_type, void* ptr)
{
    // Get the pointee types.
    auto* exception_pti = static_cast<const __pointer_type_info*>(exception_ptr_type);
    auto* catch_pti = static_cast<const __pointer_type_info*>(catch_ptr_type);
    auto* etype = exception_pti->__pointee;
    auto* ctype = catch_pti->__pointee;

    // Short-cut: if the two pointer types are the same, no conversion is needed (and therefore no adjustment either).
    if (*exception_ptr_type == *catch_ptr_type)
        return 0;

    // If the catch type is pointer to void, handle that case specially.
    if (*ctype == typeid(void))
    {
        if (checkPointerVoidConversion(exception_pti, catch_pti))
            return 0;
        else
            return {};
    }

    // The categories of the pointees need to be the same.
    auto etype_category = getTypeCategory(etype);
    auto ctype_category = getTypeCategory(ctype);
    if (etype_category != ctype_category)
        return {};

    // What kind of things as we pointing to?
    switch (etype_category)
    {
        case type_category::fundamental:
        case type_category::array:
        case type_category::enumeration:
        case type_category::pointer:
        case type_category::pointer_to_member:
            // Pointers to non-function, non-class types are handled elsewhere. They never require adjustment.
            if (checkPointerToNonClassConversion(exception_ptr_type, catch_ptr_type))
                return 0;
            else
                return {};

        case type_category::function:
            // Pointers to functions are handled elsewhere. They never require adjustment.
            if (checkPointerToNonMemberFunctionConversion(exception_ptr_type, catch_ptr_type))
                return 0;
            else
                return {};

        case type_category::class_type:
            // Compatible if the catch type is an unambiguous public base of the throw type.
            return isUnambiguousPublicBase(ctype, etype, *reinterpret_cast<void**>(ptr));

        case type_category::unknown:
            // This shouldn't happen...
            return {};
    }

    // All cases should have been handled above...
    return {};
}

static bool checkPointerToMemberDataConversion(const std::type_info* exception_ptr_type, const std::type_info* catch_ptr_type)
{
    // Get the class types being pointed to.
    auto* exception_pti = static_cast<const __pointer_to_member_type_info*>(exception_ptr_type);
    auto* catch_pti = static_cast<const __pointer_to_member_type_info*>(catch_ptr_type);
    auto* exception_class_type = exception_pti->__context;
    auto* catch_class_type = catch_pti->__context;

    // Check that the pointer qualifiers are compatible.
    if (!checkDataPointerQualifiers(exception_pti, catch_pti))
        return false;

    // The pointee types have to match exactly.
    if (*exception_pti->__pointee != *catch_pti->__pointee)
        return false;

    // The conversion is permitted if the class type of the thrown pointer is a base of the catch class type (note that
    // this is the opposite way around compared to non-member pointer conversions).
    return isUnambiguousNonVirtualPublicBase(exception_class_type, catch_class_type);
}

static bool checkPointerToMemberFunctionConversion(const std::type_info* exception_ptr_type, const std::type_info* catch_ptr_type)
{
    // Get the class types being pointed to.
    auto* exception_pti = static_cast<const __pointer_to_member_type_info*>(exception_ptr_type);
    auto* catch_pti = static_cast<const __pointer_to_member_type_info*>(catch_ptr_type);
    auto* exception_class_type = exception_pti->__context;
    auto* catch_class_type = catch_pti->__context;

    // Check that the pointer qualifiers are compatible.
    if (!checkFunctionPointerQualifiers(exception_pti, catch_pti))
        return false;

    // The pointee types have to match exactly.
    if (*exception_pti->__pointee != *catch_pti->__pointee)
        return false;

    // The conversion is permitted if the class type of the thrown pointer is a base of the catch class type (note that
    // this is the opposite way around compared to non-member pointer conversions).
    return isUnambiguousNonVirtualPublicBase(exception_class_type, catch_class_type);
}

static bool checkPointerToMemberConversion(const std::type_info* exception_ptr_type, const std::type_info* catch_ptr_type)
{
    // Is this a pointer to a member function or a data member?
    auto* exception_pti = static_cast<const __pointer_to_member_type_info*>(exception_ptr_type);
    if (typeid(*exception_pti->__pointee) == typeid(__function_type_info))
        return checkPointerToMemberFunctionConversion(exception_ptr_type, catch_ptr_type);
    else
        return checkPointerToMemberDataConversion(exception_ptr_type, catch_ptr_type);
}

// Checks if two types match for the purposes of a catch clause. Returns the required pointer adjustment if valid.
static std::optional<std::ptrdiff_t> typesMatchForCatch(const std::type_info* exception_type, const std::type_info* catch_type, void* object)
{
    // Short-circuit for the case of exactly-matching types.
    if (*exception_type == *catch_type)
        return 0;
    
    // What category of types do we have?
    auto exception_category = getTypeCategory(exception_type);
    auto catch_category = getTypeCategory(catch_type);

    // If the catch is any type of pointer and the thrown type is of std::nullptr_t, we have a match.
    if ((catch_category == type_category::pointer || catch_category == type_category::pointer_to_member) && *exception_type == typeid(decltype(nullptr)))
        return 0;

    // If the categories don't match, no further checking is needed.
    if (exception_category != catch_category)
        return {};

    // Some categories are not valid for exceptions. Others are invalid if we get to this point.
    switch (exception_category)
    {
        // These should have decayed; they're invalid in catch clauses.
        case type_category::array:
        case type_category::function:
            return {};

        case type_category::fundamental:
        case type_category::enumeration:
            // These require strict type equality, which we've already checked for.
            return {};

        case type_category::pointer:
            // This is a complex case.
            return checkNonMemberPointerConversion(exception_type, catch_type, object);

        case type_category::pointer_to_member:
            // This is a complex case.
            if (checkPointerToMemberConversion(exception_type, catch_type))
                return 0;
            else
                return {};

        case type_category::class_type:
            // This is a complex case.
            return isUnambiguousPublicBase(catch_type, exception_type, object);
    }

    // All possible outcomes should have been checked above...
    return {};
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
                // Catching a specific type. Does it match the thrown type?
                if (auto conversion = typesMatchForCatch(exception_type, catch_type, exception_object); conversion)
                {
                    // Adjust the pointer to the exception object. This needs special handling if the exception type is
                    // itself a pointer: we need to adjust the pointer itself, not the pointer to the pointer.
                    switch (auto type = getTypeInfoType(*catch_type); type)
                    {
                        case TypeInfoType::Pointer:
                        {
                            // The exception pointer points to the pointer that we need to adjust.
                            *static_cast<std::uintptr_t*>(exception_object) += *conversion;
                            break;
                        }

                        case TypeInfoType::PointerToMember:
                        {
                            // No adjustments needed.
                            //! @TODO: verify this!
                            break;
                        }

                        default:
                        {
                            // Adjust the pointer to the exception directly.
                            exception_object = reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(exception_object) + *conversion);
                            break;
                        }
                    }

                    return handler_info
                    {
                        .switch_value = action.type_filter,
                        .action_record = reinterpret_cast<const char*>(action_ptr),
                        .adjusted_ptr = exception_object,
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

[[noreturn]] static void doTerminate(_Unwind_Exception* exception)
{
    // Catch the exception then terminate, using the appropriate terminate handler.
    __cxa_begin_catch(exception + 1);
    bool native_exception = (exception->exception_class == CxxExceptionClass || exception->exception_class == CxxDependentClass);
    if (native_exception)
    {
        auto* cxa = getCxaException(exception);
        terminateWithHandler(cxa->terminateHandler);
    }
    else
    {
        terminate();
    }
}

static _Unwind_Reason_Code unwindPhase1(_Unwind_Exception* exception, _Unwind_Context* context)
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
        doTerminate(exception);

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
    bool native_exception = (exception->exception_class == CxxExceptionClass || exception->exception_class == CxxDependentClass);
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
        doTerminate(exception);

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
        cxa->handlerSwitchValue = action->switch_value;
        cxa->actionRecord = action->action_record;
        cxa->languageSpecificDataArea = reinterpret_cast<const char*>(lsda_ptr);
        cxa->catchTemp = reinterpret_cast<void*>(call_site_info.landingpad);
        cxa->adjustedPointer = action->adjusted_ptr;
    }

    return _URC_HANDLER_FOUND;
}

static _Unwind_Reason_Code unwindPhase2Cleanup(_Unwind_Exception* exception, _Unwind_Context* context)
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
        doTerminate(exception);

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

static _Unwind_Reason_Code unwindPhase2Handler(_Unwind_Exception* exception, _Unwind_Context* context)
{
    // If this is a native exception, we have all the info we need cached in the exception header already.
    bool native_exception = (exception->exception_class == CxxExceptionClass || exception->exception_class == CxxDependentClass);
    if (native_exception) [[likely]]
    {
        auto cxa = getCxaException(exception);
        _Unwind_SetGR(context, __builtin_eh_return_data_regno(0), reinterpret_cast<std::uintptr_t>(cxa->adjustedPointer));
        _Unwind_SetGR(context, __builtin_eh_return_data_regno(1), static_cast<std::uintptr_t>(cxa->handlerSwitchValue));
        _Unwind_SetIP(context, reinterpret_cast<std::uintptr_t>(cxa->catchTemp));
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
