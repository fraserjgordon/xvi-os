#include <System/ABI/C++/DynamicCast.hh>

#include <System/C++/Utility/Algorithm.hh>

namespace System::ABI::CXX
{


// Class access types.
enum class AccessType
{
    Unknown     = -1,
    Public      =  0,
    Protected   =  1,   // Not actually used; protected-vs-private is irrelevant for dynamic casts.
    Private     =  2,
    Ambiguous   =  3,
    NotFound    =  4,   // Searched-for base isn't on this path.
};

// Combines two access types and returns the most restrictive combination.
static constexpr AccessType combinedAccessType(AccessType a, AccessType b)
{
    auto aa = static_cast<int>(a);
    auto bb = static_cast<int>(b);
    return static_cast<AccessType>(std::max(aa, bb));
}

// State tracking object used in the dynamic_cast implementation.
struct DynamicCastState
{
    // The offset hint given to the dynamic cast function.
    std::ptrdiff_t                          offset_hint     = -1;    

    // The source object pointer and type.
    const void*                             source_obj      = nullptr;
    const __cxxabiv1::__class_type_info*    source_type     = nullptr;    

    // The desired destination type.
    const __cxxabiv1::__class_type_info*    dest_type       = nullptr;

    // The access level on the current path.
    AccessType                              current_access  = AccessType::Unknown;
    bool                                    current_virtual = false;

    // The current object pointer.
    const void*                             current_obj     = nullptr;


    // Returns true if the given object pointer and type match the source.
    bool isSource(const void* obj, const __cxxabiv1::__class_type_info* type)
    {
        return (obj == source_obj) && (*type == *source_type);
    }

    // Returns true if the given type matches the destination type.
    bool isDestination(const __cxxabiv1::__class_type_info* type)
    {
        return *type == *dest_type;
    }
};

// Result tracking object used in the dynamic cast implementation.
struct DynamicCastResult
{
   // Path from the most-derived dynamic type to the static source type.
    AccessType      dynamic_to_source_access    = AccessType::Unknown;
    bool            dynamic_to_source_virtual   = false;

    // Path from the most-derived dynamic type to the static destination type.
    AccessType      dynamic_to_dest_access      = AccessType::Unknown;

    // Overall results.
    AccessType      dest_to_source_access       = AccessType::Unknown;
    bool            dest_to_source_virtual      = false;
    const void*     dest_obj                    = nullptr;
};


// Returns true if inheritance in the given record is public.
static constexpr bool isPublicInheritance(const __cxxabiv1::__base_class_type_info& info)
{
    return info.__offset_flags & __cxxabiv1::__base_class_type_info::__public_mask;
}

// Returns the access type implied by the given base class record.
static constexpr AccessType accessTypeVia(const __cxxabiv1::__base_class_type_info& info)
{
    return isPublicInheritance(info) ? AccessType::Public : AccessType::Private;
}


// Performs a base-to-derived dynamic cast.
static const void* dynamicCastBaseToDerived(const void* obj, const __cxxabiv1::__class_type_info* src, const __cxxabiv1::__class_type_info* dest)
{

}

// Performs a cross-cast dynamic cast.
static const void* dynamicCastCrossCast(const void* obj, const __cxxabiv1::__class_type_info* src, const __cxxabiv1::__class_type_info* dest)
{

}


} // namespace System::ABI:CXX


//void* __cxxabiv1::__dynamic_cast(const void* __sub, const __class_type_info* src, const __class_type_info* dest, std::ptrdiff_t src2dest_offset)
//{
//    
//}
