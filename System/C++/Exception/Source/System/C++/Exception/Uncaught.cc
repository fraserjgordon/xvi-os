#include <System/C++/Exception/Uncaught.hh>

#include <System/ABI/C++/Exception.hh>


namespace __XVI_STD_EXCEPTION_NS_DECL
{


int uncaught_exceptions() noexcept
{
    const auto* globals = __SYSTEM_ABI_CXX_NS::__cxa_get_globals();
    auto uncaught = globals->uncaughtExceptions;

    // Hopefully we never have more than numeric_limits<int>::max() uncaught exceptions...
    return static_cast<int>(uncaught);
}


} // namespace __XVI_STD_EXCEPTION_NS_DECL
