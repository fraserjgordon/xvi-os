#include <System/C++/Exception/StdExcept.hh>


// Implementations of exception-throwing methods go in this file for C++ library components that optionally support
// exceptions.


namespace __XVI_STD_EXCEPTION_NS_DECL
{


namespace __detail
{


[[noreturn]] void __string_view_out_of_range()
{
    throw out_of_range("invalid index into string_view");
}


} // namespace __detail


} // namespace __XVI_STD_NS
