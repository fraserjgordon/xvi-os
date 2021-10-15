#include <System/ABI/C++/Private/Config.hh>

#include <System/ABI/C++/Exception.hh>
#include <System/ABI/C++/TypeInfo.hh>
#include <System/C++/Utility/Optional.hh>
#include <System/C++/Utility/Pair.hh>

#include <System/ABI/C++/EhPersonality/Utils.hh>


using namespace __cxxabiv1;
using namespace System::ABI::CXX;


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
static std::pair<__cxa_type_match_result, std::ptrdiff_t> typesMatchForCatch(const std::type_info* exception_type, const std::type_info* catch_type, void* object)
{
    // Short-circuit for the case of exactly-matching types.
    if (*exception_type == *catch_type)
        return {__ctm_succeeded, 0};
    
    // What category of types do we have?
    auto exception_category = getTypeCategory(exception_type);
    auto catch_category = getTypeCategory(catch_type);

    // If the catch is any type of pointer and the thrown type is of std::nullptr_t, we have a match.
    if ((catch_category == type_category::pointer || catch_category == type_category::pointer_to_member) && *exception_type == typeid(decltype(nullptr)))
        return {__ctm_succeeded, 0};

    // If the categories don't match, no further checking is needed.
    if (exception_category != catch_category)
        return {__ctm_failed, 0};

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
        {
            // This is a complex case.
            auto adjust = checkNonMemberPointerConversion(exception_type, catch_type, object);
            if (adjust)
                return {__ctm_succeeded_with_ptr_to_base, *adjust};
            else
                return {__ctm_failed, 0};
        }

        case type_category::pointer_to_member:
            // This is a complex case.
            if (checkPointerToMemberConversion(exception_type, catch_type))
                return {__ctm_succeeded, 0};
            else
                return {__ctm_failed, 0};

        case type_category::class_type:
        {
            // This is a complex case.
            auto adjust = isUnambiguousPublicBase(catch_type, exception_type, object);
            if (adjust)
                return {__ctm_succeeded, *adjust};
            else
                return {__ctm_failed, 0};
        }

        case type_category::unknown:
            // Shouldn't happen...
            return {__ctm_failed, 0};
    }

    // All possible outcomes should have been checked above...
    return {__ctm_failed, 0};
}


namespace __cxxabiv1
{


#ifdef __SYSTEM_ABI_CXX_AEABI
__cxa_type_match_result __cxa_type_match(_Unwind_Control_Block* exception, const std::type_info* catch_type, bool is_reference, void** object_ptr)
#else
__cxa_type_match_result __cxa_type_match(void* exception, const std::type_info* catch_type, bool is_reference, void** object_ptr)
#endif
{
    //! @todo: how to use is_reference?

    // Get the type info for the thrown object.
    const std::type_info* ti = nullptr;
    if (isNativeException(exception))
    {
        // Get the type info from the exception object.
        auto cxa_exception = getCxaException(exception);
        ti = cxa_exception->exceptionType;
    }
    else
    {
        // We're dealing with a foreign exception.
        ti = &typeid(__foreign_exception);
    }

    // Perform the type check.
    auto [result, adjustment] = typesMatchForCatch(ti, catch_type, *object_ptr);

    // Return early if no pointer adjustment is required.
    if (result == __ctm_failed || adjustment == 0)
        return result;

    // Adjust the pointer so that it points at the appropriate base object.
    auto adjusted_ptr = reinterpret_cast<std::uintptr_t>(*object_ptr) + adjustment;
    *object_ptr = reinterpret_cast<void*>(adjusted_ptr);

    return result;
}


} // namespace __cxxabiv1
