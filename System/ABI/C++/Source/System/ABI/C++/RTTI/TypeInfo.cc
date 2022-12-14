#include <System/ABI/C++/TypeInfo.hh>

#include <System/C++/LanguageSupport/New.hh>


// Be warned: both GCC and Clang do magic things when processing this file!
//
// In particular, compiling the virtual destructor definition for type_info causes not only emission of the vtable for
// the class but also the generation of type_info objects for all of the built-in types.


void std::type_info::operator delete(void*) noexcept
{
    // Custom no-op deleter.
    //
    // This is used to reduce the dependencies of this libary. In particular, no definition of the global delete
    // operator is required.
}


std::type_info::~type_info()
{
    // It is not possible to create type_info instances at run-time (they're exclusively generated by the compiler) so
    // this destructor doesn't need to do anything. It is only virtual to make the class a polymorphic type.
}


bool std::type_info::before(const type_info& other) const noexcept
{
    return __type_name < other.__type_name;
}


std::size_t std::type_info::hash_code() const noexcept
{
    // Because of the ABI rules requiring the type_info type name strings to have a single, unique address, we can use
    // that address as the hash code for the type_info.
    return reinterpret_cast<std::size_t>(__type_name);
}


const char* std::type_info::name() const
{
    return __type_name;
}


__cxxabiv1::__fundamental_type_info::~__fundamental_type_info() {}
__cxxabiv1::__array_type_info::~__array_type_info() {}
__cxxabiv1::__function_type_info::~__function_type_info() {}
__cxxabiv1::__enum_type_info::~__enum_type_info() {}
__cxxabiv1::__class_type_info::~__class_type_info() {}
__cxxabiv1::__si_class_type_info::~__si_class_type_info() {}
__cxxabiv1::__vmi_class_type_info::~__vmi_class_type_info() {}
__cxxabiv1::__pbase_type_info::~__pbase_type_info() {}
__cxxabiv1::__pointer_type_info::~__pointer_type_info() {}
__cxxabiv1::__pointer_to_member_type_info::~__pointer_to_member_type_info() {}
