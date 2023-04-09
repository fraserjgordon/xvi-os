#include <System/C++/Exception/Terminate.hh>

#include <System/ABI/C++/Exception.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


void terminate()
{
    System::ABI::CXX::terminate();
}

terminate_handler set_terminate(terminate_handler new_handler) noexcept
{
    return System::ABI::CXX::setTerminateHandler(new_handler);
}

terminate_handler get_terminate() noexcept
{
    return System::ABI::CXX::getTerminateHandler();
}


} // namespace __XVI_STD_EXCEPTION_NS_DECL
