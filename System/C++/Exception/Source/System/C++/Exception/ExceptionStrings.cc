#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/LanguageSupport/TypeInfo.hh>
#include <System/C++/Core/Any.hh>
#include <System/C++/Core/Expected.hh>
#include <System/C++/Core/Function.hh>
#include <System/C++/Core/Optional.hh>
#include <System/C++/Core/StdExcept.hh>
#include <System/C++/Core/Variant.hh>


// The .what() method for exception classes defined anywhere in the C++ library should be added here. This will cause
// the RTTI for the classes to be emitted in this file.


namespace __XVI_STD_NS
{


const char* exception::what() const noexcept
{
    return "unknown exception";
}

const char* bad_exception::what() const noexcept
{
    return "bad exception";
}

const char* bad_alloc::what() const noexcept
{
    return "allocation failure";
}

const char* bad_array_new_length::what() const noexcept
{
    return "invalid length for array new";
}

const char* bad_cast::what() const noexcept
{
    return "bad cast";
}

const char* bad_typeid::what() const noexcept
{
    return "bad typeid";
}

const char* bad_any_cast::what() const noexcept
{
    return "bad any_cast";
}

const char* bad_expected_access<void>::what() const noexcept
{
    return "bad expected access";
}

const char* bad_function_call::what() const noexcept
{
    return "bad function call"; 
}

const char* bad_optional_access::what() const noexcept
{
    return "bad optional access";
}

const char* bad_variant_access::what() const noexcept
{
    return "bad variant access";
}


} // namespace __XVI_STD_NS
