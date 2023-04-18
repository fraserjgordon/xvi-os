#include <System/C++/Exception/StdExcept.hh>


// Implementations of exception-throwing methods go in this file for C++ library components that optionally support
// exceptions.


namespace __XVI_STD_EXCEPTION_NS_DECL
{


namespace __detail
{


[[noreturn]] void __string_bad_alloc()
{
    throw bad_alloc();
}

[[noreturn]] void __string_length_error(const char* message)
{
    throw length_error(message);
}

[[noreturn]] void __string_out_of_range(const char* message)
{
    throw out_of_range(message);
}

[[noreturn]] void __string_view_out_of_range()
{
    throw out_of_range("invalid index into string_view");
}


} // namespace __detail


} // namespace __XVI_STD_NS
