#include <System/ABI/C++/Exception.hh>

#include <System/ABI/C++/Exception/Utils.hh>


namespace System::ABI::CXX
{


// Weakly imported symbols.
[[noreturn]] [[gnu::weak]] void __abort() asm("System.Abort");


// Global unexpected exception handler.
static std::atomic<std::terminate_handler> g_unexpectedHandler  = nullptr;


void unexpectedException()
{
    unexpectedExceptionWithHandler(g_unexpectedHandler);
}

void unexpectedExceptionWithHandler(std::unexpected_handler handler)
{
    if (handler)
        handler();

    __abort();
}

std::unexpected_handler getUnexpectedExceptionHandler()
{
    return g_unexpectedHandler;
}

std::unexpected_handler setUnexpectedExceptionHandler(std::unexpected_handler handler)
{
    return g_unexpectedHandler.exchange(handler);
}


}


namespace __cxxabiv1
{


void __cxa_call_unexpected(_Exception_T* exception)
{
    // Catch the exception then terminate, using the appropriate terminate handler.
    __cxa_begin_catch(exception);
    bool native_exception = isNative(exception);
    if (native_exception)
    {
        auto* cxa = getCxaException(exception);
        System::ABI::CXX::unexpectedExceptionWithHandler(cxa->unexpectedHandler);
    }
    else
    {
        System::ABI::CXX::unexpectedException();
    }
}


} // namespace __cxxabiv1
