#pragma once
#ifndef __SYSTEM_ABI_CXX_TYPEINFO_H
#define __SYSTEM_ABI_CXX_TYPEINFO_H


#include <System/ABI/C++/Private/Config.hh>

#include <System/C++/LanguageSupport/StdDef.hh>
#include <System/C++/LanguageSupport/StdInt.hh>


// Note: the definition of type_info *must* be in the `std` namespace. If not placed here, compilers won't recognise it
//       and won't give it the special semanics that it requires.
namespace std
{


class __SYSTEM_ABI_CXX_RTTI_EXPORT type_info
{
public:

    virtual ~type_info();
    constexpr bool operator==(const type_info&) const noexcept;
    bool before(const type_info&) const noexcept;
    std::size_t hash_code() const noexcept;
    const char* name() const;

    type_info(const type_info&) = delete;
    type_info& operator=(const type_info&) = delete;

private:

    const char*     __type_name;

protected:

    // Custom deleter.
    void operator delete(void*) noexcept;
};


constexpr bool std::type_info::operator==(const type_info& other) const noexcept
{
    // The ABI requires each type has only one, global string for its name, even if multiple type_info objects refer to
    // the type (e.g. a complete class type_info and an incomplete class type_info). This means that the type_info
    // object addresses can't be used for declaring type equality but type names can.
    return __type_name == other.__type_name;
}


} // namespace std


namespace __cxxabiv1
{


class __SYSTEM_ABI_CXX_RTTI_EXPORT __fundamental_type_info :
    public std::type_info
{
public:

    ~__fundamental_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __array_type_info :
    public std::type_info
{
public:

    ~__array_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __function_type_info :
    public std::type_info
{
public:

    ~__function_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __enum_type_info :
    public std::type_info
{
public:

    ~__enum_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __class_type_info :
    public std::type_info
{
public:

    ~__class_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __si_class_type_info :
    public __class_type_info
{
public:

    const __class_type_info* __base_type;

    ~__si_class_type_info() override;
};

struct __SYSTEM_ABI_CXX_RTTI_EXPORT __base_class_type_info
{
    const __class_type_info*    __base_type;
    long                        __offset_flags;

    enum __offset_flags_masks : long
    {
        __virtual_mask      = 0x01,
        __public_mask       = 0x02,
        __offset_shift      = 8,
    };

    std::ptrdiff_t __get_offset() const
    {
        return static_cast<std::ptrdiff_t>(__offset_flags >> __offset_shift);
    }

    long __get_flags() const
    {
        return __offset_flags & (__offset_shift - 1);
    }
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __vmi_class_type_info :
    public __class_type_info
{
public:

    unsigned int            __flags;
    unsigned int            __base_count;
    __base_class_type_info  __base_info[1];

    enum __flags_masks : unsigned int
    {
        __non_diamond_repeat_mask   = 0x01,
        __diamond_shaped_mask       = 0x02,
    };

    ~__vmi_class_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __pbase_type_info :
    public std::type_info
{
public:

    unsigned int            __flags;
    const std::type_info*   __pointee;

    enum __masks : unsigned int
    {
        __const_mask                = 0x01,
        __volatile_mask             = 0x02,
        __restrict_mask             = 0x04,
        __incomplete_mask           = 0x08,
        __incomplete_class_mask     = 0x10,
        __transaction_safe_mask     = 0x20,
        __noexcept_mask             = 0x40,
    };

    ~__pbase_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __pointer_type_info :
    public __pbase_type_info
{
public:

    ~__pointer_type_info() override;
};

class __SYSTEM_ABI_CXX_RTTI_EXPORT __pointer_to_member_type_info :
    public __pbase_type_info
{
public:

    const __class_type_info*    __context;

    ~__pointer_to_member_type_info() override;
};


} // namespace __cxxabiv1


namespace System::ABI::CXX
{


// Any type_info object must be one of the following concrete types.
enum class TypeInfoType
{
    Unknown         =   0,      // Unknown type (somehow).
    Fundamental     =   1,      // __fundamental_type_info
    Array           =   2,      // __array_type_info
    Function        =   3,      // __function_type_info
    Enum            =   4,      // __enum_type_info
    Class           =   5,      // __class_type_info
    SiClass         =   6,      // __si_class_type_info
    VmiClass        =   7,      // __vmi_class_type_info
    Pointer         =   8,      // __pointer_type_info
    PointerToMember =   9,      // __pointer_to_member_type_info
};


class VTable
{
public:

    const std::type_info* typeInfo() const
    {
        // The ABI places the type_info pointer at offset -1 from the vtable centre.
        return slotAs<const std::type_info*>(-1);
    }

    std::ptrdiff_t offsetToTop() const
    {
        // The ABI places the offset-to-top at offset -2 from the vtable centre.
        return static_cast<std::ptrdiff_t>(slot(-2));
    }

    std::ptrdiff_t virtualBaseOffset(std::ptrdiff_t offset_in_vtable) const
    {
        auto address = reinterpret_cast<std::intptr_t>(this);
        address += offset_in_vtable;
        return *reinterpret_cast<const std::ptrdiff_t*>(address);
    }


    static const VTable* fromObject(const void* obj)
    {
        return *reinterpret_cast<const VTable* const *>(obj);
    }

private:

    std::uintptr_t slot(std::ptrdiff_t n) const
    {
        return reinterpret_cast<const std::uintptr_t*>(this)[n];
    }

    template <class T>
    T slotAs(std::ptrdiff_t n) const
    {
        return reinterpret_cast<T>(slot(n));
    }
};


// Classifies the type of the given type_info object.
//
// This is a bit meta as it looks at the type_info of the type_info object - because type_info has a vtable, this is
// possible and is sufficient to determine the type of the type_info object.
constexpr TypeInfoType getTypeInfoType(const std::type_info& ti)
{
    const auto& meta_ti = typeid(ti);
    auto ptr = &meta_ti;

    if (ptr == &typeid(__cxxabiv1::__fundamental_type_info))
        return TypeInfoType::Fundamental;
    else if (ptr == &typeid(__cxxabiv1::__array_type_info))
        return TypeInfoType::Array;
    else if (ptr == &typeid(__cxxabiv1::__function_type_info))
        return TypeInfoType::Function;
    else if (ptr == &typeid(__cxxabiv1::__enum_type_info))
        return TypeInfoType::Enum;
    else if (ptr == &typeid(__cxxabiv1::__class_type_info))
        return TypeInfoType::Class;
    else if (ptr == &typeid(__cxxabiv1::__si_class_type_info))
        return TypeInfoType::SiClass;
    else if (ptr == &typeid(__cxxabiv1::__vmi_class_type_info))
        return TypeInfoType::VmiClass;
    else if (ptr == &typeid(__cxxabiv1::__pointer_type_info))
        return TypeInfoType::Pointer;
    else if (ptr == &typeid(__cxxabiv1::__pointer_to_member_type_info))
        return TypeInfoType::PointerToMember;
    else
        return TypeInfoType::Unknown;
}


// Given an object pointer, returns the dynamic type_info object.
inline const std::type_info& dynamicTypeInfo(const void* obj)
{
    return *VTable::fromObject(obj)->typeInfo();
}

// Given an object pointer, returns the pointer to the most-derived object (equivalent to dynamic_cast<void*>).
inline const void* wholeObjectPointer(const void* obj)
{
    auto address = reinterpret_cast<std::intptr_t>(obj);
    auto vtbl = VTable::fromObject(obj);

    address += vtbl->offsetToTop();

    return reinterpret_cast<const void*>(address);
}

// Adjusts the given object pointer to point at the indicated base object.
inline const void* adjustToBase(const void* obj, const __cxxabiv1::__base_class_type_info& info)
{
    // Raw offset value. The interpretation varies between the virtual and non-virtual cases.
    std::ptrdiff_t offset = info.__offset_flags >> __cxxabiv1::__base_class_type_info::__offset_shift;
    bool is_virtual = info.__offset_flags & __cxxabiv1::__base_class_type_info::__virtual_mask;

    if (is_virtual)
    {
        // The offset is the offset added to the vtable pointer to get the virtual base offset.
        offset = VTable::fromObject(obj)->virtualBaseOffset(offset);
    }

    // Return the adjusted pointer.
    auto address = reinterpret_cast<std::intptr_t>(obj);
    address += offset;
    return reinterpret_cast<const void*>(address);
}


enum class TypeInfoWalkResult
{
    Recurse,        // Recurse into child types.
    NoRecurse,      // No need to check child types.
    SkipSiblings,   // No need to check sibling types.
    Done,           // No need to check any other types.
};

template <class Callback, class State>
TypeInfoWalkResult walkTypeInfoTree(const std::type_info& root, Callback&& cb, State in_state, const std::type_info* parent = nullptr, unsigned int index = 0)
{
    // As we recurse, the state object gets copied so callers can use it to accumulate state on the way down the tree
    // that gets reverted on the way back up (for example, used to track the most restrictive access control on this
    // path through the tree).
    auto state = in_state;
    
    switch (auto type = getTypeInfoType(root); type)
    {
        case TypeInfoType::Class:
        {
            // Terminal (base) class.
            auto recurse = cb(state, root, parent, index);
            if (recurse == TypeInfoWalkResult::NoRecurse || recurse == TypeInfoWalkResult::Recurse)
                return TypeInfoWalkResult::Recurse;

            return recurse;
        }
        
        case TypeInfoType::SiClass:
        {
            // Call for this class then recurse into the base class.
            auto recurse = cb(state, root, parent, index);
            if (recurse == TypeInfoWalkResult::NoRecurse)
            {
                return TypeInfoWalkResult::Recurse;
            }
            if (recurse == TypeInfoWalkResult::Recurse)
            {
                const auto& root_si = static_cast<const __cxxabiv1::__si_class_type_info&>(root);
                recurse = walkTypeInfoTree(*root_si.__base_type, cb, state, &root, 0);

                if (recurse == TypeInfoWalkResult::Done)
                    return TypeInfoWalkResult::Done;
                else
                    return TypeInfoWalkResult::Recurse;
            }
            
            return recurse;
        }

        case TypeInfoType::VmiClass:
        {
            // Call for this class then recurse into each of the base classes.
            auto recurse = cb(state, root, parent, index);
            if (recurse == TypeInfoWalkResult::NoRecurse)
            {
                return TypeInfoWalkResult::Recurse;
            }
            else if (recurse == TypeInfoWalkResult::Recurse)
            {
                const auto& root_vmi = static_cast<const __cxxabiv1::__vmi_class_type_info&>(root);
                for (unsigned int i = 0; i < root_vmi.__base_count; ++i)
                {
                    recurse = walkTypeInfoTree(*root_vmi.__base_info[i].__base_type, cb, state, &root, i);
                    if (recurse == TypeInfoWalkResult::Done)
                        return TypeInfoWalkResult::Done;
                    else if (recurse == TypeInfoWalkResult::SkipSiblings)
                        return TypeInfoWalkResult::Recurse;
                }
            }
            
            return recurse;
        }

        default:
            // Can't be part of a class tree.
            return TypeInfoWalkResult::Done;
    }
}


} // namespace System::ABI::CXX


#endif /* ifndef __SYSTEM_ABI_CXX_TYPEINFO_H */
