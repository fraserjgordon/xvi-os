#include <System/C++/Exception/Unexpected.hh>

#include <System/ABI/C++/Exception.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


unexpected_handler set_unexpected(unexpected_handler new_handler) noexcept
{
    return System::ABI::CXX::setUnexpectedExceptionHandler(new_handler);
}

unexpected_handler get_unexpected() noexcept
{
    return System::ABI::CXX::getUnexpectedExceptionHandler();
}


} // namespace __XVI_STD_EXCEPTION_NS
