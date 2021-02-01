#include <System/ABI/C++/Exception.hh>


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
