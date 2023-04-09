#include <System/C++/LanguageSupport/Exception.hh>
#include <System/C++/Core/StdExcept.hh>


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


} // namespace __XVI_STD_NS
